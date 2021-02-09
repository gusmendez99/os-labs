#include <stdio.h>
#include <unistd.h>

int main() {
	int f = fork();
	printf("PID->%d\n", (int) getpid());

	if ( f == 0 ) {
		// Child
		execl("hello_world", (char*)NULL);
	} else {
		execl("hello_world", (char*)NULL);
		printf("PID Parent->%d\n", (int) getpid());	
	}
	return (0);
}