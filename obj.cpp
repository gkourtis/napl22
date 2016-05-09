Aggregate newPrototype();

obj __attribute__((init_priority(103))) classClass1=newPrototype();


inline obj::obj(Token t) noexcept : w(t){
#ifdef threadStack
	int d;
	if( (d=etP->processorStackIndex(this)) != -1 )
		etP->trackedS.push(d);
#endif
};


obj::~obj() noexcept{
#ifdef threadStack
	int d;
	if( (d=etP->processorStackIndex(this)) != -1 ){
		if(etP->trackedS.elements()==0)
			LOG("unexistent obj to pop")
		else	
			if(etP->trackedS.pop()!=d)
				LOG("obj destructor inconsistency")
	}
#endif
};


inline vect* obj::operator->() const {
	return &t2V(w);
}

vect& obj::V() const
{
	return t2V(w);
}

Word getSetW(Word &w,Word w1)
{
	Word w0=w;
	w=w1;
	return w0;
}

bool classGreaterOld(Obj ancestor,Obj descendant,bool alsoEqual=false)
{ obj f=descendant;
	if(!alsoEqual) goto l;
	while(f!=classClass_){
		assert0(f.w!=0,"problem in classGreater");
		if(f==ancestor) return true;
l:	f=f.Class();
		if(!f.isToken())
			std::cout<<"stop";
	}
	return false;
}

bool greaterEqual(Obj ancestor,Obj descendant,bool alsoEqual=false)
{ obj f=descendant;if(!alsoEqual) goto l;
	while(f!=classClass ){ // check for correctness of second condition
		//assert0(f.w!=0,"problem in classGreater");
		if(f==ancestor) return true;
l:	f=f.Class();
		if(!f.isToken()) 
			std::cout<<"stop";
	}
	return false;
}

bool operator>(Obj ancestor, Obj descendant){
	return greaterEqual(ancestor,descendant);
}
bool operator>=(Obj ancestor,Obj descendant){
	return greaterEqual(ancestor,descendant,true);
}

bool operator<(Obj descendant,Obj ancestor){
	return ancestor>descendant;
}
bool operator<=(Obj descendant,Obj ancestor){
	return ancestor>=descendant;
}

bool classGreaterEqual(Obj anchestor,Obj descendant){
	return greaterEqual(anchestor,descendant,true);
}

bool isAncestorInstance(const obj anchestor,const obj instance)
{
	return classGreaterEqual(anchestor,instance.Class());
}

Obj obj::Class() const
{	if(isInt())
		return classInt;
	else if(isToken())
		return V().Class;
	else if(isPrimitive())
		return classPrimitive;
	else
		assert0(false,"Problem in class extraction");return 0;
};

ClassT obj::ClT() const{
	return (ClassT)(Class().w);
}


Obj obj::Class(Obj o) const
{
	obj o1=V().Class;
	V().Class=(enum ClassT)o.w;
	return o1;
};
/*
Obj obj::executionClass() const{
	obj o=*this;newo
	for(;o.Class()!=.w>=i2t(executionClassesNum);o=o.Class()){}
	return o.Class();
}		
*/
Word& obj::Int(){
	if(isToken())
		return *(Word*)(V().hFixP()->dataP());
	else 
		return w;
}

bool obj::isPrimitive() const{return ::isPrimitive(w);}

Word obj::executionLevel() const { 
	return isToken() ? V().executionLevel : isPrimitive() ? 1 : 0;}

Word obj::executionLevel(Word exL) const
{ if(isToken()){
		Word exL0=V().executionLevel;
		V().executionLevel=exL;
		return exL0;
	}else return executionLevel();
}

void primitive0();

inline primitiveP obj::executeP() const
{
	//return (primitiveP)((ptr)&primitive0+primitivesAlignement*V().addr0);
	return (primitiveP)((ptr)&primitive0+primitivesAlignement*w);
};

inline primitiveP obj::executeP(primitiveP eP) const
{	addrT diff=(ptr)eP-(ptr)&primitive0;
	addrT index=diff/primitivesAlignement;

	primitiveP r;
	r=V().executeP();
	V().addr0=index;
	return r;
};

inline void obj::execute()const {
	(*executeP())();
}

#define boolFieldAccessors(field)\
bool obj::field() const {return V().field;}\
bool obj::field(bool f)const{bool r=V().field;V().field=f;return r;}

boolFieldAccessors(readOnly)
boolFieldAccessors(reference)
boolFieldAccessors(collect)
//boolFieldAccessors(keepAlive)

inline obj::operator Token() const{return w;}
//inline obj::operator size_t() const{return t2i(w);}

inline addrT obj::bodyP() const { return addrT(V());}

/*
inline ptr obj::bodyP(ptr p0){
	ptr p=V().relAddr;V().relAddr=p0;return p;
}
*/

inline addrT obj::bodyP(addrT q) const{return V().addr(q);}
ptr obj::bodyP(ptr p) const{return V().addr(p);}

inline void obj::undefine()
{
	Class(classUndefined);
}
inline bool obj::isDefined() const
{
	return Class()!=classUndefined;
}
inline bool obj::isToken() const
{
	return ::isToken(w);
}

inline bool obj::isInt() const 
{ 
	return ::isNumber(w);
}

//inline bool obj::isPrimitive() const{return ::isPrimitive(w);}

inline agusz_t obj::sizeofHeader(const classInfo& I) const
{
	return I.sizeofHeader;
}
// size of data uses pointer to method
inline agusz_t obj::sizeofData(const classInfo& I) const
{ if(I.sizeofTail) 
		return I.sizeofTail;
	else
		return (V().hGenericP()->*I.sizeofData_MP)();
}
inline agusz_t obj::sizeofBody (const classInfo& I) const
{
	return sizeofHeader(I)+sizeofData(I);
}

inline agusz_t obj::sizeofHeader() const
{ obj o=classInfoOf(*this);return o.w ? sizeofHeader(classInfos(o)) : 0;
}

inline agusz_t obj::sizeofData() const
{ obj o=classInfoOf(*this);return o.w ? sizeofData(classInfos(o)) : 0;
}

inline agusz_t obj::sizeofBody() const{	
	obj o=classInfoOf(*this);return o==notFound ? 0 : sizeofBody(classInfos(o));
}

bool obj::hasBody () const{
	obj c=classInfoOf(*this);
	bool r= (c==notFound) ? false : classInfos(c).hasBody();
	
	return r;
}

bool obj::isInstanceOf(obj anchestor) const
{
	return isAncestorInstance(anchestor,*this);
}

inline void obj::setToken(addrT q) const
{
	((hGeneric*)(M.mm()+q))->token=w;
}
inline void obj::setToken() const
{
	setToken(bodyP());
}

/*
inline ptr obj::allocateBody(uWord sz,ptr s=nullptr,uWord sz_s=std::numeric_limits<uWord>::max()) const
{ // allocates a body of size sz. Optionally the memory is copied from another chunk of memory. 
	//The token in the body is initiallized accordingly.
	ptr p=V().relAddr=mm+allocate0(sz);
	if(s) memcpy(p,s,MIN(sz,sz_s)); 
	setToken(p); // the token is initialized accordingly
	return p;
}
*/

inline addrT obj::allocateBody(uWord sz,obj o_s,uWord sz_s,addrT& qs) const
{ // allocates a body of size sz. Optionally the memory is copied from another chunk of memory. 
	//The token in the body is initiallized accordingly.
	addrT q=allocate0(sz);
	if(o_s){
		qs=o_s.bodyP();
		memcpy(M.mm()+q,M.mm()+qs,MIN(sz,sz_s));
	}
	V().addr(q);			// the address of the vector is initialized
	setToken(q);	// the token is initialized accordingly
	return q;
}

inline addrT obj::allocateBody(uWord sz,obj o_s,uWord sz_s) const{
	addrT qs;return allocateBody(sz,o_s,sz_s,qs);
}


inline void obj::deallocateBody() const{
	hasBody() ? deallocate0(bodyP(),sizeofBody()) : 0;
} //inline ptr obj::deallocateBody(uWord sz){deallocate0(addr,sz);return addr;}

//inline obj::~obj(){deallocateBody();releaseVect(w);}

inline void obj::swap(obj o)
{
	std::swap<obj>(*this,o);
	setToken();
	o.setToken();
}

template<typename T> T* obj::dataP_T() const
{
	return (T*)((ptr)V()+sizeofHeader());
}

/*template<typename T> T& obj::datum_T() const
{
	return *dataP_T<T>();
}
*/
// When the datum of a fixed size element fits in the space reserved for the addr, than it is saved there
// If the datum doesn't fit in the space reserved for the addr, than it is saved in the heap via the addr.
// The evaluation of "fits doesn't fits", is done at compile time so no "if" exist in compiled code.

template<typename T>const bool inAddr() {return sizeof(T)<=sizeof(addrT);}

template<typename T> T& obj::datum_T() const {
	if(inAddr<T>()) 
		return *(T*)(&V().addr0);
	else
		return *(T*)V().hFixP()->dataP();
}

template<typename T> T obj::datum_T(T t) const
{ std::swap<T>(datum_T<T>(),t);
	return t;
}

template<typename head_t,typename T>
head_t* obj::allocateBody(uWord elements,obj o_s,uWord elements_s)
{ elements_s=MIN(elements,o_s ? elements_s : 0);
	head_t* p=(head_t*)(M.mm()+allocateBody(sizeof(head_t)+sizeof(T)*elements,o_s,sizeof(head_t)+sizeof(T)*elements_s));
#ifndef NDEBUG
	head_t* p1=p+1;
		//if(!s) s_elements=0;if(s_elements>elements) s_elements=elements;
		memset((ptr)(p1)+sizeof(T)*elements_s,0xdd,sizeof(T)*(elements-elements_s));
#endif
	return p;
}

#define classVarAggregate(_class,_o,head_t,moreInstructions)\
	case _class:{\
		head_t* p=(_o).allocateBody<head_t,T>(size);_o;p->size=size;\
		if(pValues) memcpy(p+1,pValues,sizeof(T)*elements);\
		moreInstructions;break;\
	}

#define classBasicData(Class1,_o,head_t,cclass)\
	CASE(Class1)\
		if(sizeof(addrT)<sizeof(T)) \
			{hFixBase<T,uWord>* p=(_o).allocateBody<hFixBase<T,uWord>,T>(1);*p->dataP()=*pValues;}\
		else \
			*(T*)(&_o->addr0)=*pValues; \

// the conditions in the macro bellow get resolved during compilation so haven't effects on execution time
// the possible types are among those that may fit inside the vect::addr and vect.
#define Token2ptr(t)\
	(\
	 sizeof(T)<=sizeof(vect::addr0) ?\
	 ((T*)(&v.addr0)) : \
	 sizeof(T)<=sizeof(vect) ?\
	 ((T*)(&v)) :\
	 ((T*)(v.hfixP->data))\
	)

bool initializationDone=false;
void gctest(){if(initializationDone) {garbageCollect(M);compact(M);} }

/*
obj newClass(obj fatherClass){
	obj o=reserveObj();o.Class(fatherClass);
	o.reference(true);o.readOnly(false);o.executionLevel(1);
	return o;
}
*/

template<typename T> obj newobj(
  obj Class,
	const T* pValues=nullptr,
	uWord size=1,
	uWord elements=1,
	uWord exL=0,
	bool isReference=false,
	uWord fieldsNum=0,
	uWord levelsNum=0
)
{
// the function constructs an object of type "Class" that containes elements of type T.
// The number of elements is elements. The execution Level is given defaults to passive=0
	obj o;
	o=reserveObj();
	o.Class(Class);
	o.executionLevel(exL);
	o.readOnly(false);o.reference(isReference);//o.keepAlive(false);

	SWITCH(Class)
		CASE(classAggregate,classString)
			assert0(size<=hAggregate::max_size,"required to create a newobj with size that doesn't fit in hAggregate::maxSize");
				hAggregate* p=o.allocateBody<hAggregate,T>(size);
//				hAggregateBase_<T,suWord> *p=o.allocateBody<hAggregateBase_<T,suWord>,T>(size);
				p->size=size;p->n=elements;
				if(pValues) memcpy(p+1,pValues,sizeof(T)*elements);
				
/*
				hAggregateBase_<T,uWord> *p=o.allocateBody<hAggregateBase_<T,uWord>,T>(size);
				p->size=size;p->n=elements;
				if(pValues) 	memcpy(p+1,pValues,sizeof(T)*elements);
*/				
			
/*
		CASE(classPrototype)
			hPrototype* p=o.allocateBody<hPrototype,T>(size);
			p->size=size;p->n=elements;p->fieldsNum=fieldsNum;p->levelsNum=levelsNum;
*/	
		classBasicData(classFloat,o,hFixBase<T>,T)
		classBasicData(classDouble,o,hFixBase<T>,T)
		classBasicData(classInt,o,hFixBase<T>,T)
		classBasicData(classDeferred,o,hFixBase<T>,T)

		CASE(classUndefined)
			o->addr(addrT(0));
		CASE(classPrimitive)
			o.executeP((primitiveP)pValues);
		DEFAULT
			assert0(false,"Problem in newobj");
	endSWITCH
	if(o.Class()==classAggregate && (addrT)o==0)
		stop();
	//gctest();
	return o;
}

template <typename head_t,typename T>
void obj::resize(uWord elements) const
{ assert0(elements<=hAggregate::max_size,
		"During resize has been requested a number of elements bigger than what is possible to hold in hAggregate"
	);
	agusz_t sz0=sizeofBody(),sz1=sizeof(head_t)+sizeof(T)*elements;
	// We must first check if space is available and after take the addres, because the check may generate
	// a compaction procedure and the address may change cause of that
	addrT q0,q1=allocateBody(sz1,*this,sz0,q0);
#ifndef NDEBUG
	//signalFreePos();
	if(sz1>sz0) memset((ptr)M.mm()+q1+sz0,0xee,sz1-sz0);
#endif
	deallocate0(q0,sz0);
}

/*
template <typename head_t,typename T>
void obj::resize1(uWord elements) const
{ 
	size_t sz0=sizeofBody(),sz1=sizeof(head_t)+sizeof(T)*elements;
	// We must first check if space is available and after take the addres, because the check may generate
	// a compaction procedure and the address may change cause of that
	addrT q0,q1=allocateBody(sz1,*this,sz0,q0);
#ifndef NDEBUG
	if(sz1>sz0) memset(M.mm()+q1+sz0,0xee,sz1-sz0);
#endif
	deallocate0(q0,sz0);
}
*/










/* operations on classes
Member function	typical form for class C:
Default constructor	C::C();
Destructor	C::~C();
Copy constructor	C::C (const C&);
Copy assignment	C& operator= (const C&);
Move constructor	C::C (C&&);
Move assignment	C& operator= (C&&);
*/

// copy constructor

/*
obj::obj(Obj o){
	memcpy(this,&o,sizeof(obj));addr=allocateBody(o.sizeofBody(),o.bodyP());
	setToken();
}


// move constructor
inline obj::obj(obj&& o){
	memcpy(this,&o,sizeof(obj));o.undefine();
	setToken();
}
*/

// copy assignement

// Inside the object o we must save a cloned copy of *this.
Obj obj::cloneInRaw(Obj o) const{
		// al the vector is copied and we must cope with data that we shouldn't copy
		o.V()=(*this).V();
		o.readOnly(false);
		//assert0(o.reference()==false); // a reference shouldn be cloned

		if(hasBody())
			o.allocateBody(sizeofBody(),*this);
		else
			if(!isToken())
				o->value()=this->w;
			else
				/* o.bodyP((*this).bodyP()) */;
		return o;
}

Obj obj::clone() const{
	return cloneInRaw(reserveObj());
}

void obj::rcopy() const{
	if(isToken() && !readOnly()) readOnly(true);
}

obj obj::wcopy() const{
	//return isToken() && readOnly() ? clone() : *this;
	return clone();
}

void obj::cloneIn(Obj o) const{
	if(!o.isToken()) THROW("for clone In to o should be a token"<<o)
	if(o.hasBody()) 
		o.deallocateBody();
	cloneInRaw(o);
}	

//inline obj& obj::operator=(const obj& o){}
/*
// move assignement
inline void obj::operator=(obj&& o){
	deallocateBody();
	memcpy(this,&o,sizeof(obj));o.undefine();
	setToken();
}

*/

obj obj::New(obj newo){ // this is possible only in references to objects
	obj r=reserveObj();
	std::swap<vect>(V(),r.V());r.setToken();setToken();
	return r;
}

void obj::Old(obj oldo){ // this is possible only to references to objects
	deallocateBody();
	this->V()=oldo.V();setToken();
	release(oldo);
}

void obj::kill() const{
	deallocateBody();
	release(*this,false); // withoutlock because it is used only from garbage collect
}

obj newUndefined(Obj Class=classUndefined)
{	Obj o=newobj<Word>(Class);
	o.reference(true);
	
	return o;
}






obj newInt(Int v=0)
{
	return v;
}
obj newDouble(double v=0)
{
	return newobj<double>(classDouble,&v);
}
obj newFloat(float v=0)
{
	return newobj<float>(classFloat,&v);
}

obj newDeferred(obj o){
	return newobj<obj>(classDeferred,&o);
}

void primitive0();

obj newPrimitive(void (*p)(),Word executionLevel=1)
{	return newobj<agu>(classPrimitive,(ptr)p,0,0,executionLevel);}

obj undefined=persistent(newUndefined());


/*
template<typename T>
T valueRead(vect& v){
	if(sizeof(T)<=sizeof(vect::addr)) return *(T*)(&v.addr);
	//if(sizeof(T)<=sizeof(vect)) return *reinterpret_cast<T*>(&(v.valueBits<<shortClassBits));
	return *(T*)(v.hGenericP+1);
}

template<typename T>
T valueWrite(vect& v,T value){
	if(sizeof(T)<=sizeof(vect::addr)) *(T*)(&v.addr)=value;
	if(sizeof(T)<=sizeof(vect)) v.valueBits=(value>>shortClassBits);
	*(T*)(v.hGenericP+1)=value;
}
*/
/*
bool obj::operator==(Obj o) const
{
	return w==o.w;
}
bool obj::operator!=(Obj o) const
{
	return w!=o.w;
}
*/


bool operator==(Obj o,Word w){return o.w==w;}
bool operator==(Word w,Obj o){return o.w==w;}
bool operator==(Obj o1,Obj o2){return o1.w==o2.w;}




