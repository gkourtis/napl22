enum patternType{all,any};

Aggregate allPos(Aggregate s){
	uWord sn=s.elements();Aggregate r(sn,sn);
	for(uWord i=0;i<s.elements();i++) r[i]=i;
	return r;
}

AggregateOrdered matchEnd(Aggregate s,AggregateOrdered positions){
	AggregateOrdered r;
	if(positions.keyExists(s.elements(),false))
		r.push(s.elements());
	return r;
}

AggregateOrdered match(const Aggregate s,AggregateOrdered positions, Obj pat,AggregateOrdered r=0,Word pn=1){
// p holds the positions array that may have 1 or 2 columns per raw
#define loop(aa,step) for(Word i=0,i1=aa.elements(),p;i<i1 && (p=aa[i],true);i+=step)
	
	switch(pat.Class().w){
		case _classInt:{
			if(!r) 
				r=AggregateOrdered();
			uWord sn=s.elements();
			loop(positions,pn){
				if(0<=p && p+1<=sn && s[p]==pat.w) r.keyExists((Obj)(p+1));
			}
		};
		break;
		case _classAggregate:
			stack.push(positions);
			execute(pat,true);
			r=stack.pop();
			break;
		case _classString:
		case _classPatternAll:{
			AggregateOrdered r1=positions;
			for(auto pat1:(Aggregate)pat)
				r1=match(s,r1,pat1);
			if(!r) r=r1;else r+=r1;
			break;
		}
		case _classPatternAny:
			if(!r) 
				r=AggregateOrdered();
			for(auto pat1:(Aggregate)pat)
				match(s,positions,pat1,r);
			break;
		default:
			assert0(false,"unexpected in matchAll");
			break;
	}
	return r;
}

AggregateOrdered matchFor(Aggregate s,AggregateOrdered positions,Obj pat,uWord n1,uWord n2){
	AggregateOrdered r=positions;
	for(uWord i=n1;i--!=0;){
		r=match(s,r,pat);
		if(r.elements()==0) return r;
	}
	AggregateOrdered r1=r;
	if(n2>=n1)
		for(uWord i=n2-n1;i--!=0;){
			r1=match(s,r1,pat);
			if(r1.elements()==0) return r;
			r+=r1;
		}
	return r;
}
	
void matchFor_(uWord n1,uWord n2){
	Obj pat=stack.pop(),positions=stack.pop();
	stack.push(matchFor((AggregateOrdered)stack[~1],positions,pat,n1,n2));
}
