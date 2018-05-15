

#include <stdio.h>

#ifdef WINDOZE
int isPowerOfTwo64 (unsigned __int64 x);
#else
int isPowerOfTwo64 (unsigned long long x);
#endif

int main (int argc, char **argv)
{
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




	return 0;
}
