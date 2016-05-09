AggregateOrdered ET::match(const Aggregate s,AggregateOrdered positions, Obj pat,AggregateOrdered r){
	SWITCHstart
		CASEIF(pat.isInstanceOf(classInt))
			uWord sn=s.elements();
// looping on the positions 
// if a position is between start and end and the pattern equals 
// the value at pos then add the next position to the results.
			for(auto p:positions){
				if(0<=p && p<sn && pat.w==s[p]) 
					r&=p+1; //&= adds a single object to an ordered aggregate (if it doesn't exists already)
			}
		CASEIF(pat.isInstanceOf(classDouble))
			uWord sn=s.elements();
			for(auto p:positions){
				if(0<=p && p<sn && compare(pat,s[p])==0) 
					r&=p+1; //&= adds a single object to an ordered aggregate (if it doesn't exists already)
			}
		CASEIF(pat.isInstanceOf(classPatternAll))
			AggregateOrdered r1=positions;
			for(auto pat1:(Aggregate)pat)
				r1=match(s,r1,pat1);
			if(r.elements()==0) r=r1;else r+=r1;
		CASEIF(pat.isInstanceOf(classPatternAny))
			for(auto pat1:(Aggregate)pat)
				match(s,positions,pat1,r);
		CASEIF(pat.isInstanceOf(classAggregate))
			stack().push(positions);
			execute(pat,true); // should execute independently from the execution status ? should be explained !
			r=stack().pop();
		DEFAULT
			assert0(false,"unexpected in matchAll");
	endSWITCH
	return r;
}

AggregateOrdered ET::matchFor(Aggregate s,AggregateOrdered positions,Obj pat,uWord n1,uWord n2){
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
	
void ET::matchFor_(uWord n1,uWord n2){
	Obj pat=stack().pop(),positions=stack().pop();
	stack().push(matchFor((AggregateOrdered)stack()[~1],positions,pat,n1,n2));
}

Aggregate ET::anything(Aggregate s,AggregateOrdered positions){
	Word n=positions.elements();
	return n==0 ? allPos(s,n) : allPos(s,positions[0]);
}

void patternExecute(obj pat){
	Obj positions=etP->stack().pop();
	etP->stack().push(etP->match((AggregateOrdered)etP->stack()[~1],positions,pat));
}

Aggregate ET::allPos(Aggregate s,Word startPos){
	uWord sn=s.elements(),n=(startPos<=sn?sn-startPos:0);
	Aggregate r(n,n);
	for(uWord i=0,j=startPos;i<n;i++,j++) r[i]=j;
	return r;
}

AggregateOrdered ET::matchEnd(Aggregate s,AggregateOrdered positions){
	AggregateOrdered r;
	if(positions.keyExists(s.elements(),false))
		r.push(s.elements());
	return r;
}

