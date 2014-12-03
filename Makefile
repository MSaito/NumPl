#for GNU make

#DDEBUG = -O0 -g -ggdb -DDEBUG=1
#DDEBUG = -DDEBUG=1

CC = gcc -I. -Wall -O3 -std=c99 -Wmissing-prototypes $(DDEBUG)
KILLER_OBJ = kill_single.o kill_hidden_single.o locked_candidate.o \
	tuple.o fish.o

all:solve digging_hole convert

solve: solve.c constants.o ${KILLER_OBJ} common.o
	${CC} -DMAIN -o $@ solve.c constants.o ${KILLER_OBJ} common.o

digging_hole: digging_hole.c ${KILLER_OBJ} solve.o constants.o common.o xsadd.o
	${CC} -DMAIN -o $@ digging_hole.c solve.o constants.o ${KILLER_OBJ} \
		common.o xsadd.o

convert: convert.c constants.o  common.o xsadd.o
	${CC} -DMAIN -o $@ convert.c constants.o  common.o xsadd.o

constants.o: constants.h numpl.h

.c.o:
	${CC} -c $<

clean:
	rm -rf *.o *~ *.dSYM
