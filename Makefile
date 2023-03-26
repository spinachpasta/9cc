CFLAGS=-std=c11 -g -static -Wall -Wextra
SRCS_=$(wildcard *.c)
SRCS=$(filter-out testfunc.c,$(SRCS_))
OBJS=$(SRCS:.c=.o)

9cc: $(OBJS)
		$(CC) -o 9cc $(OBJS) $(LDFLAGS)

$(OBJS): 9cc.h

test: 9cc
		python3 testcases.py 

clean:
		rm -f 9cc *.o *~ tmp*

.PHONY: test clean
