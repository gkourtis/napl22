Token localsUnsetT;

void localsSet(){
// Takes an aggregate from the stack with the variables to assign values to
// Afterwards pops the necessary values from the stack and assigns them.
// V0 V1...V(n-1) AggN(var0...var(n-1)) -- 
	Aggregate l=(Aggregate)stack().pop();
	Aggregate a(newAggregate(0x10));
	for(auto i=l.elements();--i>=0;){
		obj v=stack().pop();obj o=l[i];
		a.push2(o,o.New(v));
	}
	exS.push2(a,localsUnsetT);
	
}

void localsUnset(){
	Aggregate a=(Aggregate)stack().pop();
	for(int i=a.elements();i-=2,i>=0;){
		obj o=a[i];o.Old(a[i+1]);
	}
}

