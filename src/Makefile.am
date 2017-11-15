AM_CFLAGS = -Wall -Wextra -Werror -std=c99 -pedantic

bin_PROGRAMS = my-bittorrent
my_bittorrent_CPPFLAGS = -I$(srcdir)/include
my_bittorrent_SOURCES = \
  bencode.c \
  bencode_json.c \
  bencode_parser.c \
  dump_peers.c \
  my-bittorrent.c
