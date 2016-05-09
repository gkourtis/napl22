inline void adjustOffset(const Word n,Word& i,const char* m){
	//the convention is that 
	//positions counted from start are expressed from 0 to n-1 and n for the position past the end (e.g. in order to append)
	//positions counted from the end are counted from ~0 (~0=-1 ) to ~(n-1)= -n  
	Word i0=i;
	if(i<0)
		i-=~n; 
	// bounds should be between 0 and n included (when used as an insertion position).
	if(i<0 || i>n)
		THROW(m<<" adjustOffset n="<<n<<" i="<<i)
	else
		;
}

#define ADJUSTOFFSET(n,i) adjustOffset(n,i,__PRETTY_FUNCTION__)
#define ADJUSTOFFSET2(n,i,j) ADJUSTOFFSET(n,i);ADJUSTOFFSET(n,j)
#define ADJUSTOFFSET3(n,i,j,k) ADJUSTOFFSET(n,i);ADJUSTOFFSET(n,j);ADJUSTOFFSET(n,j)


/*
inline void adjustOffset(Word n,Word &i,Word &j){
	ADJUSTOFFSET(n,i);ADJUSTOFFSET(n,j);
}

inline void adjustOffsets(Word n,Word& i,Word &j,Word &k){
	adjustOffset(n,i);adjustOffset(n,j);adjustOffset(n,k);
}
*/
	/*
	uWord Aggregate::nextSize(uWord currentSz,uWord requiredSz){
		uWord r;
		for(r=currentSz;r<requiredSz;r=r+1+r/4)
		return r;
	}
	*/

bool Aggregate::adjustIsInternal(Word& i){
	Word n=elements();
	if(i<0) 
		i-=~n;
	return i<n;
}

	Aggregate::operator String(){return String(w);}
	
	void Aggregate::print(const wchar_t* s){std::wcout<<"AggregatePrint"<<std::endl;}
	
	Token_iterator Aggregate::begin() const {return Token_iterator{w,0};};
	Token_iterator Aggregate::end() const {return Token_iterator{w,elements()};};

	Token_iterator Aggregate::rbegin() const {return Token_iterator{w,elements()-1};};
	Token_iterator Aggregate::rend() const {return Token_iterator{w,-1};};
	
	//Aggregate operator=(Token t){return (Aggregate)(obj(t));}
	hAggregate* Aggregate::hP() const {return V().hAggregateP();}
	
	Word* Aggregate::dataP() const {return hP()->dataP();}
	Word& Aggregate::data(uWord i) const {return hP()->data(i);}
	Word Aggregate::maxElements() const{return hP()->size;}
	Word Aggregate::elements() const{return hP()->n;}
	Word Aggregate::elements(uWord n) const{
		uWord r=hP()->n;hP()->n=n;return r;
}

	bool Aggregate::isEmpty() const{return elements()==0;}

	#define For(w,loopCode)		\
	{for(uWord _i=0;_i<size;_i++)	{Word& w=hAggregateP->data[i];loopCode}}
	#define ForRev(i,loopCode)	\
	{for(uWord _i=size;--_i>=0)		{Word& w=hAggregateP->data[i];loopCode}}

// negative values accepted and ignored the size is always < maxuInt/4

//hAggregate& h(){return *hAggregateP;} 

	void Aggregate::checkResize(Word n) const{
		uWord m=n+hP()->n,r=hP()->size;
		if(m>r){
			for(;m>r;r+=1+r/8);
			resize<hAggregate,Word>(r);
			hP()->size=r;
		}
	}

	void Aggregate::checkUnderflow(Word n,const char* m)const{
		if(!isToken() || !((obj)n).isInt() || n>hP()->n){
			std::stringstream s;
			if(m) s<<m;
			s<<" checkUnderflow n="<<n<<" elements()="<<elements();
			throw(std::runtime_error(s.str()));
		}
	}
	
#define CHECKUNDERFLOW(n) checkUnderflow(n,__PRETTY_FUNCTION__)

	void Aggregate::pushN(uWord n, ...)const{
		CHECKUNDERFLOW(n);
		va_list tokens;va_start(tokens,n);
			for(uWord i=elements();i<elements()+n;i++) data(i)=va_arg(tokens,Token);
		va_end(tokens);
		elements(elements()+n);
	}

	void Aggregate::popN(Token n, ... ) const{
		CHECKUNDERFLOW(n);
		va_list refVars;va_start(refVars,n);
			for(uWord i=hP()->n-n;i<hP()->n;i++) *va_arg(refVars,Word*)=data(i);
		va_end(refVars);
		hP()->n-=n;
	}

// the routine must be changed and allow a variable number of pos and word for insertion
Word* Aggregate::ins_backdel(Word pos,Word n) const{
	checkResize(n);ADJUSTOFFSET(elements(),pos);
	if(pos<elements()) 
		memmove(&data(pos+n),&data(pos),sizeof(hAggregate::el_t)*(elements()-pos));
	elements(elements()+n);return &data(pos);
}

Word* Aggregate::ins_backdel(Word* P,Word n) const{
	return ins_backdel(P-dataP(),n);
}

void Aggregate::insert(Word pos,Obj o){*ins_backdel(pos)=o.w;}

inline void WordMove(Word* d,Word* s,Word n){memmove(d,s,n*sizeof(hAggregate::el_t));}
inline void WordCpy(Word* d,Word* s,Word n){memcpy(d,s,n*sizeof(hAggregate::el_t));}

// appends to the current aggregate a slice from a given aggregate from position i1 to position i2
void Aggregate::insertSlice(Aggregate a,Word pos,Word i1,Word i2){
	ADJUSTOFFSET(elements(),pos);ADJUSTOFFSET2(a.elements(),i1,i2);assert0(i1<=i2,"appendSlice");
	Word sz=i2-i1;checkResize(sz);
	elements(elements()+sz);
	WordMove(dataP()+pos+sz,dataP()+pos,elements()-pos);
	WordCpy(dataP()+pos,a.dataP()+i1,sz);
}

void Aggregate::roll(bool direction,int n){
	int d=elements();Word w;int i;
	if(direction)	{i=d-1;w=hP()->data(i);for(;i>=(d-n+1)	;i--) hP()->data(i)=hP()->data(i-1);}
	else						{i=d-n;w=hP()->data(i);for(;i<d-1			  ;i++) hP()->data(i)=hP()->data(i+1);}
	hP()->data(i)=w;
}

// moves elements from one aggregate to another
/*
void Aggregate::transfer(Aggregate a,int l){
	assert0(l<=hP()->n,"aggregate transfer not sufficient elements");
	checkResize(l);
	for(uWord i=elements()-l;i<elements();i++) push(hP()->data(i));
	hP()->n-=l;
}
*/


Word& Aggregate::at(Word i) const {
	ADJUSTOFFSET(elements(),i);
	return hP()->data(i);
}

Word& Aggregate::operator[](Word i) const {
#ifndef NBOUNDCHECKS
	ADJUSTOFFSET(elements(),i);
#endif
	return hP()->data(i);
}

void Aggregate::push(Word w) const{
	checkResize(1);
	hP()->data(hP()->n++)=w;
}

void Aggregate::pushint(Word w) const{
	push(w>>1);
}

void Aggregate::push2(Word w1,Word w2) const {checkResize(2);hP()->data(hP()->n++)=w1;hP()->data(hP()->n++)=w2;}

Word Aggregate::pop() const{
	if(hP()->n<1) THROW("pop underflow");
	return data(--hP()->n);
}

Word Aggregate::popv() const{
	if(hP()->n<1) THROW("popv underflow");
	obj o=data(--hP()->n);
	if(!o.isToken()) 
		return o.w;
	else
		if(o.Class()==classInt) 
			return o.datum_T<Word>();
		else
			return 0;
}

Word Aggregate::popvint() const{
	return popv()<<1;
}

void Aggregate::pop2(Token& t1,Token& t2) const{t2=pop();t1=pop();}
void Aggregate::pop2(Aggregate& t1,Token& t2) const{t2=pop();t1=pop();}
void Aggregate::pop2(Token& t1,Aggregate& t2) const{t2=pop();t1=pop();}


void Aggregate::drop(uWord n) const{
	CHECKUNDERFLOW(n);hP()->n-=n;
}

void Aggregate::dropPush(uWord n,Token t) const{
	drop(n);push(t);
}

void Aggregate::dropAt(Word pos,Word n) const{
	Word N=hP()->n;pos=~pos;
	adjustOffset(N,pos,"pos uncorrect");
	if(pos>N or n>pos) THROW("trying to delete from pos:"<<pos<<" and for length of"<<n<<" but aggregate size N="<<N)
	ins_backdel(pos,-n);
}

Token& Aggregate::tos() const{return (*this)[~1];}
Token& Aggregate::nos() const{return (*this)[~2];}

void Aggregate::insert1(Word w,int pos){*ins_backdel(pos)=w;}
void Aggregate::insert2(Word w1,Word w2,int pos){Word *p=ins_backdel(pos,2);*p++=w1;*p++=w2;}
		
void Aggregate::nip(){Word w=pop();pop();push(w);}
void Aggregate::tuck(){Word w2=pop(),w1=pop();push(w2);push(w1);push(w2);}
void Aggregate::dropall(){drop(elements());}

//void neg(){pushint(-popvint());}

void Aggregate::pick(Word i){
// the pick word is used werever a copy of a stack element is requested
// depending on the kind of object the copy could return a cloned copy
	obj o=(*this)[i];
	if(o.isToken() && !o.reference() && !o.readOnly()) o.readOnly(true);
	push(o.w);
}
void Aggregate::dup(){pick(~1);}
void Aggregate::over(){pick(~2);}
void Aggregate::pick(){pick(popv());}

void Aggregate::swap(){std::swap<Word>(at(~1),at(~2));}

bool Aggregate::in(obj o) const{
	for(uWord i=0,n=elements();i<n;i++) 
		if(data(i)==o.w) return true;
	return false;
}

bool Aggregate::elementary(){
	for(obj i:*this)
		if(!i.isInt()) return false;
	return true;
}


// the routine allocates memory for a multibyte string and convers a Char string into a multibyte one.
// the result returned should be deleted immediatelly after it has been used.
char* Chars2mbs(const Char* s,size_t n,char* d0=nullptr,uWord sz=0){
	// one character more used for null termination
	if(!sz) sz=MB_CUR_MAX*n+1;
	if(!d0) d0=new char[sz];
	// if Char has a different size than wchar_t then each character is converted one by one. wctomb is intialized
	bool stateDependent=wctomb(nullptr,0);char* d=d0;
	for(int i=n;--i>=0 && d+MB_CUR_MAX+1<=d0+sz;) {
// if d0 is passed using a propietary sz, than d should hold MB_CUR_MAX+1 before arriving to d0+sz		
		int mbsz=wctomb(d,*s++);
		if(mbsz==-1)
			{delete[] d0;return nullptr;}
		d+=mbsz;
	}
	*d=0; // null termination
	return d0;
}

// mbtowc: If the argument passed as pmb is not a null pointer, the size in bytes of the multibyte character pointed by pmb is returned when it 
// forms a valid multibyte character and is not the terminating null character. 
// If it is the terminating null character, the function returns zero, 
// and in the case they do not form a valid multibyte character, -1 is returned.

__attribute__((noinline)) size_t mbs2Chars(Char* d0,const char* s,size_t n0=-1){
	Char* d=d0;size_t n=n0;mbtowc(nullptr,0,0);
	while(n){
		wchar_t wc;int mbsz=mbtowc(&wc,s,n);
		if(mbsz==0) return d-d0;
		if(mbsz==-1 || mbsz>n) return d-d0;
			//THROW(std::hex<<"error of conversion from mbs char s[0]="<<(int)s[0]<<" s[1]="<<(int)s[1]<<" s[2]="<<(int)s[2]<<" s[3]="<<(int)s[3])
		*d++=wc;n-=mbsz;s+=mbsz;
	};
	return d-d0;
}

Aggregate newAggregate(obj Class,const Word* p,uWord size,uWord elements){
	assert0(elements<=size,"newAggregate elements not less than maxSize");
	assert0(size<=hAggregate::max_size);
	return newobj<Word>(Class,p,size,elements);
}

Aggregate newAggregate(obj Class=classAggregate,uWord maxSize=0x6){
	return newobj<Word>(Class,nullptr,maxSize,uWord(0));
}

Aggregate newPrototype(){
	return newAggregate(classAggregate);
}

template<class T>
void copyElements(T* d0,T* s0,Aggregate::sz_t i1,Aggregate::sz_t i2){
	if(i1<=i2)
		for(auto s=s0+i1,d=d0;s!=s0+i2;) *d++=*s++;
	else
		for(auto s=s0+i1,d=d0;s!=s0+i2;) *d++=*--s;
}

Aggregate concatenateSlices0(Word n,bool extended, ... ){
// cols may be 1 or 3 if 1 means that the arguments are only aggregate objects if 3 means that are aggregate objects with start and end parameters.
//#define adjustOffset(i) if(i<0)i=h.n-~i;if(i<0)i=0;if(i>h.n)i=n

	va_list args;Word sz=0;obj Class=0;
#define Instructions sz+=n
#define individuateClass
	#include "concatenateSlices.cpp"
#undef  individuateClass
#undef  Instructions
	if(Class.w!=0){
		Aggregate r=newAggregate(classAggregate,sz);
		r.Class(Class);
		r.elements(sz);hAggregate::el_t *p=r.dataP();
		#define Instructions copyElements<hAggregate::el_t>(p,h.dataP(),i1,i2);p+=n;
		//memcpy(p,h.dataP()+i1,sizeof(hAggregate::el_t)*n);p+=n
			#include "concatenateSlices.cpp"
		#undef Instructions
		return r;
	}else{
		return (Aggregate)0;
	}

}

Aggregate operator+(Aggregate a1,Aggregate a2){
	return concatenateSlices0(2,false,a1.w,a2.w);
}

String concatenateStrings(String s1,String s2){
	return concatenateSlices0(2,false,s1.w,s2.w);
}
String concatenateStrings(String s1,String s2,String s3){return concatenateSlices0(3,false,s1.w,s2.w,s3.w);}

Aggregate slice(const Aggregate a,Int i1,Int i2){
	return concatenateSlices0(1,true,a,i1,i2);
}

/*
String concatenateStrings(String s1,String s2,String s3,String s4){return concatenateSlices0(4,1,s1,s2,s3,s4);}

String concatenateStrings(String s1,Word i1s,Word i1e,String s2,Word i2s,Word i2e){
	return concatenateSlices0(2,3,s1,i1s,i1e,s2,i1s,i1e);
}
String concatenateStrings(String s1,Word i1s,Word i1e,String s2,Word i2s,Word i2e,String s3,Word i3s,Word i3e){
	return concatenateSlices0(3,3,s1,i1s,i1e,s2,i2s,i2e,s3,i3s,i3e);
}
*/  

/* 
enum sign compare1(Obj o1,Obj o2){
	Word r,c;
	if( (r=(o1.Class().w-o2.Class().w)) ) return sign(r);
	if(false){
	}else if(o1.Class()==classAggregate || o1.Class()==classString){
// just o1 and o2 are reinterpreted as Aggregates,o1 and o2 aren't valid after the copy
			const Aggregate a1(o1),a2(o2); 
			if( (r=(a1.elements()-a2.elements())) ) return sign(r);
			for(Word i=0;i<a1.elements();i++) if ( (r=(a1[i]-a2[i])) ) return sign(r);
			return equal;
	}else{
			if( (r=(o1.sizeofData()-o2.sizeofData())) ) return sign(r);
			return sign(memcmp(o1.dataP_T<char>(),o2.dataP_T<char>(),o1.sizeofData()));
	}
}
*/

// when two objects are of different class the sign of the class difference is returned
// when two objects are of the same class then comparison proceeds


enum sign compare(Obj o1,Obj o2){
	if(o1.w==o2.w) return equal;

	Word r;obj c;
	if(isNumber(o1) && isNumber(o2)) return sign(o1-o2);
	if(isNumber(o1)) return (enum sign)-1;
	if(isNumber(o2)) return (enum sign)1;

	if(o1.reference() && o2.reference()) return sign(o1-o2);
	if(o1.reference()) return (enum sign)1;
	if(o2.reference()) return (enum sign)-1;
	
	if( (r=(c=o1.Class().w-o2.Class().w)) ) return sign(r);
	SWITCH(c)
		CASE(classInt)
			return sign(o1.w-o2.w);
		CASE(classDouble)
			return sign(o1.datum_T<double>()-o2.datum_T<double>());
		CASE(classFloat)
			return sign(o1.datum_T<float>()-o2.datum_T<float>());
		CASE(classInt64)
			return sign(o1.datum_T<int64_t>()-o2.datum_T<int64_t>());
		CASEIF(o1.isInstanceOf(classAggregate))
// just o1 and o2 are reinterpreted as Aggregates,o1 and o2 aren't valid after the copy
			const Aggregate a1{o1},a2{o2}; 
			for(uWord i=0;i<MIN(a1.elements(),a2.elements());i++){
				r=(enum sign)compare(a1[i],a2[i]);
				if(r) return (enum sign)r;
			}
			return(sign(a1.elements()-a2.elements()));
		DEFAULT
			ptr p1=&(o1.datum_T<char>()); 
			ptr p2=&(o2.datum_T<char>());
			if( (r=memcmp(p1,p2,MIN(o1.sizeofData(),o2.sizeofData()))) )
				return sign(r);
			r=sign(o1.sizeofData()-o2.sizeofData());
			return sign(r);
	endSWITCH
}

Aggregate Aggregate::clone() const{ 
	Aggregate r=((obj)(*this)).clone();
	for(obj i:r)
		i.rcopy();
	return r;
}

Aggregate Aggregate::wcopy() const{
	return (isToken() && readOnly()) ? this->clone() : *this;
}

Aggregate Aggregate::rcopy() const{
	if (isToken() && readOnly()) readOnly(true);
	return *this;
}

