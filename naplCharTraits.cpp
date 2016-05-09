
class char_traits_Char : public std::char_traits<Char> {
public:
//	typedef Char char_type;
	typedef uWord size_type;
	typedef int32_t int_type;
	typedef Char* pointer; typedef const Char* const_pointer;
	typedef Char& reference; typedef const Char& const_reference;
	//static size_type length(){return size();}
	//static int_type eof(){return int_type(-1);};	

/*
	static constexpr char_type to_char_type (int_type a) noexcept { 
// The int is given by conversion routines that interpret the external codification of characters
// e.g. it could be utf8 or iso8859-7 or any multibyte codification.
// Being a int32_t covers al cases contemplated by unicod actually.
// Depending on our internal type we may be able or not to express that character.
// if the internal type is char8_t only 64 characters are possible.
// if the internal type is char16_t then only 2^14 characters are possible.
// if the internal type is char32_t then all characters are possible.
// if the internal type is char64_t then all characters are possible.		
	return a==0 ? 0 :(a!=(a&WordMask) ? Char(0x3F) : a )|CharMasked;
}
*/
  
  static void assign(char_type& r,const char_type& a){
	r=a;
}
	
static void assign(char_type* p0,size_t n, char_type c){
	for(char_type* p=p0;p<p0+n;p++) assign(*p++,c);
}
	
/*
static char_type* copy(char_type* dest, const char_type* src, size_t n){
	return (char_type*)memcpy(dest,src,sizeof(char_type[n]));
}
*/

	static constexpr int_type to_int_type(char_type c) noexcept {
		return c; //&WordMask;
	}

};
	
typedef std::basic_string< Char,char_traits_Char,Allocator<Char>> SString;

typedef std::basic_ifstream<Char,char_traits_Char> ifStream;
typedef std::basic_ofstream<Char,char_traits_Char> ofStream;
typedef std::basic_fstream<Char,char_traits_Char> fStream;

