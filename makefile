all:
	gcc -pthread -o chat Chat.c

clean:
	rm chat
	rmdir *