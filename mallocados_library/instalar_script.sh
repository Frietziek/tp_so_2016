#! /bin/bash
gcc -c -Wall -Werror -fpic comunicaciones.c 
gcc -c -Wall -Werror -fpic utiles.c 
gcc -c -Wall -Werror -fpic serializacion.c 
gcc -shared -o libcomunicaciones.so comunicaciones.o
gcc -shared -o libutil.so utiles.o
gcc -shared -o libserializacion.so serializacion.o
cp libcomunicaciones.so /usr/lib/
cp libutil.so  /usr/lib/
cp libserializacion.so  /usr/lib/
cp comunicaciones.h /usr/include/
cp utiles.h  /usr/include/
cp serializacion.h  /usr/include/

