#include <stdio.h> 
#include <sys/types.h> 
#include <unistd.h>
#include<stdbool.h>

int main() 
{ 
    pid_t pid1;
    
    pid1 = fork();
    if( pid1 == 0 ) {
        // Code for child
        printf("Hello from Child!\n"); 
    } else {
        while(true)
        {
        // Nothing to do here...
        }
    }
    
    
    return 0; 
} 
