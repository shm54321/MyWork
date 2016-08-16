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
#include <stdlib.h>
#include "sem.h"

int main(int argc, char **argv)
{
	int Status;
 if(argc!=3){
 printf("inadequate number of parameters");
 exit(0);
 }
 remove("backfile"); // DELETING EXISTING BACKING FILE
 int M=atoi(argv[1]); //No. Of Philosphers
 int N=atoi(argv[2]); // No. of Times each philospher will executre
 semaphore_t *semap;
 semap = semaphore_create("backfile",M); // INITIALIZING SHARED MEMORY
 int i=0;
 pid_t pid,wpid;
 char str1[15];
 sprintf(str1, "%d",N);
 int status;
 semap->phil_count=M;
 semaphore_close(semap);
 char *args[] = {"./Philosopher"};
 int aInt;
 char str[15];
 // creating M forks
 for(i=0;i<M;i++)
 {
   aInt=i;
   sprintf(str, "%d",i);
   pid=fork();
   if(pid==0)
   {
   execlp(args[0], args[0],str,str1,NULL);
   perror("execve");   /* execve() returns only on error */
   exit(EXIT_FAILURE);
   }
 }
while ((wpid = wait(&status)) > 0); 
}
