static inline size_t freeMem(){
// As said in the "Allocator0.h" file, the freeMemory should be considerd all but the last available byte.
// that is because due to the unsigned arithetic used when freePos is 0 ( that should happen when the memory
// is exausted, the addition of freePos to the initialAddress doesn't gives us the correct address.
// But anyway when the memory is exausted due to the last allocation freePos becomes 0 but will never refer to 
// an address as no allocation may happen when freePos is 0.
// On the other side the allocated memory may never be all the possible one ( e.g. 0x10000 ) as in that case
// the first freePos would be 0 but no way of differentiation between an empty memory with freePos=0 and 
// a vergin memory with freePos=0
	return -freePos;
}

#define castAs(a,type) (*(type*)(&a))

// the address holding the next free is fetched and replaced with -1.
// if it's content was !=-1 then we continue and fetch that address contents
// if not the system waits for a return to a value !=-1;

inline obj& nextFree(Obj& o){return *(obj*)(M.vv+t2i(o.w));}

void release0(atomicToken& o0,obj o,bool lock=true){
	#ifndef NDEBUG
		memset(&o.V(),0xFF,sizeof(vect));
	#endif
	if(lock){
		stopETs.lock();
			nextFree(o)=o0;o0=o;
		stopETs.unlock();
	}else{
		nextFree(o)=o0;o0=o;
	}
}

obj reserve1(atomicToken& o0){
// the value 0 is used in order to lock other processes a special value listEnd!=0 is used to signal the end of list.
		obj r=0;
l0:	for(int i=8;i--!=0;)
			if( (r=o0.exchange(r)) ) break;
		wait();goto l0;	
		if(r.w==listEnd){
			//r.w=garbageCollect(M);
			if(r.w==listEnd)
			THROW("vectors exhausted")
	}
	o0=nextFree(r);
	#ifndef NDEBUG
		memset(&r.V(),0x0,sizeof(vect));
	#endif
	return r;
}

Obj reserve(atomicToken& o0){
	stopETs.lock();
l:	obj r=o0.load();
		if(r.w==listEnd){
			garbageCollect(M);
			assert0(o0.load()!=listEnd,"No more vectors available");goto l;
		}
		o0=nextFree(r);
	stopETs.unlock();
	#ifndef NDEBUG
		memset(&r.V(),0x0,sizeof(vect));
	#endif
	return r;
}

size_t freeVectors(atomicToken& freeo,bool clearCollect){
	size_t r=0;
	if(clearCollect)
		for(obj o=freeo.load();o!=listEnd;++r,o=nextFree(o))
			((obj)o)->collect=false;
	else
		for(obj o=freeo.load();o!=listEnd;o=nextFree(o),++r);
	return r;
}

size_t freeVectorsLU(atomicToken& freeo){
	size_t r=0;
	stopETs.lock();
		r=freeVectors(freeo);
	stopETs.unlock();
	return r;
}

void release(Obj o,bool lock=true){
	if(t2i(o.w)<executionClassesNum)
		release0(freeClass,o,lock);
	else
		release0(freeObj,o,lock);
}

inline obj reserveClass(){return reserve(freeClass);}
inline obj reserveObj(){return reserve(freeObj);}
//inline obj reserveReleaseClass(){obj r=reserveClass();release(r);return r;}

void garbageCollect();

static inline addrT allocate0(addrT sz);

/*
obj newClass(bool hasClassInfo,uWord extraSz=0){
	constexpr int sizeInfoSz=roundUpDivision(sizeof(classInfo0),sizeof(Word));
	obj o=reserveClass(); // reserve vectors for defined classes
	o.Class(classClass);
	o.executionLevel(0);
	new (o->addr(M.mm()+allocate0(sizeof(classInfo)))) classInfo(o.w,sizeInfoSz+extraSz,sizeInfoSz+extraSz);
	return o;
}
*/ 

Obj classClass_=reserveClass();


// because the allocator is a template structure, and because the request of C++ is to have the 
// identity of allocator between different cases, we cannot use static variables inside the allocator
// because these would be different for each different template argument.

void __attribute__((constructor(102))) initializeAllocator(){
// reinitialization should be added with the possibility to have vv and mm not = to nullptr;
//	mm=(ptr)new agu[sizeof(obj[vvSize])+sizeof(agu[mmSize])];
//mm=mm_+mmSize;
//std::cout.hex;
std::cout<<std::endl;
std::cout<<"initializeAllocator started"<<" mm:"<<std::hex<<(void*)M.mm()<<std::endl;
#ifndef NDEBUG
	memset((void*)M.mmStart,0xAA,mmSize);
	memset((void*)M.vv,0xBB,vvSize);
#endif			
	//freePos=mmStart;
	freePos=-mmSize;

	for(uWord i=vvElements;i--!=0;)
		release(i2t(i),false);
	std::cout<<"Freevectors:"<<freeVectors(freeObj,false)<<std::endl;
#ifndef NDEBUG
	assert0(sizeof(classInfo)%sizeof(Word)==0,
	"problem in initializeAllocator size of classInfo not multiple of sizeof Word"
	);
#endif
	
/*	
	addrT sizeInfoSz=countof(classInfos_);
	for(Word i=_classClass;i!=_classEnd;i++){
		obj o=newClass(0);assert0(i==o.w,"Problem in initialize allocator");
	}
*/

#ifndef NDEBUG
std::cout<<"initializeAllocator ended Free memory="<<(int)freeMem()<<std::endl;
#endif

}

void compact();

static inline addrT allocate0(addrT sz){
// sz and freePos are unsigned values	
	stopETs.lock();
l:	addrT r=freePos;
		if(sz<(addrT)-freePos){
#ifndef NDEBUG
			memset(M.mm()+r,0x99,sz);
#endif
			freePos+=sz;
	stopETs.unlock();
			return r;
	}
	garbageCollect(M);compact(M);assert0(sz<=-freePos);goto l;
}		

static inline addrT deallocate0(addrT q,uWord n){
	*(uWord*)(M.mm()+q)=n;
#ifndef NDEBUG
	if(n>sizeof(uWord))
		memset( ((uWord*)(M.mm()+q))+1 , 0xF0 , n-sizeof(uWord) );
#endif
	return q;
}

struct Token_ptr{
	obj _obj;uWord _offset;

/*
	Token_ptr(void* p){
		_obj=*((Word*)(p)-1);
		if(!_obj.isToken()) 
			{_obj=*((Word*)(p)-4);_offset=4;}
		else 
		_offset=1;
	}
*/
	Token_ptr(obj o,uWord offset):_obj(o),_offset(offset){}
	operator void*(){
		assert0(_obj.w!=0,"Token_ptr problem");return (void*)_obj.bodyP();
	}
	Token_ptr& operator++(){_offset++;return *this;} // preincrement
	Token_ptr& operator--(){_offset--;return *this;} // predecrement
	Token_ptr operator++(int){Token_ptr r=*this;_offset++;return r;} // postincrement
	Token_ptr operator--(int){Token_ptr r=*this;_offset--;return r;} // postdecrement

	Token_ptr operator+(int d){return Token_ptr(_obj,_offset+d);}
	Token_ptr operator-(int d){return Token_ptr(_obj,_offset-d);}

	Word& operator*(){return _obj->hAggregateP()->data(_offset);}
		
	bool operator >(Token_ptr o1){return _offset>o1._offset;}
	bool operator >=(Token_ptr o1){return _offset>=o1._offset;}

	bool operator <(Token_ptr o1){return _offset<o1._offset;}
	bool operator <=(Token_ptr o1){return _offset<=o1._offset;}

};

bool operator==(const Token_ptr &p1,const Token_ptr &p2)
{ 
	return p1._offset==p2._offset && p1._obj==p2._obj;
};

bool operator!=(const Token_ptr &p1,const Token_ptr &p2){return !(p1==p2);}

/*
struct Token_ref:Token_ptr{
	Token_ptr 
	
	operator Word(){return vv(_t).hAggregateP->data[_o];}
	operator Char(){return Char(vv(_t).hAggregateP->data[_o]);}
};
*/

namespace std{
	template<> class iterator_traits<Token_ptr>{
	public:void xxx(){
}

		typedef Word difference_type;
		typedef Word value_type;
		typedef Token_ptr pointer;
		typedef Word& reference	;
		typedef bidirectional_iterator_tag iterator_category ;
	};

	template<> class iterator_traits<const Token_ptr>{
	public:
		typedef Word difference_type;
		typedef Word value_type;
		typedef const Token_ptr pointer;
		typedef const Word& reference	;
		typedef bidirectional_iterator_tag iterator_category ;
	};
}

struct Token_iterator:
std::iterator<
	std::bidirectional_iterator_tag,
	Word,
	Token_ptr,
	Word&
> 
,Token_ptr
{Token_iterator(Token t,Word o):Token_ptr(t,o){}};

/*
template<> std::iterator_traits::<Token_ptr<Word>>{
	difference_type	Iterator::difference_type
	value_type	Iterator::value_type
	pointer	Iterator::pointer
	reference	Iterator::reference
	iterator_category	Iterator::iterator_category
}
*/



template <typename T>
class Allocator {
// http://www.codeprvoid xxx(){
public:

	typedef T value_type; 
	typedef addrT size_type;typedef addrT difference_type;
	
	typedef Token_ptr pointer; typedef const Token_ptr const_pointer;
	typedef Word& reference; typedef const Word& const_reference;
	
//	typedef T* pointer; typedef const T* const_pointer;
//	typedef T& reference; typedef const T& const_reference; 
	template<typename U> struct rebind{typedef Allocator<U> other;};

	// The initialization of freePos and freeT is done independently of the class definition because
	// according to c++ conventions, the creation of another instance of the allocator class with another <T>
	// has to work in the same memory and not reinitialize the allocator.
  Allocator() throw(){}
  //typedef true_type propagate_on_container_move_assignment;
  Allocator(const Allocator& a) throw(){}
	~Allocator() throw(){} 
  template<typename U>Allocator(const Allocator<U>&) throw() {}
	template<typename U> Allocator(U){}
	

	static inline pointer allocate(size_type totalSz){
		obj o=reserveObj();
		o.bodyP(allocate0(totalSz));
		o->hGenericP()->token=o;
		return Token_ptr(o,0);
	}
	static void deallocate(pointer p,size_type n){
		deallocate0(p._obj.bodyP(),n);
		release(p._obj);
	}
	inline const size_type max_size() {return std::numeric_limits<size_type>::max()/sizeof(T);}
	void construct(pointer p, const T& t){}
	void destroy(){};void destroy(T*) {}
};

template <typename T> bool operator==(Allocator<T> const &, Allocator<T> const &) {return true;}
template <typename T> bool operator!=(Allocator<T> const &, Allocator<T> const &) {return false;}

// the char set used by the system is supposed to be the utf8 char set
// So the functions: a) identity b) mbrtoc16 c) mbrtc32 convert to utf8 utf16 utf32 if the analogous flags are defined
// Inversely the functions: a) identity b) c16rtomb c) c32rtomb convert the other way out.


