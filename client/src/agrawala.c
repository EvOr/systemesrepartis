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
//! @brief Clock of the client
int clock = 0;
//! @brief numero du client
int client_id;

/* Some prototypes */
void agrawala_main_loop();
void agrawala_send_request();
void broadcast(message_t* msg);
void send_message(request_t client, message_t* msg);


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

    client_id = port;
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
    agrawala_send_request();
}


void coucou(){
    printf("coucou\n");
    fflush(stdout);
}


void agrawala_send_request()
{
    //! @brief The message to send
    message_t msg;

    /* Populating the structure */
    msg.type = REQ;
    msg.clock = ++clock;
    msg.client = client_id;

    /* Then we broadcast the message */
    broadcast(&msg);
}


void agrawala_send_ack(short port)
{
}


void agrawala_enter_critical_section()
{
}


//! @brief Envoie un message a tout les clients
//! @param message Le message à envoyer
void broadcast(message_t* msg)
{
    int i=0;
    for(;i<nb_clients;i++){
	send_message(clients[i], msg);
    } 
}


//! @brief Envoie un message a un client
//! @param client a qui envoyer
//! @param message a envoyer
void send_message(request_t client, message_t* msg){
    int fd_socket;	// The socket's file descriptor
    struct sockaddr_in adr;	// The destination

    /* Preparation de la socket de communication */
    fd_socket = socket(AF_INET, SOCK_DGRAM, 0);
    /* Initialisation des champs de la structure */
    adr.sin_family=AF_INET;
    adr.sin_addr.s_addr=INADDR_ANY;
    adr.sin_port=htons(client.port);

    sendto(fd_socket, (char*) msg, sizeof(message_t), 0, (struct sockaddr *) &adr, sizeof(adr));
}

