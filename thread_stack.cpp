#pragma once

#include <pthread.h>
#include "../utilities/utilities.h"

#define processorStackGrowsDownwards true
#define processorStackAlignement 4

thread_local void* processorStack;
thread_local size_t processorStackSize;

__attribute__((constructor(101)))
void processorStackDataInit(){
	pthread_attr_t t;
	if( pthread_getattr_np(pthread_self(),&t) || pthread_attr_getstack(&t, &processorStack, &processorStackSize))
		THROW("unable to get stack data");
}

int processorStackIndex(void* p){
	uintptr_t d;
	if(processorStackGrowsDownwards){
		if( (d=(uintptr_t)processorStack-(uintptr_t)p)>processorStackSize )
			return -1;
		}else{	
			if ((d=(uintptr_t)p-(uintptr_t)processorStack)>=processorStackSize)
			return -1;
		}
	return d/processorStackAlignement;
}

Token& tokenOfStackIndex(Word i){
	Token* p=(Token*)( ((uintptr_t)processorStack+(processorStackGrowsDownwards ? -1 : 1)*processorStackAlignement*i) );
	return *(Token*)p;
}


