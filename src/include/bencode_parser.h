#ifndef BENCODE_PARSER_H
# define BENCODE_PARSER_H

char *bencode_parse_str(char **bencode, long long *size);
long long bencode_parse_int(char **bencode, long long *size);
struct be_node **bencode_parse_lst(char **bencode, long long *size);
struct be_dico **bencode_parse_dic(char **bencode, long long *size);

#endif
