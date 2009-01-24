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


//! @brief Display the Usage message
void usage()
{
    fprintf(stderr, "Usage : ./client <instance name> <port>\n    instance name : 8 characters maximum\n    port : port number <1 - 65535>\n");
}


//! @brief The main function of the program
//! @param argc The number of arguments entered on the command line
//! @param argv  A tabular containing the arguments entered on the command line
//! @return 0 if the program ended correctly, otherwise an errorcode
int main(int argc, char** argv){
    request_t request;	// Our request to the RPC server
    response_t response;	// The response from the RPC server
    int rpc_errno;

    /* Checking arguments and populating RPC request */
    if(argc<2 || strlen(argv[1]) > MAX_NAME_SIZE){
	usage();
	exit(1);
    }
    strcpy(request.name, argv[1]);

    request.port = atoi(argv[2]);
    if(request.port < 1 || request.port >= 65535){
	usage();
	exit(1);
    }

    /* Initializing Agrawala */
    agrawala_init(request.port);

    /* Contacting RPC server */
    rpc_errno = callrpc("localhost", (long) NUMERO_PROG, (long) NUMERO_VERSION, (long) REGISTER_NB,
	    (xdrproc_t) xdr_request, (char*) &request, (xdrproc_t) xdr_response, (char*) &response);
    if(rpc_errno != 0){
	fprintf(stderr, "Failed to contact RPC server\n"); 
	//        clnt_perrno(rpc_errno);
	agrawala_close();
	exit(1);
    }

    /* Handling response */
    printf("%d %d\n", response.nb_clients, response.port);
    if(response.nb_clients == 0){
	if(response.port == 100)
	    fprintf(stderr, "There are too many clients connected to the server.\n");
	else if(response.port == 200)
	    fprintf(stderr, "An invalid port number was provided.\n");
	else
	    fprintf(stderr, "An unknown error has occured\n");
	agrawala_close();
	exit(1);
    }

    /* Entering Agrawala main loop */
    agrawala_run();

    /* Terminating properly our work down there */
    agrawala_close();
    return 0;
}

