CC = gcc

CFLAGS = --std=gnu99 -O2 -Wall -Wextra -fno-strict-aliasing -I..

test: test.o ../libzbitmap.a
	$(CC) $(LDFLAGS) -o test test.o ../libzbitmap.a

../libzbitmap.a: FORCE
	$(MAKE) -C .. --no-print-directory
FORCE:

test.o: test.c ../libzbitmap.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -o test.o -c test.c

clean:
	rm -f test.o test
