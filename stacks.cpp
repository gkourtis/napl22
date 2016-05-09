PRIMITIVE primitive0(){};

void newStack(Word executionLevel){
	stacks.push(newAggregate());
}
void newString(Word executionLevel){
	stacks.push(::newString());
}

Aggregate closeStack(){
	return stacks.pop();
}

void openStack(){
	Aggregate a=stack().pop();
	if(!a.reference() && a.readOnly())
		a=a.clone();
	stacks.push(a);
}

void oldStack(){
	obj o=closeStack();
	stack().push(o);
}

void interStackMove(){
	stack().push( ((Aggregate)(stacks[~2])).pop() );
}

void interStackDup(){
	stack().push( ((Aggregate)(stacks[~2])).tos() );
}

