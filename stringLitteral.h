// here we define a litteral string (given in utf8 format) that creates a string in napl
// and gives as litteral the Token of it.

// creates a string in the garbage-collected-heap, returns the results in Token t,Char*& dataP. Size
class String;
String newString(uWord reservedElements,uWord actualElements,Char*& dataP); 

String wcs2StringT(const wchar_t* s0,size_t n=0){
	if(n>hAggregate::max_size) return wcs2StringT(L"");
	Char c,*d;if(n==0) n=wcslen(s0);
	String t=newString(n+1,n,d);
	for(auto s=s0;s<s0+n;s++,d++){
		c=*s & WordMask;if(c!=*s) c=CHR(@);*d=c;
	} *d=0;
	return t;
}

inline  int mbReadCharWrite(char* &s,Char* &d,size_t &sz){
	wchar_t w;int r=mbtowc(&w,s,sz);
	if(r>0){
		s+=r;sz-=r;*d++= (w&WordMask)==w ? (w&WordMask) : CHR(@) ;
	}
	return r;
}

__attribute__((init_priority(106))) std::map<const char32_t*,String> StringLitterals;

String operator"" _napl(const char32_t* s0,size_t n){
	// the input is always a utf32 string written as U" xxx " or using the _napl suffix.
	// the above should be converted first to ucs4 and afterwards to our internal rappresentation 
	// Char ( char8_t char16_t char32_t ).
	String& s=StringLitterals[s0];
	if(s.elements()!=n) 
		s=wcs2StringT((wchar_t*)s0,n);
//	s.keepAlive(true);
	return s;
	#if   CharBits == 8
	#elif CharBits == 16
	#elif CharBits == 32
	#elif CharBits == 64
	#endif 
}

#define NAPL(s) (U###s##_napl)

const String emptyString=NAPL();

