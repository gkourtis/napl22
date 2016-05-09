#pragma once 

struct level{
	Word l;
	level(Word i):l{i}{};
	level():l{0}{};
	operator Word(){return l;}
//both postfix operators must take a dummy int parameter in order to differentiate from the prefix increment
	Word operator--(int){return l ? l-- : l;} 
	Word operator++(int){return l++;}										 
// prefix operators don't
	Word& operator--(){return l ? --l : l;}
	Word& operator++(){return ++l;}
	
	level operator<<(level lev){
		if(l>lev.l) l=lev.l;return *this;
	};
};
