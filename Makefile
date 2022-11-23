CC = gcc

CFLAGS = --std=gnu99 -O3 -Wall -Wextra -fno-strict-aliasing -I..

libzbitmap.a: libzbitmap.o
	ar rcs libzbitmap.a libzbitmap.o

libzbitmap.o: libzbitmap.c libzbitmap.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -o libzbitmap.o -c libzbitmap.c

clean:
	rm -f libzbitmap.o libzbitmap.a
