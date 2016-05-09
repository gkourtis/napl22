//enum AggregateOrderedNeutrals { AggregateOrderedNone=0,AggregateOrderedAll=-1};

enum AggregateOrderedNeutrals{ 
	AggregateOrderedNone=0,
	AggregateOrderedAll=-1
};

class AggregateOrdered:public Aggregate{
// The AggregateOrdered contains a number of rows of equal size
// In one of the columns is contained the key, in another the value
// Normally we search for the key and find the value
public:
	AggregateOrdered(Word w):Aggregate(w){}
	AggregateOrdered(Obj o):Aggregate(o){}
	AggregateOrdered(bool b):Aggregate(b){
		;
	}
	AggregateOrdered():Aggregate(classAggregate,nullptr){}

	bool findIn_(
		Word keysNum,Obj keys[],
		bool insert=false,bool replace=false,bool asUndefined=false,
		obj *oP=nullptr,uWord *iP=nullptr,
		Word cols=2,Word keyCol=0,Word valueCol=1)
	const;

	bool findIn(
		Obj key,
		bool insert=false,bool replace=false,bool asUndefined=false,
		obj *oP=nullptr,uWord *iP=nullptr,
		Word cols=2,Word keyCol=0,Word valueCol=1)
	const;

	
	bool keyExists(Obj key,bool insert=false) const;
	
	bool revFindIn(Obj value,obj& name,uWord &i,Word cols=2,Word keyCol=0,Word valueCol=1) const;
	
	obj nameOf(Obj value) const;

	obj valueOf(Obj key) const;

	void define(Obj key,Obj o0) const;

	void operator+=(Aggregate a);
	void operator&=(Aggregate o);


};

