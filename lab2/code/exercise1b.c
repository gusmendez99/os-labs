#include <stdio.h> 
#include <sys/types.h> 
int main() 
{ 
    int i;
    for (i = 0; i < 4; i++) {
	fork();	
    }
    printf("hello\n"); 
    
    return 0; 
} 
