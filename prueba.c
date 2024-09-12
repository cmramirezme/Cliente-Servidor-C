#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>


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



struct dogType Get_dataDogs();
void Set_dataDogs();

int main(){

	int Id;
	struct dogType data, Mascota_Get;
	/*printf("Introduzca nombre de la mascota: ");
    scanf ("%s", data.Nombre);

    printf("Introduzca tipo de mascota: ");
    scanf ("%s", data.Tipo);

    printf("Introduzca edad de la mascota: ");
    scanf ("%i", &(data.Edad));

    printf("Introduzca raza de la mascota: ");
    scanf ("%s", data.Raza);

    printf("Introduzca estatura de la mascota: ");
    scanf ("%i", &(data.Estatura));

    printf("Introduzca peso de la mascota: ");
    scanf ("%2f", &(data.Peso));

    printf("Introduzca genero mascota: ");
    scanf (" %c", &(data.Sexo));

	Set_dataDogs(&data,3);*/
	scanf("%i",&Id);
	Mascota_Get=Get_dataDogs(Id);
	
	printf("\nNombre: %s\nTipo: %s\nEdad: %i\nRaza: %s \nEstatura: %i\nPeso: %2f\nGenero: %c\n",Mascota_Get.Nombre, Mascota_Get.Tipo, Mascota_Get.Edad, Mascota_Get.Raza, Mascota_Get.Estatura, Mascota_Get.Peso, Mascota_Get.Sexo);

    return 0;
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
