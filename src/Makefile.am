AM_CFLAGS = -Wall -Wextra -Werror -std=c99 -pedantic
AM_LDFLAGS = -lcurl -lcrypto -lm

bin_PROGRAMS = my-bittorrent
my_bittorrent_CPPFLAGS = -I$(srcdir)/include
my_bittorrent_SOURCES = \
  bencode/bencode.c \
  bencode/bencode_json.c \
  bencode/bencode_parser.c \
  get_peer_list.c \
  buffer.c \
  debug.c \
  dico_finder.c \
  peer_id.c \
  my-bittorrent.c \
  socket_init.c \
  socket_close.c \
  handshake.c \
  decode_binary_peers.c \
  recieve_message.c \
  client.c \
  send_message.c \
  network_loop.c
