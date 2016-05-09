const Word VECT_COUNTER0=__COUNTER__+1;
#define Cl(x1,className,x3,x4,x5) className = TokenMin+__COUNTER__-VECT_COUNTER0,
	enum class ClassT:Word { 
		classInfoNames
	};
#undef Cl

#define Cl(x1,className,x3,x4,x5) #className,
const char* classNames[]={
	classInfoNames
};
#undef Cl

const char* className(ClassT c){
	return classNames[(uWord)c<<1>>1];
}



#define executionLevelBits 0
// the order of the fields allows for a fast access to some of the data (actionId and executionLevel)
#define 	bitFields \
	bf(actionId,8) bf(executionLevel,executionLevelBits) \
	bf(collect,1) bf(readOnly,1) bf(reference,1) \
	bf(multipleInheritance,1) bf(fields,1) bf(isclassInfo,1) bf(hideFields,1)  //bf(any,1)

// the code betwen #define #undef counts the number of defined bits
#define bf(fieldName,bits) + bits
	const int bitsUsed = bitFields;
#undef bf

#undef executionLevelBits

// the vect holds a class, an address, an execution level, and bit fields
//struct __attribute__((packed)) vect{

const int minExecutionLevelBits=2;
const int remainingBits=sWordBits-bitsUsed%sWordBits;
const int executionLevelBits=(remainingBits>=minExecutionLevelBits ? remainingBits : sWordBits);


struct vect{
	enum ClassT Class;
	addrT addr0;
#define bf(fieldName,bits) suWord fieldName:bits;
	bitFields
#undef bf
	
// methods
	inline ptr addr(const void* p);
	inline addrT addr(const addrT q);
	
	inline operator ptr();
	inline operator addrT();
	
	inline Token& value(){return *(Token*)&addr0;}

	void vectss(std::stringstream &ss){
		ss<<"Class:"<<className(Class)<<" ";
		ss<<"addr0:"<<addr0<<" ";
#define bf(fieldName,bits) ss<<#fieldName<<":"<<fieldName<<" ";
		bitFields
#undef bf
	}

	#define fa(T,name) T name(){return (T)operator ptr();}
		fa(hGeneric*,hGenericP)
		fa(hAggregate*,hAggregateP)
		fa(hAggregateS*,hAggregateSL)
		fa(hAggregateL*,hAggregatePL)
		fa(hFix*,hFixP)
		fa(hVarBase<agu>*,hVarBaseP)
		fa(primitiveP,executeP)
	#undef fA
} __attribute__((__packed__));

#undef bitFields

void __attribute((constructor)) message_vect0(){
	std::cout<<" sizeof(Word)="<<sizeof(Word)<<" sizeof(addrT)="<<sizeof(addrT)<<" sizeof(vect)="<<std::dec<<sizeof(vect)<<"\n";
}
