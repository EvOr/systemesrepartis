/*
   Ce programme produit une PDU-UDP à destination du processus
   lié au port UDP 6545
   à intervalle de temps calculé aléatoirement
 */

#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>

#define PORT 6545

int main( int argc, char **argv)
{
   int socketfd;
   struct sockaddr_in SADDR;
   struct hostent *temp;
   char systeme_distant[100];
   char donnee[50] ="123456789 123456789 123456789 123456789 123456789";
   int dort,i;

   printf("Envoi de donnees par UDP vers %s\n",argv[1]);

   /* On recupere l adresse IP du systeme distant */
   if( !(temp = gethostbyname(argv[1])) )
   {
      printf(" gethostbyname - %d\n", h_errno);
      exit(-1);
   }

   /*  Preparer la socket de communication */
   socketfd = socket( AF_INET, SOCK_DGRAM, 0 );


   /* Initialisation des champs de la structure */
   SADDR.sin_family = AF_INET;
   SADDR.sin_port = htons( PORT ); 

   memcpy( &SADDR.sin_addr, temp->h_addr, 4 );

   /* Envoi des données */
   while(1)
   {
      dort = random()%10;	
      /* calcule un delai entre 0 et 9 secondes 
	 affiche des points et une etoile pour montrer le delai */
      for (i=0;i<dort-1;i++) printf(".");printf("*"); fflush(0);
      /* s'endort pour la durée du delai */
      sleep(dort);
      /*  envoie la PDU */
      sendto(socketfd, &donnee, 50, 0, (struct sockaddr *)&SADDR, sizeof(SADDR));
   }
}
