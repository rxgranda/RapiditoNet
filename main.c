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
Vector bufferPlanB;
Vector bufferPlanC;



 /*
 *Descripcion: Punto de inicio de la Instancia del servidor Nubesoft
 */
 void  main(int argc, char  *argv[])
 { 	
 	int opt=0;
 	while ((opt = getopt(argc, argv, "N:")) != -1) {
 		switch(opt) {
 			case 'N':
	 			N=atoi(optarg);
	 			printf("\nNúmero de Hilos =%d", N);
	 						
 			break; 		

 			case '?': 			
 				printf("\nArgumento desconocido");
 			
 			break; 			
 		}
 	} 	
 	vector_init(&bufferPlanA);
 	vector_init(&bufferPlanB);
 	vector_init(&bufferPlanC);
 	printf("\nIniciando....");	 
 	printf("\nIniciando....");	
 	struct sockaddr_in si_me, si_other;
	int s, i, slen=sizeof(si_other);
	char buf[BUFLEN];

	if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
		perror("sock err");
		memset((char *) &si_me, 0, sizeof(si_me));
		si_me.sin_family = AF_INET;
		si_me.sin_port = htons(PORT);
		si_me.sin_addr.s_addr = htonl(INADDR_ANY);
		if (bind(s, &si_me, sizeof(si_me))==-1)
		      perror("bind");

	while(1){
		if (recvfrom(s, buf, BUFLEN, 0, &si_other, &slen)==-1)
			perror("recvfrom err");
	    printf("Received packet from %s:%d\nData: %s\n\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port), buf);
	}
	close(s);	
 }


