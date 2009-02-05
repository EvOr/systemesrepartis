/** 
 * @file agrawala.c
 * @brief The Ricart and Agrawala algorithm implementation
 * @author Julien Dessaux & Jan Villeminot
 * @date 2009-01-17
 */

#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/ttydefaults.h>
#include <termios.h>
#include <unistd.h>

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
//! @brief The client's index in the clients list
int client_index = -1;
//! @brief The tabular that contains the messages received from clients.
message_t *messages = 0;
//! @brief The file descriptor set for multiplexing IOs
fd_set rfsd;
//! @brief A queue for ack messages
int *queue = 0;
//! @brief The number of messages in queue
int nb_messages_in_queue = 0;

// --- Prototypes ------------------------------------------------------------
void agrawala_main_loop();
void agrawala_init_algo();
void agrawala_show_status();
void agrawala_handle_packet();
void agrawala_send_request();
void agrawala_send_ack(short client_id);
void agrawala_enter_critical_section();
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
    if(queue)
	free(queue);
}


// --- Private functions -----------------------------------------------------
//! @brief The agrawala algorithm main loop
void agrawala_main_loop()
{
    int retval;	// handles the return value from select function
    char key;	// The key entered on the keyboard
    char done = 0;	// handles the end of the main loop

    //    struct termios term1, term2;

    //    int flag = fcntl(STDIN_FILENO, F_GETFL);
    //    flag |= O_NONBLOCK;
    //    fcntl(STDIN_FILENO, F_SETFL, flag);

    //    ioctl(STDIN_FILENO, TCGETS, (unsigned long*) &term1);
    //    ioctl(STDIN_FILENO, TCGETS, (unsigned long*) &term2);
    //    term1.c_lflag = term1.c_lflag | ~ICANON;
    //    ioctl(STDIN_FILENO, TCSETS, (unsigned long*) &term1);

    agrawala_init_algo();
    agrawala_show_status();

    printf("Valid commands are :\n    - e : request an enter in critical section\n    - s : show current status\n    - f : exit\n");

    while(!done){
	//	agrawala_show_status();

	/* Prepare the IO multiplexing system */
	FD_ZERO(&rfsd);
	FD_SET(STDIN_FILENO, &rfsd);
	FD_SET(s_ecoute, &rfsd);
	/* waits for IOs */	
	retval = select(s_ecoute + 1, &rfsd, NULL, NULL, NULL);

	if(FD_ISSET(STDIN_FILENO, &rfsd)){
	    read(0, &key, 1);
	    fflush(stdin);	// We want to avoid being anoyed by a psychotic user
	    switch(key){
		case 'e':
		    agrawala_send_request();
		    break;
		case 'f':
		    done = 1;
		    break;
		case 's':
		    agrawala_show_status();
		    break;
	    }
	}
	else if(FD_ISSET(s_ecoute, &rfsd)){
	    agrawala_handle_packet();
	}
	else{
	    fprintf(stderr, "unhandled result from the select function!\n");
	}
    }

    //    ioctl(STDIN_FILENO, TCSETS, (unsigned long*) &term2);

    /* Testing */
    agrawala_send_request();
}


//! @brief Executes the correct initializations for the agrawala algorithm
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
	msg.client_id = i;
	memcpy(&(messages[i]), &msg, sizeof(message_t));
	printf("%s : %d\n", clients[i].name, clients[i].port);
    }

    /* Preparing the message queue */
    queue = (int*) malloc(nb_clients * sizeof(int));

    /* Find our index in the clients list */
    for(i=0; i<nb_clients; ++i){
	if(clients[i].port == client_id){
	    client_index = i;
	    break;
	}
    }
}


//! @brief Prints the last messages received from each client
void agrawala_show_status()
{
    int i;	// counter

    printf("Current status at time : %d\n", clock);
    for(i=0; i<nb_clients; ++i){
	printf("%8s - %5d : %s(%d, %d)\n", clients[i].name, clients[i].port, (messages[i].type == REQ ? "Req": "Ack"), messages[i].clock, messages[i].client_id + 1);
    }
}


//! @brief Handle the reception of an agrawala message, an determine the action that has to be performed
void agrawala_handle_packet()
{
    struct sockaddr_in caller;	// A structure to handle the caller parameters
    char buffer[1024];	// A buffer to handle the socket response
    int received_bytes;	// The number of bytes received from the socket
    int struct_size;	// A int to retrieve the structure size from the recvfrom function
    message_t* him;	// the message we received
    message_t* me = &(messages[client_index]);	// our last message

    /* Retrieving the results from the sender */
    received_bytes = recvfrom(s_ecoute, buffer, sizeof(buffer), 0, (struct sockaddr *) &caller, (socklen_t*)&struct_size);
    him = (message_t*) buffer;
    memcpy(&(messages[him->client_id]), him, sizeof(message_t));

    printf("--- Received message : %s(%d, %d)\n", (him->type == REQ ? "Req": "Ack"), him->clock, him->client_id + 1);

    if(him->client_id == client_index){
	/* This message is from ourself, so we are fine and we just check if we can enter the critical section */
	if(him->type == REQ){
	    printf("--- Stacking Ack message for client %d\n", him->client_id + 1);
	    queue[nb_messages_in_queue++] = him->client_id;
	}
	agrawala_enter_critical_section();
    }
    else{
	/* the message is from someone else, we need to process it */
	if(him->clock > clock)
	    clock = him->clock + 1;
	else
	    ++clock;

	if(him->type == REQ){
	    /* the message we received is a request, so we prepare a ack */
	    if(me->type == REQ){
		/* we have also sent a request */
		if(me->clock < him->clock || (me->clock == him->clock && me->client_id < him->client_id)){
		    /* our request precedes, we add the ack to the queue and we check if we can enter the critical section */
	            printf("--- Stacking Ack message for client %d\n", him->client_id + 1);
		    queue[nb_messages_in_queue++] = him->client_id;
		}
		else{
		    /* our request does not precede, so we ack */
		    agrawala_send_ack(clients[him->client_id].port);
		}
	    }
	    else{
		/* we are not requesting for now, so we ack */
		agrawala_send_ack(clients[him->client_id].port);
	    }
	}
	else if(him->type == ACK){
	    /* the message we received is a ack, so we check if we can enter the critical section */
	    agrawala_enter_critical_section();
	}
    }
}


//! @brief Sends an agrawala request message
void agrawala_send_request()
{
    message_t msg; // The message to send

    if(messages[client_index].type != REQ){
	/* Populating the structure */
	msg.type = REQ;
	msg.clock = ++clock;
	msg.client_id = client_index;

	/* Then we broadcast the message */
	broadcast(&msg);
    }
    else{
	fprintf(stderr, "You cannot make another request right now.\n");
    }
}


//! @brief Sends an agrawala ack message
void agrawala_send_ack(short client_id)
{
    message_t msg; // The message to send

    /* Populating the structure */
    msg.type = ACK;
    msg.clock = ++clock;
    msg.client_id = client_index;

    /* Then we broadcast the message */
    printf("+++ Sent message : %s(%d, %d)\n", (msg.type == REQ ? "Req": "Ack"), msg.clock, msg.client_id + 1);
    send_message(client_id, &msg);
}


//! @brief Enters in critical section if necessary
void agrawala_enter_critical_section()
{
    int min_index = 0;	// the message with the minimal clock
    int i;	// counter

    /* First of all, we determine if we are supposed to enter critical section */
    for(i=1; i < nb_clients; ++i){
	if(messages[i].clock < messages[min_index].clock)
	    min_index = i;
    }

    /* Then we deal with it */
    if(messages[client_index].type == REQ && min_index == client_index){
	printf("Entering critical section.\n");
	/* We print one . every second */
	for(i=0; i < 10; ++i){
	    sleep(1);
	    printf(".");
	    fflush(stdout);
	}
	printf("\nLeaving critical section.\n");

	/* Avoid an infinite loop */
	messages[client_index].type = ACK;

	/* Flushing the message queue */
	for(i=0; i < nb_messages_in_queue; ++i){
	    agrawala_send_ack(clients[queue[i]].port);
	}
    }
}


//! @brief Broadcasts an agrawala message to each client
//! @param message Le message à envoyer
void broadcast(message_t* msg)
{
    int i;	// counter

    printf("+++ Broadcasted message : %s(%d, %d)\n", (msg->type == REQ ? "Req": "Ack"), msg->clock, msg->client_id + 1);
    for(i=0; i < nb_clients; ++i){
	send_message(clients[i].port, msg);
    } 
}


//! @brief Sends a message to a client
//! @param client The client to send to
//! @param message The message that is to be sent
void send_message(unsigned short client_id, message_t* msg){
    struct sockaddr_in adr;	// The destination

    /* Initialisation des champs de la structure */
    adr.sin_family=AF_INET;
    adr.sin_addr.s_addr=INADDR_ANY;
    adr.sin_port=htons(client_id);

    /* Sending message */
    sendto(s_ecoute, (char*) msg, sizeof(message_t), 0, (struct sockaddr *) &adr, sizeof(adr));
}

