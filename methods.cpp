bool methodActionApplicable(const Aggregate p){
// patterns are a way to say if an action is applicable to a kind of object.
// the pattern may be:
// a) a specific elementary or non elementary object. In that case the object should be equal to the TOS.
// b) a class specifying wich is the class that the TOS should belong too.
// c) an aggregate specifiying a list of elements that all should be matched agains the stack() contenents.
// Case c creates the possibility of misinterpretation between case a and case c wher the aggregate could be 
// interpreted as a whole value to be matched by equality or as a list of elements to match to the stack() contents.
// That means that a way must be found to differentiate between those different cases.
// In that sense a matcher operator with all possible flexibilities could be used.
	Obj cl=p.Class();
	SWITCH(cl)
		CASE(classClass)
			if(cl==obj(stack().tos()).Class()) return true;
		CASE(classAggregate)
			uWord N=stack().elements(),n=p.elements(),i;
			if(N<n) return false; // if the objects of the stack arent enough than the metod isn't applicable
			for(i=0;i<n;i++) 
				if( !(obj(stack()[N-n+i]).isInstanceOf( obj(p[i]) ) ) ) break;
			return i==n; // if i==n then the for loop was completed and so a full match is found
		DEFAULT
			return false;
	endSWITCH
	return false;
}
