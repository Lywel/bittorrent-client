#ifndef BENCODE_PARSER_H
# define BENCODE_PARSER_H

struct dico_entry
{
    char *key;
    char *val;
};

struct dico
{
    s_entry *head;
    s_dico *tail;
};

int bencode_file_pretty_print(FILE *cout, char *path);

#endif
