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


//! @brief The client's socket file descriptor
int s_ecoute = -1;

//! @brief The list of clients
short * ports = NULL;

//! @brief The total number of clients
int nb_clients = 0;


//! @brief Init the agrawala algorithm processor
//! @param port The port we want to bind the socket on
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


void agrawala_send_request()
{
}


void agrawala_send_ack(short port)
{
}


void agrawala_enter_critical_section()
{
}


//! @brief The agrawala algorithm main loop
void agrawala_main_loop()
{
    /* Testing */
    for(i=0; i<nb_friends; ++i)
	printf("%d\t", ports[i]);
    printf("\n");
}


//! @brief Runs the agrawala algorithm
void agrawala_run()
{
    struct sockaddr_in caller;	// A structure to handle the caller parameters
    char buffer[1024];	// A buffer to handle the socket response
    int received_bytes;	// The number of bytes received from the socket
    int struct_size;	// A int to retrieve the structure size from the recvfrom function
    int nb_friends, i;

    /* Retrieving the results from the server */
    received_bytes = recvfrom(s_ecoute, buffer, sizeof(buffer), 0, (struct sockaddr *) &caller, (socklen_t*)&struct_size);
    ports = (short*) malloc(nb_friends * sizeof(short));
    if(!ports){
	fprintf(stderr, "\ndynamic memory allocation failed\n");
	exit(1);
    }
    nb_clients = received_bytes / 2 - 1;
    strncpy((char*) ports, buffer, nb_clients * 2);

    /* Entering the agrawala main loop */
    agrawala_main_loop();
}


//! @brief Properly terminate the agrawala processing
void agrawala_close()
{
    if(s_ecoute != -1)
	shutdown(s_ecoute, SHUT_RDWR);
    if(ports)
        free(ports);
}

