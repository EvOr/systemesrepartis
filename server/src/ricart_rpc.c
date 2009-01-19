/** 
 * @file ricart_rpc.c
 * @brief The RPC tools implementation
 * @author Julien Dessaux & Jan Villeminot
 * @date 2009-01-19
 */

#include "ricart_rpc.h"

int xdr_request(XDR *xdrp, request_t *p){
    return (xdr_int(xdrp, &p->port) &&
	    xdr_vector(xdrp, p->name, MAX_NAME_SIZE, sizeof(char), (xdrproc_t) xdr_char));
}

int xdr_response(XDR *xdrp, response_t *p){
    return (xdr_int(xdrp, &p->nb_clients) && xdr_int(xdrp, &p->port));
}

