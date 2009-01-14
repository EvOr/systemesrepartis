
#include <rpc/types.h>
#include <rpc/xdr.h>
#include <rpc/rpc.h>
#include <signal.h>
#include <stdio.h>

#include "ricart_rpc.h"

#define MAX_CLIENTS 3
char* register_on_server(request_t*);

//! @brief How many clients we are connected to
int nb_clients = 0;
//! @brief Our clients' information
request_t clients[MAX_CLIENTS];

void clean_terminus(void)
{
    printf("Terminate!!!\n");
   svc_unregister(NUMERO_PROG, NUMERO_VERSION);
    exit(1);
}

//! @brief main function
//! @param argc The number of arguments entered on the command line
//! @param argv A tabular of the arguments entered on the command line
//! @return An error code
int main(int argc, char** argv){
   char failed;	// rpc registered successfull?
   signal(2, clean_terminus);

   printf("Registering rpc server...\n");
   failed = registerrpc(NUMERO_PROG, NUMERO_VERSION, REGISTER_NB, register_on_server, xdr_request, xdr_request);
   if(failed)
   {  /* Registering failed */
      fprintf(stderr, "Failed to register rpc server\n");
      exit(1);
   }
   svc_run();

   svc_unregister(NUMERO_PROG, NUMERO_VERSION);
   return 0;
}

//! @brief rpc server core function
//! @param *p The argument received from the client
//! @return The result of the argument process
char* register_on_server(request_t *p){
   static request_t res;	// A structure that handle the result

   if(nb_clients < MAX_CLIENTS)
   {  /* We can handle more clients */
      if(strlen(p->name) <= MAX_NAME_SIZE && p->port > 0 && p-> port < 65535)
      {  /* We received a valid connection attempt */
	 printf("Received advertisement from client \"%s\" at port %d.\n", p->name, p->port);
	 ++nb_clients;
	 /* Storing information received from client */
	 strcpy(clients[nb_clients].name, p->name);
	 clients[nb_clients].port=p->port;
	 /* compiling result */
	 strcpy(res.name, p->name);
	 res.port=0;
      }
      else
      {  /* We received an invalid connection attempt */
      }
      fprintf(stderr, "Invalid arguments received from client \"%s\" at port %d.\n", p->name, p->port);
   }
   else
   {  /* We can't take any more client */
      res.port=100;
      res.name[0]='0';
   }
   return ((char*)&res); 
}

