TARGET:=uapp

CC:=gcc
CFLAGS:=-I./inc -g
LDFLAGS:=-L./lib -lubox -lubus
SRCS:=$(wildcard *.c)
OBJS:=$(patsubst %.o,%.c,$(SRCS))

.PHONY: all clean

all: $(TARGET)

$(TARGET):$(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) 

$(OBJS):$(SRCS)
	$(CC) -c $^ $@

clean:
	rm -f $(OBJS) $(TARGET)
