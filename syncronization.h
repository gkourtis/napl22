std::atomic<uint> activeETs{0};

thread_local uint ll=0;

struct syncro{
	uWord locked:1;
	uWord waiting:(WordBits-1);
	//syncro():locked{0},waiting{0}{}
};

class semaphore{
	std::atomic<syncro> ss;
	void (*waitP)()=0;
// the current state is given and the state is inverted	
public:
	typedef void (*waitP_t)();
	semaphore(waitP_t w=nullptr):waitP{w}{syncro tt{false,0};ss.store(tt);}
		
	int waiting(){return ss.load().waiting;}
	bool locked(){return ss.load().locked;}
	void lock(),unlock(),wait();
	void store(bool l,uWord w){ss.store(syncro{l,w});}

};


semaphore stopETs;

void waitAllStopped(){
	bool b;uint i1,i2;
	do{
		i1=activeETs.load(),i2=stopETs.waiting();
	}while(i1!=i2);
}

