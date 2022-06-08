
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>


int main(void){


    int fd;
    char comando[100];
    int buffer[3];

    fd = open("/dev/drv_tp4", O_RDWR);

    if(fd < 0){

		printf("No se pudo hacer el open.\n");

	}else{

        while(1){

            scanf(" %99[^\n]", comando);

            if(strcmp(comando, "w") == 0){
                write(fd, "a", 1);
                printf("W ok.\n");
            }else if(strcmp(comando, "r") == 0){
                read(fd, buffer, sizeof(int)*3);
                printf("R %d %d %d.\n", buffer[0], buffer[1], buffer[2]);
            }else if(strcmp(comando, "Fin")){
                break;
            }else{
                printf("Error en el comando.\n");   
            }

        }

    }

    close(fd);
    
    return 0;

}