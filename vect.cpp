	inline ptr vect::addr(const void* p){
		addr0=(ptr)p-(ptr)M.vv;
		return (ptr)p;
	}
		
	inline addrT vect::addr(const addrT q){
		addrT r=addr0;addr0=q;return r;
	}
	
	inline vect::operator addrT(){
		return addr0;
	}

	vect::operator ptr(){
		return (ptr)M.mm()+addr0;
	}
	
