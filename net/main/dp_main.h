#ifndef __DP_MAIN_H__
#define __DP_MAIN_H__

#include <dp_types.h>
#include <dp_bitops.h>

struct dp_rx_queue_info{
	u8 port_id;
	u8 pad;
	u16 queue_id;
};

struct dp_lcore_conf {
	u16 nb_rx_queue;
	u16 nb_tx_port;
	u16 tx_queue_id[RTE_MAX_ETHPORTS];
	struct dp_rx_queue_info rx_queue[RTE_MAX_ETHPORTS * RTE_MAX_QUEUES_PER_PORT];
};

struct data_plane {
	u8 bind_mode;
	u8 log_mode;
	u8 nb_dispatch;
	u8 nb_transfer;
	u8 nb_forward;
	u64 disp_lcore_maps[BITS_TO_LONGS(RTE_MAX_LCORE)];
	u64 trans_lcore_maps[BITS_TO_LONGS(RTE_MAX_LCORE)];
	u64 fwd_lcore_maps[BITS_TO_LONGS(RTE_MAX_LCORE)];
	struct rte_mempool *mbuf_pool;
	struct dp_lcore_conf lcore_conf[RTE_MAX_LCORE];
	struct thread_master *master;
};


#endif //__DP_MAIN_H__
