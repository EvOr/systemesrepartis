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


// --- Private structures ----------------------------------------------------
//! @brief An agrawala message
typedef struct message{
    //! @brief the type of the message
    int type;
    //! @brief The clock's value when the message is sent
    int clock;
    //! @brief The client id
    unsigned short client_id;
} message_t;


// --- Private variables -----------------------------------------------------
//! @brief The client's socket file descriptor
int s_ecoute = -1;
//! @brief The list of clients
request_t *clients = 0;
//! @brief The total number of clients
short nb_clients = 0;
//! @brief The client's clock
int clock = 0;
//! @brief The client's id
int client_id = 0;
//! @brief The tabular that contains the messages received from clients.
message_t *messages = 0;


// --- Prototypes ------------------------------------------------------------
void agrawala_main_loop();
void agrawala_init_algo();
void agrawala_show_status();
void agrawala_send_request();
void broadcast(message_t* msg);
void send_message(unsigned short client_id, message_t* msg);


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

    /* Finishing */
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
	exit(EXIT_FAILURE);
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
    if(messages)
	free(messages);
}


// --- Private functions -----------------------------------------------------
//! @brief The agrawala algorithm main loop
void agrawala_main_loop()
{
    agrawala_init_algo();

    agrawala_show_status();

    /* Testing */
    agrawala_send_request();
}


void agrawala_init_algo()
{
    message_t msg;	// the default message
    int i;	// counter

    /* Writing the default message : ack 0, i */
    msg.type = ACK;
    msg.clock = 0;

    /* Preparing the data structure that will handle the messages */
    messages = (message_t*) malloc(nb_clients * sizeof(message_t));
    if(!messages){
	fprintf(stderr, "\ndynamic memory allocation failed\n");
	exit(EXIT_FAILURE);
    }
    for(i=0; i<nb_clients; ++i){
	msg.client_id = clients[i].port;
	memcpy(&(messages[i]), &msg, sizeof(message_t));
	printf("%s : %d\n", clients[i].name, clients[i].port);
    }
}


void agrawala_show_status()
{
    int i;	// counter
    
    printf("\nCurrent status at time : %d\n", clock);
    for(i=0; i<nb_clients; ++i){
	printf("%8s - %5d : %s(%d, %d)\n", clients[i].name, clients[i].port, (messages[i].type == REQ ? "Req": "Ack"), messages[i].clock, messages[i].client_id);
    }
}


void coucou(){
    printf("coucou\n");
    fflush(stdout);
}


void agrawala_send_request()
{
    message_t msg; // The message to send

    /* Populating the structure */
    msg.type = REQ;
    msg.clock = ++clock;
    msg.client_id = client_id;

    /* Then we broadcast the message */
    broadcast(&msg);
}


void agrawala_send_ack(short client_id)
{
    message_t msg; // The message to send

    /* Populating the structure */
    msg.type = ACK;
    msg.clock = ++clock;
    msg.client_id = client_id;

    /* Then we broadcast the message */
    send_message(client_id, &msg);
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
	send_message(clients[i].port, msg);
    } 
}


//! @brief Envoie un message a un client
//! @param client a qui envoyer
//! @param message a envoyer
void send_message(unsigned short client_id, message_t* msg){
    int fd_socket;	// The socket's file descriptor
    struct sockaddr_in adr;	// The destination

    /* Preparation de la socket de communication */
    fd_socket = socket(AF_INET, SOCK_DGRAM, 0);
    /* Initialisation des champs de la structure */
    adr.sin_family=AF_INET;
    adr.sin_addr.s_addr=INADDR_ANY;
    adr.sin_port=htons(client_id);

    sendto(fd_socket, (char*) msg, sizeof(message_t), 0, (struct sockaddr *) &adr, sizeof(adr));
}

