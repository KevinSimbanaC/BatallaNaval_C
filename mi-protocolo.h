#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_C 10
#define LONG_BUFFER 50
#define LUN 10
#define max_buf 30
typedef struct mensaje{
	char datos[max_buf];
	char flag[LONG_BUFFER];
	int opc;
	int puntuacion_ext;
	char coordenada_ant[max_buf];
	int num_conectados;
	char nombre[LONG_BUFFER];
	int id;
	char codigo;
	int sd_ext;
	char estado;
}MENSAJE;
typedef struct{
		struct sockaddr_in directorio;
		char estado;
		int sd;
		char nombre[LONG_BUFFER];
}DATOS;
