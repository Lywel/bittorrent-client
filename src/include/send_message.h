#ifndef SEND_MESSAGE_H
# define SEND_MESSAGE_H

#include <stddef.h>
#include "client.h"
#include "message.h"

int send_message(void *message, size_t len, struct peer *p);
int send_request_message(struct peer *p, int index, int begin, int length);
int send_message_type(enum type, struct peer *p);

#endif
