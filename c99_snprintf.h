#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>

int snprintf(char* dest,size_t destSize,const char* fmt,...);


#ifdef __cplusplus
}
#endif