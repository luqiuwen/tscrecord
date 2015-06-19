
#ifndef tscplugin_h__
#define tscplugin_h__

#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>

#define TSCCOMMON_SYMMAX		64
#define SHM_FILEMODE			00777
#define TSCCOMMON_TRACEMAX		1024

static inline uint64_t tscrecord_rdtsc(void)
{
	union {
		uint64_t tsc_64;
		struct {
			uint32_t lo_32;
			uint32_t hi_32;
		};
	} tsc;

	asm volatile("rdtsc" :
	"=a" (tsc.lo_32),
		"=d" (tsc.hi_32));
	return tsc.tsc_64;
}

struct tracepoint_s
{
	bool enable;
	char name[TSCCOMMON_SYMMAX];
	uint64_t runtime;
	uint64_t cycle;
};

struct tracepoint_shm_s
{
	struct tracepoint_s tracepoint[TSCCOMMON_TRACEMAX];
};

extern struct tracepoint_shm_s * traceshm;
extern pthread_mutex_t tscplugin_register_mutex;

static inline void __attribute__((always_inline))
TSCRECORD_REGISTER(unsigned int id, const char * name)
{
	pthread_mutex_lock(&tscplugin_register_mutex);
	if (!traceshm->tracepoint[id].enable)
	{
		strncpy(traceshm->tracepoint[id].name, name, TSCCOMMON_SYMMAX);
		traceshm->tracepoint[id].enable = 1;
	}
	pthread_mutex_unlock(&tscplugin_register_mutex);
	return;
}

static inline void __attribute__((always_inline))
TSCRECORD_START(unsigned int id, uint64_t * counter)
{
	* counter = tscrecord_rdtsc();
	return;
}

static inline void __attribute__((always_inline))
TSCRECORD_STOP(unsigned int id, uint64_t * counter)
{
	uint64_t end = tscrecord_rdtsc();
	uint64_t start = *counter;
	__sync_fetch_and_add(&(traceshm->tracepoint[id].runtime), end - start);
	__sync_fetch_and_add(&(traceshm->tracepoint[id].cycle), 1);
	return;
}

#endif // tscplugin_h__