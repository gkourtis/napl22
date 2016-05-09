uWord ET::Pos ET::matchSingle(
	Aggregate s,
	uWord sn,
	Aggregate pat,
	uWord p,
	PatternKind patKind=patInferred,
	cmpFuncP cmpP=equalComparison,
	bool negated=false
){uWord r;
	if(mp.patKind==patInferred)
		mp.patKind=patKindOf(mp.pat);

	switch(patKind)
		case patInt:
			if( 0<=p && p<mp.sn && (negated != (*mp.cmpP)(mp.pat,mp.s[p]))){
				r=p+1;
			}
		case patSeq:
			for(Aggregate pat1:pat){
				if( (p=matchSingle(s,sn,pat1,p)==uWordMax) ) break;
			}

// when we have a sequential pattern each element of the pattern is sequentially applied 
// to the walking changing positions
			if(matchMarked){
				bool f=true;
				for(Aggregate P:PP){
					Word d=pat;if(!P.adjustIsInternal(d)){f=false;break;}
				}
				if(f){
					RR=AggregateOrdered{};
					for(AggregateOrdered P:PP){
						Word t=slice((Aggregate)s,P[pat],P[pat+1]);
						tAggregate s1=(Aggregate)t;
						tAggregateOrdered PP1{};;PP1.push(P.rcopy());
						RR+=match(s,PP1,s1,seq,equalComparison,false,false);
						// alternatively to that it may be checked if the whole string is equal -- faster method !!!
					}
				}
				
				//						if(!PP.isEmpty() && ((Aggregate)PP[0]).adjustOffsetIsInternal() )
//						pat1=slice(s,)
			}else{
				RR=(AggregateOrdered)(PP.clone());
					for(auto pat1:(Aggregate)pat)
						RR=match(s,RR,pat1,inferred,cmpP,negated,false);
			}
// when the pattern is alternative it's [New Thread 0xf6db4940 (LWP 18090)]
elements are applied to the initial positions and the union of the 
// results of each application is the final result. "r" is the result where we accumulate
				CASE(any)
					RR=(AggregateOrdered) AggregateOrderedNone;
					for(auto pat1:(Aggregate)pat)
						RR+=match(s,PP,pat1,inferred,cmpP,negated,0);
				CASE(none)
					if(Pat.elementary()){
						RR=AggregateOrdered{};uWord sn=s.elements();
						for(Aggregate P:PP){
							Word p=P.tos();
							if(p<sn && negated!=!Pat.in(s[p])){
								Aggregate P1=P.clone();P1.tos()++;
								RR^=P1;
							}
						}
					}else{
						RR=(AggregateOrdered) AggregateOrderedAll;
						for(auto pat1:(Aggregate)pat)
							RR*=match(s,PP,pat1,inferred,cmpP,!negated,0);
					}
				CASE(exe)
					stack().push(PP);
					execute(pat,true); // should execute independently from the execution status ? should be explained !
					RR.w=stack().pop();
				DEFAULT
					assert0(false,"unexpected in matchAll");
			endSWITCH
	endSWITCH
	return RR;
}





AggregateOrdered ET::match(matchParameters1){ 
	return match(s,PP,pat,inferred,equalComparison,false,0);
}

AggregateOrdered ET::matchFor(Int n1,Int dn,matchParameters1,matchParameters2,bool matchMax){
// the routine is intended to make an initial match of at least n1 sequential times continuing to end if matchMad is set
// afterwards any number or dn successive times is accepted
	tobj s0=s,PP0=PP,pat0=pat;
	tAggregateOrdered RR{PP},RR1;
	
// the loop is executed for minimum n1 times exiting with a null aggregate if it doesn't match that number of times
// Afterwards, if the matchMax flag is set continues up to end (null) returns the last not null result.

/*
	for(uWord i=n1;!RR.isEmpty() && (i!=0 || matchMax);RR=RR1){
		RR1=match(s,RR,pat,inferred,cmpP,negated,0);
// only if matchMark is true we may have i==0. In that case if we have null we return having RR set to the previous value.
		if(i!=0) --i; else if(RR1.isEmpty()) break;
	}
*/

	for(uWord i=n1;i!=0;!RR.isEmpty();--i)
		RR=match(s,RR,pat,inferred,cmpP,negated,0);

if(matchMax){
	for(Aggregate R:RR){
		
	}
}
	



// here a minimum result is the same RR as it satisfies for 0 times
	RR1=RR;
	for(uWord i=dn;i--!=0 && !RR1.isEmpty();RR+=RR1)
		RR1=match(s,RR1,pat,inferred,cmpP,negated,0);
	return RR;
}

/*	
void ET::matchFor_(Int n1,Int n2,matchFlag flags=0){
#define oPop (obj)(stack().pop())
	tObj pat=oPop,positions=oPop;
	assert0(stack().elements()>=1,"matchFor lacks sufficient arguments");
	stack().push(
		matchFor(
			n1,n2,
			(AggregateOrdered)stack()[~1],
			positions,
			pat,
			inferred,
			&equalComparison,
			flags
		)
	);
}
*/
Aggregate ET::anything(Aggregate s0,AggregateOrdered PP0){
	//return n==0 ? allPos(s,n) : allPos(s,positions[0]);
	tAggregate s=s0,PP=PP0;tAggregateOrdered P{};
	Word n=s.elements();
	for(Aggregate a:PP){
		SWITCHstart
// the code bellow remains from when the positions where a single level aggregate
/*			CASEIF(o.Class()==classInt)
				for(Word j=i;j<=n;j++)
					P^=j;
*/
			CASEIF(a.isToken() && a.Class()==classAggregate)
				tAggregate a1{0};
				for(Word j=a.tos();j<=n;j++){
// it isn't necessary to use tAggregate because the insertion in P allows for the alive state					
					Aggregate a1=a.clone();
					a1.tos()=j;
					P^=a1;
					//checkCompact(M,false);
				}
		endSWITCH
	}
	return P;
}

void ET::patternExecute(Obj pat){
	tObj positions=oPop;
	DROPPUSH(2,match(TOS2(AggregateOrdered),TOS1(Aggregate),pat));
}

Aggregate ET::allPos(Aggregate s,Word startPos){
	uWord sn=s.elements(),n=(startPos<=sn?sn-startPos:0);
	Aggregate r(n,n);
	for(uWord i=0,j=startPos;i<n;i++,j++) r[i]=j;
	return r;
}

AggregateOrdered ET::matchEnd(Aggregate s,AggregateOrdered positions){
	tAggregateOrdered r;Word n=s.elements();
	for(obj i:positions)
		SWITCHstart
			CASEIF(i.isInstanceOf(classInt))
				if(i==n) 
					r^=i;
			CASEIF(i.isInstanceOf(classAggregate))
				tAggregate a=(Aggregate)i;
				if(a.elements() && a.tos()==n)
					r^=a;
		endSWITCH
/*
	if(positions.keyExists(s.elements(),false))
		r.push(s.elements());
*/
	return r;
}

enum markAction:Word{markDupAction,markDropAction,markDrop2Action};

Aggregate mark_(Aggregate A0,markAction action=markDupAction){
	tAggregate A = A0.wcopy();
	for(uWord i=0,n=A.elements();i<n;++i){
		SWITCHstart
			CASEIF(((Aggregate)A[i]).isInstanceOf(classAggregate))
				A[i]=((Aggregate)A[i]).wcopy();
				switch(action){
					case markDupAction:((Aggregate)A[i]).dup();break;
					case markDrop2Action:((Aggregate)A[i]).nip();
					case markDropAction:((Aggregate)A[i]).nip();
					break;
				}
		endSWITCH
	}
	switch(action){
		case markDropAction: case markDrop2Action:
			eliminateDuplicates(A);
			break;
		case markDupAction:
			break;
	}

/*
			CASEIF((obj)i.isInstanceOf(classInt))
				Aggregate a{};a.push2(i,i);ii=a;
*/
	return A;
}

Aggregate marked(Aggregate s,Aggregate A0){
	tAggregate R{};
	for(Aggregate a:A0)
		if(a.isInstanceOf(classAggregate)){
			tAggregate r{};R.push(r);
			for(Word i=0;i+2<=a.elements();i+=2)
				r.push(slice(s,a[i],a[i+1]));
		}
	return R;
}
