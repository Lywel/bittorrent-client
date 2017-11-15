AM_CFLAGS = -Wall -Wextra -Werror -std=c99 -pedantic

bin_PROGRAMS = my-bittorrent
my_bittorrent_CPPFLAGS = -I$(top_srcdir)/include
my_bittorrent_SOURCES = \
  bencode_json.c \
  bencode_parser.c \
  my-bittorrent.c
