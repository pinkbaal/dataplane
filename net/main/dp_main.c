
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

static s32 lcore_fwd_loop(__attribute__((unused)) void *arg)
{
	u16 rx_port_id, tx_port_id, nb_ports, rx_nb_pkts, tx_nb_pkts, queue_id, pkt_index;
	u32 lcore_id;
	struct rte_mbuf *pkts_burst[8];
	
	lcore_id = rte_lcore_id();
	nb_ports = rte_eth_dev_count();

	while(1) {
		if (lcore_id == 0) {
			queue_id = 0;
		} else {
			queue_id = 1;
		}
		/* rx */
		for (rx_port_id = 0; rx_port_id < nb_ports; rx_port_id++) {
			rx_nb_pkts = rte_eth_rx_burst(rx_port_id, queue_id, pkts_burst, 8);
			if (rx_nb_pkts > 0) {
				for (pkt_index = 0; pkt_index < rx_nb_pkts; pkt_index++) {
					dp_packet_dump(pkts_burst[pkt_index]);
				}
				/* tx */
				for (tx_port_id = 0; tx_port_id < nb_ports; tx_port_id++) {
					if (tx_port_id != rx_port_id) {
						tx_nb_pkts = rte_eth_tx_burst(tx_port_id, queue_id, pkts_burst, rx_nb_pkts);
						if (rx_nb_pkts == tx_nb_pkts) {
							printf("[%s]%u# lcore:%d, queue:%d tx success rx:%d, tx:%d\n", 
								__FUNCTION__, __LINE__, lcore_id, queue_id, rx_nb_pkts, tx_nb_pkts);
						} else {
							printf("[%s]%u# lcore:%d, queue:%d tx failed rx:%d, tx:%d\n", 
								__FUNCTION__, __LINE__, lcore_id, queue_id, rx_nb_pkts, tx_nb_pkts);
						}
						break;
					}	
				}
			}
		}
		printf("[%s]%u# lcore:%d\n", __FUNCTION__, __LINE__, lcore_id);
		sleep(1);		
	}

	return 0;
}

static s32 dataplane_init(void)
{
	dp_ipv4_init();
	return 0;
}

s32 dp_config_read(s32 argc, s8 **argv)
{
	return 0;
}

s32 main(s32 argc, s8 **argv)
{
	u8 portid, nb_ports;
	u16 nb_queue = 2, queue_id;
	s32 ret;
	u32 lcore_id;
	struct rte_mempool *mbuf_pool;
	struct rte_eth_dev_info dev_info[RTE_MAX_ETHPORTS];

	ret = rte_eal_init(argc, argv);
	if (ret < 0) {
		rte_exit(ret, "Cannot init EAL ret%d\n", ret);
	}

	dp_config_read(argc, argv);

	nb_ports = rte_eth_dev_count();
	if (nb_ports < 2) {
		rte_exit(-1, "nb_ports:%u\n", nb_ports);
	}

	mbuf_pool = rte_pktmbuf_pool_create("MBUF_POOL", 8191 * nb_ports,
		250, 0, RTE_MBUF_DEFAULT_BUF_SIZE, rte_socket_id());
	if (mbuf_pool == NULL) {
		rte_exit(-1, "Cannot create mbuf pool\n");
	}

	memset(dev_info, 0, sizeof(dev_info));
	for (portid = 0; portid < nb_ports; portid++) {
		rte_eth_dev_info_get(portid, &dev_info[portid]);
		
		ret = rte_eth_dev_configure(portid, nb_queue, nb_queue, &port_conf);
		if (ret < 0) {
			rte_exit(-1, "Cannot configure device: err=%d, port=%u\n", ret, (unsigned) portid);
		}
		
		for (queue_id = 0; queue_id < nb_queue; queue_id++) {
			ret = rte_eth_rx_queue_setup(portid, queue_id, dev_info[portid].rx_desc_lim.nb_min, rte_eth_dev_socket_id(portid), NULL, mbuf_pool);
			if (ret < 0) {
				rte_exit(-1, "rte_eth_rx_queue_setup:err=%d, port=%u\n", ret, (unsigned) portid);
			}

			ret = rte_eth_tx_queue_setup(portid, queue_id, dev_info[portid].tx_desc_lim.nb_min, rte_eth_dev_socket_id(portid), NULL);
			if (ret < 0) {
				rte_exit(-1, "rte_eth_tx_queue_setup:err=%d, port=%u\n", ret, (unsigned) portid);
			}
		}
		
		ret = rte_eth_dev_start(portid);
		if (ret < 0) {
			rte_exit(-1, "rte_eth_dev_start:err=%d, port=%u\n", ret, (unsigned) portid);
		}
			
		rte_eth_promiscuous_enable(portid);
	}

	dataplane_init();

	RTE_LCORE_FOREACH_SLAVE(lcore_id) {
		rte_eal_remote_launch(lcore_fwd_loop, NULL, lcore_id);
	}

	lcore_fwd_loop(NULL);


	rte_eal_mp_wait_lcore();
	
	return 0;
}







