all:server_and.c server_or.c edge.c client.c
	gcc -o client client.c
	gcc -o server_and server_and.c
	gcc -o server_or server_or.c
	gcc -o edge edge.c
edge:
	@(./edge)
server_and:
	@(./server_and)
server_or:
	@(./server_or)
clean:
	rm client server_and server_or edge
.PHONY:server_and server_or edge client
