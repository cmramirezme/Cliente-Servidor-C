#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>


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



void Ingresar();
void MenuPrincipal();
void Ver();
void Buscar();
int Crear_Conexion();
int sendAll();
int recvAll();


int main(void) {

	int cliente;

	cliente=Crear_Conexion();

	MenuPrincipal(cliente);

	return 0;
}




void Ingresar(int cliente){

	struct dogType New_Registro;
	int flag;
	
	printf("Digite Nombre:");scanf("%s", (&New_Registro)->Nombre); 

	printf("Digite Tipo:");scanf("%s", (&New_Registro)->Tipo); 

    printf("Digite Edad [años]:");scanf("%i",&((&New_Registro)->Edad)); 

    printf("Digite Raza:");scanf("%s",(&New_Registro)->Raza ); 

    printf("Digite estatura [cm]:");scanf("%i",&((&New_Registro)->Estatura)); 

    printf("Digite Peso [Kg]:");scanf("%f",&((&New_Registro)->Peso)); 

    printf("Digite sexo [H/M]:"); scanf(" %c",&((&New_Registro)->Sexo)); 
	
	New_Registro.Siguiente=-1;

	sendAll(cliente,&New_Registro,sizeof(struct dogType),0);



	int br = recvAll(cliente, &flag, sizeof(flag), 0);

		if (br <= 0) {
			perror("Desconección");
			exit(-1);
		}

	if(flag = 1){
		printf("\n---|| El registro se ha guardado correctamente ||---\n");
	}

	system("\n\nread -p 'Press Enter to continue...' var");


}



void Ver(int cliente){

	int Num_Registros,id,intentos=0,S_N, Tamano_Out, Tamano_In;
	struct dogType Mascota;
	char *Out_Text, *In_Text,a='*';
	FILE *Historia_In;
	
	if (recvAll(cliente, &Num_Registros, sizeof(int), 0) <= 0) {
		perror("Desconección");
		exit(-1);
	}

    printf ("El numero actual de registros de mascotas es: %i",Num_Registros);
    printf ("\nIngrese el numero o el Id del registro que desea visualizar: ");
    scanf("%i", &id);
	sendAll(cliente, &id, sizeof(int), 0);


	if(id<1 || id>Num_Registros){

        printf("El id ingresado sobrepasa la cantidad de registros almacenados, le quedan %i intentos ",3-intentos);
        scanf("%i", &id);
        intentos++;
		sendAll(cliente, &id, sizeof(int), 0);
        if(intentos==3){
            return;
        }   
    }

	if (recvAll(cliente, &Mascota, sizeof(struct dogType), 0) <= 0) {
		perror("Desconección");
		exit(-1);
	}

	printf("\nNombre: %s\nTipo: %s\nEdad: %i\nRaza: %s \nEstatura: %i\nPeso: %2f\nGenero: %c\n, Anterior: %i\n",Mascota.Nombre, Mascota.Tipo, Mascota.Edad, Mascota.Raza, Mascota.Estatura, Mascota.Peso, Mascota.Sexo, Mascota.Siguiente);

	printf("\nPresione 1 si desea ver la historia clinica, de lo contrario oprima 0 \n");
    scanf("%i",&S_N);
	sendAll(cliente, &S_N, sizeof(int), 0);

	if(S_N==1){

		int Nombre_Historia_Cliente;

		if (recvAll(cliente, &Nombre_Historia_Cliente, sizeof(int), 0) < 0) {
			perror("Desconección");
			exit(-1);
		}

		char Direccion_1[34]="Historias_Clinicas_Clientes/";
        char Direccion_2[10];
        char Direccion_3[5]=".txt";
		char Nano[34]="nano ";
 
		sprintf(Direccion_2, "%i", Nombre_Historia_Cliente);
		strcat(Direccion_1,Direccion_2);
        strcat(Direccion_1,Direccion_3);

		if (recvAll(cliente, &Tamano_Out, sizeof(int), 0) < 0) {
			perror("Desconección");
			exit(-1);
		}
		
		Out_Text=malloc(Tamano_Out);
		
		if (recvAll(cliente, Out_Text, Tamano_Out, 0) < 0) {
			perror("Desconección");
			exit(-1);
		}

		Historia_In=fopen(Direccion_1,"w+");
            if(Historia_In==NULL){
                printf("error");                
			}
		if(fwrite(Out_Text,Tamano_Out,1,Historia_In)==0){
			perror("Error al escribir en Historia_In.txt");
			exit(-1);
		}

		fclose(Historia_In);

		Historia_In=fopen(Direccion_1,"a+");
            if(Historia_In==NULL){
                printf("error");                
			}
		

		strcat(Nano,Direccion_1);
        system(Nano);

		fseek(Historia_In,0L,SEEK_END);
		Tamano_In=ftell(Historia_In);

			if(Tamano_In==0){
				Tamano_In++;
				In_Text=malloc(Tamano_In);
					if(In_Text==NULL){
					perror("Error en malloc de la funcion VerServer->Our_Text");
					exit(-1);
					}	
				fwrite(&a,1,1,Historia_In);

			}

			else{
				In_Text=malloc(Tamano_In);
					if(In_Text==NULL){
					perror("Error en malloc de la funcion VerServer->Our_Text");
					exit(-1);
				}

			}

		sendAll(cliente, &Tamano_In, sizeof(int), 0);
		fseek(Historia_In,0L,SEEK_SET);

		
		fread(In_Text,Tamano_In,1,Historia_In);
		sendAll(cliente, In_Text, Tamano_In, 0);
		
		free(Out_Text);
		free(In_Text);
		fclose(Historia_In);

		printf("El Archivo fue guardado exitosamente");
	}


}





void Borrar(int cliente){
	int Num_Registros,Reg_Borrar=0;
	
	if (recvAll(cliente, &Num_Registros, sizeof(int), 0) <= 0) {
		perror("Desconección");
		exit(-1);
	}

	printf("\nEl número de registros es: %i\n", Num_Registros );
	printf("\nDigite el número de registro que desea borrar:\n");
	scanf("%i",&Reg_Borrar); 
	printf("\nReg_Borrar: %i\n", Reg_Borrar );
	sendAll(cliente,&Reg_Borrar,sizeof(int),0);

	if(Reg_Borrar>Num_Registros){
		printf("\nEl numero de Registros debe ser menor a: %i\n",Num_Registros);
	}
	
	else{

		printf("\n|| Borrando el registro número: %i ||\n\n", Reg_Borrar );
		if (recvAll(cliente, &Num_Registros, sizeof(int), 0) <= 0) {
			perror("Desconección");
			exit(-1);
		}

		printf("\n|| El registro se a eliminado satisfactoriamente. Ahora hay %i registros ||\n\n", Num_Registros );

	}
	system("\n\nread -p 'Press Enter to continue...' var");

}





void Buscar(int cliente){
	
	char busqueda[32];

	struct dogType Mascota;

	int Coincide_Busqueda = 0, Registro_Encontrado=0,Id=0, flag, Bytes_Recibidos=0;

	printf("--------------------BUSCANDO----------------------");
	printf("\nIngrese el nombre que desea buscar: ");
	scanf("%s", busqueda); 
	sendAll(cliente,&busqueda,sizeof(busqueda),0); // Busqueda -> variable con el nombre a buscar
	printf("\n--||El nombre a buscar es: %s||--\n",busqueda );

	if (recvAll(cliente, &flag, sizeof(flag), 0) <= 0) {
		perror("Desconección");
		exit(-1);
	}

		
	while(flag!=-1){

		if ((recvAll(cliente, &Coincide_Busqueda, sizeof(int), 0)) <= 0) {
			perror("Desconección");
			exit(-1);
		}



		if(Coincide_Busqueda==1){
			printf("------------------------------------------");
			Registro_Encontrado=1;
			if ((Bytes_Recibidos=recvAll(cliente, &Mascota, sizeof(struct dogType), 0)) <= 100) {
			&(Mascota)+100-Bytes_Recibidos	;
			}

			if ((recvAll(cliente, &Id, sizeof(int), 0)) <= 0) {
			perror("Desconección");
			exit(-1);
			}
		
			printf("\nNombre: %s", Mascota.Nombre);
			printf("\nTipo: %s",Mascota.Tipo );
			printf("\nEdad: %d", Mascota.Edad);
			printf("\nRaza: %s", Mascota.Raza);
			printf("\nEstatura: %d", Mascota.Estatura);
			printf("\nPeso: %.2f", Mascota.Peso);
			printf("\nSexo: %c ", Mascota.Sexo);
			printf("\nId: %i \n", Id+1);
		}
				
		
		if ((recvAll(cliente, &flag, sizeof(flag), 0)) <= 0) {
			perror("Desconección");
			exit(-1);
		}

	}

	if ((recvAll(cliente, &Coincide_Busqueda, sizeof(int), 0)) <= 0) {
			perror("Desconección");
			exit(-1);
		}

		if(Coincide_Busqueda==1){
			printf("------------------------------------------");
			Registro_Encontrado=1;
			if ((recvAll(cliente, &Mascota, sizeof(struct dogType), 0)) <= 0) {
			perror("Desconección");
			exit(-1);
			}

			if ((recvAll(cliente, &Id, sizeof(int), 0)) <= 0) {
			perror("Desconección");
			exit(-1);
			}
		
			printf("\nNombre: %s", Mascota.Nombre);
			printf("\nTipo: %s",Mascota.Tipo );
			printf("\nEdad: %d", Mascota.Edad);
			printf("\nRaza: %s", Mascota.Raza);
			printf("\nEstatura: %d", Mascota.Estatura);
			printf("\nPeso: %.2f", Mascota.Peso);
			printf("\nSexo: %c ", Mascota.Sexo);
			printf("\nId: %i \n", Id+1);
		}
				
		
		

	if(Registro_Encontrado){printf("\n----|| La Busqueda Ha Sido Exitosa ||----\n"); }
	else{ // si no hay coincidencias se le notifica al usuario
		printf("\nNo existen registros con el nombre:  %s \n", busqueda );
	}
	system("\n\nread -p 'Press Enter to continue...' var");



}





int Crear_Conexion(){

// Aqui se crea el socket (enchufe) por parte del cliente
	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = inet_addr("127.0.0.1");
	direccionServidor.sin_port = htons(8080);
	int cliente = socket(AF_INET, SOCK_STREAM, 0);

//La funcion connect crea una conexion entre el cliente y el servidor
	if (connect(cliente, (void*) &direccionServidor, sizeof(direccionServidor)) != 0) {
		perror("Error en la funcion connect()");
		exit -1;
	}

	return cliente;
}


void MenuPrincipal(int cliente){
	
	int x,n;

     do {
        // Opciones del menú
          printf("\n1. Ingresar Registro");
		  printf("\n2. Ver Registro");
          printf("\n3. Borrar Registro");
          printf("\n4. Buscar Registro");
		  printf("\n5. Salir\n");

          scanf("%i",&n); 

		//Envia la opcion elegida por el cliente al servidor  
		  sendAll(cliente, &n, sizeof(n), 0);

          switch (n) {
               case 1:

				// Llama metodo ingresar
                    Ingresar(cliente); 
					break; 
				
               case 2:

				// llama metodo ver
					Ver(cliente); 
					break; 
               case 3:

				// llama metodo borrar
                    Borrar(cliente); 
					break; 

               case 4:

				// llama metodo buscar
                    Buscar(cliente); 
					break; 
					
               case 5:
               printf("Saliendo....");
			   close(cliente);

          }
     } while(n == 1 || n == 2 || n == 3 || n == 4 ); // Opcion 5, salir.

};

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




