#ifndef __DP_QUAGGA_H__
#define __DP_QUAGGA_H__


struct thread_master *dp_quagga_master_get(void);
s32 dp_quagga_init(void);

s32 dp_quagga_rtnl_init(void);
s32 dp_quagga_cmd_init(void);


#endif //__DP_QUAGGA_H__
