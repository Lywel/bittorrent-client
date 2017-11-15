#ifndef DEBUG_H
# define DEBUG_H

# if DEBUG_LVL >= 1
#  define DEBUG(FMT, ...) printf(stderr, FMT, __VA_ARGS__)
# else
#  define DEBUG(FMT, ...)
# endif

# define TRACREQ_FMT "%s: tracker: requesting peers to %s\n"
# define CONNECT_FMT "%s: peers: %s: %s:%d\n"
# define PEERMSG_FMT "%s: msg: %s: %s:%d: %s"

# define LOG_TRACREQ(...) printf(TRACREQ_FMT, __VA_ARGS__)
# define LOG_CONNECT(...) printf(CONNECT_FMT, __VA_ARGS__)
# define LOG_PEERMSG(...) formatmsg(0, __VA_ARGS__)

static inline void formatmsg(char c, ...)
{
  va_list args;
  va_start(args, c);

  // TODO: print peer messages
  printf(PEERMSG_FMT, args);

  va_end(args);
}

#endif
