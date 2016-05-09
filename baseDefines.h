//#include <bits/c++config.h>
//#define _GLIBCXX_FULLY_DYNAMIC_STRING 1
//#include <stdint.h> //#include <stddef.h> //#include <memory>


//typedef void primitive();
typedef void(*primitiveP)();


// chars are considered unsigned - the compilation flag -funsigned-char should be enabled

typedef uint8_t char8_t; typedef uint64_t char64_t; // just for symmetry, char64 may exist when tokens are 64bit.
typedef char agu; // in our model the address granularity unit is the minimum addressable unit by the architecture, we consider that 
typedef agu* ptr;	// the C definition of byte as the minimum addressable unit is problematic. In our model byte is 8bits and
										// the byte could be more than 1 agu ( e.g. nible addressing or even bit addressing ).
typedef agu Void[0]; // Void is used as something with 0 size.

#define aguBits 8 // the addressing granularity is the number of bits of an addressing unit architecture dependent
#define ptrBits (__SIZEOF_POINTER__*aguBits) // may be 32 or 64.

//#define WordBits 32 // may be 8,16,32,64 change by hand !
//#define sWordBits 16 // may be 4,16,32 change by hand !

//#define wordModelSmall
#define wordModelMedium
//#define wordModelBig

#ifdef wordModelSmall
	#define WordBits 16
	#define sWordBits 8
#endif

#ifdef wordModelMedium
	#define WordBits 32
	#define sWordBits 16
#endif

#ifdef wordModelBig
	#define WordBits 64
	#define sWordBits 32
#endif

// we suppose that sizeof instead of giving a size in char units, it gives it in addressGranularityUnits.
#define __SIZEOF_WORD__ (WordBits/aguBits)



#define CharBits WordBits 
//typedef CONCAT(char,CharBits,_t) Char; // may be char8_t or char16_t or char32_t or char64_t
const int WordHexChars=WordBits/4;

typedef CONCAT(int,WordBits,_t) Word;

typedef CONCAT(uint,WordBits,_t) uWord;
typedef CONCAT(uint,sWordBits,_t) suWord;

typedef Word Token; // the token is an offsetted index in a Vector's array

const Word WordMask= uWord(-1)>>2; // we reserve two bits for the class_tag

//~(Word)0)<<(WordBits-2));

// the macro STR(xxxxx) permits us to write strings with characters of 8 bit 16 bit 32 bit (later on 64 bits) depending on what has 
// been choosen

#if CharBits == 8
	typedef char Char;
	#define _STR(s) (#s)
	#define __STR(s) ((Char*)(#s))
	#define _CHR(s) (__STR(s)[0])
	#define CHR(s) (_CHR(s))
#elif CharBits == 16
	typedef char16_t Char;
	#define _STR(s) (u ## #s)
	#define __STR(s) ((Char*)(u ## #s))
	#define _CHR(s) (__STR(s)[0])
	#define CHR(s) _CHR(s)
#elif CharBits == 32
	typedef wchar_t Char; // architecture dependend
	#define _STR(s) (U ## #s)
	#define __STR(s) ((Char*)(U ## #s))
	#define _CHR(s) (__STR(s)[0])
	#define CHR(s) (_CHR(s))
#elif CharBits == 64

#define STRv(v) STR(v)

char64_t* operator"" _char64_t(const char * p,size_t n){
	char64_t* a=new char64_t[n+1];
	for(size_t i=0;i<n;) a[i++]=*p++;
	a[n]=0;return a;
}
	#define _STR(s) (#s ## _char64_t)
	#define __STR(s) ((Char*)(#s ## _char64_t))
	#define _CHR(s) (__STR(s)[0])
	#define CHR(s) _CHR(s)

#endif 


// The system compiles in 3 modes for x86 architectures
// -m32 is 32 bit mode with 32 pointers and 32 bit integers and 32 bit longs
// -m64 is 64 bit mode with 64 bit pointers and 32 bit integers and 64 bit longs
// -mx32 is 64 bit mode with 32 bit pointers and 32 bit integers

// In the language "object tokens" are an index into the table of vectors.
// Tokens contain both "object tokens" and "integer tokens" and have the same size.
// The integer of the language is different from the integer of the compiler and the size of 
// tokens and thus of "integers tokens" is defined independently of the size of integers of the compiler.
// 2^8 2^16 or 2^32 or 2^64 bits is partitioned between tokens and integers.
// Integers occupy the space of small integers that depending on the size are:
// 8bit case: Int: -0x40 to 0x3F Token: 0x40 to 0x7F and Char: -0x80 to -0x41 
// Accordingly are 16bit 32bit and 64bit case
// Integers have the two most significant bits equal ( both 1 or both 0 ) instead Tokens have
// the two most significant bits 10 and characters 01.

// C++ by definition sets sizeof(char)=1. But imposes too that the the char should contain at least 8 bits.
// That is a limitation for achitectures of eg 4bits or even bit by bit addressing.
// So we choose a different interpretation where the granularity of addressing is undefined
// the Sizeof gives it's result in that addressing granularity and so Sizeof(char) may be != 1 .
// the definition bellow is architecture dependent.
// in an architecture where there exists bit addressing should be: #define Sizeof(x) (8*sizeof(x))


const Char primitiveMax=(Char)std::numeric_limits<Word>::max();
const Char primitiveMin=(Char)std::numeric_limits<Word>::max()/2+1; // division rounds down

typedef Word Token; // the token is an offsetted index in a Vector's array
const Token TokenMin=(Token)std::numeric_limits<Word>::min();
const Token TokenMax=(Token)std::numeric_limits<Word>::min()/2-1;

typedef int64_t bitString;

typedef Word Int; // our INT uses a range of TokenBits-1 bits.
const Int IntMin=(Int)std::numeric_limits<Word>::min()/2; // even division 
const Int IntMax= (Int)std::numeric_limits<Word>::max()/2; // division by 2 rounds down

//attention to definitions bellow are misleading
const uWord uWordMax=std::numeric_limits<uWord>::max();
const Word WordMax=std::numeric_limits<Word>::max();

// the index in the table of tokens, is limited by the max address space related to the size of the 
// vect structure, and to the max number expressable with a token in it's half space.

constexpr Token i2t(Word i){return (Token)(i + TokenMin);}
constexpr Word t2i(Token t){return (Word)(t - TokenMin);}


constexpr Word TokenMasked=TokenMin & ~WordMask;
constexpr Word primitiveMasked=primitiveMin & ~WordMask;


bool isNumber(Word w){return w<<1>>1==w;}

bool isToken (Word w){
	bool r=(w&~WordMask)==TokenMasked;
/*
	if(r)
		if(!(i2t(0)<=w&&w<i2t(vvElements))){
			std::cout<<"stop in baseDefines::isToken";
			return false;
		}
*/
	return r;
}

bool isValue(Word w){return isNumber(w);}
bool isPrimitive  (Word w){return (w&~WordMask)==primitiveMasked;}

typedef Token executionLevel_t;typedef void* vptr;

constexpr size_t primitivesAlignement=0x8;
void primitive0();

Word primitive2t(primitiveP p){
	size_t d=(ptr)p-(ptr)&primitive0;
	if(d%primitivesAlignement==0)
		return primitiveMasked|(uintptr_t)d/primitivesAlignement;
	std::cout<<"Problem in primitive conversion to pointer p="<<std::endl;
	return 0;
}
primitiveP t2primitiveP(Word w){return (primitiveP)((ptr)&primitive0+w*primitivesAlignement);}

// heads are allocated sequentially on the heap memory. After the head the object's body of data is allocated
// The body may have any size in bytes. Allignement may be requested on specific memory boundaries depending on the processor.
// the alignement defined should be any of 1,2,4,8,16,32,64 etc.

/*
const size_t alignement=1; // may be 1,2,4,8,16,32,64 etc
size_t align(size_t size){return (size-1+alignement)&~(alignement-1);}
*/

// if addrT has a size smaller than (void*) when extension of addrT happens in order to be
// addeed to an address then it should be negativelly extended due to the implementation.
// In order to support at least half of the addressable space we should at least sign extend it 
// during conversion to a larger size.
// If we need to support all the addressable space, than the simple solution is to use a 
// zero extended addrT ( so an unsigned type ) but to change the mm memory in order to 
// accomodate a negative high prefix.

// addrT is an offseted address into the heap.

typedef uint32_t addrT;

// debugging notes: the code doesn't work under the architecture m64.

// the addrT may be set as uint8_t, uint16_t, uint32_t.
// the problem is that in that case the addresss of functions should fit in that space
// and that isn't always the case.
// In case we need to do it, we may use the Token as an index into a table of primitives
// and align those primitives t some fixed multiple of 2.
// in that way, these primitives would have an implied class and an execution level of 1

// Explanation of readOnly: When an object is created as a unique instance, we may reuse it's space as a new 
// object when that is necessary ( e.g. when creating a String and using it in an operation ).
// When the object is created thus the flag is false.
// If a copy of the object is taken than the flag is set and modifications must create a clone where to operate.

// As there is duality between values and references, when a vector is used for a variable the reference flag is set
// and so the index of the vector in the stack indicates the reference to the vector and not the value contained.
// Instead values pushed should have the reference flag cleared, and so indicate the value itself.

std::stringstream errss;

#define PRIMITIVE void __attribute__((aligned(primitivesAlignement)))  

