#include "conexion.h"
#include "mi-protocolo.h"
void portada();
void ingresar_mapa();
void registro(int sd);
void estados(char a,int sd);
void recibir();
void marcador();	
void opciones();

void verificar();
void comparar();
void imprimir_mapa(int map[LUN][LUN]);
void in_coord(int k, int h); //Toma los valores de las coordenadas de cada barco
void in_orien(int k, int h);  //Ubica la direccion de cada mapa
void construir_mapa(int j, int var); //construye el mapa con los barcos
char* err="[ERROR]"; //Solo se imprime en cada error 
char* exito="FELICIDADES ACERTASTE!!";
char* aviso="YA HABIAS ACERTADO EN ESA COORDENADA";
char* fallo="ERRASTE EL TIRO";
char* desconectar="EL JUGADOR SE HA DESCONECTADO";
char coord[max_buf]; //Recibe las coodenadas de cada barco.								
char orient[max_buf]; //Recibe la orientacion de cada barco
int mapa[LUN][LUN];							 
int orig_mappa[LUN][LUN];							
int i, j, k, vero, i_for, x, y, n_client, var;		
int check1, check2, rig, col, x1, x2, y_1, y2;
int p1=0,p2=0;

MENSAJE mensaje;
int main(int argc, char *argv[]){
	int sd;
	int tamano;
	int i,a,b;
	int n;
	int j=1;
	int opcion;
	DATOS datos[MAX_C];
	
	if(argc != 3){
		fprintf(stderr, "Número de parámetros incorrecto. Use %s <ip> <puerto>\n", argv[0]);
		exit(-1);
	}
	
	if((sd = conectarTCP(argv[1], atoi(argv[2])))<0){
					exit(-1);
	}
	portada();
	scanf("%d",&opcion);
	if(opcion==1){ingresar_mapa();
	}else{printf("\n Saliendo...\n");return -1;}
	
	registro(sd);
	
	do{
		printf("1.Ver jugadores\n2.Enviar solicitud de juego\n3.Esperar juego\n4.Imprimir tu Tabla\n5.Salir\n");
		printf("\nOpcion: ");
		scanf("%d",&n);
		if(n!=5 && n!=4){//para no confundir al servidor con la opcion 4 q es enviar mensajes
			mensaje.opc=n;
			if((tamano = send(sd, &mensaje, sizeof(mensaje), 0)) != sizeof(mensaje)){//envia la opcion al servidor para sepa que hacer
							fprintf(stderr, "Error send() %d envio\n", tamano);
						return -1;}
		}			
		switch(n){//1=solicitud_de_personas_conectadas 2=enviar_un_mensaje 3=recibe mensajes 4=sale
			case 1:	system("clear");
					if((tamano =  recv(sd,&mensaje, sizeof(MENSAJE), 0)) < 0){//recibe mensaje.num_conectados que sirve para el lazo
						fprintf(stderr, "Error recv!\n");
						return -1;}
					if((tamano =  recv(sd,datos, sizeof(DATOS[MAX_C]), 0)) < 0){//recibe la estructura con todos los datos
						fprintf(stderr, "Error recv!\n");
						return -1;}
					printf("\n");	
					for(i=0;i<mensaje.num_conectados;i++){//imprime la estructura previamente recibida 
						if(datos[i].estado!='d'){
							printf("%d. %s \t%s \t%d \t--> %c\n",j,datos[i].nombre,inet_ntoa(datos[i].directorio.sin_addr),datos[i].directorio.sin_port,datos[i].estado);
						j++;
						}
					}j=1;//j me permite imprimir los indices de los clientes
					printf("\n");
					break;
			case 2:	
					printf("\nIngrese el nombre del jugador con quien desea jugar:");
					while(getchar()!='\n');
					fgets(mensaje.nombre, LONG_BUFFER-1, stdin);
					mensaje.nombre[strlen(mensaje.nombre)-1]='\0';
					if((tamano = send(sd, &mensaje, sizeof(MENSAJE), 0)) != sizeof(MENSAJE)){//me envia mensaje.nombre
						fprintf(stderr, "Error send() %d envio\n", tamano);
							return -1;
						}
					if((tamano =  recv(sd, &mensaje, sizeof(MENSAJE), 0)) < 0){//recibe mensaje.codigo para ver si existe o esta ocupado
						fprintf(stderr, "Error recv!\n");
							}
					if(mensaje.codigo != 'e'){//entra en la condicion si es q mensaje.codigo=v
						estados('o',sd);//el cliente le dice al servidor que le ponga en ocupado
						do{	
							mensaje.opc=4;//le dice al servidor que quiere enviar un mensaje
							if((tamano =  send(sd,&mensaje, sizeof(mensaje), 0)) < 0){
								fprintf(stderr, "Error send!\n");
									return -1;}//este send solo envia la opc
							opciones();
							
							if((tamano = send(sd, &mensaje, sizeof(MENSAJE), 0)) != sizeof(MENSAJE)){
								fprintf(stderr, "Error send() %d envio\n", tamano);
								return -1;}//aqui se envian los datos
							if((strcmp(mensaje.datos, "exit")==0)){ 
									system("clear");
									printf("\n Jugador se deconecto!!\n");
									break;
							}//sale si el envia un exit por lo cual ya no entra al recv
							if((tamano = recv(sd, &mensaje, sizeof(mensaje), 0))<0){
							fprintf(stderr, "Error recv()!\n");
							return -1;}//aqui recibe los datos
							system("clear");
							printf("\n %s \n",mensaje.flag);
							if(strlen(mensaje.coordenada_ant) != 0){
								a=mensaje.coordenada_ant[0]-65;
								b=mensaje.coordenada_ant[1]-48;
								mapa[b][a]=3;
						    }
							  if(mensaje.puntuacion_ext > 0){
							  p1=mensaje.puntuacion_ext;
							     if(mensaje.puntuacion_ext == 6){
									 printf("\n FELICIDADES GANASTE!!\n");
									return 0;
									 }
						        }
							comparar();	
							 if(mensaje.puntuacion_ext == 6){
									 printf("\n HAS PERDIDO!!\n");
									 mensaje.opc=5;
									 if((tamano = send(sd, &mensaje, sizeof(MENSAJE), 0)) != sizeof(MENSAJE)){
								           fprintf(stderr, "Error send() %d envio\n", tamano);
								            return -1;}
									return 0;
									 }				
						}while(strcmp(mensaje.datos, "exit") != 0);
					printf("\n");
					}else{
						system("clear");
						printf("\nEl jugador no se encuentra disponible\n");}
					estados('n',sd);//le dice al servidor que le cambie a n
					break;
			case 3:    system("echo Espera tu turno...");
						mensaje.estado='s';
							
							if((tamano =  send(sd,&mensaje, sizeof(mensaje), 0)) < 0){//envia la opc 3 al servidor pa' q cambie el estado
								fprintf(stderr, "Error send!\n");
									return -1;}
					do{		
							
							if((tamano = recv(sd,&mensaje, sizeof(mensaje), 0))<0){//espera recibir un mensaje
							fprintf(stderr, "Error recv()!\n");
							return -1;}//recibe datos
							if((strcmp(mensaje.datos, "exit")==0)){
									system("clear");
									printf("\n Jugador se deconecto!!\n");
									break;
								}//si el mensaje es un exit sale del bucle
							system("clear");
							printf("\n %s\n",mensaje.flag);
							if(strlen(mensaje.coordenada_ant) != 0){
							a=mensaje.coordenada_ant[0]-65;
							b=mensaje.coordenada_ant[1]-48;
							mapa[b][a]=3;
						        }
						        if(mensaje.puntuacion_ext > 0){
							  p1=mensaje.puntuacion_ext;
							     if(mensaje.puntuacion_ext == 6){
									 printf("\n FELICIDADES GANASTE!!\n");
									return 0;
									 }
						        }	
							comparar();
							     if(mensaje.puntuacion_ext == 6){
									 printf("\n HAS PERDIDO!!\n");
									 mensaje.opc=5;
									 if((tamano = send(sd, &mensaje, sizeof(MENSAJE), 0)) != sizeof(MENSAJE)){
								           fprintf(stderr, "Error send() %d envio\n", tamano);
								            return -1;}
									return 0;
									 }
							mensaje.opc=5;//le dice al servidor que quiere enviar un mensaje
							if((tamano =  send(sd,&mensaje, sizeof(mensaje), 0)) < 0){//envia la opc 5								
								fprintf(stderr, "Error send!\n");
									return -1;}//solo envia la opcion
							
							opciones();
							
							if((tamano = send(sd, &mensaje, sizeof(MENSAJE), 0)) != sizeof(MENSAJE)){
								fprintf(stderr, "Error send() %d envio\n", tamano);
								return -1;}//envia datos
					}while(strcmp(mensaje.datos, "exit") != 0);
					printf("\n");
					estados('n',sd);//lo mismo que arriba
					break;
			case 4:
			      system("clear");
			      imprimir_mapa(orig_mappa);
			      printf("\n");
			       break;
			case 5: //sale del bucle
					estados('d',sd);//si se desconecta
					mensaje.opc=6; //opcion 6 le dice al servidor que se desconecto
					if((tamano =  send(sd,&mensaje, sizeof(mensaje), 0)) < 0){//envia la opcion 6 mensaje.opc
						fprintf(stderr, "Error send!\n");
						return -1;
					}
					system("clear");
					printf("\nConexiòn cerrada\n");
					break;	
					
			default:  printf("\nOpcion incorrecta\n");
					  //break;
		}
		
	}while(n!=5);
	
	close(sd);
	return 0;
}
void estados(char a,int sd){//funcion que me cambia los estados enviando opc 3 al servidor
	int tamano;
	mensaje.opc=3;
	if((tamano =  send(sd,&mensaje, sizeof(mensaje), 0)) < 0){//envia la opc 3 al servidor pa' q cambie el estado
		fprintf(stderr, "Error send!\n");
						exit(-1);}
						
	switch(a){
		case 's':	mensaje.estado='s';
					if((tamano =  send(sd,&mensaje, sizeof(mensaje), 0)) < 0){//envia los estados
						fprintf(stderr, "Error send!\n");
						exit(-1);}
					break;
		case 'n': 	mensaje.estado='n';
					if((tamano =  send(sd,&mensaje, sizeof(mensaje), 0)) < 0){//"                "
						fprintf(stderr, "Error send!\n");
						exit(-1);}
					break;
		case 'd':	mensaje.estado='d';
					if((tamano =  send(sd,&mensaje, sizeof(mensaje), 0)) < 0){//"                 "
						fprintf(stderr, "Error send!\n");
						exit(-1);}
					break;
		case 'o':	mensaje.estado='o';
					if((tamano =  send(sd,&mensaje, sizeof(mensaje), 0)) < 0){//"                 "
						fprintf(stderr, "Error send!\n");
						exit(-1);}
					break;
	}
}
void in_coord(int var, int h)
{	
	printf("Porfavor inserte las coordenadas del bote %d de tamano %d: ", var, h);
	setbuf(stdin,NULL);									
	scanf("%s", coord); 									
	//controla que sea correcta la sintaxis de las coordenadas
	while( (coord[0]<'A' || coord[0]>'J') || (coord[1]<'0' || coord[1]>'9') || coord[2]!='\0' ){ 
		printf("%sCoordenada no valida\n",err);
		printf("Las coordenadas deben ser ingresadas de la sguiente forma:[A-J][0-9]\n UNA LETRA EN MAYUSCULA SEGUIDA DE UN NUMERO SIN CORCHETES\n"); 
		printf("Porfavor inserte las coordenadas del bote %d de tamano %d: ", var, h);
		setbuf(stdin,NULL);				
		scanf("%s", coord); 
	}
 }

void in_orien(int var, int h)
{	
	printf("Porfavor inserte la orientacion del bote %d de tamano %d: ",var, h);
	setbuf(stdin,NULL);								
	scanf("%s", orient); 							
		
	//controla que sea correcta la sintaxis de la orientacion
	while(!(orient[0]=='H' || orient[0]=='V') || orient[1]!='\0'){	
		printf("%sOrientacion no valida\n",err);
		printf("La orientacion debe ser H para horizontal o V para vertical\n"); 
		printf("Porfavor inserte la orientacion del bote %d de tamano %d: ", var, h);
		setbuf(stdin,NULL);			
		scanf("%s", orient);
	}
}

void construir_mapa(int j, int var){
	int i;
	check1=check2=1; //Serviran como auxiliares para indicar cuantas veces se realizara el lazo
	x= coord[0]-65;										//coordenada x (columna)
	y= coord[1]-48;										//coordinata y (riga)
	
	//inicializan en cero el mapa original
    if(j==6){
	for(rig = 0; rig < LUN; rig++)
    		for(col = 0; col < LUN; col++)
      			orig_mappa[rig][col]=0;
    }
		x1=x2=x;
		y_1=y2=y;
	//j - indica el tamano del bote, lo usaromos para saber el numero de espacios q se le asignaran
	//controla la posicion del bore y su orientacion
	while(check1){	
		for(i=0; i<j;i++){
			if (orig_mappa[y_1][x1]!=1){					//se =1 quiere decir que existe otra nave
				if(orient[0]=='H') x1++;  
				if(orient[0]=='V') y_1++;				
			}
			else
				check2=0;
		}

		if (check2 != 1){
			printf("%sEl bote no puede ir encima de otro, intenta en otra posicion.\n",err);
			in_coord(var, j);								//Vueleve a pedir la cordenada
			x1=x2= coord[0]-65;
			y_1=y2= coord[1]-48;
			in_orien(var, j);								//vueleve a pedir la orientacion
			check2=1;                                        //Si ya estan correctas las coordenadas y la orientacion se sale del lazo
			continue;						
		}
		else 
			check1=0;
	}
	//controla que las coordenadas estan dentro del mapa
	check1=check2=1;
	while(check1){	
		if(orient[0]=='H') 
			if((x2+j)>10)   //si el valor de la posicion mas el tamano del bote es 10 estara fuera del mapa y se debe volver a pedir los datos
				check2=0;				
		if(orient[0]=='V') 
			if((y2+j)>10)
				check2=0;				
	
		if (check2 != 1){
			printf("%sEl bote no puede salir del mapa, intenta en otra posicion.\n",err);
			in_coord(var,j);								//Se vuelve a pedir la coordenada		
			x2= coord[0]-65;
			y2= coord[1]-48;
			in_orien(var, j);								//Se vuelve a pedir la orientacion
			check2=1;                                      //Si ya estan correctas las coordenadas y la orientacion se sale del lazo
			continue;						
		}
		else 
			check1=0;
	}
	//Inserta la nave en el mapa
	for(i=0; i<j;i++){			
		orig_mappa[y2][x2]=1;	
		if(orient[0]=='H') x2++;
		if(orient[0]=='V') y2++;
	}
}

void imprimir_mapa(int map[LUN][LUN])	
{	
	printf("\n    A  B  C  D  E  F  G  H  I  J\n");	
	printf("   -----------------------------\n");
	
	for(rig = 0; rig < LUN; rig++){
		printf("%d | ", rig);							//imprime el mapa
    		for(col = 0; col < LUN; col++){
				if(	map[rig][col] ==0)
					printf(".  ");
				if(	map[rig][col] ==1)
					printf("n  ");
				if(	map[rig][col] ==2)
					printf("X  ");
				if(	map[rig][col] ==3)
					printf("O  ");
			}				
		printf("\n");
	}
}

void verificar(){	
	printf("\nPorfavor inserte la coordenada a la que desea atacar: ");
	setbuf(stdin,NULL);
	scanf("%s", coord);
	setbuf(stdin,NULL);	                                                                              									
	//controla que sea correcta la sintaxis de las coordenadas
	while( (coord[0]<'A' || coord[0]>'J') || (coord[1]<'0' || coord[1]>'9') || coord[2]!='\0' ){ 
		printf("%sCoordenada no valida\n",err);
		printf("Las coordenadas deben ser ingresadas de la sguiente forma:[A-J][0-9]\n UNA LETRA EN MAYUSCULA SEGUIDA DE UN NUMERO SIN CORCHETES\n"); 
		printf("\nPorfavor inserte la coordenada a la que desea atacar: ");
		setbuf(stdin,NULL);				
		scanf("%s", coord);
		system("echo Espera tu turno..."); 
	}
	system("echo Espera tu turno...");
	sprintf(mensaje.datos,"%s",coord);
}
void opciones(){
	
    imprimir_mapa(orig_mappa);
	int a;
	char *e="exit";
	do{
	printf("\n Seleccione una opcion: \n1.Dar mi turno \n2.Ver tabla rival \n3.Salir \nSeleccione: ");
	setbuf(stdin,NULL);
	scanf("%d",&a);
	switch(a){
		case 1:
		      verificar();
		      break;
		case 2:
		      imprimir_mapa(mapa);
		      break;
		case 3: 
				sprintf(mensaje.datos,"%s",e);
				break;
		     
		default:
		      printf("\n No es una opcion");
		}
	 }while(a!=1 && a!=4);
	}

void comparar(){
	sprintf(mensaje.coordenada_ant,NULL);
	mensaje.puntuacion_ext=0;
	int j=mensaje.datos[0]-65;
	int i=mensaje.datos[1]-48;
	if(orig_mappa[i][j]==1){
		orig_mappa[i][j]=2;
		p2=p2+1;
		mensaje.puntuacion_ext=p2;
		sprintf(mensaje.flag,"%s",exito);
		sprintf(mensaje.coordenada_ant,"%s",mensaje.datos);
		}
    else if(orig_mappa[i][j]==2){
		sprintf(mensaje.flag,"%s",aviso);
	    }
	else if(orig_mappa[i][j]==0){
		sprintf(mensaje.flag,"%s",fallo);
		}	
	   marcador();	
}

void marcador(){
 //system("clear");
 printf("\n El jugador apunto a la coordenada %s",mensaje.datos);
  if(strcmp(mensaje.flag,exito) == 0){
	  printf("\n \tTE HA DADO");
	  printf("\n \tEl marcador es:\n \tTu: %d\n \tSegundo: %d ",p1,p2);
	  }	
  else if(strcmp(mensaje.flag,fallo) == 0){
	  printf("\n \tHA FALLADO");
	  printf("\n \t El marcador es:\n \t  Tu: %d\n \t Segundo: %d ",p1,p2);
	  }	
  else if(strcmp(mensaje.flag,aviso) == 0){
	  printf("\n \tYA HABIA ACERTADO EN ESA COORDENADA");
	  printf("\n \t El marcador es:\n \t  Tu: %d\n \t Segundo: %d ",p1,p2);
	  }	
	
}
void ingresar_mapa(){
	int j,k;
	j=6, var=1;
	for(k=0; k<1; k++){	  
	  system("clear");
	  imprimir_mapa(orig_mappa);
	  in_coord(var, j);
	  in_orien(var, j);		
	  construir_mapa(j,var);
	  printf("\n");
	  var++;
	  if(k==0){
		 j=j-2;}
	  else{
		 j--;}
   }imprimir_mapa(orig_mappa);
}
void portada(){
	system("clear");
	printf("\n\n\t    ~BATALLA NAVAL~\n\n\t\t#\n\t\t#|\n\t\t#|#\n\t\t#|##\n\t\t#|###\n\t\t#|####");
    printf("\n\t\t##|#####\n\t\t#|######\n\t#########################\n\t _______________________");
    printf("\n\t  ####/)###############\n\t   ###(/##############\n\t    #################\n\t     ###############");
    printf(" \t\n\n Seleccione una opciòn \n 1.- Llenar mi Tabla de juego \n 2.- Cualquier tecla para salir \n Opción: ");
}
void registro(int sd){
	int tamano;
	printf("\nIngrese un nombre de usuario: ");
	setbuf(stdin,NULL);
	fgets(mensaje.nombre, LONG_BUFFER-1, stdin);
	mensaje.nombre[strlen(mensaje.nombre)-1]='\0';
	if((tamano = send(sd, &mensaje, sizeof(mensaje), 0)) != sizeof(mensaje)){//envia la opcion 
							fprintf(stderr, "Error send() %d envio\n", tamano);}
	system("clear");
}
