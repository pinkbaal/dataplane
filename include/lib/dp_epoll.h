#ifndef __DP_EPOLL_H__
#define __DP_EPOLL_H__


#include <dp_types.h>
#include <dp_list.h>

typedef struct dp_ep_event_t dp_ep_event_s;

typedef s32 (*dp_ep_handler_f)(dp_ep_event_s *);

struct dp_ep_event_t {
	s32 fd;
	dp_ep_handler_f func;
	void *args;
	struct list_head list;
};

s32 dp_epoll_evnet_handle(s32 timeout);
s32 dp_epoll_del_fd(s32 fd);
s32 dp_epoll_add_fd(s32 fd, dp_ep_handler_f func, void *args, u32 events);
s32 dp_epoll_init(void);

#endif //__DP_EPOLL_H__
