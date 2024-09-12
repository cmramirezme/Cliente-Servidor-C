#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void Set_Num_Registros(int Num_Registros){

	FILE *F_Num_Registros;

	if((F_Num_Registros=fopen("F_Num_Registros.txt","w"))==NULL){
		perror("Error en la funcion Set_Num_Registros");
		exit(-1);
	}

	fwrite(&Num_Registros,sizeof(int),1,F_Num_Registros);
	fclose(F_Num_Registros);
}

int main(){
    int num_reg;
    printf("Escriba el numero de registros que actualmente existen: ");
    scanf("%i",&num_reg);
    Set_Num_Registros(num_reg);

    return 0;
}
