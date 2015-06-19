
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "tscplugin.h"


int main()
{
	printf(" This is the example program in tscrecord \n");
	printf(" the pid of process is %jd\n", (intmax_t)getpid());
	
	TSCRECORD_REGISTER(0, "example_memcpy");
	while (1)
	{
		char * from = malloc(1024);
		char * dest = malloc(1024);
		
		uint64_t counter;
		TSCRECORD_START(0, &counter);
		memcpy(dest, from, 1024);
		TSCRECORD_STOP(0, &counter);

		free(from);
		free(dest);
	}
	return 0;
}