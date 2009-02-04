/** 
 * @file main.c
 * @brief The server implementation for the Ricart and Agrawala algorithm
 * @author Julien Dessaux & Jan Villeminot
 * @date 2009-01-17
 */

#include <netdb.h>
#include <netinet/in.h>
#include <rpc/types.h>
#include <rpc/svc.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>

#include "ricart_rpc.h"


/* Some prototypes */
char* register_on_server(request_t*);
void advertise_all();

//! @brief How many clients we are connected to
short nb_clients = 0;
//! @brief Our clients' information
#define MAX_CLIENTS 3
request_t clients[MAX_CLIENTS];


//! @brief A function that finish cleaning in the case the program is badly interrupted
void clean_terminus()
{
    printf("Cleaning...\n");
    svc_unregister(NUMERO_PROG, NUMERO_VERSION);
    exit(1);
}


//! @brief main function
//! @param argc The number of arguments entered on the command line
//! @param argv A tabular of the arguments entered on the command line
//! @return An error code
int main(int argc, char** argv)
{
    char failed;	// rpc registered successfull?

    /* Arming SIG_TERM signal */
    signal(2, clean_terminus);

    /* Registering RPC server */
    printf("Registering RPC server...\n");
    failed = registerrpc(NUMERO_PROG, NUMERO_VERSION, REGISTER_NB, register_on_server, xdr_request, xdr_response);
    if(failed)
    {  /* Registering failed */
	fprintf(stderr, "Failed to register rpc server\n");
	exit(1);
    }

    /* Running RPC server */
    svc_run();

    /* Unregistering RPC server */
    svc_unregister(NUMERO_PROG, NUMERO_VERSION);

    return 0;
}


//! @brief rpc server core function
//! @param *p The argument received from the client
//! @return The result of the argument process
char* register_on_server(request_t *p)
{
    static response_t response;	// A structure that handle the response

    if(strlen(p->name) <= MAX_NAME_SIZE && p->port > 0 && p-> port < 65535)
    {  /* We received a valid connection attempt */
	if(nb_clients < MAX_CLIENTS)
	{  /* We can handle more clients */
	    printf("Received advertisement from client \"%s\" at port %d.\n", p->name, p->port);
	    /* Storing information received from client */
	    strcpy(clients[nb_clients].name, p->name);
	    clients[nb_clients].port = p->port;
	    /* compiling result */
	    response.nb_clients = ++nb_clients;
	    response.port = p->port;
	    /* was it the last client we were waiting for? */
	    if(nb_clients == MAX_CLIENTS)
		advertise_all();
	}
	else
	{  /* We can't handle any more client */
	    fprintf(stderr, "Cannot handle any more client : \"%s\" at port %d.\n", p->name, p->port);
	    response.nb_clients = 0;
	    response.port = 100;
	}
    }
    else
    {  /* We received an invalid connection attempt */
	fprintf(stderr, "Invalid arguments received from client \"%s\" at port %d.\n", p->name, p->port);
	response.nb_clients = 0;
	response.port = 200;
    }

    return ((char*)&response); 
}


//! @brief Advertise all clients on their UDP port that they can begin their stuff
void advertise_all()
{
    int fd_socket;	// The socket's file descriptor
    struct sockaddr_in adr;	// The destination
    char buff[sizeof(int) + MAX_CLIENTS * sizeof(request_t)];	// A buffer for the stuff we need to send
    int i;	// counter

    /* Preparation de la socket de communication */
    fd_socket = socket(AF_INET, SOCK_DGRAM, 0);
    /* Initialisation des champs de la structure */
    adr.sin_family=AF_INET;
    adr.sin_addr.s_addr=INADDR_ANY;

    /* Populating tab */
    memcpy(buff, (char*) &nb_clients, sizeof(short));
    memcpy(buff + sizeof(short), (char*) clients, sizeof(clients));
    /* sending the information to every client */
    for(i = 0; i < MAX_CLIENTS; ++i)
    {
	adr.sin_port=htons(clients[i].port);
	sendto(fd_socket, (char*) buff, sizeof(buff), 0, (struct sockaddr *) &adr, sizeof(adr));
    }

    /* closing socket */
    shutdown(fd_socket, SHUT_RDWR);
}

