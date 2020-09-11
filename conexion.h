#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>
#include <arpa/inet.h> // es necesaria para que se reconozca la firma adecuada de la funciOn inet_ntoa
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define BACKLOG 4

int conectarTCP(char *ip_server, unsigned short puerto_server){
	int sd;
	struct sockaddr_in dir_server;
	if((sd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
		fprintf(stderr, "Error socket()\n");
		return -1;
	}
	dir_server.sin_family = AF_INET;
	dir_server.sin_port = htons(puerto_server);
	dir_server.sin_addr.s_addr = inet_addr(ip_server);	
	if((connect(sd, (struct sockaddr *)&dir_server, sizeof(dir_server))) < 0){
		fprintf(stderr, "Error connect()\n");
		return -1;
	}
	return sd;
}

int EscucharTCP(char *ip_server, unsigned short puerto_server){
	int sd;
	struct sockaddr_in dir_server;
	if((sd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
		fprintf(stderr, "Error socket()\n");
		return -1;
	}
	dir_server.sin_family = AF_INET;
	dir_server.sin_port = htons(puerto_server);
	dir_server.sin_addr.s_addr = inet_addr(ip_server);
	if((bind(sd, (struct sockaddr *)&dir_server, sizeof(dir_server))) < 0){
		fprintf(stderr, "Error bind()\n");
		return -1;
	}	
	if((listen(sd, BACKLOG))<0){
		fprintf(stderr, "Error listen()\n");
		return -1;
	}
	return sd;
}
