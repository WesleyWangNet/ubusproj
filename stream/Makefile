TARSERVER:=server
CC:=gcc
CFLAGS:=-I../inc -g
LDFLAGS:=-L../lib -lubox -lubus

.PHONY: all clean

all: $(TARSERVER)

$(TARSERVER): s_server.c
	@$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) 

clean:
	@rm -f $(TARSERVER)
