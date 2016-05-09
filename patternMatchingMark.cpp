AggregateOrdered ET::match(
	const Aggregate s, 										// the aggregate that is checked for pattern matching ( usually a string )
	AggregateOrdered positions, 					// the current positions
	Obj pat,															// the patern to apply
	AggregateOrdered r, 									// an optionally filled aggregete where results are accumulated
	const cmpFuncP cmpP,									// the function to use for checking for match between CHARs and CHARpat
	bool negated
){
	SWITCHstart
		CASEIF(pat.isInstanceOf(classInt))
			uWord sn=s.elements();
// looping on the positions 
// if a position is between start and end of the subject and the pattern CHAR matches 
// the CHAR value at pos then match is true and we should add the next position to the results.
// The positions should be an aggregate of stacks where the TOS of them is the current position
			for(Aggregate P:positions){
				Word p=P.tos();
				if(0<=p && p<sn && (*cmpP)(pat,s[p])){
					Aggregate Q=P.clone();
					Q.tos()+=1;
					r^=Q; //&= adds a single object to an ordered aggregate (if it doesn't exists already)
				}
			}
/*  MUST BE CORRECTED AS ABOVE !!!
		CASEIF(pat.isInstanceOf(classDouble))
			Word sn=s.elements();
			for(auto p:positions){
				if(0<=p && p<sn && compare(pat,s[p])==0) 
					r&=p+1; //&= adds a single object to an ordered aggregate (if it doesn't exists already)
			}
*/
		CASEIF(pat.isInstanceOf(classPatternSeq))
// when we have a sequential pattern each element of the pattern is sequentially applied 
// to the walking changing positions
			AggregateOrdered r1=positions;
			for(auto pat1:(Aggregate)pat)
				r1=match(s,r1,pat1,AggregateOrdered{},cmpP);
// the final result is accumulated into r as that is requested from the calling protocol
			r+=r1;

		CASEIF(pat.isInstanceOf(classPatternAny))
// when the pattern is alternative it's elements are applied to the initial positions and the union of the 
// results of each application is the final result. "r" is the result where we accumulate
			for(auto pat1:(Aggregate)pat)
				match(s,positions,pat1,r,cmpP);

		CASEIF(pat.isInstanceOf(classPatternAll))
			AggregateOrdered r1=AggregateOrderedAll;
			for(auto pat1:(Aggregate)pat)
				r1*=match(s,positions,pat1,AggregateOrdered{},cmpP);
				r+=r1;

		CASEIF(pat.isInstanceOf(classPatternNone))

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
		r=match(s,r,pat,AggregateOrdered{});
		if(r.elements()==0) return r;
	}
	AggregateOrdered r1=r;
	if(n2>=n1)
		for(uWord i=n2-n1;i--!=0;){
			r1=match(s,r1,pat,AggregateOrdered{});
			if(r1.elements()==0) return r;
			r=r+r1;
		}
	return r;
}
	
void ET::matchFor_(uWord n1,uWord n2){
	Obj pat=stack().pop(),positions=stack().pop();
	stack().push(matchFor((AggregateOrdered)stack()[~1],positions,pat,n1,n2));
}

Aggregate ET::anything(Aggregate s,AggregateOrdered positions){
	//return n==0 ? allPos(s,n) : allPos(s,positions[0]);
	Word n=s.elements();tAggregateOrdered P{};
	for(auto& i:positions){
		obj o=i;
		SWITCHstart
			CASEIF(o.Class()==classInt)
				for(Word j=i;j<=n;j++)
					P^=j;
			CASEIF(o.Class()==classAggregate)
				Aggregate a=o;
				for(Word j=a.tos();j<=n;j++){
					Aggregate a1=a.clone();
					a1.tos()=j;
					P^=a1;
				}
		endSWITCH
	}
	return P;
}

void patternExecute(obj pat){
	Obj positions=etP->stack().pop();
	etP->stack().push(etP->match((AggregateOrdered)etP->stack()[~1],positions,pat,AggregateOrdered{}));
}

Aggregate ET::allPos(Aggregate s,Word startPos){
	uWord sn=s.elements(),n=(startPos<=sn?sn-startPos:0);
	Aggregate r(n,n);
	for(uWord i=0,j=startPos;i<n;i++,j++) r[i]=j;
	return r;
}

AggregateOrdered ET::matchEnd(Aggregate s,AggregateOrdered positions){
	AggregateOrdered r;Word n=s.elements();
	for(obj i:positions)
		SWITCHstart
			CASEIF(i.isInstanceOf(classInt))
				if(i==n) 
					r^=i;
			CASEIF(i.isInstanceOf(classAggregate))
				Aggregate a=i;
				if(a.elements() && a.tos()==n)
					r^=a;
		endSWITCH
/*
	if(positions.keyExists(s.elements(),false))
		r.push(s.elements());
*/
	return r;
}

Aggregate matchMark(Aggregate A0){
	tAggregate A= A0.readOnly() ? A0.clone() : A0;
	for(auto& ii:A){
		obj i=ii;
		SWITCHstart
			CASEIF(i.isInstanceOf(classAggregate))
				Aggregate a=i;a.dup();	
			CASEIF((obj)i.isInstanceOf(classInt))
				Aggregate a{};a.push2(i,i);ii=a;
		endSWITCH
	}
	return A;
}

Aggregate marked(Aggregate s,Aggregate A0){
	tAggregate R{};
	for(Aggregate a:A0)
		if(a.isInstanceOf(classAggregate)){
			Aggregate r{};R.push(r);
			for(Word i=0;i+2<=a.elements();i+=2)
				r.push(slice(s,a[i],a[i+1]));
		}
	return R;
}
