/*
obj ignore;
__attribute__((constructor))void defineIgnore(){
	ignore=persistent(newUndefined());
	dict.define(NAPL(ignore),ignore);
}
*/

char* String::mbs(char* d,Word sz) const{
	return Chars2mbs((Char*)dataP(),elements(),d,sz);
}

void String::lower() {
	for(auto &c:*this) 
		c=towlower(c);
}

void String::upper() {
	for(auto &c:*this) 
		c=towupper(c);
}

String newString(Obj Class,const Char* p,uWord maxSize,uWord elements){
	assert0(maxSize>=elements,"newString characters>size");
	return Aggregate(newobj<Char>(classString,(Char*)p,maxSize,elements));
}

template<typename T>
String newStringT(uWord maxSize=0x10,uWord elements=0,Char c=0,const T* cP=nullptr){
	assert0(elements<=maxSize,"newString elements>size");
	String s=newAggregate(classString,(Word*)nullptr,maxSize,elements);
	if(c!=0) for(uWord i=0;i<elements;i++) s.data(i)=c;
	if(cP){
		if(sizeof(T)==sizeof(Char))
			memcpy(
			(void*)s.dataP(),
			cP,sizeof(T)*elements);
		else 
			for(uWord i=0;i<elements;i++) s.data(i)=*(cP++);
	}
	return s;
}

String newString(uWord maxSize=0x10,uWord elements=0,Char c=0){
	return newStringT<Char>(maxSize,elements,c);
}

String newString(uWord maxSize,uWord elements,Char*& dP){
	String s=newStringT<Char>(maxSize,elements);
	dP=s.dataP_T<Char>();
	return s;
}

String newString(const char* mbs,uWord nn=uWordMax){
	size_t sz,nn1;
	if(nn==uWordMax) 
		mbslenghts(mbs,uWordMax-2,nn1,sz); 
	else 
		sz=nn;
	String s=newStringT<Char>(sz+1);
	uWord n=mbs2Chars(s.CharP(),mbs,nn);
	if(n!=-1){
		s.elements(n);
		s[n]=0;
	}else
		THROW("mbs returned -1")
	return s;
}

String newString(std::string s,uWord n=uWordMax){
	return newString(s.c_str(),n);
}

std::string parens(char c,unsigned int times){
	if(times<8) 
		return 	std::string(c,times);
	else 
		return std::string(c,2)+std::string("...")+std::string(c,2);
}


ClassT mct=(ClassT)0x80002213;


/*
String nameOf(obj o,String ss){
	obj r;uWord i;
	if(dict.revFindIn(o,r,i)) return r; else return num2String(o.w);
}
*/

std::ostream& operator<<(std::ostream& os, String s){
	if(	!s.isToken() || 
			!(	s.isInstanceOf(classString) || 
					s.isInstanceOf(classPatternAny) || 
					s.isInstanceOf(classPatternSeq) ||
					s.isInstanceOf(classComment) 
			)
	) THROW("unexpected argument for String s in operator<<")
	char* p=s.mbs();
	if(p){
		os<<p;
		delete[] p;
	}
	return os;
}

//constexpr char colorYellow[0x100]=tparm(set_a_background,4);
//constexpr char colorBlack[]=tparm(set_a_background,0);

char backGroundYellow[]="\x1B[43m";

char allNormal[]="\x1B[0m";

//"\x1B[0m";

enum toStringFlag:Word { tsOnlyName=1,tsShortForRef=2,tsAlsoName=4 };

#define onlyName (flags&tsOnlyName)
#define shortForRef (flags&tsShortForRef)
#define alsoName (flags&tsAlsoName)

String toString(Obj o,Word flags=tsOnlyName|tsShortForRef,Word signalPos=-1,size_t maxChars=0x200){

	std::stringstream s;Aggregate a(o);uWord l;
	const char *ls,*rs;
	bool l0Show=false; // the name doesn't need to be tracked as it is filled with token in the dict
	if(o.isInt())
		s<<o.w;
	else{
		String name{notFound};uWord i;
		bool found=dict.revFindIn(o,name,i);
		obj Class=o.Class();
		
		if(found && (onlyName || (shortForRef && o.reference()) ))
			s<<name;
		else{
			if(found && alsoName) s<<name<<" = ";
l:		SWITCH(Class)
				//case _classClass: s<<"unknown_not_existent_object_with_anchestor_class0";break;
				CASE(classBitString) 
					s<<"problem _classBitString";
				CASE(classUndefined)
					if(found) 
						s<<"Undefined:"<<name; 
					else 
						s<<"Undefined:0x"<<std::hex<<o.w;
				CASE(classPrimitive)
					s<<"P#"<<std::hex<<o.w;
//					s<<keyOf(o,NAPL(Primitive:0x));l0Show=true;

//				CASE(classStruct)
//					Aggregate c{o.Class()};
//					for(i=0;i<c.elements();i++)
//						s<<toString(c[i])<<": "<<toString(((Aggregate)o)[i])<<"  ";
//				CASE(classExtStruct) s<<"ExtStruct:0x"<<std::hex<<o.w;
//				CASE(classFix) s<<"fixedSizeClass:0x"<<std::hex<<o.w;
//				CASE(classInt) 
//					if(o.isToken())
//						s<<std::dec<<o.datum_T<Word>();
//					else
//						s<<std::dec<<o.w;

				CASE(classDeferred) 
					s<<"@"<<toString((obj)o.V().value());l0Show=true;
				CASE(classFloat) 
					s<<o.datum_T<float>();
				CASE(classDouble) 
					s<<o.datum_T<double>(); 
				CASE(classInt64) 
					s<<std::dec<<o.datum_T<int64_t>();
				CASE(classAggregate)
l0:     	l=a.executionLevel();for(obj i:a) l=MAX(l,i.executionLevel());
					switch(l){
//						case 0:ls="[";			rs="]";			break;
						case 0:ls="{";			rs="}";			break;
						case 1:ls="{";			rs="}";			break;
						case 2:ls="{{";			rs="}}";			break;
						case 3:ls="{{{";		rs="}}}";		break;	
					  default:ls="{..{";		rs="}..}";		break;
					}
					s<<ls;
l1:				VT v,vi,f;VB b;
					walkPrototype(a,v,vi,f,b);
					int n=0;
					while(s.tellp()<=maxChars && (v.size()!=0||f.size()!=0 )){
						bool h=false;if(f.size()){h=b.back();b.pop_back();}
						if(!h) s<<" ";
						if(v.size() && vi.back()==0) if(!h && n++>0) s<<"# ";
						if(f.size()){
							if(!h) s<<toString(f.back())<<":";
							f.pop_back();
						}
						if(v.size()){
							bool d=(vi.back()==signalPos);
							if(d) s<<backGroundYellow;
								if(!h){
									s<<toString(v.back());
								}
								v.pop_back();vi.pop_back();
							if(d) s<<allNormal;
							
						}
					l0Show=false;
					}
					s<<" "<<rs<<" ";
				CASE(classString)
					ls="`";rs="'";
l2:				s<<ls<<(String)a<<rs;
				CASE(classPatternSeq)
					ls="“";rs="”";
l3:				for(Word i:a) if(isToken(i)) goto l1; 
					goto l2;
				CASE(classPatternAny)
					ls="¦";rs="|";goto l3;
				CASE(classComment)
					ls="«";rs="»";goto l2;
				CASE(classClassInfo)
					goto l0;
				DEFAULT
					if(Class!=classClass){
						Class=Class.Class();goto l;
					}else
						s<<"/unknown descendant of classClass/";
			endSWITCH
		}
		if(o.executionLevel()>1 || (l0Show && o.executionLevel()==0)) s<<":"<<o.executionLevel()<<" ";
	}
	const char* p=s.str().c_str();
	return newString(s.str(),maxChars);
}

#undef onlyName
#undef shortForRef
#undef alsoName

char* toStringmbs(obj o){return toString(o).mbs();}

char S0[0x400];

char* S(const obj o){
	toString(o).mbs(S0,sizeof(S0));
	return S0;
}

char* S(Token t){return S(obj{t});}
char* SS(){return S(stack());}

/*
String toStringHttp(Obj o,bool onlyName=true,Word signalPos=-1){
	tString s=toString(o,onlyName,signalPos);
	tString t=newString(s.elements()+0x20u);
	for(auto i:s){
		switch(i){
			//case '{':t.insertSlice(NAPL(&#123;));break;
			//case '}':t.insertSlice(NAPL(&#125;));break;
			//case '[':t.insertSlice(NAPL(&#91;));break;
			//case ']':t.insertSlice(NAPL(&#93;));break;
			default: t.push(i);break;
		}
	}
	return t;
}
*/