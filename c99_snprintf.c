#pragma warning( push )
#pragma warning( disable : 4996 )

#include "c99_snprintf.h"
#include <stdio.h>
//-------------------------------

//#define snprintf(pBuf,cnt,fmt,...) ((*((pBuf)+(cnt)-1)='\0'),(snprintf_temporary_var_stringlength=_snprintf((pBuf),(cnt)-1,(fmt),__VA_ARGS__))<0?(cnt):snprintf_temporary_var_stringlength) //ƒJƒ“ƒ}‰‰ŽZŽq‚ðŽg‚Á‚ÄC³
int snprintf(char* dest,size_t destSize,const char* fmt,...){
	va_list args;
	va_start(args, fmt);
	int cnt=vsnprintf(dest, destSize-1,fmt,args);
	va_end(args);

	if(cnt<0){
		cnt=destSize;
		*(dest+destSize-1) = '\0';
	}
	return cnt;
}




#pragma warning( pop )