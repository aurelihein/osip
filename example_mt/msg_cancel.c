/*
 FILE: message.c
 AUTHOR: AYMERIC MOIZARD
 features: declaration of the SIP message format
*/ 

#include <osip/smsg.h>
#include "example_rcfile.h"

int
makecancel(sip_t     *dest   ,from_t *from   ,to_t *to  ,url_t *rquri
	,call_id_t *callid ,cseq_t   *p_cseq )
{
  char *tmp;

  if (from==NULL)
    tmp = sgetcopy(getsipconf("from"));
  else
    from_2char(from, &tmp);
  msg_setfrom(dest, tmp);
  sfree(tmp);

  to_2char(to, &tmp);
  msg_setto(dest, tmp);
  sfree(tmp);

  /* create Request-URI like for invite */
  dest->strtline = (startline_t *) smalloc(sizeof(startline_t));
  dest->strtline->sipmethod = (char *)smalloc(14);
  sprintf(dest->strtline->sipmethod,"CANCEL");
  dest->strtline->sipversion = (char *) smalloc(8*sizeof(char));
  sprintf(dest->strtline->sipversion,"SIP/2.0");

  dest->strtline->statuscode   = NULL;
  dest->strtline->reasonphrase = NULL;

  url_init(&(dest->strtline->rquri));

  if (rquri==NULL) 
    url_2char(to->url, &tmp);
  else
    url_2char(rquri, &tmp);
  url_parse(dest->strtline->rquri, tmp);
  sfree(tmp);

  tmp = (char *)smalloc(60*sizeof(char));
  sprintf(tmp,"SIP/2.0/UDP %s:%s",getsipconf("localip"),getsipconf("localport"));
  msg_setvia(dest, tmp);
  sfree(tmp);
  
  call_id_2char(callid, &tmp);
  msg_setcall_id(dest, tmp);
  sfree(tmp);
  
  cseq_init(&(dest->cseq));
  cseq_setnumber(dest->cseq,sgetcopy(p_cseq->number));
  cseq_setmethod(dest->cseq,sgetcopy("CANCEL"));
  
  msg_setcontent_length(dest, "0");

  return 1;
}

