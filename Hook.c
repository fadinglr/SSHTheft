#ifdef MAIN
#include<stdio.h>
#include <unistd.h>
#include <string.h>


int  main()
{
	char * buf;
	buf = new char[0x10];
	memset(buf,0,0x10);
	while(1)
	{
		memcpy(buf, "Hello World", 0xb);
		printf("%s%d\n","Hello World",strlen(buf));
		sleep(3);
	}
	return 0;
}
#endif



/********************************************************************************/
#ifdef HOOK
#include <stdarg.h>
#include <stdio.h>
#include <dlfcn.h>
#include "src/ptraceAPI.h"
#include "src/inject.hpp"
#include <signal.h>
typedef int(*PRINTF)(const char * ,...);

#define MaxArgCount 8
typedef struct{
	long long int t;
	void * ptr[MaxArgCount];
}list;

#include <stdarg.h>
#define ARG(plst) plst->ptr[0], plst->ptr[1], plst->ptr[2], plst->ptr[3],plst->ptr[4], plst->ptr[5], plst->ptr[6], plst->ptr[7] 
#define getlist(arg) ((list *)*(void**)((char*)&arg+16)) //return list *
//Usage :
// va_list args;
// va_start(args, TYPE);
// list * plst= getlist(args);
// func(t1 a1, t2 a2, ARG(plst)); == func(t1 a1, t2 a2, ...)
// va_end(args);

//int printf_HOOOOOOOOOOOK(const char * format, ...)
//{/*{{{*/
//	va_list args;
//	char * stack[8];
//	void * handle =dlopen("libc.so.6",RTLD_LAZY);
//	PRINTF fnc = (PRINTF)dlsym(handle, "printf");
//	if(!fnc)
//		puts("Failed get sym");
//	fnc("Hey, Fuck YOU!\n");
//	va_start(args, format);
//	list * plst = getlist(args);
//	int ret =fnc(format, plst->ptr[0], plst->ptr[1], plst->ptr[2]);	
//	va_end(args);
//	return ret;
//
//}/*}}}*/

unsigned int offsetAAA=0;
#ifndef LIBNAME
#define LIBNAME "hook.so"
#endif
typedef pid_t (*FORK)(void);
pid_t  fork(void)
{/*{{{*/
	pid_t child;
	FILE * logfile;
	char * buffer;
	char buf [20];
	const char * libName = LIBNAME;
	void * handle = dlopen("libc.so.6",RTLD_LAZY);
       	FORK fnc = (FORK)dlsym(handle, "fork")	;

       	if(!fnc)
	      puts("Failed get Sym!\n");
      	child = fnc(); 
	if(child ==0)
	{
		kill(getpid(), SIGSTOP);
		return child;
	}

//	kill(getpid(),SIGSTOP);
	gothook(child , "execv", "_Z10execv_hookPKcPPc", libName);
	kill(child,SIGCONT);
	return child;
}/*}}}*/

#define size_t int
typedef  size_t (*STRLEN)(const char *);
size_t strlenhook(const char *s)
{/*{{{*/
	FILE* fp;
	void * handle = dlopen("libc.so.6",RTLD_LAZY);
       	STRLEN fnc = (STRLEN)dlsym(handle, "strlen")	;
	puts("Hook");
	fp =fopen("/tmp/strlens","a");
	fwrite(s, strlen(s),1,fp);
	fwrite("\n",1,1,fp);
	fclose(fp);
	return fnc(s);
}/*}}}*/

#include <string>
#include <fstream>
using std::fstream;
extern  char ** environ ;
typedef int (*EXECV)(const char * , char *  []);
int execv_hook(const char * pathname , char * argv[])
{/*{{{*/
#define OFF__ 0x461e4
#define OFF2__ 0x53336
#define Writable 0xd0000
//	kill(getpid(),SIGSTOP);
	FILE * fp;
	ofstream f;
	struct timespec tim,tim2;
	tim.tv_sec  = 0;
	tim.tv_nsec = 200000000L;
	void * hd= dlopen("libc.so.6",RTLD_LAZY);
	char buf [20];
	char buf2[200];
	pid_t cpid,tid;
	EXECV fnc = (EXECV )dlsym(hd, "execv");
	tid = fork();
	if(!tid)
	{
		cpid= getpid()+1;
		nanosleep(&tim, &tim2);
		if(kill(cpid,0)==0)
		{
		void * loadaddr= (void*)(getLoadAddr(cpid));
		breakpoint(cpid, addr(loadaddr, offsetAAA));
		exit(0);
		}
	}
	
	return fnc(pathname, argv);
}/*}}}*/

void __attribute((constructor)) con()
{

	ofstream fs;
	pid_t pid ;
	pid = getpid();
	void * load = (void *)getLoadAddr(pid);
	void * secd;
	void * addr;
	load = (char *)load+0xa000;
	while(1)
	{
	addr =procfind(pid,(const unsigned char *)"\xfc\xff\xff\x07",4,load,(char*)load+0xf0000 );	
	load=(char *)addr+1;
	secd =procfind(pid, (const unsigned char *)"\x31\xc0\x48",3,(char *)addr-0x10,(char*) addr);	
	if(secd !=NULL)
		break;
	}
	load=(char*)secd-0x30;
	void * final =procfind(pid,(const unsigned char *)"\x55",1,load, secd);
#ifdef DEBUG
	fs.open("/tmp/findres");
#endif
	offsetAAA=(char*)final-(char *)getLoadAddr(pid);
#ifdef DEBUG
	fs<<  std::hex<<(char*)final-(char *)getLoadAddr(pid) << std::endl;
	fs.close();
#endif
	return;
}
#endif
