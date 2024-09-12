#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>


int Funcion_Hash();

int main(){


    FILE *Tabla_Hash;

    int Arreglo_Hash[1000], opcion, salir=0;
    char Nombre[32];

    Tabla_Hash=fopen("Tabla_Hash.txt","r");
    
        if(Tabla_Hash==NULL){
            perror("Error al abrir el archivo Tabla_Hash.txt");
            exit(-1);
        }

    fread(Arreglo_Hash,1000*sizeof(int),1,Tabla_Hash);


    while(salir==0){

    

    printf("\n1) Ver el Id del registro almacenado en la tabla hash \n2) Salir");
            
    scanf("%i",&opcion);

        

        switch(opcion)
        {

            case 1:
                
                system("clear");   

                printf("\nDigite un nombre");
                scanf("%s",Nombre);

                printf("El rergistro que en la tabla hash que le corresponde al nombre %s es: %i ",Nombre,Arreglo_Hash[Funcion_Hash(Nombre)]);

                
                break;

            case 2:
                salir=1;
                
        }
    }


    return 0;
}


int Funcion_Hash(char *Nombre){

    int indice=0,recorredor=0;

    while(*(Nombre+recorredor)!=0){

        indice=indice+tolower(*(Nombre+recorredor));

        recorredor++;
    }

    return indice%1000;

}