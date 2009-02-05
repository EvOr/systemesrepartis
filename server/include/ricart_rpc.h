/** 
 * @file ricart_rpc.h
 * @brief The RPC tools header
 * @author Julien Dessaux & Jan Villeminot
 * @date 2009-01-19
 */

#ifndef _RICART_H_
#define _RICART_H_

#include <rpc/types.h>
#include <rpc/xdr.h>
#include <rpc/rpc.h>
#include <sys/types.h>
#include <sys/socket.h>

#define NUMERO_PROG 0x7044400
#define NUMERO_VERSION 1
#define REGISTER_NB 1

#define MAX_NAME_SIZE 8

//! @brief The data structure for a request
typedef struct request{
    //! @brief A tabular for the client's name
    char name[MAX_NAME_SIZE + 1];
    //! @brief The UDP port the client is binded on
    unsigned short port;
} request_t;

//! @brief The function that will handle a request data structure
//! @param xdrp The XDR pointer
//! @param p A pointer on the data structure to handle
int xdr_request(XDR *xdrp, request_t *p);


//! @brief The data structure for a response
typedef struct response{
    //! @brief The number of clients that are ready, or 0 if an error occured
    int nb_clients;
    //! @brief The port provided by the client, or an error code (100: too many clients, 200: port number error)
    unsigned short port;
} response_t;

//! @brief The function that will handle a response data structure
//! @param xdrp The XDR pointer
//! @param p A pointer on the data structure to handle
int xdr_response(XDR *xdrp, response_t *p);

#endif

