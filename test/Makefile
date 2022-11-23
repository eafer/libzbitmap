CC = gcc

CFLAGS = --std=gnu99 -O3 -Wall -Wextra -fno-strict-aliasing -pthread -I..

test: test.o ../libzbitmap.a
	$(CC) $(LDFLAGS) -pthread -o test test.o ../libzbitmap.a

../libzbitmap.a: FORCE
	$(MAKE) -C .. --no-print-directory
FORCE:

test.o: test.c ../libzbitmap.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -o test.o -c test.c

clean:
	rm -f test.o test
