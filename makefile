all:
	gcc client.c -o client; gcc server.c server

run:
	./server ; ./client