#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE 1
#endif

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/times.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <sys/resource.h>
#include <string.h>

#define COMPUTETIME 2
#define READ 0
#define WRITE 1
#define MAX_NAME_SZ 256

struct sigaction sigact; /* structura global de señal */
int done;

void AlarmHandler( int signum )
{
  static int ten_times= 0;
  if (++ten_times>10){ //Controla que se escriba en el pipe 10 veces
   exit(0);
  }
  done = 1;
}
void setUp(){
  sigact.sa_handler = AlarmHandler;
  sigemptyset( &sigact.sa_mask );
  sigact.sa_flags = 0;
  sigaction( SIGALRM, &sigact, NULL );
  alarm(COMPUTETIME);
}

void child(){
  setUp();
  while(!done){}
}


int main(int argc, char *argv[]){
    int n=0;
    int k=0;
    int pipe_descs[2];
    const char msg1[]="Introduzca el mensaje que quiere enviar\n";
    int retval = pipe(pipe_descs);
    int pid = fork();
    char buf[50] = {0} ;
    if(pid == 0){   //Dentro del hijo 1
        close(pipe_descs[READ]); 
        while(1){
            if (k>0) child();
            if(!k) write(1,msg1, sizeof(msg1)-1);
            read(STDIN_FILENO, buf, 50); //Lee el mensaje que se quiere enviar
            if ((strlen(buf) > 0) && (buf[strlen (buf) - 1] == '\n'))
            buf[strlen (buf) - 1] = '\0';
            if (!k) child();
            if(write(pipe_descs[WRITE], &buf, sizeof(buf)) != sizeof(buf) && k<10){
                perror("write failed");
                exit(EXIT_FAILURE);
            }
            ++k;
            if(k>9) {
                close(STDIN_FILENO);
            }
        }

    }

    else if(pid > 0){   //Dentro del padre
        int pid = fork();
        if(pid == 0){   //Dentro del hijo 2
            int pipestat;
            char buf2[50];
            close(pipe_descs[WRITE]);
            while((pipestat = read(pipe_descs[READ], &buf2, sizeof(buf2))) > 0 && n<10){               
                  printf("read: %s \n", buf2); 
                n++;
            }
            if (pipestat==-1) {
                perror("");
                fprintf(stderr, "Error para leer el pipe\n");
                close(pipe_descs[READ]);
                exit(EXIT_FAILURE);
            }
            close(pipe_descs[READ]);
            exit(0);
        }
    }

    int status;
    int i = 2;
    while (i > 0){
        wait(&status);
        i--;
    }
}