
#ifndef ricart_h
#define ricart_h

#include <rpc/types.h>
#include <rpc/xdr.h>
#include <rpc/rpc.h>
#include <sys/types.h>
#include <sys/socket.h>

#define NUMERO_PROG 0x7044400
#define NUMERO_VERSION 1
#define REGISTER_NB 1
#define MAX_NAME_SIZE 8

/*
 * registerrpc(n°Prog, n°Version, n°proc, nom proc, nom param, nom return)
 *
 */
struct couple{
   int port;
   char nom[MAX_NAME_SIZE+1];
};
int xdr_couple();
char*register_on_server();

#endif
