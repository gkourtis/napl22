// An aggregate with ordered unique key
/*
bool AggregateOrdered::findIn(
	tObj key,
	bool insert,bool replace,bool asUndefined,
	obj *oP,uWord *iP,
	Word cols,Word keyCol,Word valueCol) const
{ 
// Searches for a key 
// if insert is true, and the key isn't found, it is adeed 
// if the key is found and replace is true, the value is replaced with the given value
// If both insert is true and replace is true, both actions happen
// If the asUndefined flag is true, the value to assign when a value must be set, is 
// a newly created object of class classUndefined.
// If the asUndefined flag is false, the value to assign when a value must be set, is the object pointed by oP.
// If the key is found or inserted, the position where the group of key,value,etc line was found is returned via the iP pointer.
// If not found false is returned and *iP indicates a position greater than the searched value.
// *iP is returned as an index in the array
	uWord i;bool b;
	assert0(0<=keyCol && keyCol<cols,"col out of range in AggregateOrdered::findIn");
	assert0(elements()%cols==0,"The elements divided cols not exact AggregateOrdered::findIn");
	int i1,i2,r,raws=elements()/cols;
	for(i1=0,i2=raws;i=(i1+i2)/2,i1!=i2; ){
		r=compare(key,(Aggregate)(*this)[i*cols+keyCol]);
		if(r>0) i1=i+1;
		else if(r<0) i2=i; 
		else{
			if(replace) goto l;
			if(oP) *oP=(*this)[i*cols+valueCol]; b=true;goto e;
		}
		// i1 includes i2 excludes.In case t>a[] then i1=i+1 to exclude if t<a[] then it is sufficient to set i2=i;
	}
	if(insert){
		ins_backdel(i*cols,cols);
		(*this)[i*cols+keyCol]=key.w;
l:		tobj o1(0); if(asUndefined){ o1=newUndefined();if(oP) *oP=o1;} else if(oP) o1=*oP;
		if(cols>=2)(*this)[i*cols+valueCol]=o1.w;
		b=true;
	}else b=false;
e:	if(iP) 
		*iP=i*cols;
	return b;
}
*/
bool AggregateOrdered::findIn_(
	Word keysNum,Obj keys[],
	bool insert,bool replace,bool asUndefined,
	obj *oP,uWord *iP,
	Word cols,Word keyCol,Word valueCol) const
{ 
// Searches for a key 
// if insert is true, and the key isn't found, it is addeed 
// if the key is found and replace is true, the value is replaced with the given value
// ** attention: regarding the dictionary that isn't an acceptable behaviour ** see comments
// If both insert is true and replace is true, both actions happen
// If the asUndefined flag is true, the value to assign when a value must be set, is 
// a newly created object of class classUndefined.
// If the asUndefined flag is false, the value to assign when a value must be set, is the object pointed by oP.
// If the key is found or inserted, the position where the group of key,value,etc line was found is returned via the iP pointer.
// If not found false is returned and *iP indicates a position greater than the searched value.
// *iP is returned as an index in the array

	if(keys[0]==0x3)
		printf("stopHere");

	uWord i;bool b;
	assert0(0<=keyCol && keyCol<cols,"col out of range in AggregateOrdered::findIn");
	assert0(elements()%cols==0,"The elements divided cols not exact AggregateOrdered::findIn");
	int i1,i2,r=-1,raws=elements()/cols;
	for(i1=0,i2=raws;i=(i1+i2)/2,i1!=i2; ){
		for(Word k=0;k!=keysNum;k++){	
			r=compare(keys[k],(Aggregate)(*this)[i*cols+keyCol+k]);
			if(r>0){i1=i+1;break;}
			if(r<0){i2=i;break;}
		}
		if(r==0){
			if(replace) goto l;
			if(oP) *oP=(*this)[i*cols+valueCol]; b=true;goto e;
		}
	}
		// i1 includes i2 excludes.In case t>a[] then i1=i+1 to exclude if t<a[] then it is sufficient to set i2=i;

	if(insert){
		ins_backdel(i*cols,cols);
		for(Word k=0;k!=keysNum;k++)	
			(*this)[i*cols+keyCol+k]=keys[k];
l:		obj o1; 
			if(asUndefined)
				{ o1=newUndefined();if(oP) *oP=o1;} 
			else 
				if(oP) o1.w= oP ? oP->w : 0;
		if(cols>keysNum)(*this)[i*cols+valueCol]=o1.w;
		b=false;
	}else b=false;
e:	if(iP) 
		*iP=i*cols;
	return b;
}

bool AggregateOrdered::findIn(
	Obj key,																		// the key to search for
// should insertion happen if unexistent ? should replacement happen ? should a newly created undefined object be assigned ?
	bool insert,bool replace,bool asUndefined,	
	obj *oP,uWord *iP,
	Word cols,Word keyCol,Word valueCol) const
	{	Obj keys[]={key};
		return findIn_(1,keys,insert,replace,asUndefined,oP,iP,cols,keyCol,valueCol);
	}

bool AggregateOrdered::keyExists(Obj key,bool insert) const{
	switch(key.w){
		case AggregateOrderedAll: return true;
		case AggregateOrderedNone: return false;
		default:
			return findIn(key,insert,false,false,nullptr,nullptr,1,0,0);
	}
}

/*
bool AggregateOrdered::keysExists(Obj key1,Obj key2,bool insert) const{
	Obj keys[]={key1,key2};
	return findIn_(countof(keys),keys,insert,false,false,nullptr,nullptr,1,0,0);
}
*/

bool AggregateOrdered::revFindIn(Obj value,obj& name,uWord &i,Word cols,Word keyCol,Word valueCol) const{
	for(i=0;i<elements();i+=cols)
		if((*this)[i+valueCol]==value.w){
			name=(*this)[i+keyCol];return true;
		}
	return false;
}

obj AggregateOrdered::nameOf(Obj value) const{
	obj o{notFound};uWord i;
	revFindIn(value,o,i);
	return o;
}

obj AggregateOrdered::valueOf(Obj key) const{
	obj o{notFound};
	findIn(key,true,false,true,&o);
	return o;
}

/*
void AggregateOrdered::define_(Obj key,Obj o0) const{
// define assigns for the first time a value to a non existent name		
	assert0(findIn(key,true,true,false,(obj*)&o0),"Problem In define");
}
*/

void AggregateOrdered::define(Obj key,obj v0) const{
	const bool insert=true,replace=false,asUndefined=false;
	auto v=v0;uWord i;
	if(!key.isInstanceOf(classString))
		THROW("define key isn't an aggregate");
		puts("Defining: ",(String)key);
	bool b=findIn(key,insert,replace,asUndefined,&v,&i);
	if(b) 
		v0.cloneIn(v);
}

__attribute__((init_priority(104))) AggregateOrdered dict;

// Aggregate comparison is defined as the sequential comparison of each constituent.
// Elementary types are equal if their values match.
//bool operator==(const obj o1,const obj o2){return true;};

String Int2String(Int n,Int digits=1,Int base=10,bool sign=false);

String keyOf(Obj o,String prefix){
	obj key{notFound};uWord i;
	if(dict.revFindIn(o,key,i)) 
		return key;
	else 
		return concatenateSlices0(2,true,prefix,(Word)0,(Word)~0,Int2String(o.w,WordBits/4,16),(Word)~0,(Word)0);
		// return concatenateStrings(prefix,Int2String(o.w,8,16));
}

//AggregateOrdered::AggregateOrdered(Aggregate a){for(auto i:a) keyExists(i);}

template <bool unionf,bool yes1=true,bool yes2=true>
AggregateOrdered union_intersection(AggregateOrdered a1,AggregateOrdered a2){
// if any of a1,a2 is the number 0 then it is interpreted as the empty Aggregate
// if any of a1,a1 is the number -1 then it is interpreted as the all containing Aggregate
	tobj a10=a1,a20=a2;
	if(a1.w == (unionf?AggregateOrderedNone:AggregateOrderedAll)) return a2;
	if(a2.w == (unionf?AggregateOrderedNone:AggregateOrderedAll)) return a1;

	uWord n1=a1.elements(),n2=a2.elements();
	tAggregateOrdered r{};
	uWord i1=0,i2=0;
	
	while(i1<n1 && i2<n2){
		switch(compare(a1[i1],a2[i2])){
			case -1:if(unionf && yes1) r.push(a1[i1]);		++i1;break;
			case +1:if(unionf && yes2) r.push(a2[i2]);		++i2;break;
			case  0:if(yes1&&yes2) r.push(a1[i1]);			++i1;++i2;break;
		}
	}
	if(unionf && yes1) while(i1<n1) r.push(a1[i1++]);
	if(unionf && yes2) while(i2<n2) r.push(a2[i2++]);
	return r;
}

// operator of union of aggregates is + 
// operator of intersection between aggregates is * 
// assignement union is done using +=
// assignement intersection is done using *=
// assignement singleElement insertion should be done using ^=

 
AggregateOrdered operator+(AggregateOrdered a1,AggregateOrdered a2){
	return union_intersection<true>(a1,a2);
}

AggregateOrdered operator-(AggregateOrdered a1,AggregateOrdered a2){
	return union_intersection<true,true,false>(a1,a2);
}

AggregateOrdered operator*(AggregateOrdered a1,AggregateOrdered a2){
	return union_intersection<false>(a1,a2);
}

AggregateOrdered operator/(AggregateOrdered a1,AggregateOrdered a2){
	return union_intersection<false,true,false>(a1,a2);
}

AggregateOrdered operator+=(AggregateOrdered &a1,AggregateOrdered a2){return a1=a1+a2;}
AggregateOrdered operator-=(AggregateOrdered &a1,AggregateOrdered a2){return a1=a1-a2;}

AggregateOrdered operator*=(AggregateOrdered &a1,AggregateOrdered a2){return a1=a1*a2;}

AggregateOrdered operator^=(AggregateOrdered &a,obj o){
	if(!a) {
		a=AggregateOrdered{};
		a.push(o);
	}else
		a.keyExists(o,true);
	return a;
}

AggregateOrdered eliminateDuplicates(AggregateOrdered a){
	uWord n=a.elements();
	if(n>=2)
		for(uWord i=0,j=0;++j<n AFTERFOR(a.elements(++i));)
			if(compare(a[i],a[j])!=0)
				a[++i]=a[j];
	return a;
}
