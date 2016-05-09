/* conventions used for the execution stack ( top of stack on the right ).
 * when a secondary is entered into the execution stack then word:offset are pushed into it.
 * A deallocator to be executed during unwinding is entered as subject:action 
 * There is no confilict between the two previous cases because of the bisection between 
 * integers and objects having as a requirement that the action must not be an Int.
 * A tagged exit point is entered as: error_handler:tag
 * Again there is no clash under the condition to have action!=tag
 * Syntax for named leaves:
 * word0  :{ ~ label exitPoint word1 word2 ... wordn }
 * word2  :{ word10 word11 ... word1n ... }
 * word1n :{ word20 word21 ... label leave ... }
 * 
 * The above syntax allows word1n to exit at the level before word0.
 * 
 * The syntax "1 leave" allows to leave by 1 level
 * The syntax "2 leave" allows to leave by 2 levels etc.
 * The syntax "n leaveRetry" allows to leave by n levels and reexecute the same instruction executed the last time.
 * 
 * Unwinding the stack means executing the deallocators etc.
 * 
 * 
*/ 
	

	bool mustContinue(){return true;}

	void threadSetAlive(std::thread& th){
		setAlive(threadVar(th,stacks));
		setAlive(threadVar(th,exS));
		setAlive(threadVar(th,trackedS));
	}

	bool debugNext(Aggregate& a,Word& n,Word& i){
		if(debugLevel>=nestLevel && a){
l:		print(toString(a,tsAlsoName,i));
			showStack();
			tString s=getLine();
			for(obj c:s){
				switch(tolower(c.w)){
					case 'e':
						if(debugLevel<=nestLevel) ++debugLevel;break;
					case 'n':
						break;
					case 'x':
						--debugLevel;break;
					case 'b':
						if(i)--i;goto l;
					default:
						break;
				}
			}
		}
		return true;
	}
/*
	void ET::execute000(obj o,int d,bool forceFirst,int debugLevel) {
	// executes the object. If the object is a secondary (aggregate) and forceFirst is true then the secondary 
	// is executed regardless of executionLevel.
		Int i,n,nestLevel=0;Aggregate a{0};bool secondaryLoaded=false;obj leave{0};
	// ATTENTION on "Ap" the size of aP could be different than Word !!! something different must be done.

	// operator precedence is High:< mid:&& Low:||  Lazy evaluation is guaranted to happen.
	// The expression E.counter-- is overloaded

l0:	try{
		while(true){
			if(leave) goto l1;
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
			if(i<n && leave==0 ){	
				if(debugLevel>=nestLevel)
					debugNext(a,n,i);
				stopETs.wait();
				o=a[i++];
			}else{
l1:			if(d>=exS.elements()) return;
				if(!leave.isInt()){
				}else{
					int N=MAX((int)leave,1);leave=0;
					while(exS.elements()>d && N>0){
						i=exS.pop();a=((Aggregate)exS.pop());--nestLevel;if(debugLevel>nestLevel) debugLevel=nestLevel;
						if(isNumber(i))N--;else{stack().push(a);execute(i,true);}
					}
					if(N) LOG("Unexpected N!= in execute0");
					if((leave=N)==0 && a.w!=0)goto l;else return; 
				}
			}		
		}
		}catch(obj o){
			leave=o;goto l0;
		}
	}
*/

	int getExecutionLevel(obj o,bool setIt){
		int l=0;
		if(o.isInstanceOf(classAggregate)){
			Aggregate a=(Aggregate)o.w;
			for(int i=0;i<a.elements();i++) l=MAX(l,getExecutionLevel((a[i])));
			if(setIt) o.executionLevel(l);
		}else{
			l=o.executionLevel();
		}
		return l;
	}
	void setExecutionLevel(){std::wcout<<L"level:"<<getExecutionLevel(stack().tos(),true)<<std::endl;}

	// curlyOpen is executed after leaving on the stack two parameters: closeAction level
	// closeAction is left on the stack in order to be used by curlyClose. That action is executed after 
	// curlyClose is executed.

	
	PRIMITIVE curlyOpen(/* Token closeAction */ uWord level){
		// The stack contains the Token of closeAction too but it isn't used.
		if(level==execution.l){
			paren++;
			stack().drop();
			newStack(0);
		}else if(level<execution.l) stack().push2(level,curlyOpenT);
		else{
			stack().push2(execution.l,paren.l);
			execution=level;counter=0;paren=1;
			newStack(0);
		}
	}


	PRIMITIVE curlyClose(uWord level){
		Token t,closeAction;assert0(level<=execution.l,"closing curly level higher than current level");
		if(level==execution.l){
			if(--paren>0){
				oldStack();
			}else{
				t=closeStack();paren=stack().pop();execution=stack().pop();
	//			stack().pop2((Token&)E.level(),(Token&)E.paren());
				closeAction=stack().pop();stack().push(t);execute(closeAction,true);
			}
		}else{ // the level is less than the current level, the instruction is compiled
			stack().push2(level,curlyCloseT);
		}
	}

#include "stacks.h"
#include "controlStructures.cpp"
#include "patternMatchingGeneric.cpp"
// #include "patternMatchingSinglePos.cpp"
#include "patternMatchingMultiPos.cpp"
#include "interpreter.cpp"

#include "methods.cpp"

	void executeMethod(Obj o){
		const Aggregate& a=*(Aggregate*)&o;
		for(uWord i=0;i<a.elements();i+=2) 
			if(methodActionApplicable((obj)a[i])){
				execute(a[i+1],true);return;
		}
		stack().push(a);// else continue texpectedo passive behaviour	
		return;
	};

void execute(obj o,bool forceFirst) {
	execute0(o,forceFirst);
}

void executeNow(obj o){
	backup<level> t(execution,1);
	execute0(o,true);
}


bool isSingleCode(char c){return 240<=c & c<=250;}
bool isDoubleCode(char c){return 251<=c & c<=254;}

void clearTelnetCodes(char* mbs,ssize_t &n){
	for(ssize_t i=0;i<n;){
		if(mbs[i]==0xFF){
			if(i+3<=n && isDoubleCode(mbs[i+1])){memmove(mbs+i,mbs+i+3,n-3-i);n-=3;continue;}
			if(i+2<=n && isSingleCode(mbs[i+1])){memmove(mbs+i,mbs+i+2,n-2-i);n-=2;continue;}
		}
		else i++;
	}
}

inline bool wouldBlock(){
#if EAGAIN==EWOULDBLOCKread
	return errno==EAGAIN;
#else
	return errno==EAGAIN || errno==EWOULDBLOCK;
#endif
}

String getLine(){
	const bool rl=false;
	if(rl){
		char* mbs=readline((char*)"> ");
		if(!mbs)
			return emptyString;
		else{
			add_history(mbs);
				String r=::newString(mbs);r.push('\n');
			free(mbs);
			return r;
		}
	}else{ 
		// each line is supposed < in length than 0x1000.
		static int cnt=0;
		char mbs[0x200];ssize_t n=0;
#ifndef ndebug
		memset(&mbs,0xAB,sizeof(mbs));
#endif
#define fileError 12
		if( true ){ //&ETs[0]==this || cnt>=10000000){
			for(int i=0x10;--i>=0 & n==0;){
				while(-1==(n=::read(et::in,mbs,sizeof(mbs)-1)))
					if(wouldBlock()) stopETs.wait();
				// if we arrive here it means that n is 0 or more
				// bellow is a delay in order to get delayed characters eventually available.
					usleep(100000);
					if(n!=0){
						int n1=::read(et::in,mbs+n,sizeof(mbs)-1-n);if(n1!=-1) n+=n1;
					}
					clearTelnetCodes(mbs,n);
			}
		}else{
			stopETs.wait();
			const char* cmd="`bello' `brutto' | drop \n";
			n=strlen(cmd);
			memcpy(mbs,cmd,n);
		}

		String r=::newString(mbs,n);
		return r;
	}
}

