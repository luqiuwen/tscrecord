
/*  
	Program Profile Analizer in Poll Mode(x86_64)
	This file is the plugin using in target program.

	Author: Lu Qiuwen <luqiuwen@gmail.com>
	Date  : 2015/06/19
*/

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

struct tracepoint_shm_s * traceshm = NULL;
static char shm_symbol[TSCCOMMON_SYMMAX];
pthread_mutex_t tscplugin_register_mutex = PTHREAD_MUTEX_INITIALIZER;

int __attribute__((destructor, used)) tscplugin_destroy()
{
	traceshm = NULL;
	shm_unlink(shm_symbol);
	return 0;
}

int __attribute__((constructor, used)) tscplugin_init()
{
	sprintf(shm_symbol, "TSCRECORD_SHMSYMBOL_%jd", (intmax_t)getpid());
	
	int shm_fd = shm_open(shm_symbol, O_RDWR | O_CREAT, SHM_FILEMODE);
	ftruncate(shm_fd, sizeof(struct tracepoint_shm_s));

	void * shm_ptr = mmap(NULL, sizeof(struct tracepoint_shm_s), 
		PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

	if (shm_ptr == NULL)
		return -1;

	traceshm = (struct tracepoint_shm_s *)shm_ptr;
	memset(traceshm, 0, sizeof(struct tracepoint_shm_s));
	return 0;
}