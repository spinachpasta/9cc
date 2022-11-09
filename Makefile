CFLAGS=-std=c11 -g -static -Wall

9cc: 9cc.c

test: 9cc
		python3 test.py 

clean:
		rm -f 9cc *.o *~ tmp*

.PHONY: test clean
