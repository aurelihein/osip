/*
 FILE: message.c
 AUTHOR: AYMERIC MOIZARD
 features: declaration of the SIP message format
*/ 

#include <osip/smsg.h>
#include "example_rcfile.h"

int
makeack(sip_t *dest,sip_t *response)
{
  char *tmp;

  from_2char(response->from, &tmp);
  msg_setfrom(dest, tmp);
  sfree(tmp);
  
  to_2char(response->to, &tmp);
  msg_setto(dest, tmp);
  sfree(tmp);


  /* create Request-URI like for invite */
  dest->strtline->sipmethod = (char *)smalloc(14);
  sprintf(dest->strtline->sipmethod,"ACK");
  dest->strtline->sipversion = (char *) smalloc(8*sizeof(char));
  sprintf(dest->strtline->sipversion,"SIP/2.0");

  dest->strtline->statuscode   = NULL;
  dest->strtline->reasonphrase = NULL;

  url_init(&(dest->strtline->rquri));


  url_2char(dest->to->url, &tmp);
  url_parse(dest->strtline->rquri, tmp);
  sfree(tmp);
  /* if some headers and params exist remove unwanted information if
     They do not appear on request URI */
  /* TODO...  */


    tmp = (char *)smalloc(60*sizeof(char));
    sprintf(tmp,"SIP/2.0/UDP %s:%s",getsipconf("localip"),getsipconf("localport"));
    msg_setvia(dest, tmp);
    sfree(tmp);

    call_id_2char(response->call_id, &tmp);
    msg_setcall_id(dest, tmp);
    sfree(tmp);


  /* MODIFICATION IS REQUIRED, IT MUST BE "number ACK" */
  cseq_init(&(dest->cseq));
  cseq_setnumber(dest->cseq,sgetcopy(response->cseq->number));
  cseq_setmethod(dest->cseq,sgetcopy("ACK"));
  
  msg_setcontact(dest, getsipconf("contact"));

  msg_setcontent_length(dest, "0");

  return 1;
}

