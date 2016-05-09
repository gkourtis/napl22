void checkPositions1(const Aggregate &PP,const char* m){
	for(Aggregate P:PP){
		if(!P.isInstanceOf(classAggregate)) 
			THROW("the positions aren't an aggregate instance "<<m);
		for(obj p:P)
			if(!p.isInt())
				THROW("the positions elements aren't integers "<<m);
	}
}

patternKind patKindOf(Aggregate pat){
	patternKind r;
	SWITCHstart
		CASEIF(pat.isInstanceOf(classInt))					r=patInt;
		CASEIF(pat.isInstanceOf(classPatternSeq))		r=patSeq;
		CASEIF(pat.isInstanceOf(classPatternAny)) 	r=patAny;
		CASEIF(pat.isInstanceOf(classPatternNone))	r=patNone;
		CASEIF(pat.isInstanceOf(classAggregate))		r=patExe;
		CASEIF(pat.isInstanceOf(classPrimitive))		r=patExe;
		DEFAULT 
			THROW("unable to deduce patterKind for pat.Class()="<<std::hex<<pat.Class().w)
	endSWITCH
	return r;
}
