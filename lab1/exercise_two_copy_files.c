#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#define MODE 0666
#define BUF_SIZE 8192
/* 
This is partially working

int main(int argc, char* argv[]){
  char buf[BUFF_SIZE + 1];
  int outft, inft,fileread;
  // output file opened or created
  if((outft = open(argv[1], O_CREAT | O_APPEND | O_RDWR, 0666))==-1){
    perror("open");
  }
  // lets open the input file
  inft = open(argv[2], O_RDONLY);
  if(inft >0){ // there are things to read from the input
    fileread = read(inft, buf, sizeof(buf));
    printf("%s\n", buf);
    write(outft, buf, sizeof(buf));
    close(inft);
  }
  close(outft);
  return 0;
}*/

void main(int argc, char *argv[]) {
	int source, dest, in, out;
	char buf[BUF_SIZE];

	if (argc != 3) exit(1);

	source = open(argv[1], O_RDONLY); if (source < 0) exit(2);
	dest = creat(argv[2], MODE); if (dest < 0) exit(3);
	while (1) {
		in = read(source, buf, BUF_SIZE);
		if (in <= 0) break;
		out = write(dest, buf, in);
		if (out <= 0) break;
	}
	close(source); close(dest); exit(0);
}
