	void ET::execute0(obj o,int d,bool forceFirst,int debugLevel) {
	// executes the object. If the object is a secondary (aggregate) and forceFirst is true then the secondary 
	// is executed regardless of executionLevel.
		Int i,n,nestLevel=0;Aggregate a{0};bool secondaryLoaded=false;
	// ATTENTION on "Ap" the size of aP could be different than Word !!! something different must be done.

	// operator precedence is High:< mid:&& Low:||  Lazy evaluation is guaranted to happen.
	// The expression E.counter-- is overloaded
		try{
		while(true){

			if(counter-- || o.isValue() ||(!postClear(forceFirst)&&o.executionLevel()<execution.l)){
				pushObjAction(o);
			}else if(o.isPrimitive()){
				(*t2primitiveP)(o.w)();
			}else{
		// the behaviour is active
				if(o.Class()!=classAggregate){ // non secondary execution
					Obj C=classInfoOf(o);
					if(C==notFound) // 21-03-2016 changed from o== to C==
						pushObjAction(o);
					else
						(*classInfos(C).executeFunction)(o);
				}else{ // secondary execution
					if(postSet(secondaryLoaded))
						exS.push2(a,i); // if a secondary was loaded in a previous cycle push a.t,i
					else 
						exS.push2(0,0); // if a secondary wasn't loaded in a previous cycle then push 0,0
					++nestLevel;
					i=0;a=Aggregate{o};
		l:	  n=a.elements();
				}
			}
					
			if(!secondaryLoaded) return;
		// the execution of the previous code was inside a secondary.
			if(i<n && leaveN==0 ){	
				if(debugLevel>=nestLevel){
					if(a) 
						print(toString(a,tsAlsoName,i));
					showStack();
					tString s=getLine();
					for(obj i:s){
						char c=i.w;
						if(c=='e' || c=='E')
							if(debugLevel<=nestLevel) ++debugLevel;break;
					}
				}
				stopETs.wait();
				o=a[i++];
			}else{
l1:			if(d>=exS.elements()) return;
				int N=MAX((int)leaveN,1);leaveN=0;
				while(exS.elements()>d && N>0){
					i=exS.pop();a=((Aggregate)exS.pop());--nestLevel;if(debugLevel>nestLevel) debugLevel=nestLevel;
					if(isNumber(i))N--;else{stack().push(a);execute(i,true);}
				}
				if(N) 
					std::wcout<<L"bello";
				if((leaveN=N)==0 && a.w!=0)goto l;else return; 
			}		
		}
		}catch(obj o){
			goto l1;
		}
	}

