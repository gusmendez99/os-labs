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
        int i;
        for(i=0; i<4000000;i++)
        {
            printf("%d, ", i);
        } 
    } else {
        while(true)
        {
        // Nothing to do here...
        }
    }
    
    
    return 0; 
} 
