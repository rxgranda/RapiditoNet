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
#define MAX_THREAD_COUNT 1000



// Número de Hilos
int N=4;
Vector bufferPlanA;
Vector bufferPlanB;
Vector bufferPlanC;
pthread_mutex_t planA_mutex;
pthread_mutex_t planB_mutex;
pthread_mutex_t planC_mutex;
pthread_mutex_t respuesta_mutex;  
pthread_t * planA_threads[MAX_THREAD_COUNT];
pthread_t * planB_threads[MAX_THREAD_COUNT];
pthread_t * planC_threads[MAX_THREAD_COUNT];





void iniciarThreads();
void iniciarListener();
void agregarPaquete( char paquete,int secuencia);
void *leerPaquete(void* param );
void imprimirRespuesta(char paquete,int secuencia);
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
 	printf("\nIniciando....");	
 	
 	vector_init(&bufferPlanA);
 	vector_init(&bufferPlanB);
 	vector_init(&bufferPlanC);
 	
 	iniciarThreads();
 	printf("\nIniciando Listener2"); 
 	usleep(3000000);	
	iniciarListener();

  	int k;
	for (k=0; k<vector_size(&bufferPlanA);k++){
 		printf("%c :%d\n",vector_get_PAQUETE(&bufferPlanA,k),vector_get(&bufferPlanA,k));
 	}
 	
	for (k=0; k<vector_size(&bufferPlanB);k++){
 		printf("%c :%d\n",vector_get_PAQUETE(&bufferPlanB,k),vector_get(&bufferPlanB,k));
 	}
 	
	for (k=0; k<vector_size(&bufferPlanC);k++){
 		printf("%c :%d\n",vector_get_PAQUETE(&bufferPlanC,k),vector_get(&bufferPlanC,k));
 	}


 }

 void iniciarThreads(){
 	//printf("\nIniciando Hilos");	

	int n;
	pthread_attr_t attr;
	pthread_attr_init(&attr);	

 	pthread_mutex_init(&planA_mutex, NULL);
 	pthread_mutex_init(&planB_mutex, NULL);
 	pthread_mutex_init(&planC_mutex, NULL);
 	pthread_mutex_init(&respuesta_mutex, NULL);

 	//planA_threads = malloc(sizeof(pthread_t) * N);
 	//planB_threads = malloc(sizeof(pthread_t) * N/2);
 	//planC_threads = malloc(sizeof(pthread_t) * N/4);

 	 
	for (n = 0; n < N/4; n++)
	{
		
		int *arg = malloc(sizeof(int));       
        *arg = 1;		
 		pthread_create(&planA_threads[n],&attr,leerPaquete,(void *)arg);
 		
	}
	for (n = 0; n < N/2; n++)
	{
		
		int *arg = malloc(sizeof(int));       
        *arg = 2;		
 		pthread_create(&planB_threads[n],&attr,leerPaquete,(void *)arg);
	}
	for (n = 0; n < N; n++)
	{
		
		int *arg = malloc(sizeof(int));       
        *arg = 3;		
 		pthread_create(&planC_threads[n],&attr,leerPaquete,(void *)arg);
	}
	printf("\nHilos Iniciados");	
 }

 void iniciarListener(){
	printf("\nIniciando Listener");	
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
	//for (i=0; i<NPACK; i++) {
		if (recvfrom(s, buf, BUFLEN, 0, &si_other, &slen)==-1)
			perror("recvfrom err");
	    //printf("Received packet from %s:%d\nData: %s\n\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port), buf);
	    char* seq_paq=buf;
	    seq_paq++;
	    int secuencia=atoi(seq_paq);
	    char paquete=buf[0];	    
	    agregarPaquete(paquete, secuencia);	  	      
	}
	
	close(s);

 }
void agregarPaquete( char paquete,int secuencia){
							 
	switch(paquete){
	    	case 'A':
	    		pthread_mutex_lock(&planA_mutex);	
	    		vector_append(&bufferPlanA,secuencia,'A');
	    		pthread_mutex_unlock(&planA_mutex);
	    	break;
	    	case 'B':
	    		pthread_mutex_lock(&planB_mutex);	
	    		vector_append(&bufferPlanB,secuencia,'B');
	    		pthread_mutex_unlock(&planB_mutex);
	    	break;
	    	case 'C':
	    		pthread_mutex_lock(&planC_mutex);	
	    		vector_append(&bufferPlanC,secuencia,'C');
	    		pthread_mutex_unlock(&planC_mutex);
	    	break;
	    }

}

void *leerPaquete(void* param ){
	// printf("\nIniciando Plan C");
	int plan = *((int *) param);
	//char plan='D';
	switch(plan){
	    	
	    	case 3:
	    		printf("\nIniciando Plan C");	
	    		while(1){
	    			        			
	    				int flag=0, secuencia;
	    				char paquete;
	    				pthread_mutex_lock(&planC_mutex);	    					    			
	    				if(vector_size(&bufferPlanC)>0){
	    					paquete=vector_get_PAQUETE(&bufferPlanC,0);
	    					secuencia=vector_get(&bufferPlanC,0);	    					
	    					desencolar(&bufferPlanC);	
	    					flag=1;    					
	    				}	    				
	    				pthread_mutex_unlock(&planC_mutex);
	    				if(flag==1)
	    					imprimirRespuesta(paquete,secuencia);	    		
				}	 
	    	break;

	    	case 2:
	    		printf("\nIniciando Plan B");	
	    		while(1){	    			
	    				int flag=0, secuencia;
	    				char paquete;
	    				pthread_mutex_lock(&planB_mutex);	    					    			
	    				if(vector_size(&bufferPlanB)>0&&vector_size(&bufferPlanC)<=0){
	    					paquete=vector_get_PAQUETE(&bufferPlanB,0);
	    					secuencia=vector_get(&bufferPlanB,0);	    					
	    					desencolar(&bufferPlanB);	
	    					flag=1;    					
	    				}	    				
	    				pthread_mutex_unlock(&planB_mutex);
	    				if(flag==1)
	    					imprimirRespuesta(paquete,secuencia);	    		
				}	 
	    	break;
	    	case 1:
	    	printf("\nIniciando Plan A");
	    	//printf("\nIniciando Plan A");
	    		while(1){	    			
	    				int flag=0, secuencia;
	    				char paquete;
	    				pthread_mutex_lock(&planA_mutex);	    					    			
	    				if(vector_size(&bufferPlanA)>0&&vector_size(&bufferPlanB)<=0&&vector_size(&bufferPlanC)<=0){
	    					paquete=vector_get_PAQUETE(&bufferPlanA,0);
	    					secuencia=vector_get(&bufferPlanA,0);	    					
	    					desencolar(&bufferPlanA);	
	    					flag=1;    					
	    				}	    				
	    				pthread_mutex_unlock(&planA_mutex);
	    				if(flag==1)
	    					imprimirRespuesta(paquete,secuencia);	    		
				}	    		
	    		
	    	break;
	    }
	
	    printf("\nIniciando Planaaaaaaaaaaaa %d",plan);
printf("\nIniciando Planaaaaaaaaa" );
}

void imprimirRespuesta(char paquete,int secuencia){	
	pthread_mutex_lock(&respuesta_mutex);	
	printf("Respuesta Plan: %c Secuencia:%d\n", paquete,secuencia);
	pthread_mutex_unlock(&respuesta_mutex);

}
