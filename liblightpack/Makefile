CC = gcc
CFLAGS = -Wall -g -fPIC

all: testclient liblightpack.so

liblightpack.so: liblightpack.o
	$(CC) $(CFLAGS) -shared -o $@ $<
liblightpack.o: liblightpack.c
	$(CC) $(CFLAGS) -c $<

liblightpack.a: liblightpack.o
	ar rc $@ liblightpack.o
	ranlib $@

testclient: liblightpack.o liblightpack.a
	$(CC) $(CFLAGS) testclient.c -L./ -llightpack -o $@

clean:
	rm -f *.a *.o *.so testclient

.PHONY: all clean
