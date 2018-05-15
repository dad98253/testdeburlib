

#include <stdio.h>

int isPowerOfTwo64 (unsigned __int64 x);

int main (int argc, char **argv)
{
	unsigned __int64 ullint = 0;
	int i;

	printf(" uint = 0x%x\n",1);

	for (i=0;i<129;i++) {
		printf (" i = %i, ullint = 0x%I64x  isPowerOfTwo64 = %i\n",i,ullint, isPowerOfTwo64(ullint));
		ullint++;
	}

	printf("test big numbers...\n");
	ullint = 0x1ui64;
	for (i=0;i<64;i++) {
		printf (" shift = %i, ullint = 0x%I64x  isPowerOfTwo64 = %i\n",i,ullint, isPowerOfTwo64(ullint));
		printf (" shift = %i, ullint +1 = 0x%I64x  isPowerOfTwo64 = %i\n",i,ullint+1ui64, isPowerOfTwo64(ullint+1ui64));
		ullint = ullint <<1;
	}




	return 0;
}