#! /bin/bash
gcc -c -Wall -Werror -fpic comunicaciones.c 
gcc -c -Wall -Werror -fpic utiles.c 
gcc -shared -o libcomunicaciones.so comunicaciones.o
gcc -shared -o libutil.so utiles.o
cp libcomunicaciones.so /usr/lib/
cp libutil.so  /usr/lib/
cp comunicaciones.h /usr/include/
cp utiles.h  /usr/include/

