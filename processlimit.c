#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

int main( int argc, char **argv)
{
	pid_t id;
	id = 0;	
	int contador=0;
	while(id >= 0){
		id = fork();
	if (id == 0){
	contador ++;
	} else if (id > 0) {
	
	id = wait(NULL);
				break;
	}else{
		break;
	}
	}
	if (id < 0){
			printf("\n Cantidad de Procesos Activos : %i \n\n",contador);
	}
	return 0;
}
		
