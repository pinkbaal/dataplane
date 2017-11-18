
#include <stdio.h>
#include <unistd.h>

#include <dp_types.h>
#include <lib/dp_memory.h>
#include <net/ipv4_public.h>

#include <rte_debug.h>
#include <rte_eal.h>
#include <rte_lcore.h>
#include <rte_ethdev.h>
#include <rte_mbuf.h>
#include <rte_cycles.h>




static s32 lcore_fwd_loop(__attribute__((unused)) void *arg)
{
	while(1) {
		printf("[%s]%u# lcore id:%d\n", __FUNCTION__, __LINE__, rte_lcore_id());
		sleep(1);
	}

	return 0;
}

static s32 dataplane_init(void)
{
	dp_ipv4_init();
	return 0;
}

s32 main(s32 argc, s8 **argv)
{
	s32 ret;
	u32 lcore_id, nb_ports;
	struct rte_mempool *mbuf_pool;
	s8 *p = NULL;

	ret = rte_eal_init(argc, argv);
	if (ret < 0) {
		rte_exit(ret, "Cannot init EAL ret%d\n", ret);
	}

	nb_ports = rte_eth_dev_count();
	if (nb_ports < 2) {
		rte_exit(-1, "nb_ports:%u\n", nb_ports);
	}

	mbuf_pool = rte_pktmbuf_pool_create("MBUF_POOL", 8191 * nb_ports,
		250, 0, RTE_MBUF_DEFAULT_BUF_SIZE, rte_socket_id());
	if (mbuf_pool == NULL) {
		rte_exit(-1, "Cannot create mbuf pool\n");
	}

	p = us_malloc(0, 32);
	printf("p:%p\n", p);
	
	dataplane_init();

	RTE_LCORE_FOREACH_SLAVE(lcore_id) {
		rte_eal_remote_launch(lcore_fwd_loop, NULL, lcore_id);
	}

	lcore_fwd_loop(NULL);


	rte_eal_mp_wait_lcore();
	
	return 0;
}







