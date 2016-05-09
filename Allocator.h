const Token listEnd{-1};

__attribute__((init_priority(101))) atomicToken freeObj{listEnd};
__attribute__((init_priority(101))) atomicToken freeClass{listEnd};

// the vectors and heap zone is defined as a static memory in order to have a constant address for it.
// the heap zone "mm" is situated at the beginning, the end of that zone is kept in register mm.
// at the address of register mm starts the vector zone. Everything is addressed relatively to mm.

//constexpr agu* mm=mm_+mmSize;

#if true
//	constexpr agu* mm=heap.mm_+mmSize;
//	constexpr vect* vv=heap.vv_;
#else
	register agu* mm asm("%r15"); 
	__attribute(( constructor(102) )) void mmInit(){mm=mm_+mmSize;}
#endif

std::atomic<addrT> freePos(mmSize);

//vect vv[vvElements]=((vect*)mm);

//#define vv ((vect*)mm)
//constexpr auto vv=(vect*)mm;

inline vect& i2V(uWord i){return M.vv[i];}
inline vect& t2V(Token t){
	return i2V(t2i(t));
}
uWord O2i(const obj o){return t2i(o.w);}
uWord O2t(const obj o){return i2t(O2i(o));}

size_t freeVectors(atomicToken& freeo,bool clearCollect=false);

