#for GNU make

#DDEBUG = -O1 -g -ggdb -DDEBUG=1
#DDEBUG = -DDEBUG=1

CC = gcc -I. -Wall -O3 -std=c99 -Wmissing-prototypes $(DDEBUG)



constants.o: constants.h numpl.h

.c.o:
	${CC} -c $<

clean:
	rm -rf *.o *~ *.dSYM
