typedef std::vector<Token> VT;
typedef std::vector<bool> VB;

void walkPrototype(Aggregate a,VT &values,VT &valuesIndex,VT &fields,VB &hidden){
	for(int i=0x100;a!=classClass && i--!=0;){
		if(a->fields)
			for(auto i=a.elements();i--!=0;){
				fields.push_back(a[i]);
				hidden.push_back(a.V().hideFields); 
			} 
		else
			for(auto i=a.elements();i--!=0;){
				values.push_back(a[i]);
				valuesIndex.push_back(i);
			}
		a=a.Class();
	}
}