struct vect;

struct classInfo;

class obj;
typedef const obj Obj;

class obj {
public:

Word w;
//obj()=default;


constexpr obj(ClassT c) noexcept : w((Token)c){};

obj(Token t=0) noexcept;
~obj() noexcept;

obj(obj Class,const Word* p,uWord maxSize,uWord elements);

//constexpr obj(const Token t):obj(t){};

//obj(suWord i){w=i2t(i);}
//obj()=default;

vect* operator->() const;
vect& V() const;

hGeneric* hP();

ClassT ClT() const;
Obj Class() const; // returns the class of the obj
Obj Class(Obj) const; // sets the class of the obj and returns the old one


Obj executionClass() const;

Obj ro(Obj) const;

Word& Int();

Word executionLevel() const; // returns the execution level of the obj
Word executionLevel(Word) const; // sets the ExLevel of the obj and returns the old one.

primitiveP executeP() const;
primitiveP executeP(primitiveP) const;
void execute() const;

#define declBoolField(field) bool field() const; bool field(bool) const;
	declBoolField(readOnly)
	declBoolField(reference)
	declBoolField(collect)
	declBoolField(keepAlive)
#undef declBoolField

inline addrT bodyP() const;
inline addrT bodyP(addrT) const;
ptr bodyP(ptr p) const;
inline void undefine();
inline bool isDefined() const;

operator Token() const; // returns the token of the current vect
//operator size_t() const; // returns the index of the current vect

private:
addrT sizeofHeader(const classInfo& I) const;
addrT sizeofData(const classInfo& I) const;
addrT sizeofBody(const classInfo& I) const;

public:
addrT sizeofHeader() const; // the header contains various size and type data
addrT sizeofData() const;		// The data part contains the data of the object
addrT sizeofBody() const;		// the body is header + data part.

bool hasBody() const;
bool isClass() const;
bool isInstanceOf(obj anchestor) const;
bool isValue() const{return false;}
bool isPrimitive() const;
bool isToken() const;
bool isInt() const;

inline void setToken(addrT) const;
inline void setToken() const;

//size_t allocateBody(uWord sz,ptr s,uWord sz_s) const;

inline addrT allocateBody(uWord sz,obj o_s,uWord sz_s,addrT& qs) const;
inline addrT allocateBody(uWord sz,obj o_s=0,uWord sz_s=std::numeric_limits<uWord>::max()) const;
 
void deallocateBody() const;

void assignBody(ptr p) const;

ptr assignementCopy(obj o,bool keepOldBody);

void replaceBody(ptr p) const;
void releaseBody() const;

inline void swap(obj o);

//operator bool(){return w;}

//Obj operator=(Obj);
//void operator=(obj&& o);

obj operator=(Token t){return w=t;};

obj New(obj newo);
void Old(obj oldo);

Obj cloneInRaw(Obj) const;
Obj clone() const;
void cloneIn(Obj) const;
void kill() const;

void rcopy() const; obj wcopy() const;


//obj& operator=(const obj&);

template<typename T> T* dataP_T() const;
template<typename T> T& datum_T() const;
template<typename T> T datum_T(T) const;
template<typename head_t,typename T> 
	head_t* allocateBody(
		uWord elements,
		Obj o_s=0,
		uWord s_elements=std::numeric_limits<uWord>::max()
	);

template <typename head_t,typename T> void resize(uWord elements) const;

};

bool operator==(Obj,Obj);
bool operator!=(Obj o1,Obj o2){return !(o1==o2);}

Obj notFound{0};

//bool operator==(Obj o,const Token t){return o.w==t;}
//bool operator==(const Token t,Obj o){return t==o.w;}
bool operator!=(Obj o,const Token t){return o.w!=t;}
bool operator!=(const Token t,Obj o){return t!=o.w;}
