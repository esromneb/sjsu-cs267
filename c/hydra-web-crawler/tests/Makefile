CC ?= gcc
CFLAGS = -g -Wall -pedantic --std=gnu99 -DCU_UNIT_TEST
LDFLAGS = -Lcu/ -lcu -lcurl


OBJS = testparse1.o test.o testweb1.o testredis.o

all: test

test: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) ../hiredis/libhiredis.so

%.o: %.c %.h ../*.c
	$(CC) $(CFLAGS) -c -o $@ $<
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o test

.PHONY: all clean
