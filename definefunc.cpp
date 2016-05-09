obj definefunc(String s,void (*p)(),int executionLevel=1) {
	obj o;
	if(executionLevel!=1){
		o=newDeferred(primitive2t(p));o.executionLevel(executionLevel);
	}else
		o.w=primitive2t(p);
	dict.define(s,o);
	return o;
}

obj definefunc(const wchar_t* ws,void (*p)(),int executionLevel=1){
	String s{wcs2StringT(ws)};
	return definefunc(s,p,executionLevel);
	
}

/*
obj definefunc(const wchar_t* ws,void (ET::*p)(),int executionLevel=1){
	tString s(wcs2StringT(ws));
	return definefunc(s,p,executionLevel);
}
*/

Token lastToken;

#define defC4(t,nameStr,funcName,level) __attribute__((constructor)) void autoname(){t=definefunc(L##nameStr,&funcName,level);}

#define defC3(nameStr,funcName,level) defC4(lastToken,nameStr,funcName,level)

#define defC1(name) defConstructor(name,name,1)

#define def5(t,funcName,nameStr,code,level) \
	PRIMITIVE funcName(){code} defC3(nameStr,funcName,level)

#define def3(funcName,nameStr,code) def5(lastToken,funcName,nameStr,code,1)

#define defAutoAsCode(nameStr,code) def5(lastToken,autoname,nameStr,code,1)

#define defAsCode(name,code) def5(lastToken,name,#name,code,1)

#define defAssign(nameStr,funcName) defC3(nameStr,funcName,1)

