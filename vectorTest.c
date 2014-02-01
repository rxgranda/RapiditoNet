#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "vector.h"

#define BUFLEN 512
#define NPACK 100
#define PORT 5555


// Número de Hilos
int N=4;
Vector bufferPlanA;



 /*
 *Descripcion: Punto de inicio de la Instancia del servidor Nubesoft
 */
 void  main(int argc, char  *argv[])
 { 	
 	
 	vector_init(&bufferPlanA);
 	vector_append(&bufferPlanA,0,'a');
 	vector_append(&bufferPlanA,1,'a');
 	vector_append(&bufferPlanA,2,'a');
 	int i;
 	for ( i=0; i<vector_size(&bufferPlanA);i++){
 		printf("%d\n",vector_get(&bufferPlanA,i));
 	}
 	desencolar(&bufferPlanA);
 	
 	for (i=0; i<vector_size(&bufferPlanA);i++){
 		printf("%d\n",vector_get(&bufferPlanA,i));
 	}
 	vector_append(&bufferPlanA,3,'b');
 	vector_append(&bufferPlanA,4,'b');
 		desencolar(&bufferPlanA);
 	for (i=0; i<vector_size(&bufferPlanA);i++){
 		printf("%c\n",vector_get_PAQUETE(&bufferPlanA,i));
 	}
 }


