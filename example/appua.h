
#ifndef _APPUA_H_
#define _APPUA_H_

#include <osip/sip.h>

typedef struct _session_t {

  call_id_t *callid;
  cseq_t *cseq;

  from_t *from;
  to_t *to;
  int   transactionid;
  int   use_proxy;
  url_t *proxy;
  list_t *headers; /* list of header_t */
  contact_t *contact;

} session_t;

/* Those method names SHOULD be used to initiate event requesting */
/* new transactions. This could be extended on the same model     */
/* shown in "ualogic" directory */

int    sip_invite  (  session_t **session);
int    sip_bye     (  session_t **session);
void   sip_register(  char      *expires);

/******************************************* */
/* callback procedures you MUST implement    */
/* one exemple is provided in "ua",          */
/* "ualogic" and "uamess" directories        */
/* Those methods could be used to implement  */
/* any transactions types.                   */
/******************************************* */


/* this is a facility to annouce YOUR high-level application layer */
/* so it is aware of sessions */
void   uaapp_annouceincinvite(sip_t *invite,int transactionid);
void   uaapp_annouceincrequest(sip_t *request,int transactionid);
void   uaapp_annouceincack(sip_t *request,int transactionid);
void   uaapp_annouceincresponse(sip_t *response,int transactionid);
void   uaapp_annouceoutinvite(sip_t *invite,int transactionid);
void   uaapp_annouceoutrequest(sip_t *request,int transactionid);
void   uaapp_annouceoutresponse(sip_t *response,int transactionid);

/* these are the method's names you SHOULD use to create YOUR answers. */
/* You could extend it. */
void   respondtorequest(int tid,int code);
void   respondtoinvite (int tid,int code);

/* these are the method's names you SHOULD use to create YOUR requests */
/* An exemple is given in "uamess" */
/* But you SHOULD define your own way to create messages.*/

int    makeinvite  (sip_t *dest);
int    makeack     (sip_t *dest     ,sip_t    *response);
int    makeregister(sip_t *dest,call_id_t *rcallid,char *rnumber,char *expires);

/*int    makebye(sip_t     *dest   ,sip_t    *invite); */
int    makebye     (sip_t     *dest   ,from_t   *from    ,to_t *to
		    ,url_t    *rquri  ,call_id_t *callid  ,cseq_t *p_cseq );

#endif
