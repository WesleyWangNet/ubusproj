TARGET:=blobmsgtest


SRCS:=$(wildcard *.c)
OBJS:=$(patsubst %.c, %.o, $(SRCS))
CFLAGS:=-I/usr/local/include
LDGLAGS:=-L/usr/local/lib/ -lubox -lblobmsg_json
CC=gcc



.PHONY: all clean
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $^ $(CFLAGS) $(LDGLAGS) -g -o $@ 

%.o: %.c
	$(CC) -c $< -g $(CFLAGS)

clean:
	rm -f $(OBJS) $(TARGET)
