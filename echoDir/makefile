all: client server newClient

client: client.c
	gcc -o client client.c -Wall -Werror

newClient: newClient.c
	gcc -o newClient newClient.c -Wall -Werror

server: server.c
	gcc -o server server.c -Wall -Werror

clean:
	rm -rf client server newClient
