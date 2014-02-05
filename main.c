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
#define MAX_THREAD_COUNT2 100000



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
pthread_t * socketIn_threads[MAX_THREAD_COUNT2];
pthread_cond_t count_threshold_planC;
pthread_cond_t count_threshold_planB;
pthread_cond_t count_threshold_planA;
int planStatus[3]={0,0,0};





void iniciarThreads();
void iniciarListener();
//void agregarPaquete( char paquete,int secuencia);

void *agregarPaquete( void* param);
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
	int n;
	pthread_attr_t attr;
	pthread_attr_init(&attr);	

 	pthread_mutex_init(&planA_mutex, NULL);
 	pthread_mutex_init(&planB_mutex, NULL);
 	pthread_mutex_init(&planC_mutex, NULL);
 	pthread_mutex_init(&respuesta_mutex, NULL);
 	pthread_cond_init (&count_threshold_planC, NULL);
 	pthread_cond_init (&count_threshold_planB, NULL);
 	pthread_cond_init (&count_threshold_planA, NULL);
 	
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
	printf("\nIniciando Listener\n");	
 	struct sockaddr_in si_me, si_other;
	int s, i, slen=sizeof(si_other),n=0;
	char buf[BUFLEN];
	pthread_attr_t attr;
	pthread_attr_init(&attr);	

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
	   char* seq_paq=buf;
	    seq_paq++;
	    char paquete=buf[0];	    
	    int *arg = malloc(sizeof(int)*2); 
	    if(paquete=='A')      
        	arg[0] = 1;
        else if(paquete=='B')      
        	arg[0] = 2;
        else if(paquete=='C')      
        	arg[0] = 3;
        arg[1]=atoi(seq_paq);
	    pthread_create(&socketIn_threads[n],&attr,agregarPaquete,(void *)arg);	
	    n=(n+1)%MAX_THREAD_COUNT2;  		   	  
	}	
	close(s);
 }
void *agregarPaquete( void* param){
	 int secuencia,paquete;	 
	 paquete = *((int *) param);
	 secuencia=((int *) param)[1];	
				
	switch(paquete){
	    	case 1:
	    		pthread_mutex_lock(&planA_mutex);	
	    		vector_append(&bufferPlanA,secuencia,'A');
	    		planStatus[0]=1;
	    		pthread_cond_signal(&count_threshold_planA);
	    		pthread_mutex_unlock(&planA_mutex);
	    	break;
	    	case 2:
	    		pthread_mutex_lock(&planB_mutex);	
	    		vector_append(&bufferPlanB,secuencia,'B');
	    		planStatus[1]=1;
	    		pthread_cond_signal(&count_threshold_planB);
	    		pthread_mutex_unlock(&planB_mutex);
	    	break;
	    	case 3:
	    		pthread_mutex_lock(&planC_mutex);	
	    		vector_append(&bufferPlanC,secuencia,'C');
	    		planStatus[2]=1;
	    		pthread_cond_signal(&count_threshold_planC);
	    		pthread_mutex_unlock(&planC_mutex);
	    	break;
	    	case 'A':
	    		pthread_mutex_lock(&planA_mutex);	
	    		vector_append(&bufferPlanA,secuencia,'A');
	    		planStatus[0]=1;
	    		pthread_cond_signal(&count_threshold_planA);
	    		pthread_mutex_unlock(&planA_mutex);
	    	break;
	    	case 'B':
	    		pthread_mutex_lock(&planB_mutex);	
	    		vector_append(&bufferPlanB,secuencia,'B');
	    		planStatus[1]=1;
	    		pthread_cond_signal(&count_threshold_planB);
	    		pthread_mutex_unlock(&planB_mutex);
	    	break;
	    	case 'C':
	    		pthread_mutex_lock(&planC_mutex);	
	    		vector_append(&bufferPlanC,secuencia,'C');
	    		planStatus[2]=1;
	    		pthread_cond_signal(&count_threshold_planC);
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
	    				if(vector_size(&bufferPlanC)==0){
	    					planStatus[2]=0;
	    					pthread_cond_wait(&count_threshold_planC, &planC_mutex);  
	    				}	    			    					    		
	    				if(vector_size(&bufferPlanC)>0){
	    					paquete=vector_get_PAQUETE(&bufferPlanC,0);
	    					secuencia=vector_get(&bufferPlanC,0);	    					
	    					desencolar(&bufferPlanC);	
	    					imprimirRespuesta(paquete,secuencia);	    						    					     			
	    				}
	    				pthread_mutex_unlock(&planC_mutex);	    				    						    	
				}	 
	    	break;

	    	case 2:
	    		printf("\nIniciando Plan B");	
	    		while(1){	    			
	    				int flag=0, secuencia;
	    				char paquete;
	    				pthread_mutex_lock(&planB_mutex);	
	    				if(vector_size(&bufferPlanB)==0){
	    					planStatus[1]=0;  
	    					pthread_cond_wait(&count_threshold_planB, &planB_mutex); 
	    				}
	    				if(vector_size(&bufferPlanB)>0&&planStatus[2]==0){	    				
		    					paquete=vector_get_PAQUETE(&bufferPlanB,0);
		    					secuencia=vector_get(&bufferPlanB,0);	    					
		    					desencolar(&bufferPlanB);		    				 
	    						imprimirRespuesta(paquete,secuencia);		    						     				
	    				}	    				
	    				pthread_mutex_unlock(&planB_mutex);	    				 		
				}	 
	    	break;
	    	case 1:
	    	printf("\nIniciando Plan A");
	    		while(1){
	    				int flag=0, secuencia;
	    				char paquete;
	    				pthread_mutex_lock(&planA_mutex);
	    				if(vector_size(&bufferPlanA)==0)
	    					pthread_cond_wait(&count_threshold_planA, &planA_mutex);

	    				if(vector_size(&bufferPlanA)>0 &&planStatus[1]==0&& planStatus[2]==0 ){	    				
	    					paquete=vector_get_PAQUETE(&bufferPlanA,0);
	    					secuencia=vector_get(&bufferPlanA,0);	    					
	    					desencolar(&bufferPlanA);
	    					imprimirRespuesta(paquete,secuencia);			    				  				
	    				}	    				
	    				pthread_mutex_unlock(&planA_mutex);	    					    		
				}	    			    		
	    	break;
	    }		
}

void imprimirRespuesta(char paquete,int secuencia){	
	printf("Respuesta Plan: %c Secuencia:%d\n", paquete,secuencia);
}
