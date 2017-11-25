AM_CFLAGS = -Wall -Wextra -Werror -std=c99 -pedantic -D_POSIX_C_SOURCE=200112L
AM_LDFLAGS = -lcurl -lcrypto -lm

bin_PROGRAMS = $(top_builddir)/my-bittorrent
__top_builddir__my_bittorrent_CPPFLAGS = -I$(srcdir)/include
__top_builddir__my_bittorrent_SOURCES = \
  bencode/bencode.c \
  bencode/bencode_json.c \
  bencode/bencode_parser.c \
  init_client/client.c \
  init_client/get_peer_list.c \
  init_client/decode_binary_peers.c \
  buffer.c \
  debug.c \
  dico_finder.c \
  peer_id.c \
  my-bittorrent.c \
  socket_init.c \
  socket_close.c \
  handshake.c \
  recieve_message.c \
  send_message.c \
  network_loop.c
