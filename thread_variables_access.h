#include <thread>
template<typename T>
inline T& threadVar(std::thread& th,T& v){
	auto p=pthread_self();
	intptr_t d=(intptr_t)&v-(intptr_t)p;
	return *(T*)(th.native_handle()+d);
}
