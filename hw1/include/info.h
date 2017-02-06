#ifndef INFO_H
#define INFO_H
#include <stdlib.h>
#include <stdio.h>

#ifdef INFO
	#define info(fmt, ...) do{fprintf(stderr, "INFO: %s:%s:%d " fmt, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)}while(0)
#else
	#define info(fmt, ...)
#endif

#endif