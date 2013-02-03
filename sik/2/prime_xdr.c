#include <rpc/rpc.h>
#include "prime_xdr.h"

bool_t xdr_str(XDR *xdrs, char *str)
{
  if (!xdr_vector(xdrs, str, MAX_STR_LEN, sizeof(char), (xdrproc_t) xdr_char))
    return FALSE;
  return TRUE;
}

bool_t xdr_check_req(XDR *xdrs, char *chrq)
{
  if (!xdr_vector(xdrs, chrq, MAX_STR_LEN + sizeof(int), sizeof(char), (xdrproc_t) xdr_char))
    return FALSE;
  return TRUE;
}
