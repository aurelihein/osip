/*
 FILE: message.c
 AUTHOR: AYMERIC MOIZARD
 features: declaration of the SIP message format
*/ 

#include <osip/smsg.h>
#include "example_rcfile.h"


int
makeinvite(sip_t *dest)
{
  char *tmp;
  int i;
  /* To header MUST be set and headers must be */
  /* yet !!INITIALISED!!                       */

  dest->strtline->sipmethod = (char *)smalloc(14);
  sprintf(dest->strtline->sipmethod,"INVITE");
  dest->strtline->sipversion = (char *) smalloc(8*sizeof(char));
  sprintf(dest->strtline->sipversion,"SIP/2.0");

  dest->strtline->statuscode   = NULL;
  dest->strtline->reasonphrase = NULL;

  url_init(&(dest->strtline->rquri));
  {
    i = url_2char(dest->to->url, &tmp);
    i = url_parse(dest->strtline->rquri, tmp);
    sfree(tmp);
  }
  /* if some headers and params exist remove unwanted information if
     They do not appear on request URI */
  /* TODO...  */

  /* if a proxy is given use it.... */
    /* TO BE DONE
  if (strcmp(getsipconf("useproxy"),"yes"))
    dest->startline_t= dest->to->username@proxy.net
    dest->startline_t->urlparam=NULL
    */

  msg_setfrom(dest, getsipconf("from"));

  {
    char *via = (char *)smalloc(60*sizeof(char));
    sprintf(via,"SIP/2.0/UDP %s:%s",getsipconf("localip"),getsipconf("localport"));
    msg_setvia(dest, via);
    sfree(via);
  }

  {
    char *callid = (char *)smalloc(60*sizeof(char));
    unsigned int number = new_random_number();
    sprintf(callid,"%u@%s",number,dest->from->url->host);
    msg_setcall_id(dest, callid);
    sfree(callid);
  }

  {
    char *cseq = (char *)smalloc(10*sizeof(char));
    sprintf(cseq,"1 INVITE");
    msg_setcseq(dest, cseq);
    sfree(cseq);
  }

  msg_setcontact(dest,getsipconf("contact"));

  {
    char   *ctype;
    char   *ctypev;
    ctype  = sgetcopy("content-type\0");
    ctypev = sgetcopy("application/sdp\0");
    msg_setheader(dest, ctype, ctypev);
    sfree(ctype);
    sfree(ctypev);
  }

  /* given by user layer */
  /* setheader(dest,"subject","Are you there?"); */

  /* support only ONE payload... */
  {
    char *body = (char *)smalloc(strlen("v=0\r\no=- 1890408335 1890408335 IN IP4 130.10.11.147\r\ns=session\r\nc=IN IP4 130.10.11.147\r\nt=3185086562 0\r\nm=audio 10060 RTP/AVP 0\r\na=rtpmap:0 PCMU/8000\r\nm=application 10050 UDP wb\r\na=orient:portrait\r\na=recvonly\r\n")+1);
    char *size= (char *)smalloc(5*sizeof(char));
    sprintf(body,"v=0\r\no=- 1890408335 1890408335 IN IP4 130.10.11.147\r\ns=session\r\nc=IN IP4 130.10.11.147\r\nt=3185086562 0\r\nm=audio 10060 RTP/AVP 0\r\na=rtpmap:0 PCMU/8000\r\nm=application 10050 UDP wb\r\na=orient:portrait\r\na=recvonly\r\n");
    /* sprintf(body,"v=0\r\no=- 1890408335 1890408335 IN IP4 130.10.11.147\r\ns=session\r\nc=IN IP4 130.10.11.147\r\nt=3185086562 0\r\nm=audio 10060 RTP/AVP 0\r\na=rtpmap:0 PCMU/8000\r\n"); */
    msg_setbody(dest, body);
    sprintf(size,"%i",strlen(body));
    sfree(body);
    msg_setcontent_length(dest, size);
    sfree(size);
  }

  return 1;
}
