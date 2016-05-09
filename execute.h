Token curlyOpenT,curlyCloseT;

#include "level.h"
#include "thread_stack.cpp"

std::vector<std::thread> threads;

std::atomic_bool abandon;

namespace et{
thread_local 					int in,out;
thread_local 					FILE *sin=nullptr,*sout=nullptr;
}

thread_local 					Aggregate stacks,exS,trackedS;

thread_local level 		execution=1,paren=0,counter=0,nestLevel=0,debugLevel=-1;
thread_local Int 			iCounter=0;

thread_local level 		charLevel=0,charCounter=0;

thread_local Char 		delim=0;

thread_local Word 		markLevel=0;

thread_local bool 		exitInterpretLoop=false;

thread_local 					Aggregate sec{0};

thread_local 					Word sec_ip=0; // sec_ip should be defined as number althought not used because unNest uses it.

thread_local 					volatile bool threadEnded;


void threadSetAlive();
Aggregate stack() {return stacks.tos();}

typedef void connectionHandler_t(size_t,std::atomic_bool*);

#include "stacks.h"

void interpretLoop();

// the code bellow is done inside each single task
void initializeExecuteThread(size_t ind,std::atomic_bool *readyP,int in_=-1,int out_=-1){
	while(!*readyP) 
		usleep(0);
	*readyP=false;
	
	et::in=in_;et::out=out_;
	++activeETs;
#ifdef threadStack
	processorStackDataInit();
#endif
	if(in_==-1) in_=0;if(out_==-1) out_=et::in;
	et::sin=fdopen(et::in,"r");et::sout=fdopen(et::out,"w");
	newStack(0);
	puts("New Execution Thread created and running");
		interpretLoop();
	puts("..connection handler closed");
	fclose(et::sin);fclose(et::sout);
	threadEnded=true;--activeETs;
	usleep(1000000);
}

// the code bellow is done from the main process
int newThread(int in,int out){
	int i=threads.size();
	static std::atomic_bool ready{false};
		threads.emplace_back(&initializeExecuteThread,i,&ready,in,out);
	ready=true;
	while(ready);
	return i;
}

int write(const char*s, size_t sz){
	return ::write(et::out,s,sz);
}

int write(const char*s){
	return write(s,strlen(s));
}

int write(std::stringstream& ss){
	return write(ss.str().c_str(),ss.str().size());
}
	
int write(String s){
	int r;
	if(!s.isInstanceOf(classString))
		r=write("write got an object not an instance of classString");
	else{
		char* p=s.mbs();
		if(p){
			r=write(p);delete[] p;
		}else{
			r=write("mbs didn't returned a valid mbs pointer");
		}
	}
	return r;
}

void puts(){
	write("\n");
}
	
template<typename T,typename ...TT>
void puts(T s, TT... ss){
	write(s);puts(ss...);
}
			
bool normal(),mustContinue();
void execute(Obj o,bool isSecondaryIgnoreExecutionLevel=false);
void execute0(Obj o,bool isSecondaryIgnoreExecutionLevel=false);
void executeNow(Obj o);

	
//void execute00(obj o,bool isSecondaryIgnoreExecutionLevel=false);
bool executeSingle(obj o);
bool executeSecondary(Aggregate a,bool continously=false);
	
void nest(Aggregate a);
bool unNest();

void showStack(std::stringstream& ss);
void showStack();
void htmlStack(std::stringstream& ss);
void htmlStack();

	
int print(obj);
bool debugNext(Aggregate& a,Word& n,Word& i);
int getExecutionLevel(obj o,bool setIt=false);
void setExecutionLevel();
void curlyOpen(uWord level),curlyClose(uWord level);
String getLine();
void* connection_handler();
	
#include "controlStructures.h"
#include "patternMatching.h"
#include "interpreter.h"
#include "methods.h"

void executeMethod(Obj o);
template<typename... TT> void print(TT... tt);

void track(Token t){
	if(trackedS.w) trackedS.push(t);
}

void untrack(){
	trackedS.pop();
}


