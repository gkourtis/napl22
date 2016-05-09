__attribute__((constructor))
void definePrimitives(){

#define Cl(a,x,c,d,x5) if(x!=notFound) dict.define(NAPL(x),x);
	classInfoNames
#undef mcr


//classInfos(classClass).executeFunction=[](obj o){stack().push(o);};
//classInfos(classComment).executeFunction=[](obj o){};
//classInfos(classPatternSeq).executeFunction=&patternExecute;
//classInfos(classPatternAny).executeFunction=&patternExecute;
}

//defAsCode(primitive0,)

defAsCode(decimalPoint,
	lconv* p=localeconv();
	PUSH((Char)*p->decimal_point);
)	

defAsCode(drop,stack().drop(1);)
def3(dotDrop,".drop",Word n=POP;stack().dropAt(n,1););
defAsCode(dropall,stack().dropall();)
defAsCode(depth,stack().push(stack().elements());)

defAsCode(nip,stack().nip();)

defAsCode(tuck,stack().tuck();)

// stack() operations duplicating an element should enable the readOnly flag.
defAsCode(over,stack().over();)
defAsCode(pick,int i=stack().popv();stack().push(stack().at(stack().elements()-i));)
defAsCode(dup,stack().dup();)

defAsCode(clone,TOS=((Aggregate)TOS).clone();)

defAsCode(swap,Token t2=stack().pop();Token t1=stack().pop();stack().push(t2);stack().push(t1);)
def3(rollRight,"roll>",stack().roll(1,stack().popv());)
def3(rollLeft,"roll<",stack().roll(0,stack().popv());)

def3(intPlusMinus,"int+-",stack().push(newInt(-stack().pop()).datum_T<Int>());)

def3(intdivmod,"int/mod",
	Word w2=stack().popvint();Word w1=stack().popvint();
	div_t r=div(w1,w2);
	stack().push(r.quot);stack().pushint(r.rem);
);

def3(openBracket,"[",	newStack(0);) // creates a new stack() and pushes it into the stack() hierarchy
//def3(openStack,"_[",openStack();)	 // takes the tos that should be an aggregate and pushes it into the stack() hierarcy 

defAssign("_[",openStack)

def3(closeBracket,"]",oldStack();) 

defAutoAsCode("trackedS",PUSH(trackedS);)

PRIMITIVE naplis(){
	POP2obj(name,value);
	if(value.isToken()) 
		value->reference=true;
	if(name.Class()==classString)
		dict.define(name,value);
	else{
		THROW("FirstArgument isn't a String. name="<<toString(name)<<" value="<<toString(value));
	}
}

defAssign("is",naplis)

def3(concatenate,"@+",
	PUSH ((AggregateOrdered)POP+(AggregateOrdered)POP);
);

def3(intersect,"@*",
	PUSH ((AggregateOrdered)POP*(AggregateOrdered)POP);
);


PRIMITIVE AggregateDiff(bool inverted=false){
	AggregateOrdered b=POP;AggregateOrdered a=POP;
	Aggregate r= !inverted ? a-b : b-a ;
	PUSH(r);
}

defAutoAsCode("@-",AggregateDiff(););
defAutoAsCode("@s-",AggregateDiff(true););

defAsCode(ordUnion,AggregateOrdered a2=POP;AggregateOrdered a1=POP;PUSH(a1+a2););

defAsCode(ordIntersection,AggregateOrdered a2=POP;AggregateOrdered a1=POP;PUSH(a1*a2););

defAutoAsCode("slice",
	Word j=stack().pop();Word i=stack().pop();
	Aggregate a=(Aggregate)stack().pop();
	stack().push(concatenateSlices0(1,3,a.w,i,j));
);

defAsCode(bye,exitInterpretLoop=true;);

defAsCode(setAutoLevel,setExecutionLevel(););
defAsCode(setLevel,
	int l=stack().pop();
	obj o=stack().tos();
	o.executionLevel(l);
);

defAsCode(vectShow,
	Obj o=TOS;std::stringstream ss;
	o->vectss(ss);ss<<std::endl;
	write(ss);
);

defAsCode(execute,execute(POP,true););

PRIMITIVE debug(){
	backup<level> b(debugLevel,nestLevel+1);
	execute(POP,true);
}

defAssign("debug",debug);

defAsCode(ifelse,
	Token t2=POP;Token t1=POP;if(POP)execute(t1,true);else execute(t2,true);
)
defAsCode(if_,
	Token t=stack().pop();
		if(stack().pop())
			execute(t,true);
)		

defAsCode(include,
	print("Including ",toString(obj(stack().tos())));
	tString s(file2String(stack().pop()));
	interpret(s);
	print(" included");
)

defAsCode(file2String,PUSH(file2String(POP));)

defAsCode(print,print((obj)stack().tos());stack().drop(););		

defAsCode(show,std::cout<<std::endl<<toString(stack().pop(),0)<<std::endl<<std::endl;)
	
defAutoAsCode("newline",write("\n"););
defAsCode(i,stack().push(iCounter);)
defAutoAsCode("i->",iCounter=stack().popv();)

defAutoAsCode("loop",
	Word w1=POP;
	loop_(w1);
);
// attention, because the iCounter value may be used after the loop when !mustContinue shouldn't
// be inside the loop conditions with the incrementing operator because wrong results may be given
// if the checks are done in the wrong order

defAutoAsCode("for.",
	Word w1=POP;Word w2=POP;Word w3=POP;Word w4=POP;
	PUSH(for_(w4,w3,w2,w1));
);

PRIMITIVE timesDot(){
	Word w1=POP;Word w2=POP;
	PUSH(for_(w2,0,-1,w1));
}

defAutoAsCode("times.",
timesDot();
//	Word w1=POP;Word w2=POP;
//	PUSH(for_(w2,0,-1,w1));
);

def3(forQ,"for?",
	Token t=stack().pop();
		int step=stack().popv(); int i2=stack().popv();
	backup<Token> b(iCounter,stack().popv());
	if     (step>0) 
		for( ; iCounter<i2 ; iCounter+=step) {execute(t,true);if(!mustContinue())break;}
	else if(step<0)
		for( ;iCounter+=step,iCounter>=i2; ) {execute(t,true);if(!mustContinue())break;}
	else
		std::wcout<<L"problem with step == 0 in for";
	stack().push(iCounter);
);


//defAutoAsCode("execute00",execute00(POP););

defAutoAsCode("?.exit",int n=POPV;bool b=POPV;exitSec(b,n);)
defAutoAsCode("?exit",
	exitSec(POPV,1);
)
defAutoAsCode(".exit",exitSec(true,POPV);)
defAutoAsCode("exit",exitSec(true,1);)
	
defAutoAsCode("until",exitSec(stack().popv(),1);)
defAutoAsCode("while",exitSec(stack().popv()==0,1);)

defAutoAsCode("compact",stopETs.lock();compact(M);stopETs.unlock();)
	
defAutoAsCode("checkCompact",checkCompact(M);)
	
defAutoAsCode("~",counter++;)
defAutoAsCode("2~",counter.l+=2;)

defAutoAsCode(".~",counter.l+=stack().pop();)

defAutoAsCode("elc",stack().push(counter.l);)
defAutoAsCode("->",
	obj o1=stack().pop();
	obj o=stack().pop();
	o.cloneIn(o1);
)

defAutoAsCode("<-",
		obj o{stack().pop()}; obj o1{stack().pop()};
		o.cloneIn(o1);
)	
	
defAutoAsCode("??",showStack();std::cout<<std::endl;)

defAutoAsCode("noop",);

defAutoAsCode(">|>",interStackMove();)
defAutoAsCode("moveOnOldStack",interStackMove();)

defAutoAsCode("dup>|>",interStackDup();)
defAutoAsCode("isName",interStackDup();)
defAutoAsCode("int2double",int i=stack().pop();stack().push(newDouble((double)i));)

defAutoAsCode("s1",PUSH(stack()[~1]);)
defAutoAsCode("s2",PUSH(stack()[~2]);)
defAutoAsCode("s3",PUSH(stack()[~3]);)
defAutoAsCode("s4",PUSH(stack()[~4]);)

defAutoAsCode("ss1",PUSH( ((Aggregate)stacks[~2])[~1]);)
defAutoAsCode("ss2",PUSH( ((Aggregate)stacks[~2])[~2]);)
defAutoAsCode("ss3",PUSH( ((Aggregate)stacks[~2])[~3]);)
defAutoAsCode("ss4",PUSH( ((Aggregate)stacks[~2])[~4]);)

defAutoAsCode("sss1",PUSH( ((Aggregate)stacks[~3])[~1]);)
defAutoAsCode("sss2",PUSH( ((Aggregate)stacks[~3])[~2]);)
defAutoAsCode("sss3",PUSH( ((Aggregate)stacks[~3])[~3]);)
defAutoAsCode("sss4",PUSH( ((Aggregate)stacks[~3])[~4]);)

defAutoAsCode("foreach",obj action=stack().pop();Aggregate a=(Aggregate)stack().pop();foreach(a,action,false);)
defAutoAsCode("foreachRev",obj action=stack().pop();Aggregate a=(Aggregate)stack().pop();foreach(a,action,true );)


PRIMITIVE setClassTest(){
	obj class_t=POP;obj o=POP;
	if(!o->reference && o->readOnly)
		o=o.isInstanceOf(classAggregate) ? (Aggregate)o.clone() : o.clone();
	o.Class(class_t);
	PUSH(o);
}

defAssign("setClass",setClassTest)

defAutoAsCode(".class",PUSH(TOS.Class());)

defAutoAsCode("msecs",stack().push(newInt(msecs()));)

defAsCode(in,obj o=stack().pop();Aggregate a=(Aggregate)stack().pop();stack().push(newInt(a.in(o)));)

PRIMITIVE at(){
	Int i=POP;Aggregate a{POP};
	PUSH(a.at(i));
}

defAssign("at",at);

defAsCode(elements,Aggregate a=(Aggregate)stack().pop();stack().push(newInt(a.elements()));)

defAutoAsCode("isEmpty", PUSH( ((Aggregate)TOS).isEmpty() );)

defAutoAsCode("tab",std::wcout<<L"\t";)
	
defAutoAsCode("localsSet",localsSet();)

defAutoAsCode("dictShowAll",dictShow(true);)
defAutoAsCode("dictShow",dictShow();)

defAsCode(asFields,((obj)stack().tos())->fields=true;)
defAsCode(asData,((obj)stack().tos())->fields=false;)

defAsCode(hideFields,((obj)stack().tos())->hideFields=true;)
defAsCode(showFields,((obj)stack().tos())->hideFields=false;)

defAsCode(cwd,
		char* d=getcwd(nullptr,0);
			stack().push(::newString(d));
		free(d);
	)

defAutoAsCode("garbageCollect",stopETs.lock();garbageCollect(M);stopETs.unlock();)

defAsCode(allPos,stack().push(allPos(stack()[~1]));)
defAsCode(anything,
	tAggregateOrdered o{stack().pop()};
	stack().push(anything(stack()[~1],o));
)

#define DROPPUSHdef(...){																																										\
	/* tAggregate not needed as copy is kept on stack */																											\
	AggregateOrdered RR{(match(TOS3(Aggregate),TOS2(AggregateOrdered),TOS1(const obj), __VA_ARGS__ ))};		\
	DROP(2);PUSH(RR);																																													\
}


PRIMITIVE match(){
	DROPPUSHdef(patInferred);
/*
	AggregateOrdered PP=TOS3(AggregateOrdered),QQ=TOS2(AggregateOrdered);															\
	match(TOS4(Aggregate),PP,QQ,TOS1(const obj));																			\
	DROP(3);
	PUSH2(QQ,PP,);
*/																																								\
}

PRIMITIVE match1(){
	DROPPUSHdef(patInferred,equalComparison,matchKeepFailures);
}

defAssign("match",match);
defAssign("match1",match1);

PRIMITIVE matchI(){
	DROPPUSHdef(patInferred,&upperCaseComparison);
}

PRIMITIVE matchNone(){
	DROPPUSHdef(patNone);
}

PRIMITIVE matchINone(){
	DROPPUSHdef(patNone,&upperCaseComparison);
}

void matchMark_(Int markDepth,bool negate){
	PUSH(markDepth);
	DROPPUSHdef(patSeq,&equalComparison,(matchFlag)((int)matchMarked | (negate ? (int)matchNegated : 0)));
}

PRIMITIVE matchMark(){
	matchMark_(POP,false);
}

defAssign("matchMark",matchMark);

defAsCode(markLevelLess,--(markLevel););
defAsCode(markLevelMore,++(markLevel););

defAssign("matchI",matchI)
defAssign("matchNone",matchNone)

defAsCode(matchLastMark,matchMark_(~3,false););

defAsCode(matchPreviousMark,matchMark_(~5,false););


/*
		Obj pat=stack().pop();Obj positions=stack().pop();
		stack().push(match((AggregateOrdered)stack()[~1],positions,pat,true));
)
*/
#ifndef nofor
defAutoAsCode("match0+",matchFor_(0,IntMax);)

defAutoAsCode("match01",matchFor_(0,1);)
defAutoAsCode("match1+",matchFor_(1,IntMax);)

defAutoAsCode("nmatch+",matchFor_(POP,IntMax);)

PRIMITIVE matchPlusPlus(Int n1=IntMax){
	

	DROPPUSH(2,matchPlusPlus(n1,TOS3(Aggregate),TOS2(AggregateOrdered),TOS1(obj)));
}

defAutoAsCode(".match++",matchPlusPlus(POP););

defAutoAsCode("0match++",matchPlusPlus(0););
defAutoAsCode("1match++",matchPlusPlus(1););

#endif

defAutoAsCode("onlyFailed",PUSH(onlyFailed(POP));)

//defAutoAsCode("matchFor",uWord n2=POP;uWord n1=POP;matchFor_(n1,n2);)
defAutoAsCode("matchEnd",
		AggregateOrdered positions=(Aggregate)stack().pop();
		stack().push(matchEnd(stack()[~1],positions));
	)

defAsCode(mark,PUSH(mark_(POP));)
defAsCode(markDrop,PUSH(mark_(POP,markDropAction));)
defAsCode(markNip,PUSH(mark_(POP,markNipAction));)
defAsCode(unmark,PUSH(mark_(POP,markDoubleNipAction));)
defAsCode(marked,PUSH(marked(stack()[~2],stack()[~1]));)
defAsCode(advance,PUSH(mark_(POP,advanceAction));)

defAutoAsCode("eliminateDuplicates",TOS=TOS.wcopy();eliminateDuplicates(TOS);)

defAutoAsCode("compilerConf",compilerConf();)
	
defAsCode(ref,stack().push((obj)newDeferred(stack().pop()));)

defAsCode(Bye,exitInterpretLoop=abandon=true;)

//defAsCode("setExcludeFirst",obj{stack().tos()}->excludeFirst=true;)

//defAsCode("asPrototype",((obj)stack().tos()).Class(classPrototype);)

defAutoAsCode("$",stack().push(toString(stack().tos(),tsShortForRef));)

#define binaryOper(name,operation) \
defAutoAsCode(#name,Token t2=stack().popv();Token t1=stack().popv();int r=operation(t1,t2);stack().push(r<<1>>1);)
#define binaryOperInt(name,operation) binaryOper(name,std::operation<Token>())
#define d(name,op) binaryOperInt(name,op)

d(int+,plus) d(int-,minus) d(int*,multiplies) d(int/,divides) d(intMod,modulus) 
d(int>,greater) d(int>=,greater_equal) d(int<,less) d(int<=,less_equal) d(int=,equal_to) d(int<>,not_equal_to)
d(intBitAnd,bit_and) d(intBitOr,bit_or) d(intBitXor,bit_xor) binaryOper(intBit<=,bitLessEqual)

#define compare2(comparison) obj o1=POP;obj o2=POP;PUSH(compare(o1,o2)comparison);
defAutoAsCode("compare",compare2());
defAutoAsCode("=",compare2(==0));

#define operFloat(name,xxx) \
defAutoAsCode(#name,\
	Obj o2=stack().pop();Obj o1=stack().pop();\
	double r= std::xxx(o1.datum_T<double>(),o2.datum_T<double>());stack().push(newDouble(r));\
)

operFloat(double+,plus<double>())
operFloat(double-,minus<double>())
operFloat(double*,multiplies<double>())
operFloat(double/,divides<double>())
operFloat(double^,pow)

def5(curlyOpenT,curlyOpen,".{",
	curlyOpen(stack().pop());
	,std::numeric_limits<executionLevel_t>::max()
)
	
def5(curlyCloseT,curlyClose,".}",
		curlyClose(stack().pop());
		,std::numeric_limits<executionLevel_t>::max()
)

def5(localsUnsetT,autoname,"localsUnset",localsUnset();,1)

def5(tilde_of_level_2,autoname,"~~",
	counter++;
	,2
);

void dokdok(){
	obj o0;obj o1;
	o0.w=5;o1.w=7;
	o0.w+=o1.w;
}

defAutoAsCode("dokdok",dokdok(););
