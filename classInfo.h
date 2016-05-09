/* 
Classes are divided into 2 categories:
a) raw classes without a body attached to them.
b) normal classes carrying a body

Raw classes have only a method that is applicable to their descendants: the execute method. 
That method is stored in the Addr Field ( as a relative address of a C++ function )

The body of normal classes may carry information about the descendants in the form of a classInfo0 C++ struct
The struct carries information as: 
as execution method, sizeof head of descendant, sizeofelement of descendant, method to calculate the size of tail

Objects descending from objects descending from a class do not need to keep those informations as these derive from 
the anchestor object.

So finally there are 3 type of classes:
a) Raw classes
b) classes having a body with a classInfo structure describing information about descendants
c) classes that do not have a classInfo structure but that derive it from anchestor classes
*/
 
const uWord executionClassesNum=0x100;
/* raw classes that carry only executional behaviour are limited in number and we infer that these are execution
 * classes from the low class number. The maximum number of available execution classes is the executionClassesNum
 * The ... 
*/

//void pushObj(Obj o); 
// declares the function that is used for objects that their execution just leaves them selfs on the stack

typedef uWord (hGeneric::*sizeofData_MP_t)();
typedef void (*ActionP)(Obj o);

class ET;
/*
A classInfo0 structure describes characteristics of the descendants of a particular classInfo.
It is a raw description of it. 
That raw description will be made an anchestor of a high level "object".
An executeFunction is available that always contains the function executed by descendants during execution.
SizeofHeader is directly available as a fixed value. 
The header is guaranted not to be 0 as it always has as size at least the size of the backreference (token).
The SizeofElement is also directly available as a fixed value.
A classInfo could omit it by allowing it to have a predefined special value ( e.g. -1 or 0 ).
the classInfo0 is appended after a header.
*/

const uWord noBody=(uWord)0; //special value to use to indicate that a body doesn't exists.

struct classInfo0{
// the first element of a classInfo0 is the address of a function that is the executional behaviour of a descendant object.
	//void (*executeFunction)(Obj)=nullptr; 
	ActionP executeFunction;
	//void (*executeFunction)(Obj o);
	// executing an undefined executeFunction creates a general exception // =&pushObj; 
// the size of the header of the body. If = to the special value noBody means that there is no header and so no body. 
	uWord sizeofHeader; 
	uWord sizeofElement; 
	uWord sizeofTail;
// while the sizeofHeader is a fixed value 
// the sizeofData is much more flexible as it is calculated using a generic method indicated by a method pointer. 
// The sizeofData usually depends on the sizeofElement and the max number of elements.
// The sizeofData could also be named sizeofTail ( we could decide to change it ).
	union {
// the union is used in order to allow access in two ways to the method pointer
		sizeofData_MP_t sizeofData_MP{nullptr};	// here is a pointer to a method function.
		struct{
			void* sizeofDataP;
			size_t offset0;
		};
	};
	bool hasBody(){return sizeofHeader!=0;}
};

// The structure classInfo0 is included in an agggregate.
// So the classInfo has as header the hAggregate and as body the classInfo0 and eventually also other data

struct classInfo:hAggregate,classInfo0{
	obj Class(){return token;}; // returns the Class of the classInfo
	bool hasHeapBody(){					// returns if the instances related to that classInfo have a body.
		return sizeofHeader!=noBody;
	}
	classInfo(Token t,hAggregate::sz_t sz,hAggregate::sz_t n):hAggregate{t,sz,n}{}
};

// given a class object o, we get a reference to the classInfo structure.
classInfo& classInfos(obj o){
		return *(classInfo*)((ptr)o.V());
}

// As soon as we have created a new class vector and body
// The function bellow initializes the classInfo structure as required 
// given a type of head a type of data and a number of elements data (default 1).

template<typename hT,typename dT,Word n=1>
	void classInfoInit(obj newClass,obj fatherClass,bool fix=false) // the fix indicates if the descendants have fixed size
	{ classInfo& i=classInfos(newClass);
		newClass.Class(fatherClass); // set the Class of new Class as fatherClass.
		
		i.sizeofData_MP= (sizeofData_MP_t)&hT::sizeofData; // the sizeofData method is set getting from the header type = hT.
			
// instead of defining it as a variable it could be a static method,
// the problem is that the compacter could not understand at run time which should be the related static method.

// objects with a data size that could fit inside the ptr are saved directly there ( e.g. shortFloats, etc ).
// So in that case the object hasn't a header and sizeof(hT)=noBody.

// the subsequent calculations are compile time only and no efficency degradation is introduced
		i.sizeofHeader= fix && sizeof(dT)*n<=sizeof(addrT) || sizeof(hT)==0  ?  noBody : sizeof(hT) ;
		i.sizeofElement= ( sizeof(dT)==0 ? noBody : sizeof(dT) );
	};

// bellow is a specialization that is used due to the fact that the null type creates compilation errors to the previous def
template<>
	void classInfoInit<void,void>(obj newClass,obj fatherClass,bool fix)
	{ classInfo& i=classInfos(newClass);
		newClass.Class(fatherClass);
		i.sizeofData_MP=nullptr; // as the descendants haven't a body reference to that function creates a genera exception
		i.sizeofHeader= noBody;
		i.sizeofElement= noBody;
	};

	// init a classInfo with instances that do not have body.
	void classInfoInitDefault(Obj c,Obj C){classInfoInit<void,void>(c,C,true);}
		
	// Given the class <T> init a classInfo with instances that have a hFixBase header and a fixed size datum existing once.
	template<typename T> void classInfoInitFix(Obj c,Obj C){
		classInfoInit<hFixBase<T,uWord>,T>(c,C,true);
	};

// some classes do not have size information, instead only executional behavior
// The definition bellow has a variable number of classes as params and essentially initializes a classInfo according to 
// these parameters. 
// The classInfo has an hAggregate as header and as data Words.
// Their number may be passed (when !=1) when fixed ( due to the flexibility of the definition ).

	template<typename... TT>
	void classInfoInitAggregate(TT... aa){classInfoInit<hAggregate,Word>(aa...);};

/*
obj Class_(obj o){
	if(o!=classClassInfo) return o.Class(); else return o;
}
*/
// given an object o, we backtrace until we find an anchestor of classInfo type that we return as result
// the classClassInfo object has as classInfo the classAggregate.

obj classInfoOf(obj o){
	if(o->isclassInfo) return classAggregate;
	while(o!=classClass){ 
		o=o.Class();
		if(o->isclassInfo) 
			return o;
	}
	return notFound;
}

