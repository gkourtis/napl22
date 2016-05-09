// The final output to the file, doesn't depens on the underlying type of streambuf.
// Depending on that type in text mode different conversions may happen that transform the 
// streambuf underlying type into a simple char.
// We use a wchar_t stream_buf and we finally read write in the code page supported by the system.

typedef std::basic_ifstream< wchar_t,std::char_traits<wchar_t> > ifstreamW;

String istream2String(std::ifstream &is,Char c=0){
	if(!is) 
		return String(undefined.w);
	else{
		uWord sz_s=std::numeric_limits<hAggregate::sz_t>::max()-2;
		//uWord sz_s=uWordMax-2;
		is.seekg(0,std::ios::end);
		std::streamoff l=is.tellg();
		if(l!=-1)
			if(sz_s>=is.tellg()) sz_s=is.tellg();is.seekg(0,std::ios::beg);
			char* mbs=new char[sz_s+2];
				is.read(mbs,sz_s);
				if(c) mbs[sz_s++]=c;mbs[sz_s++]=0;
				String r=newString(mbs);
			delete[]mbs;
		
/*
			String r=newStringT<wchar_t>(sz_d+2,sz_d); // it may be compiled using <wchar_t> given that the type of t is wchar_t*
			sz_d=mbs2Chars((Char*)r.dataP(),s,sz_s);
			r.elements(sz_d);
*/
			if(c) r.push(c);
			
//			res=newStringT<wchar_t>(sz+1,sz,0,s); // it may be compiled using <wchar_t> given that the type of t is wchar_t*
		return r;
//		f.close(); should be done automatically on destruction, check that we may rely and report.
	} 
}

// when doing text (as opposed to binary) IO a conversion happens between the internal and external character sets
// the same conversion could happen for the filenames but it is essentially undefined the 
// conversion between wcstombs and filename codepage.
// Bellow we suppose a utf8 codepage for filenames.
/*
String istream2String1(std::istream& is,Char c=0){
	const Word bSz=0x20;char b[bSz];String r=newString();bool f;
	if(!is) 
		return String(undefined.w);
	else
		do{	is.readsome(b,sizeof(b));size_t sz=is.gcount();
				if(sz==-1) return r;
				r.checkResize(sz);
				Char* d=(Char*)r.dataP()+r.elements();char* s=b;
				for(;f=mbReadCharWrite(s,d,sz)>0;){};
				r.elements((Word*)d-r.dataP());
		}while(f==0);
	return r;
}
*/

String file2String(const String fn, char c=0){
	char* d=fn.mbs();
		std::ifstream fs(d,std::ios_base::in|std::ios::binary); // ifStream accepts mbs if the underlying charset is unicode
	delete d;
	return istream2String(fs,c);
}

