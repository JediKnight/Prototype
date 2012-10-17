#!/bin/sh

[ ! -z $1 ] && opt="-D${1}"
gcc -Wall $opt client.c mystring.c -o client
gcc -Wall $opt server.c mystring.c -o server
