const int aggSz=6;
class Aggregate;class String;class Token_iterator;

Aggregate newAggregate(obj Class,const Word* p,uWord maxSize,uWord elements);

struct Aggregate:public obj{
	//static uWord nextSize(uWord currentSz,uWord requiredSz);
public:
	//{	aggregate(Word maxSize=0x10,Token Class=classAggregate) _t{newobj<Word>(Class,nullptr,maxSize,0)}{}

//	~Aggregate(){}

	typedef hAggregate::sz_t sz_t;

	Aggregate(Token t):obj(t){}
	Aggregate(obj o):obj(o){}

/*
	Aggregate(bool keepAliveF):Aggregate(newAggregate(classAggregate.w,nullptr,aggSz,0)){
		keepAlive(keepAliveF);
	}
*/

	bool adjustIsInternal(Word& i);
	Aggregate(obj Class,Word* p):Aggregate(newAggregate(Class,p,aggSz,0)){}
	Aggregate(uWord maxsz,uWord sz):Aggregate(newAggregate(classAggregate,nullptr,maxsz,sz)){}
  Aggregate():Aggregate(0x8,0){}
	//Aggregate():Aggregate(newAggregate(_classAggregate,nullptr,0x10,0)){}

	operator String();
	
	void print(const wchar_t* s);
	
	Token_iterator begin() const;
	Token_iterator end() const;
	Token_iterator rbegin() const;
	Token_iterator rend() const;
	
	//Aggregate operator=(Token t){return (Aggregate)(obj(t));}
	hAggregate* hP() const ;
	
	Word* dataP() const ;
	Word& data(uWord i) const ;
	Word maxElements() const,elements() const,elements(uWord n)const;
	bool isEmpty() const;

	void checkResize(Word n)const,checkUnderflow(Word n,const char* m=nullptr) const;

	void pushN(uWord n, ...)const,popN(Token n, ... ) const;

// the routine must be changed and allow a variable number of pos and word for insertion
	Word* ins_backdel(Word pos=~0,Word n=1) const;
	Word* ins_backdel(Word* wP,Word n=1) const;
	
	void insert(Word pos,Obj o);

// appends to the current aggregate a slice from a given aggregate from position i1 to position i2
	void insertSlice(Aggregate a,Word pos=~0,Word i1=0,Word i2=~0);
	void roll(bool direction,int n);

	void transfer(Aggregate a,int l);

	Word& at(Word i) const;
	Word& operator[](Word i) const;

	void push(Word w) const;
	void pushint(Word w) const;
	void push2(Word w1,Word w2) const;

	Word pop() const,popv() const;

	Word popvint() const;
	void pop2(Token& t1,Token& t2) const;
	void pop2(Aggregate& t1,Token& t2) const;
	void pop2(Token& t1,Aggregate& t2) const;

	void drop(uWord n=1) const;
	void dropPush(uWord n,Token t) const;
	void dropAt(Word pos=~0,Word n=1) const;
	Token &tos() const,&nos() const;

	void insert1(Word w,int pos=~0);
	void insert2(Word w1,Word w2,int pos=~0);
		
	void nip(),tuck(),dropall(),neg(),dup(),over(),pick(),pick(Word),swap();

// transfer the last data of a stack to another one keeping their order

/*
bool findIn(Word w0,Word &res,Word i0=0,Word i1=1,bool ca=false,uWord columns=2){
	Word w1;uWord i;
	for(i=0;i<h.n;i+=columns){
		w1=h.data[i+i0];if(w0==w1) goto r1;
		if(ca) if(isToken(w0)&&isToken(w1)) if(compareAggregates((obj&)w1,(obj&)w0)) goto r1;
	}
	return false;
r1:
	res=at(i+i1);return true;
}
*/

bool in(obj o) const;
Aggregate clone() const;

bool elementary();

Aggregate rcopy() const,wcopy() const;

};


#define TOS ((obj)(stack()[~1]))
#define NOS ((obj)(stack()[~2]))

#define TOS1(t) ((t)(stack()[~1]))
#define TOS2(t) ((t)(stack()[~2]))
#define TOS3(t) ((t)(stack()[~3]))
#define TOS4(t) ((t)(stack()[~4]))

#define PUSH(x) (stack().push(x))
#define PUSH2(x,y) (stack().push(y),stack().push(x))
#define DROPPUSH(n,expr) {obj o=expr;stack().drop(n);stack().push(expr);}
#define POP ((obj)(stack().pop()))
#define POPV ((obj)(stack().popvint()))

#define DROP(n) (stack().drop(n))
//#define PRINT(x) print(x)
//#define WRITE(x) write(x)

#define POP1obj(a,b) obj a{POP};
#define POP2obj(a,b) obj b{POP},a{POP};
#define POP3obj(a,b,c) obj c{POP},b{POP},a{POP};
#define POP4obj(a,b,c,d) obj d{POP},c{POP},b{POP},a{POP};

#define FORstart(A,a,i1,i2) for(Word i=i1;i!=i2;++i){	a{(A)[i]};
#define endFOR }
