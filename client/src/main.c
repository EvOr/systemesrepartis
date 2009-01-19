/** 
 * @file main.c
 * @brief The client implementation for the Ricart and Agrawala algorithm
 * @author Julien Dessaux & Jan Villeminot
 * @date 2009-01-17
 */

#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "agrawala.h"
#include "ricart_rpc.h"

//! @brief The main function of the program
//! @param argc The number of arguments entered on the command line
//! @param argv  A tabular containing the arguments entered on the command line
//! @return 0 if the program ended correctly, otherwise an errorcode
int main(int argc, char** argv){
//    struct request_t me, ret;
//    int managed;
    /* Truc pour le socket */
//    int s_ecoute, lg_app,nb_client,i=0, recu_udp;
//    struct sockaddr_in adr, appelant;
//    char mes[1024], recus[1024];
    /* Fin truc pour le socket */
//    if(argc<1 || strlen(argv[1]) < MAX_NAME_SIZE) {
//	fprintf(stderr, "Usage : first argument must be the instance name and 8 characters maximum.");
//	exit(1);
//    }

//    strcpy(me.nom,argv[1]);

printf("%d\n", agrawala_init());
return 0;




//    managed=callrpc("localhost", (long) NUMERO_PROG, (long) NUMERO_VERSION, (long) REGISTER_NB,
//	    (xdrproc_t) xdr_request, (char*) &me, (xdrproc_t) xdr_response, (char*) &ret);

//    if(managed==0)
//    {
//	printf("%s\n",me.nom);
//	printf("%s\n",ret.nom);
//    }
//    else
//    {
//	fprintf(stderr, "could not contact rpc server\n"); 
//    }
//    /* Socket attend de recevoir */
//    lg_app = sizeof(struct sockaddr_in);
//    recu_udp =recvfrom(s_ecoute, mes, sizeof(mes), 0, (struct sockaddr *)& appelant, (socklen_t*) &lg_app);
//    if(recu_udp <=0){
//	fprintf(stderr, "Socket did not receive what it was waiting for.");
//	exit(1);
//    }else{
//	/* traitement de mes */
//	nb_client=atoi(mes);
//	/* Parser une chaine qui contient tout le bordel HostName + PortNumber */
//    }
//
//    /* Fin attente du socket */
//    /* if (bind(s_ecoute,(struct sockaddr *) &adr, sizeof (struct sockaddr_in)) !=0)
//       {
//       printf("Pb de connexion\n");
//       exit(1); 
//       }*/
//    /* Envoi des données */
//
//    /*   sendto(socketfd,&donnee, 50,0, (struc sockaddr*) &SADDR, sizeof(SADDR));*/

    return 0;
}

