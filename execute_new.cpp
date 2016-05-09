void execute0(obj o,bool isSecondaryIgnoreExecutionLevel){
	if(isSecondaryIgnoreExecutionLevel || !executeSingle(o))
		executeSecondary(o);
}

bool executeSingle(obj o){
	if(counter-- || o.isValue() || o.executionLevel()<execution.l){
		pushObjAction(o);return true;
	}else if(o.isPrimitive()){
		(*t2primitiveP)(o.w)();return true;
	}else if(o.Class()==classAggregate)
		return false;
	// non secondary execution
	Obj C=classInfoOf(o);
	if(C==notFound) // 21-03-2016 changed from o== to C==
		pushObjAction(o);
	else
		(*classInfos(C).executeFunction)(o);
	return true;
}				

// executes the object. If the object is a secondary (aggregate) and forceFirst is true then the secondary 
// is executed regardless of executionLevel.

// Regarding the denesting loop:
// if isNumber means that the couple is related to a secondary execution
// if i isn't a number but instead is a primitive or secondary, it means that it must be executed and is 
// a finalizer action not related to secondaries. It's subject is the a element.

void nest(Aggregate a){
	exS.push2(sec,sec_ip);
	++nestLevel;
	sec=a;sec_ip=0;
}

// unNest returns if a nesteLevel was unnested
bool unNest(){
	bool r;
	exS.pop2(sec,sec_ip);
	if(r=isNumber(sec_ip)){
		--nestLevel;//debugLevel<<nestLevel;
												// case of a return from secondary
	}else{
		stack().push(sec);
		execute0(sec_ip);		// case of a cleanUp routine
	}
	return r;
}

#define mustUnNest\
	if(!unNest())\
		THROW("unNest() should had returned true but didn't");

bool executeSecondary(const Aggregate a0,bool continuously){
	Int nl0=nestLevel,d=exS.elements()+2;
	obj o{a0};bool r=false;
i:try{
l:	nest(o);
l1:	for(Int n=sec.elements();sec_ip!=n || continuously && exS.elements()==d && (sec_ip=0,true);){
			if(debugLevel>=nestLevel)
				debugNext(sec,n,sec_ip);
			stopETs.wait();
			o=sec[sec_ip++];
			if(!executeSingle(o)) 
				goto l;		// secondary case and we nest again
		}
		while(exS.elements()>d)
			if(unNest()) goto l1;
	}

	catch(obj e){
		if(!isNumber(e)){
			LOG("unhandled case of catch with token");
		}else{
			Int nestLevel1=nestLevel-e.w;
			while(exS.elements()>d && nestLevel>nestLevel1)
				if(unNest()) 
					--e.w;
			if(e.w!=0 && e.w!=1){
				mustUnNest;
				throw(e);
			}else{
				mustUnNest;
				return false;
			}
		}
	}
	mustUnNest;
	return true;
}
