#include <stdio.h>
#include <time.h>

int main() 
{
    double time_spent = 0.0;
    int iterations = 1000000;
    clock_t begin, end;
    int i;

    begin = clock();
    for(i=0; i< iterations; i++){
        
    }

    for(i=0; i< iterations; i++){
        
    }

    for(i=0; i< iterations; i++){
        
    }
    end = clock();

    time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Time elapsed is %f seconds\n", time_spent);

    return 0;
    



}
