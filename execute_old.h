struct level{
	Word l;
	level(Word i):l{i}{};
	level():l{0}{};
	operator Word(){return l;}
//both postfix operators must take a dummy int parameter in order to differentiate from the prefix increment
	Word operator--(int){return l ? l-- : l;} 
	Word operator++(int){return l++;}										 
// prefix operators don't
	Word& operator--(){return l ? --l : l;}
	Word& operator++(){return ++l;}
	
	level operator<<(level lev){
		if(l>lev.l) l=lev.l;return *this;
	};
};

Token curlyOpenT,curlyCloseT;

struct ET : std::thread {
	#include "thread_stack.cpp"
	size_t index;int in,out;FILE *sin=nullptr,*sout=nullptr;
	Aggregate stacks,exS,trackedS;
	void setAlive();
	Aggregate stack() const{return stacks.tos();}
	level execution=1,paren=0,counter=0,nestLevel=0,debugLevel=-1;Int iCounter;
	level charLevel=0,charCounter=0;Char delim=0;
	Word markLevel=0;
	bool exitInterpretLoop=false;
	Aggregate sec{0};Word sec_ip=0; // sec_ip should be defined as number althought not used because unNest uses it.
	
	volatile bool threadEnded;

	typedef void connectionHandler_t(size_t,std::atomic_bool*);
	ET(connectionHandler_t f,size_t ind,std::atomic_bool *readyP,int in_=-1,int out_=-1)
	:std::thread{f,ind,readyP}, in{in_}, out{out_}, index{ind}{
		++activeETs;
#ifdef threadStack
	processorStackDataInit();
#endif
// The code bellow initializes the processorStack and processorStackSize variables		
		std::cout<<" native_handle="<<native_handle()<<" pthread_self"<<pthread_self()<<std::endl;
		
		if(in_==-1) in_=0;if(out_==-1) out_=in;sin=fdopen(in,"r");sout=fdopen(out,"w");
		newStack(0);puts("New Execution Thread created and running");
	}

	void ETclose(){
		fclose(sin);fclose(sout);
		threadEnded=true;
		--activeETs;
	}
	
	int write(const char*s, size_t sz){
		int out1;
		if(etP)
			out1=out;
		else
			out1=1;
		return ::write(out1,s,sz);
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
	
	template<typename T,typename ...TT>
	void puts(T s, TT... ss){
		write(s);puts(ss...);
	}
	
	void puts(){
		write("\n");
	}
			
	bool normal(),mustContinue();
	void execute(obj o,bool isSecondaryIgnoreExecutionLevel=false);
	void execute0(obj o,bool isSecondaryIgnoreExecutionLevel=false);
	
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
	void curlyOpen(uWord level);
	void curlyClose(uint level);
	String getLine();
	void* connection_handler();
	
#include "stacks.h"
#include "controlStructures.h"
#include "patternMatching.h"
#include "interpreter.h"
#include "methods.h"

void executeMethod(Obj o);
template<typename... TT> void print(TT... tt);

};

void track(Token t){
	if(etP && etP->trackedS.w) etP->trackedS.push(t);
}

void untrack(){
	if(etP) etP->trackedS.pop();
}

std::vector<ET> ETs;

std::atomic_bool abandon;

