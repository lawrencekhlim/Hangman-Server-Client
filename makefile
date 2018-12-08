all:
	gcc -o hangman_client hangman_client.c 
	gcc -o hangman_server hangman_server.c 
server:
	gcc -o hangman_server hangman_server.c 
client:
	gcc -o hangman_client hangman_client.c 
clean:
	rm *.o 
	rm hangman_client
	rm hangman_server

	