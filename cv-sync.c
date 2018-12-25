/* On-demand 3-thread synchronization */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> // Compile with "gcc cv-sync.c -o cv-sync -lpthread"

//global vars to keep track of thread runs and total runs etc
int total = 0;
int t1Max = 0;
int t2Max = 0;
int t3Max = 0;
int t1Cur = 0;
int t2Cur = 0;
int t3Cur = 0;
int t1Tally = 0;
int t2Tally = 0;
int t3Tally = 0;

int count = 0;
int run = 1;

//Locks and condition vars
pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock3 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond3 = PTHREAD_COND_INITIALIZER;

//Function to print out proper usage
void usage(char * bin_str) {
    printf("Usage: %s total_runs t1_runs t2_runs t3_runs\n", bin_str);
}

//Function for thread3 which increments corresponding global vars and releases the next thread
void* print3(void* ptr)
{
	while(run){
	    pthread_mutex_lock(&lock3);
    		pthread_cond_wait(&cond3, &lock3);
		if(t1Tally + t2Tally + t3Tally < total){
			for(int i = 0; i < t3Max; i++){			
				count++;
				t3Cur++;
				t3Tally++;
    				printf("%02d: IN T3, TID=%lu \n", count, pthread_self());
				if(t1Tally + t2Tally + t3Tally == total){
					run = 0;
					break;
				}
			}
			t3Cur = 0;

		}
		if(t1Tally + t2Tally + t3Tally < total){
			usleep(1000);
   		 	pthread_cond_signal(&cond1);
		}
    		pthread_mutex_unlock(&lock3);
	}	
	pthread_exit(NULL); 
    return NULL;
}


//Function for thread2 which increments corresponding global vars and releases the next thread
void* print2(void* ptr)
{
	while(run){
	    pthread_mutex_lock(&lock2);
    		pthread_cond_wait(&cond2, &lock2);
		if(t1Tally + t2Tally + t3Tally < total){
			for(int i = 0; i < t2Max; i++){			
				count++;
				t2Cur++;
				t2Tally++;
    				printf("%02d: IN T2, TID=%lu \n", count, pthread_self());
				if(t1Tally + t2Tally + t3Tally == total){
					run = 0;
					break;
				}
			}
			t2Cur = 0;

		}
		if(t1Tally + t2Tally + t3Tally < total){
			usleep(1000);
   		 	pthread_cond_signal(&cond3);
		}
    		pthread_mutex_unlock(&lock2);
	}	
	pthread_exit(NULL); 
    return NULL;
}


//Function for thread2 which increments corresponding global vars and releases the next thread
void* print1(void* ptr)
{
	while(run){
		 pthread_mutex_lock(&lock1);
   		 pthread_cond_wait(&cond1, &lock1);
		if(t1Tally + t2Tally + t3Tally < total){
			for(int i = 0; i < t1Max; i++){			
				count++;
				t1Cur++;
				t1Tally++;
    				printf("%02d: IN T1, TID=%lu \n", count, pthread_self());
				if(t1Tally + t2Tally + t3Tally == total){
					run = 0;
					break;
				}
			}
			t1Cur = 0;

		}
		if(t1Tally + t2Tally + t3Tally < total){
			usleep(1000);
   			pthread_cond_signal(&cond2);
		}
    		pthread_mutex_unlock(&lock1);
	}	

    pthread_exit(NULL); 
    return NULL; 
}


int main(int argc, char * argv[]) {

//checks format of input
    if (argc != 5) {
        usage(argv[0]);
        return 0;
    }

//assign args to global vars
	total = atoi(argv[1]);
	t1Max = atoi(argv[2]);
	t2Max = atoi(argv[3]);
	t3Max = atoi(argv[4]);
	
//Declare threads
	pthread_t thread1;
	pthread_t thread2;
	pthread_t thread3;

//initialize threads to start at functions
	pthread_create(&thread1, NULL, print1, NULL);
	pthread_create(&thread2, NULL, print2, NULL);
	pthread_create(&thread3, NULL, print3, NULL);
//Sleep command to allow proper start of thread loop
	usleep(1000);
//Start thread 1
	pthread_cond_signal(&cond1);

	
//Infinite loop to prevent final print until threads finish execution
	while(run == 1){
	}
	printf("In main thread: T1 printed %d times. T2 printed %d times. T3 printed %d times.\n", t1Tally, t2Tally, t3Tally);

    exit(0);
}
