
#include <stdio.h>	 
#include <sys/ipc.h> 
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>	 
#include <wait.h>	 
#include <stdlib.h>	 

void sem_lock(int sem_set_id);
void sem_unlock(int sem_set_id);
void count(int sem_set_id, int* number,int* numbers_num, int proc); 

void _do(int sem_set_id, int* numbers,int* numbers_num, int proc)
{
    printf("%d-Process start\n",proc);
     static int initialized = 0;
    
        if (!initialized) {
    	srand(time(NULL));
    	initialized = 1;
        }
    while(*numbers_num<999){
       if(rand() % 2==1) {
            count(sem_set_id,numbers,numbers_num,proc);
        }
        else { printf("%d-no \n",proc);
            
        }
    }
    printf("%d -Process has been terminated\n",proc);
}



int main(int argc, char* argv[])
{
    int sem_set_id;           
    union semun {              
                int val;
                struct semid_ds *buf;
                ushort * array;
        } sem_val;    
    int shm_id;	      	      
    char* shm_addr; 	      
    
    int* numbers_num;  
    int* numbers; 
    struct shmid_ds shm_desc;
    int rc;		       
    pid_t pid;		     

    sem_set_id = semget(250, 1, IPC_CREAT | 0600);
    if (sem_set_id == -1) {
	exit(1);
    }

    sem_val.val = 1;
    rc = semctl(sem_set_id, 0, SETVAL, sem_val);
    if (rc == -1) {
	exit(1);
    }

    shm_id = shmget(100, sizeof(int)*1000, IPC_CREAT | IPC_EXCL | 0600);
    if (shm_id == -1) {
        exit(1);
    }

    shm_addr = shmat(shm_id, NULL, 0);
    if (!shm_addr) { 
        exit(1);
    }
    numbers_num = (int*) shm_addr;
    *numbers_num = 0;
    numbers = (int*) ((void*)shm_addr+sizeof(int));

        pid = fork();
        switch (pid) {
    	case -1:
    	    exit(1);
    	    break;
    	case 0:
    	    _do(sem_set_id,numbers,numbers_num, 1);
    	    exit(0);
    	    break;
    	default:
    	     _do(sem_set_id,numbers,numbers_num, 2);
    	    break;
        }

    {
        int child_status;

        wait(&child_status);
    }

    shmdt(shm_addr) ;

    shmctl(shm_id, IPC_RMID, &shm_desc) ;

    return 0;
}


void sem_lock(int sem_set_id)
{
    struct sembuf sem_op;
    
    sem_op.sem_num = 0;
    sem_op.sem_op = -1;
    sem_op.sem_flg = 0;
    semop(sem_set_id, &sem_op, 1);
}

void sem_unlock(int sem_set_id)
{
    struct sembuf sem_op;

    sem_op.sem_num = 0;
    sem_op.sem_op = 1;  
    sem_op.sem_flg = 0;
    semop(sem_set_id, &sem_op, 1);
}

void count(int sem_set_id, int* number, int* numbers_num,int proc)
{
    sem_lock(sem_set_id);
    int num=number[*numbers_num];
    (*numbers_num) ++;
    printf("%d-Process  %d\n",proc,num+1);
    number[*numbers_num]=num+1;
    sem_unlock(sem_set_id);
}


