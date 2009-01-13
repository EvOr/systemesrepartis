#include "ricart_rpc.h"

int xdr_couple(XDR *xdrp, struct couple *p){
   return(xdr_int(xdrp,&p->port) && xdr_vector(xdrp, p->nom,MAX_NAME_SIZE,
   					sizeof(char),(xdrproc_t) xdr_char));
}
