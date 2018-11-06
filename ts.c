#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE 1
#endif

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/times.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <stdbool.h>

struct sigaction sigact;
int done;

void AlarmHandler( int signum )
{
  done = true;
  exit(0);
}
void setUp(int tiempo){
  sigact.sa_handler = AlarmHandler;
  sigemptyset( &sigact.sa_mask );
  sigact.sa_flags = 0;
  sigaction( SIGALRM, &sigact, NULL );
  alarm(tiempo);
}

void child(int tiempo){
  setUp(tiempo);
  while(!done){}
}

int main( int argc, char* argv[])
{
  if (argc<6){
  perror("Cantidad de argumentos menor a la necesaria");
  return 0;
  } 
  bool pid_array1true = false; //variable con la que se detecta si se encuentra un pid perteneciente al primer ciclo
  int i;
  int w = 0;
  int k = 0;
  int h = 0;
  int status;
  pid_t pid, pid2, pid3;
  struct tms t;
  clock_t dub;
  int tics_per_second;
  int count1 = atoi(argv[2]);
  int count2 = atoi(argv[4]);
  int tiempo = atoi(argv[5]);
  double usertime1 = 0;
  double usertime2 = 0;
  double systemtime1 = 0;
  double systemtime2 = 0;
  pid_t  pid_array1[count1]; //variable en la que se van a guardar todos los pids del primer ciclo
  tics_per_second = sysconf(_SC_CLK_TCK);

for(i=0; i<count1; i++){
    pid = fork();
    pid_array1[i]=pid;
    if (pid==0){
       child(tiempo);
       while(1){}
    }
}

for(i=0; i<count2; i++){
    pid2 = fork();
    if (pid2==0){
      child(tiempo);
      while(1){}
    }
}

 /* esperar por hijos */
  for( i = 0; i < (count1+count2); i++){
    pid3 = wait(&status); //se asigna el pid del hijo a pid3 para poder comparar si salió un hijo del primer o segundo ciclo
    if (pid3 == -1)
    perror("wait() error");
  else if (!WIFEXITED(status))
    perror("El hijo no salió exitosamente");
  else if ((dub = times(&t)) == -1)
    perror("times() error");
  else {
    pid_array1true = false;
    if (i==0) printf("UID  COUNT  USERTIME  SYSTEMTIME  USER+SYSTEM\n");
    for (w = 0; w < count1; w++){  //Se recorre el array de pids del primer ciclo 
      if (pid_array1[w]== pid3){
        k++; //k va a aumentar hasta que llegue a tener el mismo valor que count2
         pid_array1true = true;
         usertime1+= (((double) t.tms_cutime))/((tics_per_second));
         systemtime1+= (((double) t.tms_cstime))/((tics_per_second));
     if(k == count1 && h == count2){ 
       printf("%s    %d     %.3f      %.4f      %.4f \n", argv[1], count1,
           (usertime1-usertime2)/count1, (systemtime1-systemtime2)/count1, (((usertime1-usertime2)/count1)+((systemtime1-systemtime2)/count1)));
    }
    else if(k == count1 && h!=count2) {
       printf("%s    %d     %.3f      %.4f      %.4f \n", argv[1], count1,
           usertime1/count1, systemtime1/count1, ((usertime1/count1)+(systemtime1/count1)));
    }
  }
  }
 if (pid_array1true == false){
  h++; //h va a aumentar hasta que llegue a tener el mismo valor que count2
  usertime2+= (((double) t.tms_cutime))/((tics_per_second));
  systemtime2+= (((double) t.tms_cstime))/((tics_per_second));
  if(h == count2 && k == count1){
       printf("%s    %d     %.3f      %.4f      %.4f \n", argv[3], count2,
           (usertime2-usertime1)/count2, (systemtime2-systemtime1)/count2, (((usertime2-usertime1)/count2)+((systemtime2-systemtime1)/count2)));
    }
  else if (h==count2 && k!=count1){
     printf("%s    %d     %.3f      %.4f      %.4f \n", argv[3], count2,
           usertime2/count2, systemtime2/count2, ((usertime2/count2)+(systemtime2/count2)));
    
  }
  }
  }
 }
  return 0;
}
