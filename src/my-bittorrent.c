#include <string.h>
#include <stdio.h>
#include "bencode_parser.h"
#include "dump_peers.h"

static int download(char *path)
{
  path = path;
  return 0;
}

static int
print_usage(char *bin)
{
  fprintf(stderr, "Usage: %s /path/to/file.torrent "
                  "[--pretty-print-torrent-file|--dump-peers]\n", bin);
  return -1;
}

static int
parse_args(int argc, char **argv)
{
  char *torrent = NULL;
  char action = 0;

  while (--argc > 0)
  {
    if (!strcmp("--pretty-print-torrent-file", argv[argc]))
      action = 'p';
    else if (!strcmp("--dump-peers", argv[argc]))
      action = 'd';
    else if (!strncmp("--", argv[argc], 2))
      return print_usage(argv[0]);
    else
      torrent = argv[argc];
  }

  if (!torrent)
    return print_usage(argv[0]);

  switch (action)
  {
  case 'p':
    return bencode_file_pretty_print(torrent);
  case 'd':
    return dump_peers(torrent);
  default:
    return download(torrent);
  }
}

int
main(int argc, char **argv)
{
  if (argc < 2)
    return print_usage(argv[0]);
  return parse_args(argc, argv);
}
