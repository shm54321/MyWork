#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/time.h>
#include <semaphore.h>
#include <pthread.h>
#define MAX 100    
 
pthread_condattr_t philsharedc[MAX];
pthread_mutexattr_t philsharedm[MAX];
int status;
struct semaphore
 {
                  pthread_mutex_t lock;
                  pthread_mutex_t philospher[MAX];
                  pthread_barrier_t barr;
                  pthread_cond_t nonzero;
                  pthread_cond_t eating_zero[MAX];
                  int eating_count[MAX];
                  int count;
                  int state[MAX];
                  int phil_count;
                                           
 };
typedef struct semaphore semaphore_t;

semaphore_t *semaphore_create(char *semaphore_name,int count);
semaphore_t *semaphore_open(char *semaphore_name);
void semaphore_post(semaphore_t *semap);
void semaphore_wait(semaphore_t *semap);
void semaphore_close(semaphore_t *semap);
void semaphore_phil_post(semaphore_t *semap1,int i);
void semaphore_phil_wait(semaphore_t *semap1,int i);


semaphore_t * semaphore_create(char *semaphore_name,int count)
{
  int fd,i;
  semaphore_t *semap;
  fd = open(semaphore_name, O_RDWR | O_CREAT | O_EXCL, 0777);
  if(fd==-1)
  {
	 printf("Error In Opening Backing File %d",fd);
  }
  ftruncate(fd, sizeof(semaphore_t));
  semap = (semaphore_t *) mmap(NULL, sizeof(semaphore_t),PROT_READ | PROT_WRITE,MAP_SHARED,fd, 0);
  semap->count=1;
  for(i=0;i<count;i++)
  {
  semap->eating_count[i]=0;
  semap->state[i]=0;
  }
  //process mutex initilization
  pthread_mutexattr_t psharedm;
  pthread_mutexattr_init(&psharedm);
  pthread_mutexattr_setpshared(&psharedm,PTHREAD_PROCESS_SHARED);
  pthread_mutex_init(&(semap->lock), &psharedm);

  // condition values initialization
  pthread_condattr_t psharedc;
  pthread_condattr_init(&psharedc);
  pthread_condattr_setpshared(&psharedc,PTHREAD_PROCESS_SHARED);
  pthread_cond_init(&semap->nonzero, &psharedc);
  
  // philospher_mutex
  pthread_mutexattr_t philsharedm[MAX];
  for(i=0;i<count;i++)
  {
  pthread_mutexattr_init(&philsharedm[i]);
  pthread_mutexattr_setpshared(&philsharedm[i],PTHREAD_PROCESS_SHARED);
  pthread_mutex_init(&semap->philospher[i], &philsharedm[i]);
  }	 
  
  for(i=0;i<count;i++)
  {
  pthread_condattr_init(&philsharedc[i]);
  pthread_condattr_setpshared(&philsharedc[i],PTHREAD_PROCESS_SHARED);
  pthread_cond_init(&semap->eating_zero[i], &philsharedc[i]);
  }	  
  
  //barrier    
  pthread_barrierattr_t barattr;
  pthread_barrierattr_setpshared(&barattr, PTHREAD_PROCESS_SHARED);
  pthread_barrier_init(&semap->barr, &barattr,count);

  close(fd);
  return semap;
}


semaphore_t * semaphore_open(char *semaphore_name)
{
  int fd;
  semaphore_t *semap;
  fd = open(semaphore_name, O_RDWR, 0777);
  if (fd < 0)
  return (NULL);
  semap = (semaphore_t *) mmap(NULL, sizeof(semaphore_t),PROT_READ | PROT_WRITE, MAP_SHARED,fd, 0);
  close (fd);
  return (semap);
}

void semaphore_post(semaphore_t *semap1)
{
  pthread_mutex_lock(&semap1->lock); 
  if (semap1->count == 0)
      pthread_cond_signal(&semap1->nonzero);
  semap1->count++;
  pthread_mutex_unlock(&semap1->lock);
 }

void semaphore_wait(semaphore_t *semap1)
{
   pthread_mutex_lock(&(semap1->lock));
   while (semap1->count== 0)
     pthread_cond_wait(&semap1->nonzero, &semap1->lock);
   semap1->count--;
   pthread_mutex_unlock(&semap1->lock);
}

void semaphore_phil_post(semaphore_t *semap1,int i)
{
	pthread_mutex_lock(&semap1->philospher[i]);
    if (semap1->eating_count[i] == 0)
     pthread_cond_signal(&semap1->eating_zero[i]);
    semap1->eating_count[i]=semap1->eating_count[i]+1;
    pthread_mutex_unlock(&semap1->philospher[i]);
}

 void semaphore_phil_wait(semaphore_t *semap1,int i)
{
	pthread_mutex_lock(&semap1->philospher[i]);
    while (semap1->eating_count[i] == 0)
       pthread_cond_wait(&semap1->eating_zero[i], &semap1->philospher[i]);
    semap1->eating_count[i]=semap1->eating_count[i]-1;
    pthread_mutex_unlock(&semap1->philospher[i]);
}

void semaphore_close(semaphore_t *semap)
{
   munmap((void *) semap, sizeof(semaphore_t));
}
