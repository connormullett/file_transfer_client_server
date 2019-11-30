
all: obj/server.o obj/client.o obj/common.o
	gcc obj/server.o obj/common.o -o server
	gcc obj/client.o obj/common.o -o client

server: obj/server.o obj/common.o
	gcc obj/server.o obj/common.o -o server

debug: obj/server.o obj/common.o
	gcc -ggdb obj/server.o obj/common.o -o d_server
	gcc -ggdb obj/client.o obj/common.o -o d_client

client: obj/client.o obj/common.o
	gcc obj/client.o obj/common.o -o client

obj/common.o: common.c common.h
	gcc -c common.c -o obj/common.o

obj/client.o: client.c
	gcc -c client.c -o obj/client.o

obj/server.o: server.c
	gcc -c server.c -o obj/server.o

clean:
	rm obj/*.o
	rm server
	rm client

