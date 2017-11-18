#ifndef DEBUG_H
# define DEBUG_H

# define TRACREQ_FMT "%s: tracker: requesting peers to %s\n"
# define CONNECT_FMT "%s: peers: %s: %s:%d\n"
# define PEERMSG_FMT "%s: msg: %s: %s:%d: %s"
# define CYAN    "\x1b[36m"

void debug(char *fmt, ...);

#endif
