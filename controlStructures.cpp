/*
void ET::foreach(Aggregate a,Token action,bool inverseDirection){
	if (!inverseDirection)
		for(auto i=a.begin();i!=a.end();++i) {stack().push(*i);execute(action,true);}
	else
		for(auto i=a.end();i-->=a.begin();)		{stack().push(*i);execute(action,true);}
}

void ET::foreach2(Aggregate a,Token action,bool inverseDirection){
	if (!inverseDirection)
		for(auto i=a.begin();i<a.end();) 		{stack().push2(*i++,*i++);execute(action,true);}
	else
		for(auto i=a.end();i>=a.begin();)	{stack().push2(*--i,*--i);execute(action,true);}
}
*/

void foreach(Aggregate a,Obj action,bool reverse){
	for(Int i=0,n=a.elements();i!=n;++i){
		stack().push( !reverse ? a[i]:a[n+~i] );
		execute0(action,true);
	}
}

// it takes a flag b and a number or aggragate Token.
// If it is a number then exits that level of secondaries.If it is a Token it exits up to that secondary.
void exitSec(bool b,Obj n_t){
	if(b) 
		throw(n_t);
}

PRIMITIVE get_i(){
	PUSH(iCounter);
}

PRIMITIVE set_i(){
	iCounter=POP;
}

// attention, because the iCounter value may be used after the loop when !mustContinue shouldn't
// be inside the loop conditions with the incrementing operator because wrong results may be given
// if the checks are done in the wrong order

#define setNewI(x) backup<Word> b(iCounter,x)

Word for_(Word i1,Word i2,Word step,Word e){
	bool isSecondary = ((Obj)e).Class()==classAggregate;
	if(step==0){LOG("step=0 in for_");return 0;}
	(step>0 ? i2 : i1)-=abs((i2-i1)%step);
	setNewI(i1);
	Word step1=0,step2=0;( step<0 ? step1:step2 )=step;
	try{
		while(iCounter!=i2){
			iCounter+=step1;
			if(isSecondary) executeSecondary(e);else executeSingle(e);
			iCounter+=step2;
		}
	}catch(obj e){
		// if o=0 or 1 it means an exit from the current loop. So no more leaves must be done.
 		if(e.w!=0) 
			throw(e);
	}
	return iCounter;
}

void loop_(Obj e){
	try{
		bool isSecondary = ((Obj)e).Class()==classAggregate;
		if(isSecondary) 
			while(executeSecondary(e));
		else 
			while(true) 
				executeSingle(e);
	}catch(obj e){
		if(e.w!=0) 
			throw(e);
	}
}


