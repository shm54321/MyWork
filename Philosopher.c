#include "pthread.h"
#include "sem.h"
#include<stdio.h>
#include<semaphore.h>
#include<pthread.h>
#include<unistd.h>

#define THINKING 0
#define HUNGRY 1
#define EATING 2

void philospher(int num,int times);
void Grabfork(int);
void Releasefork(int);
void test(int);

int N;
semaphore_t *semap;
int main(int argc,char **argv)
{
    semap = semaphore_open("backfile");
    int i=0;
    N=semap->phil_count;
    i = atoi(argv[1]);
    int times = atoi(argv[2]);
    printf("Philosopher %d is thinking\n",i+1);
    pthread_barrier_wait(&semap->barr);
    philospher(i,times);
    semaphore_close(semap);
    return (0);
              }
void philospher(int num,int times){
    while(times>0){
 
     semaphore_wait(semap);
     int i=num;
     semaphore_post(semap);
     sleep(1);
     Grabfork(i);
     sleep(1);
     Releasefork(i);
     times--;
    }
}
void Grabfork(int ph_num){
    semaphore_wait(semap);
    semap->state[ph_num] = HUNGRY;
    printf("Philosopher %d is Hungry\n",ph_num+1);
    test(ph_num);
    semaphore_post(semap);
    semaphore_phil_wait(semap,ph_num);
    sleep(1);
}
void test(int ph_num){
    if (semap->state[ph_num] == HUNGRY && semap->state[(ph_num+N-1)%N] != EATING && semap->state[(ph_num+1)%N] != EATING){
        semap->state[ph_num] = EATING;
        sleep(1);
        printf("Philosopher %d is Eating\n",ph_num+1);
        semaphore_phil_post(semap,ph_num);
    }
}     
 
void Releasefork(int ph_num){
    semaphore_wait(semap);
    semap->state[ph_num] = THINKING;
    printf("Philosopher %d is thinking\n",ph_num+1);
    test((ph_num+N-1)%N);
    test((ph_num+1)%N);
    semaphore_post(semap);
}         
