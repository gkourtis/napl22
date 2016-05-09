
void statusString(std::ostream s){
	static int cnt=0;
	s<<std::setw(6)<<cnt++<<"exS:"<<exS.elements()<<" "<<"stacks:"<<stacks.elements()+1<<" "<<toString(stack())<<std::endl;
}


bool isHttpRequest(String s,String& method,String& path,String& rest){
	static unsigned int cnt=0;obj o;
	char buf[0x100];sprintf(buf,"/home/gk/html/blueFish/%03u.req",cnt++);
	{std::ofstream ofs(buf);ofs<<s;}
	if(!dict.findIn(NAPL(HTTPREQUEST),false,false,false,&o))
		return false;
	Aggregate a1;a1.push(0);AggregateOrdered a;a.push(a1);
	AggregateOrdered r=match(s,a,o);
	if(r.elements()==0)
		return false;
	a=r[0];int i=~7;
	method	=slice(s,a[i],a[i+1]);i+=2;
	path		=slice(s,a[i],a[i+1]);i+=2;
	rest		=slice(s,a[i],a[i+1]);i+=2;
	return true;
}


void interpret(String cmd0){
	// ws indicates if the previous character was a whitespace taking into consideration 
	// starting and ending delimiters
	bool ws=true,ms,b;obj C=0;tString cmd{cmd0};suWord lev;Char curlyOpenCh=u'{';

	for(Char c:cmd){
		if(exitInterpretLoop || c==u'')
			break;
		if(ws){
			newString(0); // create a new stack with execution level 0
			delim=0;
		}
		if(charCounter>0){
			stack().push(c);
			if((ws=(--charCounter==0))){Aggregate a=closeStack();for(Token i:a)stack().push(i);}
		}
		else
			switch(c){
				case u'\\': // one character escape character, the code is the same for u8 u16 u32 u64
					if(charLevel!=0) goto push;
					charCounter=1;ws=false;break;
				case u'«'	: // comment
				case u'¦'	: // alternating pattern string starting character
				case u'“'	: // sequential pattern string starting character 
				case u'`'	:	// string starting character exists in all 
				case u'{'	: // string starting character but may be closed by finding a space
	//			case '['		:	// string starting character exists in all 

ssc:			if(ws || charLevel>0 && c==delim ){
						delim=c;++charLevel;
					}
					if(!ws) goto push;
					ws=false;break;
				
				case u'»':
					ms = delim==u'«';C=classComment;lev=1;goto sec; // sequential pattern string ending character exists only in u16 and above
				case u'|':
					ms = delim==u'¦';C=classPatternAny;lev=1;goto sec;  // alternating pattern string ending character
				case u'”':
					ms = delim==u'“';C=classPatternSeq;lev=1;goto sec;  // alternating pattern string ending character
				case u'\'':
					ms = delim==u'`';C=classString;lev=1;goto sec; // string ending character
	//			case  ']':ms = delim==u'[';C=classString;lev=0;goto sec; // string ending character
				case u'}':
					ms = delim==u'{';C=classString;lev=1;goto sec; // string ending character
			

sec:			// a string end has been found without string start.
					if(!ms) goto push;
					if(--charLevel>0) goto push;
					// we have arrived to charLevel=0 closing the last ' so we have to collect the stack().	
					stack().Class(C);stack().executionLevel(lev);
					oldStack();
					execute(stack().pop());
	//				if(C==classComment) stack.drop();
					ws=true;break;
	//			case L'"': // string starting and ending character
	//				if(charLevel==0)goto ssc;else goto sec;
				default:
					if(c>=0xFFF)
						c=c+1; // I don't really know why the code is like that. May be wrong
					else
	// code here isn't correct if the size of Char doesn't matches the size of wchar_t
						if(charCounter>0 || !std::iswspace(c) && !std::iswcntrl(c)){
	// if the counter is active or it isn't whitespace than push the character	
	push:				stack().push(c);ws=false;break;
						}else{
	// if there isn't any active char level than collect
							if(charLevel==0) goto collect;
	// in the special case that there is a char level but the delim is'nt the special case curlyOpen than just push
							if(delim!=curlyOpenCh) goto push;
	// if the delimiter was curlyOpen we must ripristinate the stack as it should be and aferwards coolect.
							stack().insert(0,curlyOpenCh);delim=0;charLevel=0;

	collect:		b=stack().elements()>0;
							tString s{closeStack()};
							if(b) 
								execute(toObj(s));
							ws=true;
							break;
					}
			}
	}
//std::cout<<"interpret ended"<<std::endl;
}

void serveResponse(const char* p){
	std::stringstream ss;size_t l=strlen(p);
	ss<<
		"HTTP/1.1 200 OK"<<crlf<<
		"Content-Type: text/html; charset=UTF-8"<<crlf<<
		"Content-Encoding: UTF-8"<<crlf<<
		"Content-Length: "<<l<<crlf<<
		"Connection: open"<<crlf<<
		crlf<<
		p;
	write(ss);
}

void serveResponse(const String s){
	char* p=s.mbs();
	serveResponse(p);
	free(p);
}

void interpretLoop(){	
		//String fl=file2String(NAPL(/home/gk/html/blueFish/blueFish2.html));
		//char* fl_=fl.mbs();int n=strlen(fl_);
		//char buf[0x80];
		
s:	try{
			int cnt=0;
			while(!postClear(exitInterpretLoop)){
				//exS.print(L"exLvl: "+to_wstring(E.execution.l)+L":"+to_wstring(E.paren.l)+L" cnt:"+to_wstring(E.counter.l)+L" EX: "+to_wstring(exS.elements()));
				//stack.print(L"Stacks:"+to_wstring(stacks.elements())+L"  Stack:"+to_wstring(stack.elements())+L" ");
				//tracked(String,cmd,=NAPL( ));
				
				String s;
				s=getLine();
				
				if(s.elements()==0) 
					break;
				
				String method,path,rest;
				
				if(isHttpRequest(s,method,path,rest)){
					SWITCHstart
						CASEIF(compare(method,NAPL(GET))==0)
							if(compare(path,NAPL(/))==0)
								path=NAPL(index.html);
							if(compare(path,NAPL(stack.html))==0){
								std::stringstream ss;
								htmlStack(ss);
								serveResponse(ss.str().c_str());
							}
							if(path[0]!='/')
								path=(String)(NAPL(/var/www/html/)+path);
						String filedata=file2String(path);
							serveResponse(filedata);
						DEFAULT
							THROW("Unexpected non GET case in HTTP")
					endSWITCH
				}else{
					interpret(s);
					std::stringstream ss;	std::locale mylocale(""); ss.imbue(mylocale); 
						showStack(ss);
						ss<<std::dec<<"\n"<<std::setw(4)<<cnt++<<" E:"<<execution.l<<" P:"<<paren.l<<" C:"<<counter.l;
						ss<<" charL:"<<charLevel<<" charCounter:"<<charCounter<<" markLevel:"<<markLevel;
						ss<<" exS:"<<exS.elements()<<" stacks:"<<stacks.elements()<<" \t"<<"n:"<<stack().elements()<<" "<<toString(stack())<<" || ";
						for(auto i:stack()) ss<<std::hex<<i<<" ";ss<<std::endl;
						//ss<<std::dec<<" freeMem="<<freeMem()<<" freeVectors="<<freeVectorsLU(freeObj)<<std::endl;
					write(ss);
				}
			}
		}
		catch(std::runtime_error e){
//			if(fl_) free(fl_);
//			puts("Error:",e.what());goto s;
		}
//	if(fl_) free(fl_);
}


class getLine;