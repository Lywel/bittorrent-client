AM_CFLAGS = -Wall -Wextra -Werror -std=c99 -pedantic
AM_LDFLAGS = -lcurl

bin_PROGRAMS = my-bittorrent
my_bittorrent_CPPFLAGS = -I$(srcdir)/include
my_bittorrent_SOURCES = \
  bencode.c \
  bencode_json.c \
  bencode_parser.c \
  request_tracker.c \
  dico_finder.c \
  peer_id.c \
  dump_peers.c \
  my-bittorrent.c
