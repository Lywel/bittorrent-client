#ifndef MESSAGE_H
# define MESSAGE_H

struct __attribute__((__packed__)) message {
    unsigned char len[4];
    uint8_t id;
    unsigned char payload[3000];
};

#endif
