void semaphore::lock(){
//	if(!etP) return;
	level i; // level has -- overloaded in order to not drop bellow 0.
	syncro tt; // the value given is irrelevant dut to the compare_exchange_strong 
							// than anyway returns current value or returns when value was ==.
	i=4;
	while(!ss.compare_exchange_strong(tt,{true,tt.waiting+(uWord)1})){ 
		if(--i==0) usleep(0);continue;
	}
	if(tt.locked){ // if the semaphore was already locked than wait
		i=4;
		do{
			tt.locked=false; // the condition we wait for is locked=false
			if(--i==0) usleep(0);
		}while(!ss.compare_exchange_strong(tt,{true,tt.waiting}));
	}
// if was !tt.locked we have set it to locked and we may exit here
}

void semaphore::unlock(){
//	if(!etP) return;
	syncro tt;
	while(!ss.compare_exchange_strong(tt,{false,tt.waiting-(uWord)1}));
	if(!tt.locked) 
		print("Problem in unlock,should be locked and isn't");
//	print(" waiting:",tt.waiting," locked",bool(tt.locked));
}

void semaphore::wait(){
	if(locked()){
		lock();unlock();
	}
}

