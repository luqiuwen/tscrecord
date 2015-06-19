#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <memory.h>
#include <pthread.h>
#include "tscplugin.h"

static char shm_symbol[TSCCOMMON_SYMMAX];
struct tracepoint_shm_s * traceshm = NULL;
struct tracepoint_shm_s * traceshm_lasttime = NULL;

int tscreader_print()
{
	while (1)
	{
		const char clr[] = { 27, '[', '2', 'J', '\0' };
		const char topLeft[] = { 27, '[', '1', ';', '1', 'H', '\0' };

		printf("%s%s", clr, topLeft);
		printf("\nRuntime Statistics  ====================================");

		for (int i = 0; i < TSCCOMMON_TRACEMAX; i++)
		{
			if (!traceshm->tracepoint[i].enable)
				continue;

			struct tracepoint_s * tracepoint = &(traceshm->tracepoint[i]);
			struct tracepoint_s * tracepoint_lasttime = &(traceshm_lasttime->tracepoint[i]);

			uint64_t delta_runtime = tracepoint->runtime - tracepoint_lasttime->runtime;
			uint64_t delta_cycles = tracepoint->cycle - tracepoint_lasttime->cycle;
			float runtime_per_cycles = delta_cycles == 0 ? 0 : (float)delta_runtime / delta_cycles;
			
			printf("\n%s\t:%7.2f", tracepoint->name, runtime_per_cycles);
		}

		printf("\n========================================================\n");
		memcpy(traceshm_lasttime, traceshm, sizeof(traceshm_lasttime));
		sleep(1);
	}
}

int main(int argc, char * argv[])
{
	if (argc < 2)
	{
		printf("need more args\n");
		return 0;
	}

	pid_t pid = strtol(argv[1], NULL, 0);
	sprintf(shm_symbol, "TSCRECORD_SHMSYMBOL_%jd", (intmax_t)pid);

	int shm_fd = shm_open(shm_symbol, O_RDWR, SHM_FILEMODE);
	if (shm_fd < 0)
	{
		fprintf(stderr, "cannot link shared memory of process %jd\n", (intmax_t)pid);
		return -1;
	}

	ftruncate(shm_fd, sizeof(struct tracepoint_shm_s));
	void * shm_ptr = mmap(NULL, sizeof(struct tracepoint_shm_s),
		PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

	if (shm_ptr == NULL)
	{
		fprintf(stderr, "cannot link shared memory of process %jd\n", (intmax_t)pid);
		return -1;
	}

	traceshm = (struct tracepoint_shm_s *)shm_ptr;
	traceshm_lasttime = (struct tracepoint_shm_s *)malloc(sizeof(struct tracepoint_shm_s));
	tscreader_print();
}
