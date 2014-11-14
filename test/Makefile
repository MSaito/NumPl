#for GNU make

#DDEBUG = -O1 -g -ggdb -DDEBUG=1
#DDEBUG = -DDEBUG=1

CC = gcc -I../ -Wall -O3 -std=c99 -Wmissing-prototypes $(DDEBUG)

test_pointer: test_pointer.o
	${CC} -o $@ $<

test_pointer.o: ../constants.h ../numpl.h ../constants.o


.c.o:
	${CC} -c $<

clean:
	rm -rf *.o *~ *.dSYM
