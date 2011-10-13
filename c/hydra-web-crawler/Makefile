hwc: main.o web.o parse.o redis-slave.o
	g++ -Wall -lcurl hiredis/libhiredis.so web.o main.o parse.o redis-slave.o -o hwc

main.o: main.c
	g++ main.c -Wall -c

web.o: web.c web.h
	g++ web.c -Wall -c

parse.o: parse.c parse.h
	g++ parse.c -Wall -c

redis-slave.o: redis-slave.c redis-slave.h
	g++ redis-slave.c -Wall -c


# clean and other non compile actions:
clean:
	rm -rf hwc *.out *.o

clean~:
	rm -f *~

cleanout:
	rm -f dirout/*

makerun: hwc
	./hwc

deps:
	@cd redis-2.2.12 && make
	@cd hiredis && make && make install
	/sbin/ldconfig
	@cd tests/cu && make
	@cd tests && make

startdb:
	redis-2.2.12/src/redis-server redis-2.2.12/redis.conf
