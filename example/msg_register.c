/*
 FILE: register.c
 AUTHOR: AYMERIC MOIZARD
 features: 
*/ 


#include <osip/port.h>

#include <osip/smsg.h>
#include "example_rcfile.h"

int
makeregister(sip_t *dest, call_id_t *rcallid,char *rnumber,char *expires)
{
  char *tmp;

  msg_setfrom(dest,getsipconf("from"));
  msg_setto(dest, getsipconf("from"));
  { /* remove the tag parameter in the to feild */
    int pos=0;
    while (!list_eol(dest->to->gen_params,pos))
      {
	generic_param_t *gp=(generic_param_t *)list_get(dest->to->gen_params,pos);
#ifndef WIN32
	if (0==strcasecmp("tag",gp->gname))
#else
	  if (0==stricmp("tag",gp->gname))
#endif
	    {
	      list_remove(dest->to->gen_params,pos);
	      generic_param_free(gp);
	      sfree(gp);
	    }
	pos++;
      }
  }
  /* create Request-URI like for invite */
  dest->strtline->sipmethod = (char *)smalloc(14);
  sprintf(dest->strtline->sipmethod,"REGISTER");
  dest->strtline->sipversion = (char *) smalloc(8*sizeof(char));
  sprintf(dest->strtline->sipversion,"SIP/2.0");
  
  dest->strtline->statuscode   = NULL;
  dest->strtline->reasonphrase = NULL;
  
  url_init(&(dest->strtline->rquri));
  
  url_2char(dest->to->url, &tmp);

  url_parse(dest->strtline->rquri, getsipconf("sipregistrar"));
  sfree(tmp);
  /* if some headers and params exist remove unwanted information if
     They do not appear on request URI */
  /* TODO...  */
  
  
  tmp = (char *)smalloc(60*sizeof(char));
  sprintf(tmp, "SIP/2.0/UDP %s:%s", getsipconf("localip")
	  ,getsipconf("localport"));
  msg_setvia(dest, tmp);
  sfree(tmp);

  call_id_2char(rcallid, &tmp);
  msg_setcall_id(dest, tmp);
  sfree(tmp);
  
  cseq_init(&(dest->cseq));
  cseq_setnumber(dest->cseq,sgetcopy(rnumber));
  tmp = sgetcopy("REGISTER");
  cseq_setmethod(dest->cseq,tmp);
  
  msg_setheader(dest, "expires", expires);
  msg_setcontact(dest, getsipconf("contact"));
  
  msg_setcontent_length(dest,"0");
  
  return 1;
}
