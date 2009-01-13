#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ricart_rpc.h"

 //! @brief The main function of the program
 /** 
 * @param argc The number of arguments entered on the command line
 * @param argv  A tabular containing the arguments entered on the command line
 * 
 * @return 0 if the program ended correctly, otherwise an errorcode
 */
int main(int argc, char** argv){
    struct couple me,ret;
    int managed;
    /* Truc pour le socket */
    int s_ecoute, lg_app,nb_client,i=0;
    struct sockaddr_in adr;
    char mes[1024];
    /* Fin truc pour le socket */
    if(argc<1 && strlen(argv[1]) < MAX_NAME_SIZE) {
	fprintf(stderr, "Usage : first argument must be the instance name	and 8 characters maximum.");
	/* PROGRAM EXIT IF NOT CORRECT ARGUMENT */
	exit(1);
    }

    strcpy(me.nom,argv[1]);
    /* Initialisation du socket */
    s_ecoute = socket(AF_INET, SOCK_DGRAM, 0);
    adr.sin_family=AF_INET;
    adr.sin_port=htons(0);
    adr.sin_addr.s_addr=INADDR_ANY;
    if(bind(s_ecoute, (struct sockaddr *)&adr, sizeof(struct sockaddr_in))!=0)
    {
	fprintf(stderr, "Socket could not be initialized");
	/* PROGRAM EXIT IF SOCKET NOT CORRECTLY INITIALIZED */
	exit(1);
    }
    /* Fin initialisation du socket */


    managed=callrpc("localhost", (long) NUMERO_PROG, (long) NUMERO_VERSION, (long) REGISTER_NB,
	    (xdrproc_t) xdr_couple, (char*) &me, (xdrproc_t) xdr_couple, (char*) &ret);

    if(managed==0)
    {
	printf("%s\n",me.nom);
	printf("%s\n",ret.nom);
    }
    else
    {
	fprintf(stderr, "could not contact rpc server\n"); 
    }
    /* Socket attend de recevoir */
    lg_app = sizeof(struct sockaddr_in);
    recus =recvfrom(s_ecoute, mes, sizeof(mes), 0, (struct sockadd *) &appelant, &lg_app);
    if(recu <=0){
	fprintf(stderr, "Socket did not receive what it was waiting for.");
	exit(1);
    }else{
	/* traitement de mes */
	nb_client=atoi(mes);
	/* Parser une chaine qui contient tout le bordel HostName + PortNumber */
    }


    /* Fin attente du socket */
    /* if (bind(s_ecoute,(struct sockaddr *) &adr, sizeof (struct sockaddr_in)) !=0)
       {
       printf("Pb de connexion\n");
       exit(1); 
       }*/
    /* Envoi des données */

    /*   sendto(socketfd,&donnee, 50,0, (struc sockaddr*) &SADDR, sizeof(SADDR));*/

    return 0;
}


