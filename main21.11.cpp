#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector> 
bool isPrime(int j)
{
    if(j <= 1) return 0; 
    for (int i = 2; i * i <= j; i++) 
        if (j % i == 0) return 0;
    return 1;
}

int main(int argc, char **argv)
{
    int N = 10;
    int p1[2];
    int p2[2];
    printf("%d children\n", N);
    
    pipe(p1);
    
    for (int n = 0; n < N; n++)
    {
        
        pipe(p2);
        
        fflush(stdout);
        
        int satrtInt =n*1000;
        write(p1[1], &satrtInt, sizeof(satrtInt));
        
        printf("Child %2d :\n", n+1);
      
        if ( fork() == 0){
            
            close(p1[1]);
            int satrtInter=0;
            
            read(p1[0], &satrtInter, sizeof(satrtInter));
            printf("Interval %d - %d\n",satrtInter+1,satrtInter+1000);
            for(int j=satrtInter+1;j<=satrtInter+1000;j++)
            {
                if(isPrime(j))
                    {
                        write(p2[1], &j, sizeof(j));
                    }
            }
            write(p2[1], &satrtInter, sizeof(int));
            close(p1[0]);
            close(p1[1]);
            close(p2[0]);
            close(p2[1]);
            exit(0);
        }
        int prime1;
        read(p2[0], &prime1, sizeof(prime1));
         while (prime1%1000 !=0)
            {
                if(prime1==0)
                    break;
                printf("\t \t %d \n", prime1);
                read(p2[0], &prime1, sizeof(prime1));
                 
            }
            close(p2[0]);
    }
    return 0;
}
