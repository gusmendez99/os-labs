#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>

int main() 
{
    double time_spent = 0.0;
    int iterations = 1000000;
    pid_t pid1, pid2, pid3; 
    clock_t begin, end;
    int i;

    begin = clock();

    pid1 = fork();
    if( pid1 == 0 ) {
        // Code for child
        pid2 = fork();
        if( pid2 == 0 ) {
            // Code for grandson
            pid3 = fork();
            if( pid3 == 0 ) {
                // Code for great-grandson
                for(i=0; i< iterations; i++){
                    
                }
            } else {
                for(i=0; i< iterations; i++){
                    
                }
                wait(NULL);            
            }
        } else {
            for(i=0; i< iterations; i++){
                
            }
            wait(NULL);         
        }
    } else {
        for(i=0; i< iterations; i++){
            
        }
        wait(NULL); 
        end = clock();

        time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
        printf("Time elapsed is %.20f seconds\n", time_spent);        
    }
    

    

    return 0;
}
