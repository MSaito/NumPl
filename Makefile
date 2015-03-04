#for GNU make

#DDEBUG = -O0 -g -ggdb -DDEBUG=1
#DDEBUG = -DDEBUG=1
#DSINGLE = -DSINGLE_FLAG=1

CC = gcc -I. -Wall -Wextra -Wno-unused-parameter -O3 -std=c99 \
-Wmissing-prototypes $(DDEBUG) $(DSINGLE)
KILLER_OBJ = kill_single.o kill_hidden_single.o locked_candidate.o \
	tuple.o fish.o xywing.o

all:solve digging_hole convert analyze normalize generate

#solve: solve.c constants.o ${KILLER_OBJ} common.o analyze.o
#	${CC} -DMAIN -o $@ solve.c constants.o ${KILLER_OBJ} common.o analyze.o

analyze: analyze.c solve.o constants.o ${KILLER_OBJ} common.o
	${CC} -DMAIN -o $@ analyze.c solve.o constants.o ${KILLER_OBJ} common.o

normalize: normalize.c analyze.o constants.o ${KILLER_OBJ} common.o convert.o \
	solve.o
	${CC} -DMAIN -o $@ normalize.c analyze.o constants.o ${KILLER_OBJ} \
	common.o convert.o solve.o

digging_hole: digging_hole.c ${KILLER_OBJ} solve.o constants.o common.o \
	analyze.o xsadd.o
	${CC} -DMAIN -o $@ digging_hole.c solve.o constants.o ${KILLER_OBJ} \
		common.o xsadd.o analyze.o

convert: convert.c constants.o common.o xsadd.o ${KILLER_OBJ} \
	solve.o analyze.o
	${CC} -DMAIN -o $@ convert.c constants.o  common.o xsadd.o \
	${KILLER_OBJ} solve.o analyze.o

generate: generate.c normalize.o analyze.o solve.o convert.o constants.o \
	${KILLER_OBJ} common.o xsadd.o
	${CC} -DMAIN -o $@ generate.c normalize.o analyze.o solve.o convert.o \
	constants.o ${KILLER_OBJ} common.o xsadd.o

doc:
	doxygen doxygen.cfg

constants.o: constants.h numpl.h

.c.o:
	${CC} -c $<

clean:
	rm -rf *.o *~ *.dSYM
