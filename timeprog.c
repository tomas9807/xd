#include <sys/times.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h> 

int main(int argc, char *argv[]) {
  if (argc < 5){
  perror("Cantidad de argumentos menor a la necesaria");
  return -1;
  } 
  int status;
  long i, j;
  struct tms t;
  clock_t dub;
  int tics_per_second;
  tics_per_second = sysconf(_SC_CLK_TCK);
  if (fork()==0){
        execlp(argv[1], argv[1],argv[2], argv[3], argv[4], (char *)NULL);
        perror("execlp"); 
        return -1;
  }
 if (wait(&status) == -1)
    perror("wait() error");
  else if (!WIFEXITED(status))
    puts("Child did not exit successfully");
  else if ((dub = times(&t)) == -1)
    perror("times() error");
  else {
    printf("Tiempo de ejecucion:     %.4f   \n",
           ((double) t.tms_cstime)/tics_per_second);
  }
  return 0;
}
