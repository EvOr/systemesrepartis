#include <netinet/in.h>
#include <sys/time.h>
#include <sys/socket.h>

#define PORT 6545

int main()
{
   /*------------------------------------------ les variables */
   int socketfd;		/* pour la réception par UDP */
   struct sockaddr_in SADDR;
   int retour, taille_SADDR;
   char donnee[100];

   char frappe[100];	/* pour la lecture clavier */

   fd_set lectures;	/* pour select() */
   struct timeval intervalle;

   /*------------------------------------------ le programme

     préparation pour la réception par socket */
   socketfd = socket(AF_INET,SOCK_DGRAM,0);
   if (socketfd == -1)
   {
      perror("ERREUR socket() : ");
      exit(1);
   }

   SADDR.sin_family = AF_INET;		/* Déclaration d'attente */
   SADDR.sin_port = htons(PORT);		/* port à écouter */
   SADDR.sin_addr.s_addr = INADDR_ANY;	/* sur toutes les adresses IP 
					   du système */
   retour = bind(socketfd,(struct sockaddr *)&SADDR,sizeof(SADDR) );
   if (retour == -1)
   {
      perror("ERREUR bind() :");
      exit(1);
   }

   printf("Attente par select sur \n");
   printf("\t- le clavier STDIN\n");
   printf("\t- le port UDP\n");
   printf("\t- échéance TEMPS\n\n");

   while(1)
   {
      /* preparation des attentes (il faut le refaire à chaque fois !!)
	 preparation du délai (3.5 secondes) */
      intervalle.tv_sec = 3;		/* 3 secondes */
      intervalle.tv_usec = 500000;	/* 0.5 seconde 
					   les ensembles de flots */
      FD_ZERO(&lectures);		/* on vide l'ensemble */
      FD_SET(0,&lectures);		/* ajout de STDIN */
      FD_SET(socketfd, &lectures);	/* ajout de la socket */

      /* l'attente proprement dite*/
      retour = select(socketfd+1, &lectures, NULL, NULL, &intervalle);

      if (retour == -1)
      {		
	 perror("ERREUR select() :");
	 exit(1);
      }

      if (retour == 0)
      {		/* c'est un timeout */
	 printf("TEMPS \n");
      }

      if (FD_ISSET(socketfd,&lectures))
      {		/* reçu sur la socket */
	 taille_SADDR=sizeof(struct sockaddr_in);
	 retour = recvfrom(socketfd, &donnee, sizeof(donnee), 0, 
	       (struct sockaddr *) &SADDR, (unsigned long *)&taille_SADDR );
	 donnee[retour] = 0;
	 printf("UDP (%s:%d): %s\n",
	       inet_ntoa(SADDR.sin_addr),
	       ntohs(SADDR.sin_port),
	       donnee);
      }

      if (FD_ISSET(0,&lectures))
      {		/* reçu sur STDIN */
	 scanf("%s",frappe);
	 printf("STDIN : %s\n",frappe);
      }
   }
}
