#include <iostream>
#include <sstream>

void print_(std::stringstream& s){};

template<typename T,typename... TT>
void print_(std::stringstream& s,T& t,TT... tt){
	s<<" "<<t;
	print_(s,tt...);
}

template<typename... TT>
void print(TT... tt){
	std::stringstream s;
	print_(s,tt...);s<<"\n";
	std::cout<<s.str();std::cout.flush();usleep(1000);
}

