#include <stdio.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>

int main(int argc, char *argv[]) {
   int s_ecoute, recus;
   char mes[20];
   struct sockaddr_in adr, appelant;
   int lg_app;

   s_ecoute=socket(AF_INET, SOCK_DGRAM,0);
   printf("la socket est cree\n");

   adr.sin_family=AF_INET;
   adr.sin_port=htons(6515);
   adr.sin_addr.s_addr=INADDR_ANY;

   if (bind(s_ecoute,(struct sockaddr *) &adr, sizeof (struct sockaddr_in)) !=0)
   {
      printf("Pb de connexion\n");
      exit(1); 
   }

   printf("je suis en ecoute\n");
   lg_app=sizeof(struct sockaddr_in);
   recus=recvfrom(s_ecoute,mes,sizeof(mes),0,(struct sockadd *) &appelant,&lg_app);
   if (recus <=0)
      printf("bug\n");
   else {
      printf("source : %s et %d\n",inet_ntoa(appelant.sin_addr.s_addr),
	    ntohs(appelant.sin_port));
      printf("message : %s\n",mes);
      sendto(s_ecoute,"OK",3,0,&appelant, lg_app); }
      close(s_ecoute);
}
