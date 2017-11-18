#ifndef DEBUG_H
# define DEBUG_H

# define KCYN "\x1b[36m"
# define KNRM "\x1B[0m"

void debug(char *fmt, ...);
void verbose(char *fmt, ...);

#endif
