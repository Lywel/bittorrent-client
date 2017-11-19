#ifndef NETWORK_LOOP_H
# define NETWORK_LOOP_H
# include <sys/epoll.h>

int network_loop(int efd, struct epoll_event *events);

#endif
