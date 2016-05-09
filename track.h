Word trackedDepth();

void track(Word w);
void untrack();

template<typename T>
	struct tracked:T{
		tracked(T t):T{t}{track(t);}
		tracked():T{}{track(T::w);}
		~tracked(){untrack();}
/*
		void setnext(){
			if(etP->trackedS.w)
				etP->trackedS.push(T::w);
			}

		void unsetnext(){
			if(etP->trackedS.w){
				Token t=etP->trackedS.pop();
				if(t!=T::w)
					std::cout<<"Problem untracking"<<std::endl;
				assert0(t==T::w,"Problem untracking");
			}
		}
	
*/	  

};

typedef tracked<obj> tobj;
typedef tracked<Obj> tObj;
typedef tracked<Aggregate> tAggregate;
typedef tracked<String> tString;
class AggregateOrdered;
typedef tracked<AggregateOrdered> tAggregateOrdered;

template class tracked<obj>;
template class tracked<Obj>;
template class tracked<Aggregate>;

/*
Word trackedDepth(){
	Word i=0;void* p=&trackedLast;
	for(;p && i<0x100;i++,p=nx(p));
	return i;
}
*/
//template class tracked<AggregateOrdered>;
//template class tracked<String>;
