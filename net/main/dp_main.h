#ifndef __DP_MAIN_H__
#define __DP_MAIN_H__

#include <dp_types.h>


struct dp_lcore_param {
	u16 nb_rx_port;
	u16 nb_tx_port;
	u16 rx_queue_id[RTE_MAX_ETHPORTS];
	u16 tx_queue_id[RTE_MAX_ETHPORTS];
};

struct dp_param {
	struct dp_lcore_param lcore_param[RTE_MAX_LCORE];
};

struct dp_config {
	u32 pad;
};

struct data_plane {
	struct dp_param param;
	struct dp_config config;
};


#endif //__DP_MAIN_H__
