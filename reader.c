
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>


int main(void){

    int i = 0;

    int fdev, flog;
    char cadena[100];
    int buffer[3];

    fdev = open("/dev/drv_tp4", O_RDWR);
    flog = open("log.csv", O_RDWR | O_CREAT);

    if(fdev < 0 || flog < 0){

		printf("No se pudo hacer el open.\n");

	}else{

        while(i < 10){

            read(fdev, buffer, sizeof(int)*3);
            write(fdev, "a", 1);

            sprintf(cadena, "%d,%d,%d\n", buffer[0], buffer[1], buffer[2]);
            write(flog, cadena, strlen(cadena));

            sleep(1);
            i++;

        }

    }

    close(fdev);
    close(flog);

    return 0;

}