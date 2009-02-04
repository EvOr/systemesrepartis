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
#include <string.h>
#include <sys/socket.h>

#include "agrawala.h"
#include "ricart_rpc.h"


//! @brief The client's socket file descriptor
int s_ecoute = -1;

//! @brief The list of clients
request_t *clients = 0;

//! @brief The total number of clients
short nb_clients = 0;


// --- Public functions ------------------------------------------------------
void agrawala_init(const int port)
{
    struct sockaddr_in adr;

    /* Initialisation du socket */
    s_ecoute = socket(AF_INET, SOCK_DGRAM, 0);
    adr.sin_family=AF_INET;
    adr.sin_port=htons(port);
    adr.sin_addr.s_addr=INADDR_ANY;

    /* Attempting to bind the UDP socket */
    if(bind(s_ecoute, (struct sockaddr *)&adr, sizeof(struct sockaddr_in)) != 0)
    {	/* Initialization error? */
	fprintf(stderr, "Socket could not be initialized");
	exit(1);
    }
}


void agrawala_run()
{
    struct sockaddr_in caller;	// A structure to handle the caller parameters
    char buffer[1024];	// A buffer to handle the socket response
    int received_bytes;	// The number of bytes received from the socket
    int struct_size;	// A int to retrieve the structure size from the recvfrom function

    /* Retrieving the results from the server */
    received_bytes = recvfrom(s_ecoute, buffer, sizeof(buffer), 0, (struct sockaddr *) &caller, (socklen_t*)&struct_size);
    memcpy((char *) &nb_clients, buffer, sizeof(short));
    clients = (request_t*) malloc(nb_clients * sizeof(request_t));
    if(!clients){
	fprintf(stderr, "\ndynamic memory allocation failed\n");
	exit(1);
    }
    memcpy((char*) clients, buffer + sizeof(short), nb_clients * sizeof(request_t));

    /* Entering the agrawala main loop */
    agrawala_main_loop();
}


void agrawala_close()
{
    if(s_ecoute != -1)
	shutdown(s_ecoute, SHUT_RDWR);
    if(clients)
        free(clients);
}


// --- Private functions -----------------------------------------------------
//! @brief The agrawala algorithm main loop
void agrawala_main_loop()
{
    int i;
    /* Testing */
    for(i=0; i<nb_clients; ++i)
	printf("%s : %d\n", clients[i].name, clients[i].port);
    printf("\n");
}


void agrawala_send_request()
{
}


void agrawala_send_ack(short port)
{
}


void agrawala_enter_critical_section()
{
}

