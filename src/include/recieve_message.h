#ifndef RECIEVE_MESSAGE_H
# define RECIEVE_MESSAGE_H

#include "message.h"

int recieve_message(void);
void handle_message(struct message mess);
void handle_bitfield(struct message mess);

#endif
