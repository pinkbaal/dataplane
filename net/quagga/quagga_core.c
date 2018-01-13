
#include <pthread.h>

#include <dp_types.h>
#include <lib/quagga/thread.h>
#include <lib/quagga/log.h>

typedef struct dp_quagga_t {
	struct thread_master *master;
} dp_quagga_s;

dp_quagga_s dp_quagga;

struct thread_master *dp_quagga_master_get(void)
{
	return dp_quagga.master;
}


static void *dp_quagga_loop(void *arg)
{
	struct thread fetch;
	
	while (thread_fetch (dp_quagga.master, &fetch)) {
		thread_call (&fetch);
	}

	return NULL;
}


s32 dp_quagga_init(void)
{
	int ret;
	pthread_t quagga_tid;
	pthread_attr_t attr;

	dp_quagga.master = thread_master_create();
	if (dp_quagga.master == NULL) {
		return -1;
	}

	zlog_default = openzlog ("dataplane", ZLOG_DATAPLANE,
			   LOG_CONS|LOG_NDELAY|LOG_PID, LOG_DAEMON);

	ret = dp_quagga_rtnl_init();
	if (ret < 0) {
		return -1;
	}

	ret = pthread_attr_init(&attr);
	if (ret != 0) {
		return -1;
	}

	ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if (ret != 0) {
		return -1;
	}

	ret = pthread_create(&quagga_tid, &attr, dp_quagga_loop, NULL);
	if (ret != 0) {
		return -1;
	}

	return 0;
}



