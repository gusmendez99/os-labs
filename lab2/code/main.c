#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main (){
	pid_t f = fork();
	if (f==0) {
		execl("ipc", "ipc", "15", "a", (char*)NULL);
	} else {
		//usleep(1000);
		execl("ipc", "ipc", "15", "b", (char*)NULL);
	}
}
