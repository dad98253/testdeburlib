
#ifndef WINDOZE
#include "lindows.h"
#include <strings.h>
#define _stricmp	strcasecmp
#endif	// WINDOZE

#define NEED_OS_FORK
#undef _GNU_SOURCE
#define _GNU_SOURCE 1 /* for strcasestr in legacy builds */

#include <stdio.h>
#include <string.h>
#include "debug.h"
#include "debug01.h"
#include "options.h"


#ifdef WINDOZE
int isPowerOfTwo64 (unsigned __int64 x);
#else
int isPowerOfTwo64 (unsigned long long x);
#endif


char *szProgFilename = NULL;
BOOL bStdioRedirectedToConsole;

DEBUGOPTIONSTYPE myoptions;
DEBUGOPTIONSTYPE* pmyoptions = NULL;

int locateLineNum ( long long unsigned int lluRelAddress , char * szFunctionName , char * szExeFile );
extern char * jtrunwindln (int icall);
extern char * szMainExeFile;

int main (int argc, char **argv)
{
/*
#ifdef WINDOZE
	unsigned __int64 ullint = 0;
#else
	unsigned long long ullint = 0;
#endif
	int i;

	printf(" uint = 0x%x\n",1);

	for (i=0;i<129;i++) {
#ifdef WINDOZE
		printf (" i = %i, ullint = 0x%I64x  isPowerOfTwo64 = %i\n",i,ullint, isPowerOfTwo64(ullint));
#else
		printf (" i = %i, ullint = 0x%llx  isPowerOfTwo64 = %i\n",i,ullint, isPowerOfTwo64(ullint));
#endif

		ullint++;
	}

	printf("test big numbers...\n");
#ifdef WINDOZE
	ullint = 0x1ui64;
#else
	ullint = 0x1LL;
#endif

	for (i=0;i<64;i++) {
#ifdef WINDOZE
		printf (" shift = %i, ullint = 0x%I64x  isPowerOfTwo64 = %i\n",i,ullint, isPowerOfTwo64(ullint));
		printf (" shift = %i, ullint +1 = 0x%I64x  isPowerOfTwo64 = %i\n",i,ullint+1ui64, isPowerOfTwo64(ullint+1ui64));
#else
		printf (" shift = %i, ullint = 0x%llx  isPowerOfTwo64 = %i\n",i,ullint, isPowerOfTwo64(ullint));
		printf (" shift = %i, ullint +1 = 0x%llx  isPowerOfTwo64 = %i\n",i,ullint+1ULL, isPowerOfTwo64(ullint+1ULL));
#endif
		ullint = ullint <<1;
	}
*/
#ifdef DEBUG
	printf("   DEBUG is set\n");
#else	// DEBUG
	printf("   DEBUG is not set\n");
#endif	// DEBUG
	pmyoptions = &myoptions;
	int retval = 0;
	if( ( retval = debug_init ((void**)&pmyoptions, 1 )) ) {
		printf("bad debug init\n");
		return(retval);
	}

	if (!argv[0])
		szProgFilename = (char *)"main";
	else
	if ((szProgFilename = strrchr(argv[0], '/')))
		szProgFilename++;
#if WINDOZE
	else
	if ((szProgFilename = strrchr(argv[0], '\\')))
		szProgFilename++;
#endif
	else
		szProgFilename = argv[0];

#ifdef WINDOZE
	strlwr(szProgFilename);
	if (strlen(szProgFilename) > 4 && !strcmp(szProgFilename + strlen(szProgFilename) - 4, ".exe"))
		szProgFilename[strlen(szProgFilename) - 4] = 0;
#endif




	dfprintf(__LINE__,__FILE__,TRACE,"checking for help call\n");
	int show_usage = 0;
	if (argc < 2 ||
            (argc == 2 &&
             (!_stricmp(argv[1], "--help") ||
              !_stricmp(argv[1], "-h") ||
              !_stricmp(argv[1], "-help"))))
	{
		dfprintf(__LINE__,__FILE__,TRACE,"detected show usage request\n");
		show_usage = 1;
	}

	dfprintf(__LINE__,__FILE__,TRACE,"calling opt_init...\n");
	opt_init(szProgFilename, argc, argv, show_usage);

	dfprintf(__LINE__,__FILE__,TRACE,"calling load_debug...\n");
	load_debug ();

	dfprintf(__LINE__,__FILE__,TRACE,"returned from load_debug...\n");

/*
	if( ( retval = set_debug_device ((char*)"RAM") ) ) {
		printf("bad debug device type\n");
		return(retval);
	}
*/

	// test jtrunwind
	printf("\tjtrunwind (-1) =%s\n",jtrunwind (-1));
	dfprintf(__LINE__,__FILE__,TRACE,"returned from jtrunwind (-1)...\n");
	printf("\tjtrunwind (0) =%s\n",jtrunwind (0));
	dfprintf(__LINE__,__FILE__,TRACE,"returned from jtrunwind (0)...\n");
	printf("\tjtrunwind (1) =%s\n",jtrunwind (1));
	dfprintf(__LINE__,__FILE__,TRACE,"returned from jtrunwind (1)...\n");
	long long unsigned int lluRelAddress = 0x228;
	const char * szFunctionName = "main";
	char * szExeFile;
	szExeFile = argv[0];
	locateLineNum ( lluRelAddress ,  (char *)szFunctionName ,  szExeFile );
	szMainExeFile = szExeFile;
	dfprintf(__LINE__,__FILE__,TRACE,"calling jtrunwindln (-1)...\n");
	printf("\tjtrunwindln (-1) =%s\n",jtrunwindln (-1));
	dfprintf(__LINE__,__FILE__,TRACE,"calling jtrunwindln (0)...\n");
	printf("\tjtrunwindln (0) =%s\n",jtrunwindln (0));
	dfprintf(__LINE__,__FILE__,TRACE,"calling jtrunwindln (1)...\n");
	printf("\tjtrunwindln (1) =%s\n",jtrunwindln (1));


	debug_close ();

	return 0;
}
