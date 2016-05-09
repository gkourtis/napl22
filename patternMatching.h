struct Pos{
	Aggregate pos; // it may not be an aggregate but instead an integer
	Pos(Obj o){pos=o;}
	
	void operator++(){
		if(pos.isInt())
			pos.w++;
		else{
			pos=pos.wcopy();
			pos.tos()++;
		}
	}
	
	void mark(){
		if(pos.isInt()){
			Word p=pos;
			pos=Aggregate{};
			pos.push2(p,p);
		}
		else
			pos.dup();
	}
	void unmark(){
		if(pos.isInt())
			THROW("uncorrect to unmark as element is a single integer and not an aggregate")
		else{
			if(pos.elements()<=1)
				THROW("uncorrect to drop last postion from position aggregate")
			pos.drop();
		}
	}
};

typedef bool (*cmpFuncP)(Char pat,Char c);

#define matchParameters1\
 const Aggregate s,													/* the aggregate that is checked for pattern matching ( usually a string )	*/\
 const AggregateOrdered PP,									/* used to pass by reference the variables were to leave results*						*/\
 const obj pat															/* the patern to apply																											*/

#define matchParameters2\
	patternKind patKind=patInferred,			/* how to apply the pattern, overrides the pattern Class										*/\
	const	cmpFuncP cmpP=equalComparison,	/* the function to use for checking the match between CHAR and CHARpat			*/\
	const matchFlag flags=(matchFlag)0, 	\
	/* matchNegated allows to invert the match or non match given by the previous function				*/\
	/* matchMarked allows to override the pattern with a previously saved position								*/\
	/* matchKeepFailures allows to keep failures 																									*/\				
	/* the start and limit pos of the slice of PP to consider and the pattern start and limit pos */\
	Int PPi1=0,Int PPi2=~0,Int pati1=0,Int pati2=~0

// 	patternKind patKind,
		
Aggregate allPos(Aggregate s,Word startPos=0);
Aggregate anything(Aggregate s,AggregateOrdered positions);
AggregateOrdered matchEnd(Aggregate s,AggregateOrdered positions);

AggregateOrdered match(matchParameters1,matchParameters2);
AggregateOrdered noMatch(matchParameters1);


//Pos matchSingle(matchParameters(Pos P));

void patternExecute(obj pat);
AggregateOrdered matchFor(Int n1,Int dn,matchParameters1,matchParameters2);
void matchFor_(Int n1,Int n2,matchFlag flags=(matchFlag)0);
Aggregate matchPlusPlus(Int n1,const Aggregate s,const AggregateOrdered PP,const Aggregate pat);

