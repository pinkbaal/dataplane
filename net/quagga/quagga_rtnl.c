
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>

#include <dp_types.h>
#include <lib/dp_memory.h>
#include <lib/quagga/thread.h>
#include <net/dp_quagga.h>

typedef struct dp_quagga_rtnl_t {
	s32 sock;
	s8 *buf;
	s32 bufsize;
	struct thread *t_netlink;
	struct thread_master *master;
} dp_quagga_rtnl_s;


dp_quagga_rtnl_s dp_quagga_rtnl;


static inline __u32 nl_mgrp(__u32 group)
{
	return group ? (1 << (group - 1)) : 0;
}

static s32 dp_quagga_rtnl_socket(void)
{
	s32 sock;
	u32 groups = 0;
	socklen_t addr_len;
	struct sockaddr_nl snl;
	s32 sndbuf = 32768;
	s32 rcvbuf = 1024 * 1024;

	groups |= nl_mgrp(RTNLGRP_NEIGH);

	sock = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
	if (sock < 0) {
		return -1;
	}

	if (setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &sndbuf, sizeof(sndbuf)) < 0) {
		return -1;
	}

	if (setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &rcvbuf, sizeof(rcvbuf)) < 0) {
		return -1;
	}

	memset(&snl, 0, sizeof(snl));
	snl.nl_family = AF_NETLINK;
	snl.nl_groups = groups;
	if (bind(sock, (struct sockaddr *)&snl, sizeof(snl)) < 0) {
		return -1;
	}
	
	addr_len = sizeof(snl);
	if (getsockname(sock, (struct sockaddr *)&snl, (socklen_t *)&addr_len) < 0) {
		return -1;
	}
	
	if (addr_len != sizeof(snl)) {
		return -1;
	}
	if (snl.nl_family != AF_NETLINK) {
		return -1;
	}
	
	if (fcntl (sock, F_SETFL, O_NONBLOCK) < 0) {
		return -1;
	}

	return sock;
}


static s32 dp_quagga_rtnl_callback (struct sockaddr_nl *snl, struct nlmsghdr *h)
{
	/* JF: Ignore messages that aren't from the kernel */
	if (snl->nl_pid != 0) {
		return 0;
	}

	printf("%d, %d, %d, %d\n", h->nlmsg_type, RTM_NEWNEIGH, RTM_DELNEIGH, RTM_GETNEIGH);
	switch (h->nlmsg_type) {
		case RTM_NEWNEIGH:
		case RTM_DELNEIGH:
		case RTM_GETNEIGH:
			break;
		default:
			break;
    }
	
	return 0;
}

static s32 dp_quagga_rtnl_parse (void)
{
	int status;
	int ret = 0;
	int error;
	struct iovec iov = {
		.iov_base = dp_quagga_rtnl.buf,
		.iov_len = dp_quagga_rtnl.bufsize,
	};
	struct sockaddr_nl snl;
	struct msghdr msg = {
		.msg_name = (void *)&snl,
		.msg_namelen = sizeof(snl),
		.msg_iov = &iov,
		.msg_iovlen = 1
	};
	struct nlmsghdr *h;

	while (1) {
		status = recvmsg (dp_quagga_rtnl.sock, &msg, 0);
		if (status < 0) {
			if (errno == EINTR)
				continue;
			if (errno == EWOULDBLOCK || errno == EAGAIN)
				break;
			continue;
		}
		if (status == 0) {
			return -1;
		}
		if (msg.msg_namelen != sizeof(snl)) {
			return -1;
		}
	  
		for (h = (struct nlmsghdr *)dp_quagga_rtnl.buf; NLMSG_OK(h, (unsigned int) status); h = NLMSG_NEXT(h, status)) {
			/* Finish of reading. */
			if (h->nlmsg_type == NLMSG_DONE)
				return ret;

			/* Error handling. */
			if (h->nlmsg_type == NLMSG_ERROR) {
				struct nlmsgerr *err = (struct nlmsgerr *) NLMSG_DATA (h);
				int errnum = err->error;
				int msg_type = err->msg.nlmsg_type;

				/* If the error field is zero, then this is an ACK */
				if (err->error == 0) {
					/* return if not a multipart message, otherwise continue */
					if (!(h->nlmsg_flags & NLM_F_MULTI)) {
						return 0;
					}
					continue;
				}
	 			if (h->nlmsg_len < NLMSG_LENGTH (sizeof (struct nlmsgerr))) {
					return -1;
				}
				
				return -1;
			}
			error = dp_quagga_rtnl_callback(&snl, h);
			if (error < 0) {
				ret = error;
			}
		}

		/* After error care. */
		if (msg.msg_flags & MSG_TRUNC) {
			continue;
		}
		if (status) {
			return -1;
		}
	}
	
	return ret;
}


static s32 dp_quagga_rtnl_read(struct thread *thread)
{
//	void *arg = (void *)THREAD_ARG(thread);
	dp_quagga_rtnl_parse();
	dp_quagga_rtnl.t_netlink = thread_add_read(dp_quagga_rtnl.master, dp_quagga_rtnl_read, NULL, dp_quagga_rtnl.sock);

	return 0;
}

s32 dp_quagga_rtnl_init(void)
{
	memset(&dp_quagga_rtnl, 0, sizeof(dp_quagga_rtnl));

	dp_quagga_rtnl.bufsize = 2 * sysconf(_SC_PAGESIZE);
	dp_quagga_rtnl.buf = dp_zalloc(0, dp_quagga_rtnl.bufsize);
	if (dp_quagga_rtnl.buf == NULL) {
		return -1;
	}
	
	dp_quagga_rtnl.sock = dp_quagga_rtnl_socket();
	dp_quagga_rtnl.master = dp_quagga_master_get();
	dp_quagga_rtnl.t_netlink = thread_add_read(dp_quagga_rtnl.master, dp_quagga_rtnl_read, NULL, dp_quagga_rtnl.sock);

	return 0;
}

