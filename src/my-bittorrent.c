#include "bencode_parser.h"

int
main(int argc, char **argv)
{
  if (argc < 2)
    return -1;
  bencode_file_pretty_print(stdout, argv[1]);
  return 0;
}
