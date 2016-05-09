void pushObjAction(Obj o){
// the object is passed non duplicated. If needed it must be duplicated
	if(isToken(o) && !o->readOnly && !o->reference)
		o->readOnly=true;
	stack().push(o);
};

void nopAction(Obj o){
	
};

void classDeferredAction(Obj o0){
	obj o=o0;
	for(o->value();o.Class()==classDeferred;o=o->value()){}
	o.execute();
};

void matchAction(Obj pat){
	tAggregateOrdered RR{match(TOS2(Aggregate),TOS1(AggregateOrdered),pat)};
	DROP(1);PUSH(RR);
}