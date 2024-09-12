#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void Inicializar_Hash(){

	FILE *Tabla_Hash;
	int contador=0,Arreglo_Hash[1000];

	if((Tabla_Hash=fopen("Tabla_Hash.txt","w"))==NULL){
		perror("Error en la funcion Inicializar_Hash");
		exit(-1);
	}

	while(contador<1000){
	
		Arreglo_Hash[contador]=-1;
		contador++;
	}

	fwrite(Arreglo_Hash,1000*sizeof(int),1,Tabla_Hash);
	fclose(Tabla_Hash);
}

int main(){

    Inicializar_Hash();
    printf("La tabla hash a sido inicializada.");

    return 0;
}
