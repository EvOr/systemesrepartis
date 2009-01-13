
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
 * registerrpc(n°Prog, n°Version, n°proc, name proc, name param, name return)
 */
//! @brief 
typedef struct request{
   char name[MAX_NAME_SIZE + 1];
   int port;
} request_t;

int xdr_request();

#endif
