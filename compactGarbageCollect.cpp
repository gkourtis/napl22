//bool isStackAddress(ptr p){const int m=0xFF000000; return (int(p) & m) == m;}

#define clock std::chrono::high_resolution_clock

class setAlive;
bool isSize(size_t sz){
	size_t x=hAggregate::max_sizeofBody;
	return sz<hAggregate::max_sizeofBody;
}

void checkforNullAddr(){
	for(size_t i=0,c=0;i<vvElements || (print(" with null addr:",c),false);i++){
		obj o(i2t(i));if(o->collect && o->addr0==0) 
			c++;
	}
}
// The locic besides garbage collection is the subsequent:
// Each aggregate, should point to at least 2 elements.
// If an aggregate contains only 1 pointed element than the operation is optimized
// and no stack frame is generated.
// The above thoughts, tell us that the necessary stack depth for tracking the 
// garbage collection position is log² vectorsNum.
// So if that space is preallocated, it will never serve a bigger stack to track the 
// garbage collection process.


void checkCompact(heap& H,bool f){
	ptr s,s0;agusz_t sz;bool b;
	obj o,o0;
	
	for(s=H.mmStart;s<H.mm()+freePos;o0=o){
		
		o=((hGeneric*)s)->token;
		if(o.isToken()){
			sz=o.sizeofBody();
			if(f) std::cout<<std::hex<<"addr:"<<(long)(s-H.mmStart)<<" token:"<<o.w<<" sz:"<<std::dec<<o.sizeofBody()<<std::endl;
			s0=s;o0=o;
			s+=sz;
		}
		else
			if(isSize(o.w)){
				sz=o.w;s+=sz;
				if(f) std::cout<<"addr:"<<std::hex<<(long)(s-H.mmStart)<<" sz:"<<std::dec<<sz<<std::endl;
			}else{
				std::cout<<"WrongSz:"<<std::dec<<o.w<<std::endl;
			}
	}
	if(s!=H.mm()+freePos) 
		std::cout<<"Check compact s!=mm+freePos"; 
	else 
		std::cout<<"Check compact OK ! ";
}

void compact(heap& H,bool wait){
	if(wait) waitAllStopped();
	print("Compact...");
	auto t1=clock::now();
	ptr s,d;size_t sz;bool b;
	for(s=d=H.mmStart;s<H.mm()+freePos;){
		obj o=((hGeneric*)s)->token;
		if(o.isToken()) {
			sz=o.sizeofBody();
			if(s!=d) {
				memmove(d,s,sz);
				o.bodyP(d);
			}
			s+=sz,d+=sz;
		}
		else 
			if(isSize(o.w)) 
				s+=o.w;
			else 
				assert0(false,"unexpected in compact");
	}
	assert0(H.mm()+freePos==s,"unexpected freePos!=s in compact");
	freePos=d-H.mm();
#ifndef NDEBUG
	memset(d,0xAA,freeMem());
	//signalFreePos();
#endif
	auto t2=clock::now();
	auto microseconds=std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count();
	
	//double dt=t*clock::period::N/clock::period::D;
	print(" compacted in microseconds:",microseconds);

}

// The locic besides garbage collection is the subsequent:
// Each aggregate, should point to at least 2 elements.
// If an aggregate contains only 1 pointed element than the operation is optimized
// and no stack frame is generated.
// The above thoughts, tell us that the necessary stack depth for tracking the 
// garbage collection position is log² vectorsNum.
// So if that space is preallocated, it will never serve a bigger stack to track the 
// garbage collection process.

void setAlive(obj d){
	Aggregate& A=*(Aggregate*)&d;
// The locic besides garbage collection is the subsequent:
// Each aggregate, should point to at least 2 elements.
// If an aggregate contains only 1 pointed element than the operation should be optimized and no stack frame 
// shoudl be generated. 
// The above thoughts, tell us that the necessary stack depth for tracking the 
// garbage collection position is log² vectorsNum.
// So if that space is preallocated, it will never serve a bigger stack to track the 
// garbage collection process.

	start:
	
	if(d.isToken() && d->collect ){
			
		d->collect=false;
		
		obj c=d.Class();setAlive(c);
		if(c.w==0)
			std::cout<<"c==0";
		
		SWITCH(c)
			CASE(classClass,classUndefined,classPrimitive,classInt,classFloat,classDouble,classInt64,classBitString) 
				return;
			DEFAULT
				//if(classString>=c) return; // a classString could anyway contain an object. So it is wrong to write that line
				if(	classAggregate>=c || d->isclassInfo){
					uWord n=A.elements();
					if(n==0) return;
					
// the code bellow implements tail optimization and is necessary only for function stack depth preservation according to previous comments
					uWord i;
					for(i=0;i+1<n;i++)
#ifdef threadStack
						if(etp)
							setAlive(etp->tokenOfStackIndex(A[i]));
						else
#endif
							setAlive(A[i]);
#ifdef threadStack
						if(etp)
							d=etp->tokenOfStackIndex(A[i++]);
						else
#endif
							d=A[i++];
						goto start;
				}
				if(c.Class()==classClassInfo){ // means that it is a classInfo 
					setAlive(c.Class());
					return;
				} 
				assert0(false,"ProblemIn_switch_during_garbageCollect");
		endSWITCH
	}
}



void dictionarySetAlive(Aggregate d,int cols=2){
	for(uWord i=0;i<d.elements();i+=cols){
		obj o=d[i+1];
		if(!o.isInstanceOf(classUndefined)) setAlive(o);
	}
	for(uWord i=0;i<d.elements();i+=cols){
		obj o=d[i+1];
		if(!o->collect)
			setAlive(d[i]);
		else{
			d.ins_backdel(i+cols,-cols);
			i-=cols;
		}
	}
}

using namespace std::chrono;
high_resolution_clock::duration::rep benchMicroSeconds(high_resolution_clock::time_point t){
	return duration_cast<microseconds>(high_resolution_clock::now()-t).count();
}

auto Now(){return std::chrono::high_resolution_clock::now();}

void cloneMemory(){
	auto t=clock::now();
		M1=M;
	print("clone ",sizeof(M),"bytes memory in ",benchMicroSeconds(t),"_microSeconds");
}
	
	
	
	
void garbageCollect(heap& H){
	waitAllStopped();
	cloneMemory();
	std::cout<<"\ngarbageCollect started";
	auto t1=clock::now();
	for(auto p=&H.vv[0];p!=&H.vv[vvElements];p++) p->collect=true;
	
	print(" vvElements=",vvElements);
	print(" freeClasses=",freeVectors(freeClass,true));
	print(" freeVectors=",freeVectors(freeObj,true));

	checkforNullAddr();

	// The code bellow kills all the ended tasks.
	for(auto i=0;i!=threads.size();++i){
		if( threadVar(threads[i],threadEnded) && threads[i].joinable() ){
			threads[i].join();
			threads.erase(threads.begin()+i);--i;
		}else
			threadSetAlive(threads[i]);
	}
	
	//dictionarySetAlive(dict);
	setAlive(dict);
	setAlive(persistents);
	
	for(auto& i:StringLitterals) setAlive(i.second);

	for(size_t i=0,c=0;i<vvElements || (print(" killed:",c),false);i++){
		Obj o=i2t(i);
		if(o->collect){
			c++;o.kill();
		}
	}
	
	auto t2=clock::now();
	auto microseconds=std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count();
	print("garbageCollect Ended in microseconds:",microseconds);
}
