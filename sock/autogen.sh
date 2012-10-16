#!/bin/sh

[ ! -z $1 ] && opt="-D${1}"
gcc -Wall $opt client.c -o client
gcc -Wall $opt server.c -o server
