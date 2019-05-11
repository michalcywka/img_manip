a.out: odczyt.o modul.o
	gcc odczyt.o modul.o

odczyt.o: odczyt.c modul.h
	gcc -c -Wall -pedantic odczyt.c

modul.o: modul.h modul.c
	gcc -c -Wall -pedantic modul.c
