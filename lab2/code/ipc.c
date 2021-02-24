#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>


#define BUFFER_SIZE 25
#define READ_END 0
#define WRITE_END 1

int main(int argc, char** argv) {

    //revisar que argc sea mayor igual a 3
    pid_t pid;
    int n, named_fd, fd[2];
    const int SIZE = 4096;
    const char* NAME = "/OS";
    const char* myfifo = "/tmp/myfifo";
 
    if(argc != 3) {
        fprintf(stderr, "Invalid params\n");
        return 1;
    }

    
    sscanf(argv[1], "%d", &n);
    char* x = argv[2];    
    int buffer_size = strlen(x)+1;

    printf("I am %s\n", x);
    mkfifo(myfifo, 0666);

    named_fd = open(myfifo, 0666);
    
    int shm_fd;
    char* ptr;
    shm_fd = shm_open(NAME, O_EXCL | O_CREAT | O_RDWR, 0666);
    if(shm_fd == -1){
        //already create
        printf("%c: Share mem obj already created\n", x[0]);
        shm_fd = shm_open(NAME, O_RDWR, 0666);
        int f;
        read(named_fd, &f, sizeof(f));
        printf("%c: Received shm fd: %d \n",x[0], f);
    } else {
        printf("%c: Create new shared memory object %d\n", x[0], shm_fd);
        write(named_fd, &shm_fd, sizeof(shm_fd));
        printf("%c: Initialized shared mem obj\n", x[0]);
    }
    
    close(named_fd);
    
    ftruncate(shm_fd, SIZE);
    ptr = (char *) mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    printf("%c: ptr created with value %p", x[0], ptr);


    if(pipe(fd) == -1) {
        fprintf(stderr, "Pipe failed");
        return 1;
    }
    pid = fork();

    if(pid == 0) {
        char msg_read[buffer_size];
        close(fd[WRITE_END]);
        while(read(fd[READ_END],msg_read, buffer_size) > 0) {
            sprintf(ptr, "%s", (char *) msg_read);
            ptr += strlen(msg_read);
        }
        close(fd[READ_END]);
        
    } else {
        close(fd[READ_END]);
        for(int i=0; i<SIZE; i++){
            if(i%n == 0){
                write(fd[WRITE_END], x, buffer_size);
            }
        }
        close(fd[WRITE_END]);   
        wait(NULL);
        printf("%c: Shared memory has %s\n", x[0], (char *)ptr);
    }

    
    shm_unlink(NAME);
    close(named_fd);
	munmap(ptr, SIZE);
	shm_unlink(NAME);
    

    return 0;
}