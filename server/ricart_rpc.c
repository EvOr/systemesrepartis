#include "ricart_rpc.h"

int xdr_request(XDR *xdrp, request_t *p){
   return(xdr_int(xdrp,&p->port) && xdr_vector(xdrp, p->name,MAX_NAME_SIZE,
   					sizeof(char),(xdrproc_t) xdr_char));
}
