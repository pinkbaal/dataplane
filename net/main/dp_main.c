
#include <stdio.h>
#include <unistd.h>
#include <getopt.h> 

#include <dp_types.h>
#include <dp_bitops.h>
#include <dp_common.h>
#include <lib/dp_memory.h>
#include <lib/dp_epoll.h>
#include <net/dp_vtysh.h>
#include <net/dp_ipv4.h>

#include <rte_debug.h>
#include <rte_eal.h>
#include <rte_lcore.h>
#include <rte_ethdev.h>
#include <rte_mbuf.h>
#include <rte_cycles.h>

#include "dp_main.h"


static const struct rte_eth_conf port_conf = {
	.rxmode = {
		.mq_mode = ETH_MQ_RX_RSS,
		.split_hdr_size = 0,
		.header_split   = 0, /**< Header Split disabled */
		.hw_ip_checksum = 0, /**< IP checksum offload disabled */
		.hw_vlan_filter = 0, /**< VLAN filtering disabled */
		.jumbo_frame    = 0, /**< Jumbo Frame Support disabled */
		.hw_strip_crc   = 1, /**< CRC stripped by hardware */
	},
	.txmode = {
		.mq_mode = ETH_MQ_TX_NONE,
	},
	.rx_adv_conf = {
		.rss_conf = {
			.rss_hf = ETH_RSS_IP | ETH_RSS_UDP |
				ETH_RSS_TCP | ETH_RSS_SCTP,
		}
	},
};

static const s8 short_options[] = 
	"l:"
	"b:"
	"d:"
	"t:"
	"f:";


static const struct option long_options[] = {
		{"log-mode", 1, NULL, 'l'},
		{"bind-mode", 1, NULL, 'b'},
		{"dispatch-lcore", 1, NULL, 'd'},
		{"transfer-lcore", 1, NULL, 't'},
		{"forward-lcore", 1, NULL, 'f'},
		{0, 0, 0, 0}
};


struct data_plane dataplane;

static s32 dp_packet_dump(struct rte_mbuf *pkt)
{
	s32 i, j;
	u8 *data = (s8 *)pkt->buf_addr + pkt->data_off;
	
	for (i = 0; i < pkt->data_len; i++) {
		if (i % 8 == 0) {
			printf("\n");
		}
		printf("0x%02x ", *(data+i));
	}
	printf("\n");
	
	return 0;
}


static s32 dp_xmit(struct rte_mbuf *pkt, u8 tx_port_id)
{
	u16 tx_queue_id;
	u32 lcore_id;

	lcore_id = rte_lcore_id();
	
	tx_queue_id = dataplane.lcore_conf[lcore_id].tx_queue_id[tx_port_id];
	rte_eth_tx_burst(tx_port_id, tx_queue_id, &pkt, 1);

	return 0;
}

static s32 dp_netif_rx(struct rte_mbuf *pkt, u8 rx_port_id)
{
	u8 tx_idx, tx_port_id, nb_ports;
	u16 tx_queue_id;
	u32 lcore_id;
	
	lcore_id = rte_lcore_id();
	nb_ports = rte_eth_dev_count();

	dp_packet_dump(pkt);

	for (tx_port_id = 0; tx_port_id < nb_ports; tx_port_id++) {
		if (tx_port_id != rx_port_id) {
			dp_xmit(pkt, tx_port_id);
			break;
		}
	}

	return 0;
	
}

static s32 dp_forward_loop(__attribute__((unused)) void *arg)
{
	u32 lcore_id;
	u8 rx_port_id;
	u16 rx_queue_id;
	u16 nb_rx_queue, rx_nb_pkts;
	u16 rx_idx, pkt_idx;
	struct rte_mbuf *pkts_burst[8];
	
	lcore_id = rte_lcore_id();

	while(1) {
		/* rx */
		nb_rx_queue = dataplane.lcore_conf[lcore_id].nb_rx_queue;
		for (rx_idx = 0; rx_idx < nb_rx_queue; rx_idx++) {
			rx_port_id = dataplane.lcore_conf[lcore_id].rx_queue[rx_idx].port_id;
			rx_queue_id = dataplane.lcore_conf[lcore_id].rx_queue[rx_port_id].queue_id;
			rx_nb_pkts = rte_eth_rx_burst(rx_port_id, rx_queue_id, pkts_burst, 8);
			if (rx_nb_pkts > 0) {
				for (pkt_idx = 0; pkt_idx < rx_nb_pkts; pkt_idx++) {
					dp_netif_rx(pkts_burst[pkt_idx], rx_port_id);
				}
			}
		}
	}

	return 0;
}



static s32 dp_dispatch_loop(__attribute__((unused)) void *arg)
{

}

static s32 dp_transfer_loop(__attribute__((unused)) void *arg)
{

}

static s32 dp_master_loop(__attribute__((unused)) void *arg)
{	
	while(1) {
		dp_epoll_evnet_handle(1000);
		sleep(1);
	}
}

static s32 dp_dummuy_loop(__attribute__((unused)) void *arg)
{
	
}


static s32 dp_parse_corelist(const char *corelist, u64 lcore_maps[])
{
	int i, idx = 0;
	unsigned count = 0;
	char *end = NULL;
	int min, max;

	if (corelist == NULL)
		return -1;

	/* Remove all blank characters ahead and after */
	while (isblank(*corelist))
		corelist++;
	i = strlen(corelist);
	while ((i > 0) && isblank(corelist[i - 1]))
		i--;

	/* Reset config */
	memset(lcore_maps, 0, sizeof(u64 [BITS_TO_LONGS(RTE_MAX_LCORE)]));

	/* Get list of cores */
	min = RTE_MAX_LCORE;
	do {
		while (isblank(*corelist))
			corelist++;
		if (*corelist == '\0')
			return -1;
		errno = 0;
		idx = strtoul(corelist, &end, 10);
		if (errno || end == NULL)
			return -1;
		while (isblank(*end))
			end++;
		if (*end == '-') {
			min = idx;
		} else if ((*end == ',') || (*end == '\0')) {
			max = idx;
			if (min == RTE_MAX_LCORE)
				min = idx;
			for (idx = min; idx <= max; idx++) {
				if (test_and_set_bits(idx, lcore_maps) == 0) {
					count++;
				}
			}
			min = RTE_MAX_LCORE;
		} else
			return -1;
		corelist = end + 1;
	} while (*end != '\0');

	if (count == 0)
		return -1;

	return count;
}


static s32 dp_parse_args(s32 argc, s8 **argv)
{
	s32 ret, opt, option_index = 0;
	
	while ((opt = getopt_long(argc, argv, short_options, long_options, &option_index)) != -1) {
		switch (opt) {
			case 'l':
				dataplane.log_mode = atoi(optarg);
				if (dataplane.log_mode < 0) {
					dataplane.log_mode = 0;
				}
				break;
			case 'b':
				dataplane.bind_mode = atoi(optarg);
				if (dataplane.bind_mode < 0) {
					dataplane.bind_mode = 0;
				}
				break;
			case 'd':
				ret = dp_parse_corelist(optarg, dataplane.disp_lcore_maps);
				if (ret < 0) {
					return -1;
				}
				dataplane.nb_dispatch = ret;
				break;
			case 't':
				ret = dp_parse_corelist(optarg, dataplane.trans_lcore_maps);
				if (ret < 0) {
					return -1;
				}
				dataplane.nb_transfer = ret;
				break;
			case 'f':
				ret = dp_parse_corelist(optarg, dataplane.fwd_lcore_maps);
				if (ret < 0) {
					return -1;
				}
				dataplane.nb_forward = ret;
				break;
			default:
				break;
		}
	}
	return 0;
}



static s32 dp_config_read(void)
{
	return 0;
}


static s32 dp_mem_init(void)
{
	u32 nb_ports;
	struct rte_mempool *mbuf_pool;

	nb_ports = rte_eth_dev_count();
	mbuf_pool = rte_pktmbuf_pool_create("MBUF_POOL", 8191 * nb_ports,
		250, 0, RTE_MBUF_DEFAULT_BUF_SIZE, rte_socket_id());
	if (mbuf_pool == NULL) {
		return -1;
	}

	dataplane.mbuf_pool = mbuf_pool;
	return 0;
}

static s32 dp_port_init(void)
{
	u16 port_id, lcore_id, rx_idx;
	u32 nb_ports;
	s32 ret, socket_id;
	
	struct rte_eth_dev_info dev_info[RTE_MAX_ETHPORTS];

	nb_ports = rte_eth_dev_count();
	memset(dev_info, 0, sizeof(dev_info));


	if (dataplane.bind_mode == 1){
		/* 每个lcore绑定所有网卡的一个rx ring */
		
	} else if (dataplane.bind_mode == 2) {
		/* 根据配置文件绑定 */
		
	} else {
		/* 虚拟机网卡只有一个rx ring 每个lcore绑定一个网卡 */
		port_id = 0;
		rx_idx = 0;
		RTE_LCORE_FOREACH_SLAVE(lcore_id) {
			if (test_bit(lcore_id, dataplane.fwd_lcore_maps) && port_id < nb_ports) {
				dataplane.lcore_conf[lcore_id].rx_queue[rx_idx].queue_id = 0;
				dataplane.lcore_conf[lcore_id].rx_queue[rx_idx].port_id = 0;
				dataplane.lcore_conf[lcore_id].nb_rx_queue++;
				port_id++;
				rx_idx++;
			}

		}
		for (port_id = 0; port_id < nb_ports; port_id++) {
			RTE_LCORE_FOREACH_SLAVE(lcore_id) {
				if (test_bit(lcore_id, dataplane.fwd_lcore_maps)) {
					dataplane.lcore_conf[lcore_id].tx_queue_id[port_id] = 0;
					dataplane.lcore_conf[lcore_id].nb_tx_port++;
				}
			}
		
			
			socket_id = rte_eth_dev_socket_id(port_id);
			rte_eth_dev_info_get(port_id, &dev_info[port_id]);

			ret = rte_eth_dev_configure(port_id, 1, 1, &port_conf);
			if (ret < 0) {
				return -1;
			}

			ret = rte_eth_rx_queue_setup(port_id, 0, dev_info[port_id].rx_desc_lim.nb_min, 
					socket_id, NULL, dataplane.mbuf_pool);
			if (ret < 0) {
				return -1;
			}

			ret = rte_eth_tx_queue_setup(port_id, 0, dev_info[port_id].tx_desc_lim.nb_min, 
					socket_id, NULL);
			if (ret < 0) {
				return -1;
			}

			ret = rte_eth_dev_start(port_id);
			if (ret < 0) {
				return -1;
			}
			
			rte_eth_promiscuous_enable(port_id);
		}
	}

	
	
}


static s32 dp_module_init(void)
{
	dp_ipv4_init();
	dp_vtysh_init();
	dp_epoll_init();
	
	return 0;
}

static s32 dp_eal_init(s32 argc, s8 **argv)
{
	int ret;
	
	ret = dp_parse_args(argc, argv);
	if (ret < 0) {
		return -1;
	}

	ret = dp_config_read();
	if (ret < 0) {
		return -1;
	}

	ret = dp_mem_init();
	if (ret < 0) {
		return -1;
	}

	ret = dp_port_init();
	if (ret < 0) {
		return -1;
	}

	ret = dp_module_init();
	if (ret < 0) {
		return -1;
	}
	
	
	return 0;
}

s32 main(s32 argc, s8 **argv)
{
	u8 port_id, nb_ports;
	u16 nb_queue = 2, queue_id;
	s32 ret;
	u32 lcore_id;
	struct rte_eth_dev_info dev_info[RTE_MAX_ETHPORTS];

	ret = rte_eal_init(argc, argv);
	if (ret < 0) {
		rte_exit(ret, "Cannot init EAL ret%d\n", ret);
	}
	
	argc -= ret;
	argv += ret;

	ret = dp_eal_init(argc, argv);
	if (ret < 0) {
		rte_exit(ret, "Cannot init DATA PLANE EAL ret%d\n", ret);
	}
	

	RTE_LCORE_FOREACH_SLAVE(lcore_id) {
		if (test_bit(lcore_id, dataplane.fwd_lcore_maps)) {
			rte_eal_remote_launch(dp_forward_loop, NULL, lcore_id);
		} else if (test_bit(lcore_id, dataplane.disp_lcore_maps)) {
			rte_eal_remote_launch(dp_dispatch_loop, NULL, lcore_id);
		} else if (test_bit(lcore_id, dataplane.trans_lcore_maps)) {
			rte_eal_remote_launch(dp_transfer_loop, NULL, lcore_id);
		} else {
			rte_eal_remote_launch(dp_forward_loop, NULL, lcore_id);
		}
	}

	dp_master_loop(NULL);


	rte_eal_mp_wait_lcore();
	
	return 0;
}







