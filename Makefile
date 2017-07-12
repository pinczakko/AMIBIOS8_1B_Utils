CC=gcc
CFLAGS=-I.

all: ami_1B_splitter ami_1B_combiner

ami_1B_splitter: ami_1B_splitter.c ami_1B_lib.c ami_1B.h ami_1B_internal.h 
		gcc -o $@ $^ $(CFLAGS) 

ami_1B_combiner: ami_1B_combiner.c ami_1B_lib.c ami_1B.h ami_1B_internal.h 
		gcc -o $@ $^ $(CFLAGS) 

.PHONY: clean

clean:
		rm -vf ami_1B_splitter ami_1B_combiner *.o *~ core 
