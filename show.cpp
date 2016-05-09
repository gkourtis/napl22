String oout(obj o){
	return o.isInt() 
		?	concatenateStrings(NAPL([0x),Int2String(o.w,8,16),NAPL(]))
		: toString(o,0);
}

bool noPrimitiveClass(obj Class){
	return Class!=classPrimitive && Class!=classClass;
}

void dictShow(bool showAll=false){
	std::stringstream ss;int n;
	ss<<"\n\n";
	for(uWord i=0;i<dict.elements();i+=2){
		n=ss.str().size();
		auto p=ss.str();
		if(showAll || noPrimitiveClass(obj(dict[i+1]).Class())){
			ss<<std::setw(4)<<std::right<<i/2<<": ";
			ss<<std::left<<std::setw(20)<<(String)dict[i]<<" : "<<oout(dict[i+1])<<std::endl; 
		}
	}
	write(ss);
}

using namespace std;

#define ww 20 

void showObjss(stringstream& ss,Obj o){
	bool f=o.isToken();Word wc=WordBits/4,wa=sizeof(addrT)*2+2;
	obj C=o.Class();
	ss<<hex<<right<<setw(wc)<<o.w<<"|";
	tString v{dict.nameOf(C)};
	ss<<setw(20);
	if(v!=notFound) ss<<v; else ss<<"";
	ss<<"|";
	ss<<setw(wc)<<hex<<C.w;
	ss<<"|";

	if(o.isToken()){
		vect& v=o.V();	
		ss<<(v.readOnly 			? " X |" : "   |");
		ss<<(v.reference 			? " X |" : "   |");
		ss<<(o.hasBody() 			? " X |" : "   |");
		ss<<(v.fields ? (v.hideFields ? " H |" : " V |") : "   |");
		ss<< setw(7)<<v.executionLevel<<"|";
		ss<< right<<setw(wa)<<v.addr0<<" |";
	}else{
			int w=3+4+8+3+wa+2;char s[w+1];memset(s,'-',w);s[w]=0;s[0]=' ';s[w-1]=' ';
		ss<<"   |   |   |   |       |";
		ss<<right<<setw(wa+2)<<"|";
	}
	tString n{(String)dict.nameOf(o)};
	tString s{toString(o,0)};

	SWITCH(C)
		CASE(classInt,classDouble,classFloat,classBitString,classPrimitive)
			if(n==notFound) 
				ss<<setw(ww)<<right<<s;
			else{
				const String inter=NAPL(_=_);
				int w=MAX(0,ww-n.elements()-s.elements()-inter.elements());
			}
		DEFAULT
			//ss<<setw(ww)<<left<<s; /* the code doesn't works with multibyte characters, so the patch bellow is adopted */
			ss<<tparm(set_a_foreground,6)<<left;
			if(n!=notFound) ss<<n<<" = ";
			ss<<s;
			int l=s.elements() + (n==notFound? 0 : n.elements());
			if(l<ww) 
				ss<<string(ww-l,' ');
			ss<<tparm(set_a_foreground,0);
	endSWITCH
	ss<<endl;
}

enum alignT{
	alignDisable=-1,alignLeft=0,alignCenter=1,alignRight=2
};

std::string encap(std::stringstream& ss,std::string tag,std::string s,alignT alignement=alignDisable){
	const char* al[]={ "left","center","right" };
	ss<<"<"<<tag;
	if(alignement!=alignDisable) ss<<"align="""<<al[alignement]<<"""";
	ss<<">"<<crlf<<
		s<<crlf<<
	"</"<<tag<<">"<<crlf;
}

std::string encap(std::stringstream& ss,std::string tag,String s,alignT alignement=alignDisable){
	const char* al[]={ "left","center","right" };
	ss<<"<"<<tag;
	if(alignement!=alignDisable) ss<<"align="""<<al[alignement]<<"""";
	ss<<">"<<crlf<<
		s<<crlf<<
	"</"<<tag<<">"<<crlf;
}

void showObjssHtml(stringstream& ss,Obj o){
	bool f=o.isToken();
	constexpr Word wc=WordBits/4,wa=sizeof(addrT)*CHAR_BIT/4;constexpr char cw='0'+wc,ca='0'+wa;
	constexpr char formatw[]={ '%' , cw , 'X' , '\0'};
	constexpr char formata[]={ '%' , ca , 'X' , '\0'};
	obj C=o.Class();char dd[0x20];
	sprintf(dd,formata,o.w);
	encap(ss,"td",dd,alignRight);

	tString v{dict.nameOf(C)};
	ss<<setw(20);
	if(v!=notFound)
		encap(ss,"td",(String)v,alignLeft);
	else
		encap(ss,"td","");
	encap(ss,"td",C.w,alignRight);

	if(o.isToken()){
		vect& v=o.V();	
		encap(ss,"td",v.readOnly ? "X":"",alignCenter);
		encap(ss,"td",v.reference ? "X":"",alignCenter);
		encap(ss,"td",o.hasBody() ? "X":"",alignCenter);
		encap(ss,"td",v.fields ? (v.hideFields ? "H" : "V") : "",alignCenter);
		encap(ss,"td",to_string(v.executionLevel),alignRight);
		sprintf(dd,formata,v.addr0);
		encap(ss,"td",v.addr0,alignRight);
	}else{
		for(int i=6;i--!=0;)
			encap(ss,"td","");
	}
	tString n{(String)dict.nameOf(o)};
	tString s{toString(o,0)};

	SWITCH(C)
		CASE(classInt,classDouble,classFloat,classBitString,classPrimitive)
			if(n==notFound) 
				encap(ss,"td",s,alignRight);
			else{
				encap(ss,"td",n+NAPL(_=_)+s,alignRight);
			}
		DEFAULT
			String s1= n!=notFound ? n+NAPL(_=_)+s : s;
			encap(ss,"td",s1,alignLeft);
	endSWITCH
	ss<<endl;
}

void showStack(stringstream& ss){
	Word wc=sizeof(Word)*2,wa=sizeof(addrT)*2+2;
	ss<<endl;
	ss<<tparm(set_a_foreground,5)<<enter_underline_mode;
	ss<<setw(8)<<"  Token |";
	ss<<setw(20)<<"  className    "<<"|";
	ss<<setw(wc)<<"classId "<<"|";
	ss<<"R/O|REF|Bdy|fld|  ExL  |";
	ss<<setw(wa+2)<<"Addr    |";
	ss<<string(ww,' ')<<endl;
	ss<<exit_underline_mode<<tparm(set_a_foreground,0);
	for(auto i:stack()) 
		showObjss(ss,i);
}

#undef ww


void htmlStack(stringstream& ss){
	std::stringstream tt;
	tt<<"<tr>"<<crlf;
		encap(tt,"th","Token");
		encap(tt,"th","className");
		encap(tt,"th","classId");
		encap(tt,"th","R/O");
		encap(tt,"th","REF");
		encap(tt,"th","Bdy");
		encap(tt,"th","fld");
		encap(tt,"th","ExL");
		encap(tt,"th","Addr");
		encap(tt,"th","Value");
	ss<<"</tr>"<<crlf;
	
	ss<<"<tr>"<<crlf;
	for(auto i:stack()) 
		showObjss(ss,i);
	ss<<"</tr>"<<crlf;

	encap(ss,"html",
		encap(ss,"body",
			encap(ss,"table",tt.str())
		)
	);
}


void showStack(){
	stringstream ss;std::locale mylocale(""); ss.imbue(mylocale); 
	showStack(ss);write(ss);
}

int print(Obj o){
	return write(toString(o).mbs());
}

void compilerConf(){
	std::stringstream ss;
	ss<<std::dec;
	ss<<" sizeof(vect)="<<sizeof(vect);
	ss<<" sizeof(addr)="<<sizeof(addrT);
	ss<<" sizeof(Class)="<<sizeof(vect::Class);
	ss<<" Compiled with:"<<compiler;
	#if __x86_64__ || __ppc64__
		ss<<" in 64bit mode";
	#else
		ss<<" in 32bit mode";
	#endif
	ss<<std::endl;
	write(ss);
}

