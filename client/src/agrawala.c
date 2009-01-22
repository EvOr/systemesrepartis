/** 
 * @file agrawala.c
 * @brief The Ricart and Agrawala algorithm implementation
 * @author Julien Dessaux & Jan Villeminot
 * @date 2009-01-17
 */

#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

#include "agrawala.h"


//! @brief The client's socket file descriptor
int s_ecoute;


int agrawala_init()
{
    struct sockaddr_in adr;
    socklen_t len;

    /* Initialisation du socket */
    s_ecoute = socket(AF_INET, SOCK_DGRAM, 0);
    adr.sin_family=AF_INET;
    adr.sin_port=htons(0);
    adr.sin_addr.s_addr=INADDR_ANY;

    /* Attempting to bind the UDP socket */
    if(bind(s_ecoute, (struct sockaddr *)&adr, sizeof(struct sockaddr_in)) != 0)
    {	/* Initialization error? */
	fprintf(stderr, "Socket could not be initialized");
	exit(1);
    }

    /* Return the port number we are binded on */
    getsockname(s_ecoute, (struct sockaddr *)&adr, &len);
    return adr.sin_port; /* TODO : this doesn't return the socket's port */
}

void agrawala_run()
{
}

void agrawala_close()
{
    shutdown(s_ecoute, SHUT_RDWR);
}

