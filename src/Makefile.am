AM_CFLAGS = -Wall -Wextra -Werror -std=c99 -pedantic
AM_LDFLAGS = -lcurl -lcrypto -lm

bin_PROGRAMS = my-bittorrent
my_bittorrent_CPPFLAGS = -I$(srcdir)/include
my_bittorrent_SOURCES = \
  buffer.c \
  bencode.c \
  bencode_json.c \
  bencode_parser.c \
  debug.c \
  request_tracker.c \
  dico_finder.c \
  peer_id.c \
  dump_peers.c \
  my-bittorrent.c \
  hash.c \
  socket_init.c \
  socket_close.c \
  handshake.c \
  decode_binary_peers.c \
  recieve_message.c \
  client.c
