#define marked 				(flags & matchMarked)	
#define negated 			(flags & matchNegated)	

AggregateOrdered match(
	const Aggregate s,
	const AggregateOrdered PP,
	const obj pat,
	patternKind patKind,const	cmpFuncP cmpP,const matchFlag flags,
	Int PPi1,Int PPi2,Int pati1,Int pati2
){
	//tobj s0=s,PP0=PP,pat0=pat;Aggregate& Pat=(Aggregate&)pat;
	checkPositions1(PP,__PRETTY_FUNCTION__);
	ADJUSTOFFSET2(PP.elements(),PPi1,PPi2);
	AggregateOrdered RR{0};
	if(PPi1>=PPi2) return RR;
	
	if(marked){
		AggregateOrdered RR1=PP;
		FORstart(PP,Aggregate P,PPi1,PPi2)
			if(!pat.isInt()) 
				THROW("when matchMarked the pattern must be an 'Int' index to the depth of the mark.")
			// the single specific pos at "i" is checked bellow. matchMarked means matchSeq with pattern a slice of s
			RR1=match(s,RR1,s,patSeq,cmpP,(matchFlag)(flags & ~matchMarked),i,i+1,P[pat+markLevel],P[pat+markLevel+1]);
			RR+=RR1;
		endFOR
	}else
		if(pat.isInt()){
			Word sn{s.elements()};
		
			FORstart(PP,Aggregate P,PPi1,PPi2)
				Int p{P.tos()};
				bool f= (0<=p && p<sn && (negated != (*cmpP)(pat,s[p])));
				if(f){
					Aggregate P1=P.clone();
//					if(!negated) P1.tos()=p+1; // if it is negated the match is at the same position
					P1.tos()=p+1;
					RR^=P1;
				}
			endFOR
		}else{
			ADJUSTOFFSET2(((Aggregate)pat).elements(),pati1,pati2);
			if(patKind==patInferred)
				patKind=patKindOf(pat);
			SWITCH(patKind)
				CASE(patSeq)
					RR=slice((Aggregate&)PP,PPi1,PPi2);
					FORstart((Aggregate)pat,obj pat1,pati1,pati2)
						RR=match(s,RR,pat1,patInferred,cmpP,flags);
					endFOR
				CASE(patAny)
					RR=AggregateOrdered{AggregateOrderedNone};
					AggregateOrdered RR1;
					FORstart((Aggregate)pat,obj pat1,pati1,pati2)
						RR1=match(s,PP,pat1,patInferred,cmpP,flags,PPi1,PPi2);
						RR+=RR1;
					endFOR
/*										
				CASE(patNone)
					RR=AggregateOrdered{AggregateOrderedAll};matchFlag f1=(matchFlag)(flags ^ ~matchNegated);
					FORstart(Pat,obj pat1,pati1,pati2)
						RR*=match(s,PP,pat1,patInferred,cmpP,f1,PPi1,PPi2);
					endFOR
*/
				CASE(patExe)
					PUSH(s);
					PUSH(PP);
					executeNow(pat); // should execute independently from the execution status ? should be explained !
					RR=POP;
					POP;
				DEFAULT
					THROW("DEFAULT uncontempled case for pattKind")
			endSWITCH
		}
	return RR;
	}
#undef marked
#undef negated
#undef keepFailures

bool isFailed(Aggregate PP,Int i1=0,Int i2=~0){
	ADJUSTOFFSET2(PP.elements(),i1,i2);
	FORstart(PP,Aggregate P,i1,i2)
		if(P.tos()!=patFail) return false;
	endFOR
	return true;
}

//#define nofor

#ifndef nofor

AggregateOrdered matchFor(
	const Int n1,const Int dn,
	const Aggregate s,const AggregateOrdered PP,obj pat,
	patternKind patKind,
	const	cmpFuncP cmpP,
	const matchFlag flags,
	Int PPi1,Int PPi2,Int pati1,Int pati2
	){
// the routine is intended to make an initial match of at least n1 sequential times continuing to end if matchMad is set
// afterwards any number or dn successive times is accepted
	tobj s0=s,PP0=PP,pat0=pat;
	tAggregateOrdered RR,RR1;
	
// the loop is executed for minimum n1 times exiting with a null aggregate if it doesn't match that number of times
// Afterwards, if the matchMax flag is set continues up to end (null) returns the last not null result.

/*
	for(uWord i=n1;!RR.isEmpty() && (i!=0 || matchMax);RR=RR1){
		RR1=match(s,RR,pat,inferred,cmpP,negated,0);
// only if matchMark is true we may have i==0. In that case if we have null we return having RR set to the previous value.
		if(i!=0) --i; else if(RR1.isEmpty()) break;
	}
*/
	RR1=(AggregateOrdered)PP.wcopy();
	for(uWord i=n1;i!=0 && !RR.isEmpty();--i)
		RR1=match(s,RR1,pat,patInferred,cmpP,(matchFlag)(flags & ~matchKeepFailures)); // here we don't keep failures

// here a minimum result is the same RR1 as it satisfies for 0 times
	RR=(AggregateOrdered)RR1.wcopy();
	for(uWord i=dn;i!=0 && !RR1.isEmpty();--i,RR+=(AggregateOrdered)RR1.clone())
		RR1=match(s,RR1,pat,patInferred,cmpP,flags);
	return RR;
}
#endif 

Aggregate onlyFailed(Aggregate PP0){
	tAggregate PP{},P1;bool f=PP0.readOnly();
	for(Aggregate P:PP0)
		if(P.tos()==patFail){
			// if we are the sole owner we may just copy else we must keep copies
			P1=f? P.rcopy() : P;
			P1.drop();
			PP.push(P1); 
		}
		return PP;
}

#ifndef nofor
Aggregate matchPlusPlus(Int n1,const Aggregate s,const AggregateOrdered PP,const Aggregate pat){
	return onlyFailed(
		matchFor(n1,WordMask,s,PP,pat,patInferred,&equalComparison,matchKeepFailures)
	);
}
	
void matchFor_(const Int n1,const Int n2,const matchFlag flags){
	TAKES(3);
	stack().dropPush(2,
		matchFor(
			n1,n2,
			TOS3(Aggregate),TOS2(AggregateOrdered),TOS1(obj),
			patInferred,
			&equalComparison,
			flags
	));
};
#endif

Aggregate anything(Aggregate s0,AggregateOrdered PP0){
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

void patternExecute(Obj pat){
	tAggregateOrdered RR=match(TOS2(Aggregate),TOS1(AggregateOrdered),pat);
	DROP(1);PUSH(RR);
}

Aggregate allPos(Aggregate s,Word startPos){
	uWord sn=s.elements(),n=(startPos<=sn?sn-startPos:0);
	Aggregate r(n,n);
	for(uWord i=0,j=startPos;i<n;i++,j++) r[i]=j;
	return r;
}

AggregateOrdered matchEnd(Aggregate s,AggregateOrdered positions){
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

enum markAction:Word{markDupAction,markDropAction,markNipAction, markDoubleNipAction, advanceAction};

Aggregate mark_(Aggregate A0,markAction action=markDupAction){
	if(A0==0) return A0;
	tAggregate A = A0.clone();
	for(uWord i=0,n=A.elements();i<n;++i){
		SWITCHstart
			CASEIF(((Aggregate)A[i]).isInstanceOf(classAggregate))
				A[i]=((Aggregate)A[i]).wcopy();
				switch(action){
					case markDupAction:				((Aggregate)A[i]).dup();break;
					case markDropAction:			((Aggregate)A[i]).drop();break;
					case markDoubleNipAction:	((Aggregate)A[i]).nip();
					case markNipAction:				((Aggregate)A[i]).nip();break;
					case advanceAction:				((Aggregate)A[i]).tos()++;break;
				}
		endSWITCH
	}
	switch(action){
		case markDropAction: case markNipAction: case markDoubleNipAction:
			eliminateDuplicates(A);
			break;
		default:
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

