
Char Int2Digit(Int i){
	assert0(0<=i && i<='9'+1-'0'+'Z'+1-'A',"Int2digit out of range");   
	return i<10?'0'+i:'A'+(i-10);
}

Int Digit2Int(Char d,Int base){
	//assert0('0'<=d && d<='9' || 'A'<=d && d<='Z',"digit2Int Char out of range");
	Int r;
	if('0'<=d && d<='9') r=d-'0';
	else if('A'<=d && d<='Z') r=d-'A'+10; else r=-1;
	if(r>=base)r=-1;
	return r;
}

String Int2String(Int n,Int digits,Int base,bool sign){
// Convert creating at least a result with digits number of digits, begins with least significant digits
	if(sign){sign=n<0;if(n<0)n=-n;}
	String s=newString(digits+sign,0);
	for(Word i=0;i<digits||n!=0;i++) {s.push(Int2Digit(n%base));n/=base;} 
	if(sign) s.push(CHR(-));
	return s;
}

String Int2hexString(Int i){return Int2String(i,sizeof(Token)*aguBits/4,0x10);}

bool String2Int(String s,int &r,Word base){
	int i=0;bool negative=false,inverted=false;
	switch(s[0]){
		case L'+':i++;break;
		case L'-':i++;negative=true;break;
		case L'~':i++;inverted=true;break;
	}
	long long num=0;
	if(i==s.elements()) return false;
	for(;i<s.elements();i++){
		Word d;d=Digit2Int(s[i],base);if(d<0) return false;
		num=num*base+d;
		if(!negative){
			if(num>IntMax) return false;
		}else{
			if(-num<IntMin) return false;
		}
	}
	r=num;if(negative) r=-r;if(inverted)r=~r;return true;
}

bool String2Double(String s,double& d){
	// will be defined in the future using patterns
	char *p0=s.mbs(),*p;
		d=strtod(p0,&p);
		bool b=(*p==0);
	delete[] p0;
	return b;
}	

