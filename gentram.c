#define SRV_IP "127.0.0.1"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFLEN 512
#define SECLEN 6
#define SECNUM 3
#define PORT 5555

 int main(void)
 {
 struct sockaddr_in si_other;
 int s, i,j,k,count,numpack,countA,countB,countC, slen=sizeof(si_other);
 unsigned int delay;
 char buf[BUFLEN];
 char *seq[SECNUM];
 
 seq[0]="AABBCC";
 seq[1]="ABCCCC";
 seq[2]="AABBAC";


 if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
	perror("socked");
 memset((char *) &si_other, 0, sizeof(si_other));
 si_other.sin_family = AF_INET;
 si_other.sin_port = htons(PORT);
 if (inet_aton(SRV_IP, &si_other.sin_addr)==0) {
 	fprintf(stderr, "inet_aton() failed\n");
	exit(1);
  }

printf("Seleccionar la secuencia\n\t [1]: %s\n\t [2]: %s\n\t [3]: %s\n", seq[0],seq[1],seq[2]);
scanf("%d",&j);
j=j-1;

printf("Cuantas secuencias va a enviar?\n");
scanf("%d",&numpack);

printf("Cuanto tiempo (us) debe haber entre paquete y paquete [0 1000000]? \n");
scanf("%d",&delay);


count =0;
countA=0;
countB=0;
countC=0;
for (i=0; i<numpack; i++) {  

	 for (k=0;k<SECLEN;k++) {
 	 switch	(seq[j][k]) {
	   case 'A':	count=countA++;break;
	   case 'B':	count=countB++;break;
 	   case 'C':	count=countC++;break;
           }	
 	 usleep(delay);
	 sprintf(buf, "%c%d\n", seq[j][k],count);
	 if (sendto(s, buf, BUFLEN, 0, &si_other, slen)==-1)
			perror("sendto()");
	}
}

close(s);
printf("Terminado correctamente\n" );
return 0;
}
