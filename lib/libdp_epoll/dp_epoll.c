
#include <stdio.h>
#include <sys/epoll.h>

#include <dp_types.h>
#include <dp_list.h>
#include <lib/dp_memory.h>
#include <lib/dp_epoll.h>

#define DP_EPOLL_EVENT_CNT		32

typedef struct dp_ep_master_t {
	s32 ep_fd;
	u32 nb_event;
	struct epoll_event *events;
	struct list_head head;
} dp_ep_master_s;

dp_ep_master_s *dp_ep_master;


s32 dp_epoll_init(void)
{
	dp_ep_master = dp_malloc(0, sizeof(dp_ep_master_s));
	if (dp_ep_master == NULL) {
		return -1;
	}
	dp_ep_master->ep_fd = epoll_create(1); 
	if (dp_ep_master->ep_fd < 0) {
		return -1;
	}
	dp_ep_master->events = dp_malloc(0, DP_EPOLL_EVENT_CNT * sizeof(struct epoll_event));
	if (dp_ep_master->events == NULL) {
		return -1;
	}
	dp_ep_master->nb_event = DP_EPOLL_EVENT_CNT;
	INIT_LIST_HEAD(&dp_ep_master->head);

	return 0;
}

s32 dp_epoll_add_fd(s32 fd, dp_ep_handler_f func, void *args, u32 events)
{
	struct epoll_event ev = {};
	dp_ep_event_s *dp_ev = NULL;

	if (func == NULL) {
		return -1;
	}
	dp_ev = (dp_ep_event_s *)dp_malloc(0, sizeof(dp_ep_event_s));
	if (dp_ev == NULL) {
		return -1;	
	}
	dp_ev->fd = fd;
	dp_ev->func = func;	
	dp_ev->args = args;
	list_add(&dp_ev->list, &dp_ep_master->head);

	ev.data.ptr = dp_ev;
    ev.events = events;
	if (epoll_ctl(dp_ep_master->ep_fd, EPOLL_CTL_ADD, fd, &ev) < 0) {
		return -1;
	}
	
	return 0;	
}

s32 dp_epoll_del_fd(s32 fd)
{
	dp_ep_event_s *ep_ev = NULL, *ep_ev_next = NULL;

	if (epoll_ctl(dp_ep_master->ep_fd, EPOLL_CTL_DEL, fd, NULL) < 0) {
		return -1;
	}
	list_for_each_entry_safe(ep_ev, ep_ev_next, &dp_ep_master->head, list) {
		if (ep_ev->fd == fd) {
			list_del(&ep_ev->list);
			close(ep_ev->fd);
			dp_free(ep_ev);
			return 0;
		}
	}
	
	return -1;
}

s32 dp_epoll_evnet_handle(s32 timeout)
{
	s32 nb_fd, i, nb_event;
	dp_ep_event_s *ep_event;
	struct epoll_event *ev_tmp;
	
	nb_fd = epoll_wait(dp_ep_master->ep_fd, dp_ep_master->events, dp_ep_master->nb_event, timeout);
	if (nb_fd < 0) {
		return -1;
	}

	for(i = 0; i < nb_fd; i++) {
		ep_event = (dp_ep_event_s *)dp_ep_master->events[i].data.ptr;	
		ep_event->func(ep_event);
    }

	if (nb_fd == dp_ep_master->nb_event) {
		nb_event = dp_ep_master->nb_event * 2;
		ev_tmp = dp_zalloc(0, nb_event * sizeof(struct epoll_event));
		if (ev_tmp) {
			dp_ep_master->nb_event = nb_event;
			dp_ep_master->events = ev_tmp;
		}
	}

	return 0;
}

