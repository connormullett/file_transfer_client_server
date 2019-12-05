
CC = gcc
CFLAGS = -g -O2 -Wall

OBJDIR = obj
OBJ_SERVER = $(OBJDIR)/common.o $(OBJDIR)/server.o
OBJ_CLIENT = $(OBJDIR)/common.o $(OBJDIR)/client.o

all: client server
.PHONY: all

$(OBJDIR):
	@mkdir -p $@

client: $(OBJ_CLIENT)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

server: $(OBJ_SERVER)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(OBJDIR)
	rm -f client server
.PHONY: clean

