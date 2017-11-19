#ifndef DEBUG_H
# define DEBUG_H
# include "client.h"
# include "message.h"

# define KCYN "\x1b[36m"
# define KNRM "\x1B[0m"

void debug(char *fmt, ...);
void verbose(char *fmt, ...);
void verbose_recv(struct message mess, struct peer *peer);

#endif
