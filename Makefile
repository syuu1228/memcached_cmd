OBJS = get put
CFLAGS = -Wall -Werror -g -lmemcached

all: $(OBJS)

clean:
	rm -fv $(OBJS)
