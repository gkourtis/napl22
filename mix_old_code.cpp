// the conversion of a Token to a String converts numbers to their decimal representation, Strings to the way 
// of entering them, Aggregate data are searched in the dictionary for an existing name and if existent that
// name is given.
/*
String properToken2String(Token t,Word minLevel=1){
	Token t1;bool isA=isAggregate(t);String r;
s:if ((!isA) || (minLevel<=0)){
		if(findInDict0(t,r,1,0,false)) 
			;
		else 
			if(isA){
				minLevel++;goto s;
			}else{
				vect& v=vv(t);
				switch(vv(t).getClass()){
					case classDouble:r=toString1(valueRead<double>(v));break;
					case classInt64:r=toString1(valueRead<int64_t>(v));break;
					case classBitString:r=toString1(valueRead<bitString>(v));break;
					default: 
						r=newString();
						r_.push(CHR(U'{'));
						r_.append(Int2hexString(t)); // the result must be released
						r_.append(Napl(:UnknownXXX}));
						break;
				}
			}
	}else{
		if(isString(t)){
			r_.push('`');r_.append(t);r_.push('\'');
		}else{
			aggFor(t,\
				if(r_.elements()>=0) r_.push(U' ');
				r_.append(Token2String0(w,MAX(0,minLevel-1)));
			);
			//wr=addParenLeftRight(wr,obj(t).v().executionLevel);
		}
	}
	return r;
}

String token2String0(Token t,int level){
	String r;
	if(isNumber(t))
		r=Int2String(t);
	else if(WordMask&t==TokenMasked) 
		r=properToken2String(t);
	else
		assert0(false,"token2String0 unexpected");
	return r;
}

bool isVarSz(Token t){return vv(t).getClass()==classVar;}

//bool isFixSz(Token t){Token f=vv(t).Class;return classPrimitiveId<=f && f<=classFixSzId	;}

// Attention between Aggregate and aggregate

*/

/* not needed as aggregate comparison does it

Word compareStrings(String& s1,String &s2){
// returns 1 when t1>t2 and -1 when t2>t1 while 0 when are equal
	int n,n1,n2,i,r;
	for(i=0,n=MIN(n1=s1.elements(),n2=s2.elements());i<n;i++) if((r=(s1[i]-s2[i]))!=0) break;
	return sign(i<n ? r : n1-n2);
}

String definitionsOfDict(bool tokenHex=false){
// In a dictionary the first element is the string the second the value
	const size_t sizeLimit=0x4000;
	string r=newString();
	for(uWord i=0;i<dict.elements();i+=2){
		Word t=dict.at(i+1);   // the value
		String s=dict.at(i); // the string
		string r,r1;
		if(tokenHex){r.append(Int2hexString(t));r.append(Napl(:));}
		String sp10=newString(0x10,0x10,' ');
		r.append(s);	if(s_.elements()<0x10) r.append(sp10,s_.elements(),~0);
		if(isToken(t)){
			r.append(Napl(L:));r.append(toString(vv(t).executionLevel));
			switch(Token c=classOf(t)){
				case classPrimitive:	r1=Napl(Primitive);break;
				case classAggregate:	r1=Token2String0(t,1);break;
				case classRaw:{
					Word vv.value;
					if(isNumber(v))			ws2=L"Int : " +to_wstring((int)o.v().addr);
					else if(isChar(v))	ws2=L"Char: \\" + wstring(1,(wchar_t)(v&~mask11));else ws2=L"unknown:";

					break;
				}
				case classFloat: ws2=token2wstring0(t1,L"Float: ",0);break;
					
				default:
					ws2=L"Class: "+classInfos[t2i(c)].name+L" unable to show content";
			}
		}else{
			if(isNumber(t1))	ws2=L"Int: "+to_wstring(t1);else if(isChar(t1)) ws2=L"wchar: "+wstring(wchar_t(t1),1);else ws2=L"No-Int-Char?";
		}
		ws+=ws1+L" "+ws2+L"\n";
	}
	return ws+L"\n";
}
*/


/*
void executeOld(Token t,bool forceFirst=false,bool ignore_t=false){
// data are taken from the exS
// If ignore_t is true then t and forceFirst are ignored and the routine just continues 
// execution resuming by using values from the execution stack.
	bool started=false;aggregate a;currentSecondaryP=a.t;int i=0,n=0;int d=0;head0 *p;
	d=exS.elements();if(!ignore_t) goto l2;
	
	while(d<exS.elements()){
	  exS.pop2(a.t,(Token&)i);
	for(;exS.elements()>MIN(d;exitLevel;)exS.drop(2);
	if(exitLevel==exS.elements())exitLevel=maxInt;
  while(exS.elements()>d){
		if(exitLevel<d)
		
		
		
// here we ignore t and forceFirst and we normally resume execution using the exS
	if(exitLevel<d){d=exitLevel;exitLevel=maxInt;} // if there is an exit command reset the command.
	while(exS.elements()>=d) exS.pop2(a.t,(Token&)i);
	d=exS.elements();while((int)exitStack.tos()>d)exitStack.pop();
	l1:	n=a.ha()->n;
	// attention to shortCircuit, mustContinue must be first
		while(exitLevel==maxInt && i<n ){ 


			t=a.ha()->a[i++];
		l2:		
			if(dontExecuteCounter>0){dontExecuteCounter--;goto passive;}
			if(!isToken(t))goto passive;
			p=&(H.v.v[t2i(t)]);
			if(forceFirst) forceFirst=false; else{if(p->executionLevel<executionLevel) goto passive;}
			switch(p->father){
				case classPrimitiveId: ((void(*)())p->addr)();break; 	//execute primitive function
				case classRawId: stack.push(p->addr);break; 						// push raw data
				case classAggregateId:
					if(started) exS.push2(a.t,i);
//					exS.print(L"exS:["+to_wstring(exS.depth())+L"] ");
					a.t=t;i=0;started=true;goto l1;
				case classClassId: goto passive;
				case classMethodId:
				{	aggregate b(t);int i;
					for(i=0;i<b.elements();i+=2) if(methodActionApplicable(b[i])){execute(b[i+1],true);break;}
					if(i==stack.elements()) goto passive;break;
				}
				default: 
passive:	stack.push(t);
			}
		}
	}
}
*/

void executePrimitive(Obj o){
	switch(o.Class().w){
		case _classPrimitive: (o.executeP)();break;	//execute primitive function
		//case classRaw:stack.push(t);break;  					// push raw data
/*
		case classMethod:
		{	aggregate b(t);int i;
			for(i=0;i<b.elements();i+=2) if(methodActionApplicable(b[i])){
				execute(b[i+1],true);break;
			}

			if(i<b.elements()) break;// else continue to passive behaviour			
		}
*/
		case _classClass: // continue to passive
		default: stack.push(o.w);
	}
}

/*
void define(Obj name,obj o){
	dict.findIn(name,true,false,false,&o);
}
*/


/*
int getValue(obj& o){
s:if(o.isToken()){
		switch(o.Class()){
			case classInt: =o.datum();goto s;
			default: assert0(true,"getData problem");
		}
	}
	return t;
}
*/

void print2stacks(){
	Aggregate a=(Aggregate)stacks.tos();
	stack.print(L"current stack:");
	a.print(L"old stack:");
}

/*

wstring wcwd(){
	char* s0=getcwd(NULL,0);
		wstring ws(MAXPATHLEN,0x0);int l=mbstowcs(&ws[0],s0,MAXPATHLEN);
	free(s0);
	ws.resize(l);return ws;
}


Token cwd(){
	Token t,Char* p;newString(r,p,MAXPATHLEN+1;
}
*/


// --------------------------------------------------------------

/*
wstring addParenLeftRight(wstring ws,int level){
	wstring wr,opening[]={L"[",L"{",L"{{",L"{{{",L"{{{{"},closing[]={L"]",L"}",L"}}",L"}}}",L"}}}}"};
	if(level<(int)countof(opening)){
		wr=opening[level]+L"_ "+ws+L" _"+closing[level];
	}else{
		wr=to_wstring(level)+L" ?{_ "+wr+L" _} ";
	}
	return wr;
}
*/



