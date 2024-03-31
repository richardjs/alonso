CFLAGS=-std=gnu17 -Wall -O3
LDFLAGS=-lm

objects=bitboards.o mcts.o state.o stateio.o

alonso: $(objects)
	# Temporary fix for building on Ubuntu 22.04
	cc -std=gnu17 -Wall -O3  alonso.c $(objects)  -lm  -o alonso

test: $(objects)

clean:
	rm -f *.o
	rm -f alonso
