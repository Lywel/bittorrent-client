#!/bin/sh

echo DUMP PEERS
./my-bittorrent /tests/torrents/advanced_dir_basic.torrent --dump-peers

printf "\nPRETTY PRINT\n"
./my-bittorrent ./tests/torrents/advanced_dir_basic.torrent --pretty-print-torrent-file

printf "\nBASIC\n"
./my-bittorrent ./tests/torrents/advanced_dir_basic.torrent --verbose
tree advanced_dir

printf "\nSPLIT\n"
./my-bittorrent ./tests/torrents/myreadiso-subject_split.torrent --verbose
evince myreadiso-subject.pdf

printf "\nROGUE\n"
./my-bittorrent ./tests/torrents/images_rogue.torrent --verbose
ls -l images
