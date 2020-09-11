#include "mi-protocolo.h"
#include "conexion.h"
//#define LONG 30

void * ManejarCliente(void * dir_socket);
void imprimir_clientes();
void poner_nombre(int sd);
int indice=0;
int id;

DATOS datos[MAX_C];	
int main(int argc, char *argv[]){	
	int sd_server, new_sd, ret; 
	 
	
	socklen_t long_cliente;
	pthread_t tid;
	pthread_attr_t attr;

	struct sockaddr_in  dir_cliente; 
	
	if(argc != 3){
		fprintf(stderr, "Número de parámetros incorrecto. Use %s <ip> <puerto>\n", argv[0]);
		exit(-1);
	}

	if((sd_server = EscucharTCP(argv[1], atoi(argv[2]))) < 0){
		exit(-1);
	}
	long_cliente = sizeof(dir_cliente);
	for(;;){
		if((new_sd = accept(sd_server, (struct sockaddr *) &dir_cliente, &long_cliente))<0){
			fprintf(stderr, "Error al usar accept!\n");
			continue;	
		}
		datos[indice].directorio=dir_cliente;
		datos[indice].estado='n';
		datos[indice].sd=new_sd;
		imprimir_clientes();
		pthread_attr_init(&attr);
		ret = pthread_create(&tid,&attr, ManejarCliente, (void *) &new_sd);
		if(ret){
			fprintf(stderr, "Error al crear el hilo\n");
			continue;
		}
		indice=indice+1;
		
	}
}
void imprimir_clientes(){
int i;
system("clear");
for(i=0;i<=indice;i++){
	printf("%d. %s %d --> %c\n",i+1,inet_ntoa(datos[i].directorio.sin_addr),datos[i].directorio.sin_port,datos[i].estado);
	}			
}
void * ManejarCliente(void * dir_socket){
	int sd = *((int *) dir_socket);
	int tamano;
	MENSAJE mensaje;
	int new_sd1,new_sd2;
	int i;
	int aux;
	char bandera;
	poner_nombre(sd);
	
	do{ 
		mensaje.opc=6;//para no crear un bucle infinito
		if((tamano =  recv(sd,&mensaje, sizeof(mensaje), 0)) < 0){//siempre recibe opciones
				fprintf(stderr, "Error recv 1!\n");
			}
		switch(mensaje.opc){//1=solicitud_de_personas_conectadas 2=para_ver si esta disponible 3=cambios de estado 4=cliente_1 5=cliente_2
			case 1:	
					mensaje.num_conectados=indice;
					if((tamano =  send(sd,&mensaje, sizeof(mensaje), 0)) < 0){//envia el numero de clientes conectados
						fprintf(stderr, "Error send 1!\n");
					}
					if((tamano =  send(sd,datos, sizeof(DATOS[MAX_C]), 0)) < 0){//envia la estructura con los datos
						fprintf(stderr, "Error recv 2!\n");
						}
					break;
			case 2: bandera='n';
					if((tamano =  recv(sd, &mensaje, sizeof(MENSAJE), 0)) < 0){//recibe el usuario a conectar
						fprintf(stderr, "Error recv 3!\n");
							}
					for(i=0;i<indice;i++){//aqui me compara y me setea la bandera en 's' para poder enviar el mensaje
						if(strcmp(mensaje.nombre,datos[i].nombre)==0 && datos[i].estado != 'n' && datos[i].estado !='o'
						&& datos[i].estado !='d' )
						{mensaje.id=i;bandera='s';break;}
					}
					if(bandera=='s'){//entra en la condicion si lo encontro al cliente en la estuctura y puede establecer la conexion
						mensaje.codigo='v'; //si se encuentra en la lista
						if((tamano =  send(sd,&mensaje, sizeof(mensaje), 0)) < 0){//envia el codigo al cliente para que sepa si
																			//se empieza la conversacion o no
						fprintf(stderr, "Error send 2!\n");}
					}else{
						mensaje.codigo='e';//le envia e para que el cliente sepa que no puede conectarse con el otro cliente
						if((tamano =  send(sd,&mensaje, sizeof(mensaje), 0)) < 0){//envia mensaje.codigo
							fprintf(stderr, "Error send 3!\n");
						}
					}
					break;	
			case 3:    if((tamano = recv(sd,&mensaje, sizeof(mensaje), 0)) < 0){//recibe el mensaje
								fprintf(stderr, "Error recv 4!\n");
						}
						for(i=0;i<indice;i++){if(sd==datos[i].sd){aux=i;break;}}
						switch(mensaje.estado){
							case 'n':	datos[aux].estado='n';
										break;
							case 'd':	datos[aux].estado='d';
										break;
							case 's': 	datos[aux].estado='s';
										break;
							case 'o':   datos[aux].estado='o';
										break;
						}
					   break;
			case 4: 	if((tamano =  recv(sd,&mensaje, sizeof(mensaje), 0)) < 0){//recibe el mensaje
								fprintf(stderr, "Error recv 5!\n");
						}
						new_sd1=datos[mensaje.id].sd;//mensaje.id obtenido previamente para saber a que sd debemos enviar
						datos[mensaje.id].estado='o';//apenas el mande el primer mensaje lo pone al otro cliente en ocupado
						mensaje.sd_ext=sd;//le envia al cliente a recibir el mensaje el sd desde cual esta enviando el mensaje
						if((tamano =  send(new_sd1,&mensaje, sizeof(mensaje), 0)) < 0){//envia el mensaje al otro
								fprintf(stderr, "Error send 4!\n");
								datos[mensaje.id].estado='d';//si es que el segundo cliente pone ctrl-c
								break;
							}	
						break;
			
			case 5:     if((tamano = recv(sd,&mensaje, sizeof(mensaje), 0)) < 0){//recibe el mensaje
								fprintf(stderr, "Error recv 6!\n");
							}
						new_sd2=mensaje.sd_ext;//aqui el servidor sabe a quien debe enviar el mensaje
						
						if((tamano =  send(new_sd2,&mensaje, sizeof(mensaje), 0)) < 0){//envia el mensaje al otro
								fprintf(stderr, "Error send 5!\n");
						}
						break;
					
		}
	}while(mensaje.opc!=6);
	for(i=0;i<indice;i++){if(sd==datos[i].sd){datos[i].estado='d';;break;}}//si se desconecta o pone ctrl-c
	close(sd);
return NULL; 
}
void poner_nombre(int sd){
	MENSAJE mensaje;
	int i;
	int tamano;
	if((tamano =  recv(sd,&mensaje, sizeof(mensaje), 0)) < 0){
				fprintf(stderr, "Error recv!\n");
	}
	for(i=0;i<indice;i++){
		if(datos[i].sd==sd){
			strcpy(datos[i].nombre,mensaje.nombre);
			}
	}
}

