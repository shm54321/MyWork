default: Philosopher Host

Philosopher:
	gcc -o Philosopher Philosopher.c -lpthread -w
Host:
	gcc -o Host Host.c -lpthread -w
clean:	
	rm -rf *.o Host Philosopher

