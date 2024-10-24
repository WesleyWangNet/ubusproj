TARCLIENT:=client
TARSERVER:=server
CC:=gcc
CFLAGS:=-I../inc -g
LDFLAGS:=-L../lib -ljson-c -lblobmsg_json -lubox -lubus

.PHONY: all clean

all: $(TARSERVER) $(TARCLIENT)

$(TARSERVER): server.c
	@$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) 

$(TARCLIENT): client.c
	@$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) 

clean:
	@rm -f $(TARCLIENT)
	@rm -f $(TARSERVER)
