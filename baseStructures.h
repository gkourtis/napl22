// each object body in the heap, has at the beginning the token related to that object.
// from the token we may get it's class and from the class every characteristic of the object.
// When the body is freed the token t is replaced and holds the total size fredd.
// Because the compressor scans that memory area when it finds an integer interprets it as a 
// non signed number. 

// An object is referenced by a token that is an index in the vectors table.
// So the object is vector+body. The body has a head+tail ( the tail contains the actual data ).
// The head starts with a backreference to the vector.
// If the object belongs to a fixed size class then the class holds the size of the object. 
// A special value should be hold in that field if the size instead of being fixed is variable.
// So the compressor may find in any case the body size (head+tail).
// A size of 0 isn't acceptable. The size includes the size of the initial Token.

typedef uWord agusz_t; 

struct hGeneric{

	Token token;
	hGeneric(Token t):token{t}{}
	// is used to hodl the size in agu's of the head,tail of body.
	static const agusz_t sizeofownHeader;
// The generic body has a null size for it's data. The above may be overridden by derivative classes
//	const sz_t sizeofData(){return 0;}; /* gk 20160214 sizeT -> size_t */
};
const agusz_t hGeneric::sizeofownHeader=sizeof(hGeneric);


template <typename T,typename sizeT,sizeT sz=1> 
// T is the type of the object contained in the tail,sz is the static number of such objects
// The hFixBase during definition takes as parameter both: a) the type of holdeed data "T" and the 
// "sizeT" type that is the holder for the size of data.
struct hFixBase:hGeneric{

	typedef T el_t;typedef sizeT sz_t;static const sz_t size=sz;

	hFixBase(Token t):hGeneric{t}{};
	typedef el_t dt_t[size];
// the dataP() returns the first address to the contained data
	T* dataP(){return (T*)(this+1);}
// the data(i) returns a reference to the i-th datum
	T& data(uWord i){return *(i+(T*)(this+1));}
// the sizeofData() returns the whole size of the contained data
	constexpr agusz_t sizeofData(){return sizeof(dt_t);}
	static const bool fixSize=true;
};

// the usual hFix header is defined as holding "Word" and having a type "uWord" as the holder for the size of data.
template <typename T, uWord sz=1> using hFix_=hFixBase<T,uWord,sz>;
//using hFix=hFixBase<Word,uWord>;
using hFix=hFix_<Word>;


// the hVarBase needs a type for the holded data "T", needs a type for the variable holding the number of holded data "sizeT".
template <typename T,typename sizeT> 
struct hVarBase_:hGeneric{

	typedef T el_t;typedef sizeT sz_t;
	
	sz_t size;
	
// the meaning of the 3 bellow functions is the same as in the hFixBase.
	T* dataP(){return (T*)(this+1);}
	T& data(uWord i){return *(i+(T*)(this+1));}
	agusz_t sizeofData(){return sizeof(T)*size;};
	static const bool fixSize=false;
	hVarBase_(Token t,sizeT sz):hGeneric(t),size(sz){}
	static const agusz_t sizeofownHeader;
};

template <typename T,typename sizeT> 
const agusz_t hVarBase_<T,sizeT>::sizeofownHeader=sizeof(hVarBase_<T,sizeT>);


template<typename T> using hVarBase=hVarBase_<T,uWord>;
template<typename T> using hVarBaseS=hVarBase_<T,suWord>;

// an aggregate is a variable size array with a pointer to the first free element.
// the size of the body is determined from the maxn field.

// Size of structures doesn't works correctly when data is used.
// the size is shown as bigger.

template <typename T,typename sizeT> 
struct hAggregateBase_:hVarBase_<T,sizeT>{
	typedef T el_t;typedef sizeT sz_t;
	sz_t n;
	T* dataP(){return (T*)(this+1);}
	T& data(uWord i){return *(i+(T*)(this+1));}
	//hAggregateBase_(Token t,sizeT sz,sizeT n0=0):hVarBase_<T,sizeT>{t,sz}{};  // must be deleted if found that isn't needed
	hAggregateBase_(Token t,sz_t sz=4,sz_t n0=0):hVarBase_<T,sz_t>{t,sz},n{n0}{};
	static const size_t sizeofownHeader;
	static const size_t max_size,max_sizeofData,max_sizeofBody;
};

#define U hAggregateBase_<T,sizeT>
#define TE template<typename T,typename sizeT> 
#define assConst(constName,constValue) TE const size_t U::constName=constValue;

	assConst(sizeofownHeader,sizeof(U));
	assConst(max_size,MIN(std::numeric_limits<sz_t>::max(),(std::numeric_limits<uWord>::max()-sizeofownHeader)/sizeof(el_t)));
	assConst(max_sizeofData,sizeof(el_t)*max_size);
	assConst(max_sizeofBody,sizeofownHeader+max_sizeofData);

#undef U
#undef TE
#undef assConst

// bellow we have two kind of aggregares with different kind of possible maximum size for the elements.

/*
template<typename T> using hAggregateBase=hAggregateBase_<T,uWord>;
template<typename T> using hAggregateBaseS=hAggregateBase_<T,suWord>;
*/

using hAggregateS=hAggregateBase_<Word,suWord>;
using hAggregateL=hAggregateBase_<Word,uWord>;

//using hAggregate=hAggregateL;
using hAggregate=hAggregateBase_<Word,uWord>;


// the queue is not chekced or correctly implemented, must be developed further !!! Not to be used as is actually !!!
template <typename T,typename sizeT>
struct hQueueBase_:hAggregateBase_<T,sizeT> {

	typedef T el_t;
	typedef sizeT sz_t;

	sizeT nStart;
	T* dataP(){return (T*)(this+1);}
	T& data(uWord i){return *(i+(T*)(this+1));}
};

/*
A document describing the prototype implementation must be included here or refered at.
In little words a prototype is a structure in the sense of C.
That means that a structure is something that has a list of field names with associated values.
In the simplest case a structure has it's names and values all inside the structure.
In the generic case, some names reside in an anchestor from were the structure inherits.
So there are structures that have more names than values, so in that case names are to be used from descendant structures.
*/

// It seems that prototype as defined isn't actually used as such so it is commented 

/*

template <typename T,typename sizeT> 
struct hPrototypeBase_:hAggregateBase_<T,sizeT>{
	sizeT fieldsNum,levelsNum;
	
	T* values(){return (T*)(this+1);}
	T* fields(){return (T*)(this+1)+this->n;}
	T* levels(){return (T*)(this+1)+this->n+fieldsNum;}

	T* dataP(){return (T*)(this+1);}
	T& data(uWord i){return *(i+(T*)(this+1));}
	
};

//using hPrototype=hPrototypeBase_<uint8_t,Word>;
//using hPrototype=hPrototypeBase_<uint16_t,Word>;

using hPrototype=hPrototypeBase_<Word,suWord>;
using hPrototypeL=hPrototypeBase_<Word,uWord>;

*/

template<typename T> bool same(const T& a,const T& b){return &a==&b;}
