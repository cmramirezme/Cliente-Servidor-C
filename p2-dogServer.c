#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include<string.h>
#include <time.h>
#include<sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <strings.h>
#include <ctype.h>

#define NumHilos 32
/*#define Rec_dataDogs 1
#define Rec_Num_Registros 1
#define Rec_Tabla_Hash 1
#define Rec_Historia 1*/
#define Rec_Operacion 1


int Indice_Hilo=-1,Nombre_Historia_Cliente=0;

// Apuntadores Semaforo
//sem_t *Write_dataDogs, *Sem_Set_dataDogs, *Sem_Set_Num_Registros, *Sem_Set_Tabla_Hash, *Sem_Set_Historia;
sem_t *Sem_Operacion;

// Declaración Funciones Servidor
int IngresarServer();
char* BuscarServer();
void *Hilo();
int Get_Num_Registros();
struct dogType Get_dataDogs();
void Set_Num_Registros();
void Set_dataDogs();
int BorrarServer();
int VerServer();
void Registro();
int Funcion_Hash();
int* Get_Hash();
void Set_Hash();
int sendAll();
int recvAll();


//Estructura
typedef struct dogType{
    char Nombre[32];
    char Tipo[32];
    int Edad;
    char Raza[16];
    int Estatura;
	float Peso;
    char Sexo;  
    int Siguiente;

};


typedef struct Info_Registro{
    int cliente;
	struct sockaddr_in direccion;

};




int main(void) {
	
	pthread_t tfd[NumHilos];
	/*Sem_Set_dataDogs=sem_open("Rec_dataDogs", O_CREAT, 0700,Rec_dataDogs);
	Sem_Set_Num_Registros=sem_open("Rec_Num_Registros", O_CREAT, 0700,Rec_Num_Registros);
	Sem_Set_Tabla_Hash=sem_open("Rec_Tabla_Hash", O_CREAT, 0700,Rec_Tabla_Hash);
	Sem_Set_Historia=sem_open("Rec_Historia", O_CREAT, 0700,Rec_Tabla_Hash);*/
	Sem_Operacion=sem_open("Rec_Operacion", O_CREAT, 0700,Rec_Operacion);
	
//Creacion del socket (enchufe) por parte del servidor 
	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = INADDR_ANY;
	direccionServidor.sin_port = htons(8080);
	int servidor = socket(AF_INET, SOCK_STREAM, 0);
	int activado = 1;
	setsockopt(servidor, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));
	
// La funcion bind() asigna un socket al un puerto (en este caos el 8080)
	if (bind(servidor, (void*) &direccionServidor, sizeof(direccionServidor)) != 0) {
		perror("Fallo el bind");
		return 1;
	}

// Listen marca un socket para que escuche la solicitud de un cliente que desea conectarse 
	
	listen(servidor, 32);
	struct sockaddr_in direccionCliente;
	unsigned int tamanoDireccion;
	int cliente;
	struct Info_Registro Info_Registros[NumHilos];
	
	for(;;){
	//Accept crea un socket para comunicar al cliente con el servidor
		cliente = accept( servidor, (struct sockaddr *)&direccionCliente, &tamanoDireccion );
		Indice_Hilo++;
		Info_Registros[Indice_Hilo].cliente=cliente;
		Info_Registros[Indice_Hilo].direccion=direccionCliente;
		pthread_create(&tfd[Indice_Hilo], NULL, Hilo, &(Info_Registros[Indice_Hilo]));
	}

 	return 0;
}







void *Hilo(struct Info_Registro *Informacion_Log){
	// Opcion del usuario
			int n, *cliente=&(Informacion_Log->cliente); 
			
 			do {
			// Se recibe la opcion del usuario digitada desde el cliente y valida
				int br = recvAll(*cliente, &n, sizeof(n), 0);
 					if (br <= 0) {
						/*em_close(Sem_Set_Num_Registros);
    					sem_unlink("Rec_Num_Registros");
						sem_close(Sem_Set_Tabla_Hash);
    					sem_unlink("Rec_Tabla_Hash");
						sem_close(Sem_Set_Historia);
    					sem_unlink("Rec_Historia");*/
						sem_close(Sem_Operacion);
    					sem_unlink("Rec_Operacion");
				 		perror("LOs recursos de los semaforos se han liberado correctamente");
				 		exit (-1);
				 	}

			// Se ejecuta la opcion que el cliente eligio
				switch (n) {

 		              	case 1:
 		                    
							Registro(*cliente,Informacion_Log->direccion,"Inserción",IngresarServer(*cliente)," ");
 							break;
						case 2:
							Registro(cliente,Informacion_Log->direccion,"Lectura",VerServer(*cliente)," ");
							break;
						case 3:
							Registro(cliente,Informacion_Log->direccion,"Borrado",BorrarServer(*cliente)," ");
							break;
						case 4:
 		                    
							Registro(cliente,Informacion_Log->direccion,"Busqueda",0,BuscarServer(*cliente)); 
 							break;
 		              	case 5:
							printf("---||saliendo||---" );
							close(*cliente);

 		      	}

 		 } while(n == 1 || n == 2 || n == 3 || n == 4 ); // Opcion 5, salir.

}


int IngresarServer(int cliente){

		struct dogType *Mascota;
		int r, Num_Registros=0, flag = 0, *Arreglo_Hash, Indice;

	// Se crea un espacio en la memoria dinamica y se valida
		Mascota = malloc(sizeof(struct dogType));
			if(Mascota == NULL){
				perror("error en malloc");
				exit(-1);
			}

	// Se recibe el registro a insertar en dataDogs desde el cliente
		int bytesRecibidos = recvAll(cliente, Mascota, sizeof(struct dogType), 0);
			if (bytesRecibidos <= 0) {
				perror("Error al recibir registro desde el cliente");
				exit(-1);
			}

	// Se abre dataDogs.dat para insertar el nuevo registro
		
	sem_wait(Sem_Operacion);
		Num_Registros=Get_Num_Registros();
		Num_Registros++;
		//Actualizar valores de la tabla hash y guardarlos en el archivo Tabla_Hash.c
		Arreglo_Hash=Get_Hash();
		Indice=Funcion_Hash(Mascota->Nombre);
		if(*(Arreglo_Hash+Indice)==-1){
			*(Arreglo_Hash+Indice)=Num_Registros-1;
		}
		else
		{   
			Mascota->Siguiente=*(Arreglo_Hash+Indice);
			*(Arreglo_Hash+Indice)=Num_Registros-1;
		}
		Set_Hash(Arreglo_Hash);
		free(Arreglo_Hash);


		//Ingresa registro y modificar numero de registros
		Set_dataDogs(Mascota,0);

		Set_Num_Registros(Num_Registros);
		

		//Enviar notificacion al cliente de que el registro se guardo exitosamente
		flag = 1;
		sendAll(cliente,&flag,sizeof(flag),0);
		
		free(Mascota);
	sem_post(Sem_Operacion);
		return Num_Registros;
	
}


int VerServer(int cliente){

	FILE *dataDogs,*Historia;
	struct dogType Mascota;

	int Num_Registros,id, intentos, S_N, Tamano_Out, Tamano_In;
	
	char *Out_Text,*In_Text,a='*';

sem_wait(Sem_Operacion);

	Num_Registros=Get_Num_Registros();

    sendAll(cliente, &Num_Registros, sizeof(int), 0);

	if (recvAll(cliente, &id, sizeof(int), 0) <= 0) {
		perror("Desconección");
		exit(-1);
	}

	if(id<1 || id>Num_Registros){

        if (recvAll(cliente, &id, sizeof(int), 0) <= 0) {
			perror("Desconección");
			exit(-1);
		}

		intentos++;

        if(intentos==3){
        	return -1;
    	}   
    }
	
	Mascota=Get_dataDogs(id);


	sendAll(cliente, &Mascota, sizeof(struct dogType), 0);

	if (recvAll(cliente, &S_N, sizeof(int), 0) <= 0) {
			perror("Desconección");
			exit(-1);
	}

	if(S_N==1){

		
		sendAll(cliente, &Nombre_Historia_Cliente, sizeof(int), 0);
		Nombre_Historia_Cliente++;

		if(Nombre_Historia_Cliente==32){
			Nombre_Historia_Cliente=0;
		}
		
	
    	char Direccion_1[34]="Historias_Clinicas/";
        char Direccion_2[10];
        char Direccion_3[5]=".txt";
                
        sprintf(Direccion_2, "%i", id);
        strcat(Direccion_1,Direccion_2);
        strcat(Direccion_1,Direccion_3);


		Historia=fopen(Direccion_1,"a+");
            if(Historia==NULL){
                perror("Error al abrir la historia clinica");
				exit(-1);                
			}

		fseek(Historia,0,SEEK_END);
		Tamano_Out=ftell(Historia);

			if(Tamano_Out==0){
				Tamano_Out++;
				Out_Text=malloc(Tamano_Out);
					if(Out_Text==NULL){
					perror("Error en malloc de la funcion VerServer->Our_Text");
					exit(-1);
					}	
				fwrite(&a,1,1,Historia);

			}

			else{
				Out_Text=malloc(Tamano_Out);
					if(Out_Text==NULL){
					perror("Error en malloc de la funcion VerServer->Our_Text");
					exit(-1);
				}

			}
		
		
		sendAll(cliente, &Tamano_Out, sizeof(int), 0);
		fseek(Historia,0,SEEK_SET);
		fread(Out_Text,Tamano_Out,1,Historia);
		sendAll(cliente, Out_Text, Tamano_Out, 0);
		fclose(Historia);

	
		

		if (recvAll(cliente, &Tamano_In, sizeof(int), 0) <= 0) {
			perror("Desconección");
			exit(-1);
		}
	
		
		In_Text=malloc(Tamano_In);
			if(In_Text==NULL){
				perror("Error en malloc de la funcion VerServer->In_Text");
				exit(-1);
			}

		if (recvAll(cliente, In_Text, Tamano_In, 0) < 0) {
			perror("Desconección");
			exit(-1);
		}


		Historia=fopen(Direccion_1,"w");
            if(Historia==NULL){
                perror("Error al abrir la historia clinica 2");
				exit(-1);                
			}

		if(fwrite(In_Text,Tamano_In,1,Historia)<0){
			perror("Error al actualizar el registro");
			exit(-1);
		}

		free(In_Text);
		free(Out_Text);
		fclose(Historia);
	
	}

sem_post(Sem_Operacion);
	return id;
}
	






int BorrarServer(int cliente){
	
	FILE *dataDogs, *AUX1,*Historia, *Cambio_Id;

	struct dogType Nodo_Borrar, Nodo_Siguiente, Nodo_Trasladado;

	int Num_Registros,Indice_Trasladado,Reg_Borrar, Id_Anterior, Indice, *Arreglo_Hash, Id_Siguiente=-1, Id_Antiguo, Indice_Ultimo;
	 
sem_wait(Sem_Operacion);

	Num_Registros=Get_Num_Registros();


    sendAll(cliente, &Num_Registros, sizeof(int), 0);


	if (recvAll(cliente, &Reg_Borrar, sizeof(int), 0) <= 0) {
		perror("Desconección");
		exit(-1);
	}


    if(Reg_Borrar>Num_Registros){  
		printf(" ");
    }

	else{


		Nodo_Trasladado=Get_dataDogs(Num_Registros);


		Indice_Trasladado=Funcion_Hash(Nodo_Trasladado.Nombre);


		Nodo_Borrar=Get_dataDogs(Reg_Borrar);


		Indice=Funcion_Hash(Nodo_Borrar.Nombre);


		Arreglo_Hash=Get_Hash();



		//Si entra a este if es porque el registro a eliminar esta registrado en la tabla hash
		if(Reg_Borrar-1==*(Arreglo_Hash+Indice)){

			//Si entre a este if significa que el registro a eliminar es el ultimo y ademas
			//que esta registrado en la tabla hash
			if(Num_Registros-1==*(Arreglo_Hash+Indice)){

				//Si entra a este condicional significa que el registro no esta enlazado con otros
				if(Nodo_Borrar.Siguiente==-1){
					*(Arreglo_Hash+Indice)=-1;
				}

				//Si entra a este condicional es porque el registro se encuentra enlazado
				else{

					*(Arreglo_Hash+Indice)=Nodo_Borrar.Siguiente;
				}



			}

			//Si entra a este if es porque el registro a eliminar no es el ultimo pero
			//esta registrado en la tabla hash
			else{

				//Si entra a este if es porque el registro no esta enlazado
				if(Nodo_Borrar.Siguiente==-1){
					*(Arreglo_Hash+Indice)=-1;
				}

				//Si entra a este condicional es porque el registro se encuentra enlazado
				else{

					*(Arreglo_Hash+Indice)=Nodo_Borrar.Siguiente;
				}

				*(Arreglo_Hash+Indice_Trasladado)=Reg_Borrar-1;


			}


		}


		//SI entra a este condicional es porque el registro a eliminar no esta registrado en la tabla Hash
		else{

			//Esta seccion de codigo guarda el nodo siguiente en "Nodo_Siguiente"
	
			Nodo_Siguiente=Get_dataDogs(*(Arreglo_Hash+Indice)+1);


			Id_Siguiente=*(Arreglo_Hash+Indice);

            while(Reg_Borrar-1!=Nodo_Siguiente.Siguiente){
				
				Id_Siguiente=Nodo_Siguiente.Siguiente;


				Nodo_Siguiente=Get_dataDogs((Nodo_Siguiente.Siguiente)+1);
				

            }

			
			//Si entra a este condicional es porque el nodo siguiente es el ultimo registro en dataDogs
			if(Id_Siguiente==Num_Registros-1){

				*(Arreglo_Hash+Indice)=Reg_Borrar-1;
				Nodo_Siguiente.Siguiente=Nodo_Borrar.Siguiente;

			}

			//Si entra a este condicional es porque el nodo siguiente no es el ultimo de la tabla hash
			//Y al mismo tiempo no es el ultimo en dataDogs
			else{

				Nodo_Siguiente.Siguiente=Nodo_Borrar.Siguiente;
				*(Arreglo_Hash + Indice_Trasladado)=Reg_Borrar-1;

			}


		}


		AUX1 = fopen("AUX1.dat", "a+b");
          	if (AUX1 == NULL) {
          		perror("Error al abrir el archivo auxiliar en la funcion Buscar()");
              	exit(-1);
          	}

		dataDogs=fopen("dataDogs.dat","rb+");

        	if(dataDogs==NULL){
				perror("error al abrir el archivo dataDogs.dat en la funcion Borrar_Server");
            	exit(-1);
        	}



		fseek(dataDogs,0,SEEK_SET);

		//Si entra a este condicional es porque el nodo a eliminar esta registrado en la tabla hash
		if(Id_Siguiente==-1){

			for( int i=0;i<Num_Registros-1;i++){
               
				if ((Reg_Borrar-1)!=i){

					if(fread(&Nodo_Borrar, sizeof(struct dogType), 1, dataDogs) == 0){
						perror("error fread dataDogs.dat en la funcion Buscar()");
							exit(-1);
					}
					
					if(fwrite(&Nodo_Borrar, sizeof(struct dogType), 1, AUX1) == 0){
						perror("error fwrite AUX1.dat en la funcion Buscar()");
						exit(-1);
					}


				}
		
				else{
						
					if(fread(&Nodo_Borrar, sizeof(struct dogType), 1, dataDogs) == 0){
						perror("error fread dataDogs.dat funcion Buscar() else");
						exit(-1);
					}

					fseek(AUX1,i*sizeof(struct dogType),SEEK_SET);
					
					if(fwrite(&Nodo_Trasladado, sizeof(struct dogType), 1, AUX1) == 0){
						perror("error fwrite");
						exit(-1);
					}

					fseek(dataDogs,(i+1)*sizeof(struct dogType),SEEK_SET);

					char Direccion_1[34]="Historias_Clinicas/";
					char Direccion_2[10];
					char Direccion_3[5]=".txt";

					char Historias_Clinicas[34]="Historias_Clinicas/";
					char Viejo[10];

					//Historias_Clinicas/Num_Registro.txt
					sprintf(Viejo, "%i", Num_Registros);
					strcat(Historias_Clinicas,Viejo);
					strcat(Historias_Clinicas,Direccion_3);
					
					sprintf(Direccion_2, "%i", Reg_Borrar);
					strcat(Direccion_1,Direccion_2);
					strcat(Direccion_1,Direccion_3);
				
					Historia=fopen(Direccion_1,"a");
					Cambio_Id=fopen(Historias_Clinicas,"a");
						if(Historia!=NULL){
							remove(Direccion_1);
							rename(Historias_Clinicas,Direccion_1);
						}
					fclose(Historia);
					fclose(Cambio_Id);
				
				}               
               
        	}


		}

		//Si entra a este condicional es porque el nodo a eliminar no esta registrado en la tabla hash
		else{

			for( int i=0;i<Num_Registros-1;i++){
               
				if ((Reg_Borrar-1)!=i){

					if(Id_Siguiente==i){

						if(fread(&Nodo_Borrar, sizeof(struct dogType), 1, dataDogs) == 0){
							perror("error fread dataDogs.dat en la funcion Buscar()");
								exit(-1);
						}
						
						if(fwrite(&Nodo_Siguiente, sizeof(struct dogType), 1, AUX1) == 0){
							perror("error fwrite AUX1.dat en la funcion Buscar()");
							exit(-1);
						}

					}

					else{

						if(fread(&Nodo_Borrar, sizeof(struct dogType), 1, dataDogs) == 0){
							perror("error fread dataDogs.dat en la funcion Buscar()");
								exit(-1);
						}
						
						if(fwrite(&Nodo_Borrar, sizeof(struct dogType), 1, AUX1) == 0){
							perror("error fwrite AUX1.dat en la funcion Buscar()");
							exit(-1);
						}


					}

						


				}
		
				else{

					if(Id_Siguiente==Num_Registros-1){
						if(fread(&Nodo_Borrar, sizeof(struct dogType), 1, dataDogs) == 0){
							perror("error fread dataDogs.dat funcion Buscar() else");
							exit(-1);
						}

						fseek(AUX1,i*sizeof(struct dogType),SEEK_SET);
						
						if(fwrite(&Nodo_Siguiente, sizeof(struct dogType), 1, AUX1) == 0){
							perror("error fwrite");
							exit(-1);
						}

						fseek(dataDogs,(i+1)*sizeof(struct dogType),SEEK_SET);
					}

					else{

						if(fread(&Nodo_Borrar, sizeof(struct dogType), 1, dataDogs) == 0){
							perror("error fread dataDogs.dat funcion Buscar() else");
							exit(-1);
						}

						fseek(AUX1,i*sizeof(struct dogType),SEEK_SET);
						
						if(fwrite(&Nodo_Trasladado, sizeof(struct dogType), 1, AUX1) == 0){
							perror("error fwrite");
							exit(-1);
						}

						fseek(dataDogs,(i+1)*sizeof(struct dogType),SEEK_SET);

					}	

					
					

					char Direccion_1[34]="Historias_Clinicas/";
					char Direccion_2[10];
					char Direccion_3[5]=".txt";

					char Historias_Clinicas[34]="Historias_Clinicas/";
					char Viejo[10];

					//Historias_Clinicas/Num_Registro.txt
					sprintf(Viejo, "%i", Num_Registros);
					strcat(Historias_Clinicas,Viejo);
					strcat(Historias_Clinicas,Direccion_3);
					
					sprintf(Direccion_2, "%i", Reg_Borrar);
					strcat(Direccion_1,Direccion_2);
					strcat(Direccion_1,Direccion_3);
				
					Historia=fopen(Direccion_1,"a");
					Cambio_Id=fopen(Historias_Clinicas,"a");
						if(Historia!=NULL){
							remove(Direccion_1);
							rename(Historias_Clinicas,Direccion_1);
						}
					fclose(Historia);
					fclose(Cambio_Id);
				
				}               
               
        	}
		}

		if(Reg_Borrar==Num_Registros){

                char Uno[34]="Historias_Clinicas/";
                char Dos[10];
                char Tres[5]=".txt";

                sprintf(Dos, "%i", Num_Registros);

                strcat(Uno,Dos);
                strcat(Uno,Tres);

                remove(Uno);
                
        }

		Set_Hash(Arreglo_Hash);

		free(Arreglo_Hash);
		
          
        if(fclose(dataDogs)!= 0){
           perror(" error fclose en la funcion Buscar() en dataDogs");
		   exit(-1);
	    }
          

		if(remove("dataDogs.dat")!=0){
            perror(" error al eliminar el archivo dataDogs.dat en la funcion Buscar()");
		   	exit(-1);
		}
          
          
		if(rename("AUX1.dat","dataDogs.dat")!=0){
			perror(" error al renombrar el archivo AUX1.dat a dataDogs.dat");
		   	exit(-1);
        }
          
        if(fclose(AUX1) != 0){
            perror(" error fclose en AUX1 en la funcion buscar");
			exit(-1);
        }
        
		Set_Num_Registros(--Num_Registros);
sem_post(Sem_Operacion);

    }
	sendAll(cliente, &Num_Registros, sizeof(int), 0);


	

	return Reg_Borrar;
}
















char* BuscarServer(int cliente){

	struct dogType Mascota;
	char busqueda[32];
	int r,flag, Id=0, Coincide_Busqueda,Indice, *Arreglo_Hash;

		
		// Recibe el nombre que se desea buscar desde el cliente.
		int bytesRecibidos = recvAll(cliente, busqueda, 32, 0);

			if (bytesRecibidos <= 0) {
				perror("Desconección");
				exit(-1);
			}


		Indice=Funcion_Hash(busqueda);

sem_wait(Sem_Operacion);

		Arreglo_Hash=Get_Hash();

		Mascota=Get_dataDogs(*(Arreglo_Hash+Indice)+1);	

		Id=*(Arreglo_Hash+Indice);
		sendAll(cliente,&Mascota.Siguiente,sizeof(int),0);

		while(Mascota.Siguiente!=-1){

			if(strcasecmp(Mascota.Nombre,busqueda)==0){

				Coincide_Busqueda=1;
				sendAll(cliente,&Coincide_Busqueda,sizeof(int),0);
				sendAll(cliente,&Mascota,sizeof(struct dogType),0);
				sendAll(cliente,&Id,sizeof(int),0);

			}

			else{

				Coincide_Busqueda=0;
				sendAll(cliente,&Coincide_Busqueda,sizeof(int),0);

			}

			Id=Mascota.Siguiente;

			Mascota=Get_dataDogs(Mascota.Siguiente+1);

			sendAll(cliente,&Mascota.Siguiente,sizeof(int),0);	

		}

		if(strcasecmp(Mascota.Nombre,busqueda)==0){

				Coincide_Busqueda=1;
				sendAll(cliente,&Coincide_Busqueda,sizeof(int),0);
				sendAll(cliente,&Mascota,sizeof(struct dogType),0);
				sendAll(cliente,&Id,sizeof(int),0);

		}

		else{

			Coincide_Busqueda=0;
			sendAll(cliente,&Coincide_Busqueda,sizeof(int),0);

		}
		
		free(Arreglo_Hash);

sem_post(Sem_Operacion);	

		return strdup(busqueda);

	}






int Get_Num_Registros(){

	FILE *F_Num_Registros;
	int Num_Registros;

	if((F_Num_Registros=fopen("F_Num_Registros.txt","r"))==NULL){
		perror("Error en la funcion Get_Num_Registros");
		exit(-1);
	}

	fread(&Num_Registros,sizeof(int),1,F_Num_Registros);
	fclose(F_Num_Registros);
	return Num_Registros;

}






void Set_Num_Registros(int Num_Registros){

	FILE *F_Num_Registros;

	if((F_Num_Registros=fopen("F_Num_Registros.txt","w"))==NULL){
		perror("Error en la funcion Set_Num_Registros");
		exit(-1);
	}

	fwrite(&Num_Registros,sizeof(int),1,F_Num_Registros);
	fclose(F_Num_Registros);
}





void Set_dataDogs(struct dogType *Mascota, int Empezar){

	FILE *dataDogs;
	
    	if ((dataDogs = fopen("dataDogs.dat", "ab")) == NULL) { 
	         printf("\n\tOcurrió un error al intentar insertar el registro\n");
			 exit (-1);
		} 
		
		if(Empezar==0){

			

          	if((fwrite(Mascota, sizeof(struct dogType), 1, dataDogs)) == 0){
           		perror("error fwrite funcion Set_dataDogs");
           		exit(-1);
      		}

        	if((fclose(dataDogs))!= 0){
           		perror(" error fclose funcion Set_dataDogs");
      		}

		}

		else{

			fseek(dataDogs,Empezar*sizeof(struct dogType),SEEK_SET);
			if((fwrite(Mascota, sizeof(struct dogType), 1, dataDogs)) == 0){
           		perror("error fwrite funcion Set_dataDogs");
           		exit(-1);
      		}

        	if((fclose(dataDogs))!= 0){
           		perror(" error fclose funcion Set_dataDogs");
      		}
		}

				
		

}

struct dogType Get_dataDogs(int id){

	FILE *dataDogs;
	struct dogType Mascota;
	
		dataDogs = fopen("dataDogs.dat", "r");	
    	if ( dataDogs== NULL) { 
	         printf("\n\tOcurrió un error al intentar abrir el archivo dataDogs.dat\n");
			 exit (-1);
		} 
		
		fseek(dataDogs,(id-1)*sizeof(struct dogType),SEEK_SET);
		
      	if((fread(&Mascota,sizeof(struct dogType),1,dataDogs)) == 0){
       		perror("error fread funcion Get_dataDogs");
           	exit(-1);
      	}

        if((fclose(dataDogs))!= 0){
       		perror(" error fclose funcion Set_dataDogs");
			exit(-1);
  		}	
		

		return Mascota;

}

void Registro(int cliente,struct sockaddr_in direccionCliente,char type[16],int registro,char cadena[32]){
	FILE *consulta;
	int r;
	time_t tiempo = time(0);
	struct tm *tlocal = localtime(&tiempo);
	char output[128];
	strftime(output,128,"%y/%m/%d %H:%M:%S",tlocal);


	consulta = fopen("serverDogs.log", "a");
	if (consulta == NULL) {
		perror("Error log");
	}
	fprintf(consulta,"[Fecha %s] Cliente [%s] [%s] [%i| %s]\n",output,inet_ntoa(direccionCliente.sin_addr),type,registro,cadena );
	r = fclose(consulta);
	if(r != 0){
		perror(" error fclose");
	}
}

int Funcion_Hash(char *Nombre){

    int indice=0,recorredor=0;

    while(*(Nombre+recorredor)!=0){

        indice=indice+tolower(*(Nombre+recorredor));

        recorredor++;
    }

    return indice%1000;

}

int *Get_Hash(){

	FILE *Tabla_Hash;
	int *Arreglo_Hash;

	Arreglo_Hash=malloc(sizeof(int)*1000);
	
	Tabla_Hash=fopen("Tabla_Hash.txt","r");
    
        if(Tabla_Hash==NULL){
            perror("Error al abrir el archivo Tabla_Hash.txt");
            exit(-1);
        }

	if(fread(Arreglo_Hash,1000*sizeof(int),1,Tabla_Hash)==0){

		perror("Error en la funcion fread, ubicada en la funcion Get_Hash()");
		exit(-1);
	}

	fclose(Tabla_Hash);

	return Arreglo_Hash;

}

void Set_Hash(int* Arreglo_Hash){

	FILE *Tabla_Hash;

	Tabla_Hash=fopen("Tabla_Hash.txt","w");

        if(Tabla_Hash==NULL){

            perror("Error al abrir el archivo Tabla_Hash.txt");

            exit(-1);

        }	

	if(fwrite(Arreglo_Hash,sizeof(int)*1000,1,Tabla_Hash)==0){

        perror("Error al escribir el tamaño de la tabla");

        exit(-1);
		
	}

	fclose(Tabla_Hash);

}
	
	int recvAll(int cliente,void * puntero, int size, int LOsa){
  int recibido=0;
  int res;
  while(recibido<size){
    res=recv(cliente,puntero+recibido,size-recibido,0);
  if(res==-1){
    perror("error al recibir");
    exit(-1);
  }
  recibido+=res;
  }
  return recibido;
}
int sendAll(int descriptor,void * puntero, int size, int LOsa){
  int enviado=0;
  int res;
  while(enviado< size){
    res=send(descriptor,puntero+enviado,size-enviado,0);
    if(res==-1){perror("error al enviar");}
    enviado+=res;

  }
return enviado;
}



	

