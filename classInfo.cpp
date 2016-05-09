// creates a new class that inherits from fatherClass
inline obj newClass(Obj fatherClass){
	obj o=reserveClass();
		o.Class(fatherClass);
		o.executionLevel(0);
	return o;
}

/*   
The classInfo data must reside in an integer number of dT related to the aggregate in order to be able to define 
an integer numer of dT that determines it's size.
So in case classInfo doesn't fit in an exact multiple size then it must be rounded up.
*/

template<typename dT>
const size_t classInfo0Elements=roundUpDivision(sizeof(classInfo0),sizeof(dT));

template<typename dT>
const size_t classInfo0PadSize=classInfo0Elements<dT>*sizeof(dT)-sizeof(classInfo0);

/* 
A typical classInfo structure is used besides other in order to hold:
a)	the address of the function used to execute instances of a specific class
b)	the size of the header of instances
c)	the size of the data elements of the object
d)	a pointer to a method function that allows to calcuate the size of the body tail of the instance

		
	obj o=newClass(fatherClass);o->fields=fields;
	
	typedef classInfo::el_t el_t; // classInfo contains hAggregate. hAggregate has element_t.
		totalElements=MAX(totalElements,classInfo0Elements<el_t>);
		initialElements=MAX(initialElements,classInfo0Elements<el_t>);
// classInfo is hAggregate+classInfo0
// So in order to allocate we have to allocate the size of classInfo plus the 
// and subtract the classInf0elements
		o->addr(allocate0(sizeof(classInfo)-sizeof(classInfo0)+sizeof(el_t)*totalElements)); // gk20160102
		classInfo &i=*(classInfo*)(o->operator ptr());
		new (&i) classInfo(o.w,totalElements,initialElements);
			i.executeFunction=&pushObj;
			i.sizeofData_MP=(sizeofData_MP_t)&T::sizeofData;
// if the data held in the object have a fixed size and that size may be contained inside the addrT 
// then no body is created and the data is held directly inside the addrT
			i.sizeofHeader= fix && sizeof(el_t)<=sizeof(addrT) || sizeof(T)==0  ?  noSize:sizeof(T)-sizeof(el_t) ;
			i.sizeofElement= ( sizeof(el_t)!=0 ? sizeof(el_t) : noSize );
		//}
	return o;
}

template<typename T>	obj newFixClassInfo(Obj fatherClass){
 
The function creates a fixed size classInfo structure. The classInfo structure is a specific Aggregate.
The fatherClass is given. 

	size_t totalElements;
	obj o=newClass(fatherClass);
	
		totalElements=classInfo0Elements<hAggregate::el_t>;
// classInfo is hAggregate+clDebug session ended
assInfo0
// So in order to allocate we have to allocate the size of classInfo plus the 
// and subtract the classInf0elements

The size of the header of instances may not be 0 as that size includes the token backreference
So if it is 0 it means that no body exist ( actually the same function is implemented by checking instead of 0 for -1)
As objects may be formed from dataElements the size of a single dataElement is saved as the sizeofElement
Fixed size objects, will contain thus a fixed size dataElements
Variable size objects, will contain thus a variable size dataElements
The pointer to the method function, is used in order to use the above data and calculate the size of the data portion
All those data are mainly used from the memory compacter.
*/

const uWord noSize=0;


// the newClassInfo depending on parameter T and creates a classInfo and it's data are initialized according to T
template<typename T>	obj newClassInfo(
	Obj fatherClass,bool fields=false,bool isClassInfo=false,size_t totalElements=0,size_t initialElements=0,ActionP aP=NULL
){
/* 
The function creates a classInfo structure. The classInfo structure is a specific Aggregate.
The fatherClass is given. 
A bool fields is used in order to signal that the classInfo contains too fields (not only data)
A bool fix is used t			o specify if the instances have fixedSize (not variable as Aggregates are)
*/
	obj o=newClass(fatherClass);o->fields=fields;o->isclassInfo=isClassInfo;o->reference=true;
	
	//typedef classInfo::el_t el_t; // classInfo contains hAggregate. hAggregate has el_t.
	// a classInfo is created only when no fields exist 
	
	// Asserting fields it means we need an empty aggregate to fill later on with fieldNames
	// so when fields is asserted than no values are saved into the aggregate and it is left empty.
	if(!fields){
		totalElements=MAX(totalElements,classInfo0Elements<classInfo::el_t>);
		initialElements=MAX(initialElements,classInfo0Elements<classInfo::el_t>);
	}
// classInfo is hAggregate+classInfo0
// So in order to allocate we have to allocate the size of classInfo minus classInfo0 plus the number of elements.
		o->addr(allocate0(sizeof(classInfo)-sizeof(classInfo0)+sizeof(classInfo::el_t)*totalElements)); // gk20160102
		classInfo &i=*(classInfo*)(o->operator ptr());
		new (&i) classInfo(o.w,(classInfo::sz_t)totalElements,(classInfo::sz_t)initialElements);
		if(!fields){
			void (*p)(ET*,Obj);
			i.executeFunction=aP;
			i.sizeofData_MP=(sizeofData_MP_t)&T::sizeofData;
			
// if the data held in the object have a fixed size and that size may be contained inside the addrT 
// then no body is created and the data is held directly inside the addrT
			i.sizeofHeader= T::fixSize && sizeof(typename T::el_t)<=sizeof(addrT) || sizeof(T)==0  ?  noSize : T::sizeofownHeader;
			i.sizeofElement=sizeof(typename T::el_t);
			T t{0}; // is defined only for formal compilation reasons
			i.sizeofTail = T::fixSize ? t.sizeofData() : noSize;
		}	
	return o;
}


template<> obj newClassInfo<void>\
(Obj fatherClass,bool fields,bool isClassInfo,size_t totalElements,size_t initialElements,ActionP aP){return 0;};

// 0 just reserves a vector of a class with no body
// F initializes creates an object with classInfo0 for fixed body classes, total size is fixed
// A creates a data object with classInfo0 tail and minimal necessary size. No fields are foreseen.
// B creates an object with empty tail for fields some preserved size is alloted and the classInfo should contain fields.

// Parameters to newClassInfo are: 

// Obj fatherClass,bool fields,bool isClassInfo,size_t totalElements,size_t initialElements,ActionP aP

#define Cl(L,className,fatherClass,T,action) \
	__attribute__((init_priority(103))) Obj className{\
		#L[0]=='0' ? newClass(fatherClass) :\
		#L[0]=='A' ? newClassInfo<T>(fatherClass,	false,	true,		0,		0,	action):\
		#L[0]=='B' ? newClassInfo<T>(fatherClass,	true,		false,	0x8,	0,	action):\
		#L[0]=='F' ? newClassInfo<T>(fatherClass,	false,	true,		0,		0,	action) : obj{0}\
	};
		
	classInfoNames

#undef Cl

