
#include <stdio.h>

#include <dp_types.h>

#include <lib/quagga/thread.h>
#include <net/dp_quagga.h>
#include <net/dp_cmd.h>

#define DATAPLANE_CONFIG_FILE 		"/usr/local/etc/dataplane.conf"
#define DATAPLANE_VTYSH_PATH 		"/var/run/dataplane.vty"
#define DATAPLANE_VTY_PORT			9527


typedef struct dp_cmd_t {
	struct thread_master *master;
} dp_cmd_s;

dp_cmd_s dp_cmd;

static s32 dp_cmd_vty_init(void)
{
	return 0;
}

s32 dp_cmd_init(void)
{
	memset(&dp_cmd, 0, sizeof(dp_cmd));
	
	dp_cmd.master = dp_quagga_master_get();
	
	cmd_init (1);
	
	vty_init (dp_cmd.master);

	dp_cmd_vty_init();

	vty_read_config (NULL, DATAPLANE_CONFIG_FILE);
	
	vty_serv_sock (NULL, DATAPLANE_VTY_PORT, DATAPLANE_VTYSH_PATH);
	
	return 0;
}


