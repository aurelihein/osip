
#include "app.h"
#include "example_rcfile.h"
#include <osip/sdp.h>
#include <osip/sdp_negoc.h>

/* #include <unistd.h> */ 

extern osip_t *myconfig;

char *
getbodyfor200(sip_t *request)
{
  sdp_context_t *context;
  sdp_t *remote_sdp;
  sdp_t *local_sdp = NULL;
  int i;
  char *local_body;
  local_body = NULL;
  if (MSG_IS_INVITE(request))
    {
      body_t *body;
      body = (body_t *)list_get(request->bodies,0);
      
      /* remote_sdp = (sdp_t *) smalloc(sizeof(sdp_t)); */
      i = sdp_init(&remote_sdp);
      if (i!=0) return NULL;
      
      /* WE ASSUME IT IS A SDP BODY AND THAT    */
      /* IT IS THE ONLY ONE, OF COURSE, THIS IS */
      /* NOT TRUE */
      i = sdp_parse(remote_sdp,body->body);
      if (i!=0) return NULL;
      
      /* i = sdp_2char(remote_sdp, &astring);
	 printf("This is the SDP packet!\n%s\n",astring);
	 sfree(astring); */
      
      /*	i = sdp_replyto(remote_sdp,local_sdp,getsipconf("username"),getsipconf("networktype"),getsipconf("addr_type"),getsipconf("localip"));
		local_body = sdp_2char(local_sdp);
      */
      

      i = sdp_context_init(&context);
      if (i!=0)
	{
	  sdp_free(remote_sdp);
	  sfree(remote_sdp);
	  return NULL;
	}
      i = sdp_context_set_remote_sdp(context, remote_sdp);

      i = sdp_context_execute_negociation(context);
      if (i==200)
	{
	  local_sdp = sdp_context_get_local_sdp(context);
	  i = sdp_2char(local_sdp, &local_body);

	  sdp_context_free(context);
	  sfree(context);
	  if (i!=0) {
	    TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,NULL,"Error in local SDP answer %i\n",i));
	    return NULL;
	  }
	  return local_body;
	}
      else if (i==415)
	{
	  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,NULL,"Unsupported media %i\n",i));
	}
      else
	{
	  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,NULL,"Error while building answer to SDP (%i)\n",i));
	}
      sdp_context_free(context);
      sfree(context);
    } 
  return NULL;
}

/* fill default feilds for reply.                    */
/* OUTPUT: sip_t *dest | structure to store response.*/
/* INPUT : sip_t *request | previous request.        */
/* INPUT : int status | status for response.         */
/* INPUT : char *reason | reasonphrase for response. */
/* INPUT : char *body | body for response.           */
/* INPUT : int options | add a tag in to header.     */
/* return -1 on error */
/* TODO: add built-in support for record-route and route */
int
msg_makereply(int status,char *reason,sip_t *request,
	      sip_t *response,char *body,char *content_type
	      ,int options)
{
  int pos;
  int i;

  /* initialise sip_t structure */
  /* yet done... */

  response->strtline->sipversion = (char *)smalloc(8*sizeof(char));
  sprintf(response->strtline->sipversion,"SIP/2.0");
  response->strtline->statuscode = (char *)smalloc(5*sizeof(char));
  sprintf(response->strtline->statuscode,"%i",status);
  response->strtline->reasonphrase = (char *)smalloc(strlen(reason)+1);
  sprintf(response->strtline->reasonphrase,"%s",reason);

  response->strtline->rquri     = NULL;
  response->strtline->sipmethod = NULL;

  /* should add a tag in to feild (for useragent only) */
  /* add a boolean to reqest for it */
  {
    char *tmp;
    i = to_2char(request->to, &tmp);
    if (i==-1) return -1;
    i = msg_setto(response, tmp);
    sfree(tmp);
    if (i==-1) return -1;
    /*
      if (options==ADD_TAG)
      {
      response->to->tag = (char *)smalloc(12);
      sstrncpy(response->to->tag,"tag=faketag",11);
      }
    */
  }
  {
    char *tmp;
    i = from_2char(request->from, &tmp);
    if (i==-1) return -1;
    i = msg_setfrom(response, tmp);
    sfree(tmp);
    if (i==-1) return -1;
  }

  /* via headers */
  {
    char *tmp;
    pos = 0;
    while (!list_eol(request->vias,pos))
      {
	via_t *via;
	via = (via_t *)list_get(request->vias,pos);
	i = via_2char(via, &tmp);
	if (i==-1) return -1;
	i = msg_setvia(response, tmp);
	sfree(tmp);
	if (i==-1) return -1;
	pos++;
      }
  }

  {
    char *tmp;
    i = call_id_2char(request->call_id, &tmp);
    if (i==-1) return -1;
    i = msg_setcall_id(response, tmp);
    sfree(tmp);
    if (i==-1) return -1;
  }
  {
    char *tmp;
    i = cseq_2char(request->cseq, &tmp);
    if (i==-1) return -1;
    i = msg_setcseq(response, tmp);
    sfree(tmp);
    if (i==-1) return -1;
  }

   /* add personnal contact information (to be used for next request */
   /* (only for useragent) */
   if (body!=NULL)
     {
       char *size;
       i = msg_setbody(response, body);
       if (i==-1) return -1;

       size = (char *) smalloc(6*sizeof(char));
       sprintf(size,"%i",strlen(body));
       i = msg_setcontent_length(response, size);
       sfree(size);
       if (i==-1) return -1;

       {
	 char *ct = sgetcopy("content-type");
	 i = msg_setheader(response, ct, content_type);
	 sfree(ct);
	 if (i==-1) return -1;
       }
     }
   else
     {
       char *cl = sgetcopy("0");
       i = msg_setcontent_length(response, "0");
       sfree(cl);
       if (i==-1) return -1;
     }

   return 1;
}

int
osip_ul_sendmsg(transaction_t *transaction,sip_t *msg)
{
    sipevent_t *sipevent;
    
    sipevent = osip_new_outgoing_sipmessage(msg);
    sipevent->transactionid =  transaction->transactionid;

    uaapp_annouceoutrequest(sipevent->sip,transaction);
#ifdef OSIP_MT
    fifo_add(transaction->transactionff,sipevent);
#else
    transaction_execute(transaction,sipevent);
#endif

    return 0;
}

void
respondto200forinvite(transaction_t *tr, sip_t *response)
{
  sip_t *ack;

  msg_init(&ack);

  if (1==makeack(ack,response))
    {
      osip_ul_sendmsg(tr,ack);
    }
  else
    {
    TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,NULL,"Error while creating ACK request\n"));
    msg_free(ack);
    sfree(ack);
    }
}

void
respondtoinvite(transaction_t *tr, int code )
{
  respondtorequest(tr,code);
}

void
respondtorequest(transaction_t *tr, int code )
{
  char *reason;
  char *apptype;
  sip_t *response;
  char *local_body;

  msg_init(&response);
  reason = msg_getreason(code);
  
  if (code<200&&code!=183)
    local_body = NULL;
  else
    {
      if ( !MSG_IS_BYE(tr->lastrequest)
	   && !MSG_IS_REGISTER(tr->lastrequest)
	   && !MSG_IS_CANCEL(tr->lastrequest) )
	{
	  local_body =  getbodyfor200(tr->lastrequest);
	  if (local_body==NULL)
	    code = 415;
	  /* WE MUST ADD THE CORRESPONDING WARNING ERROR */
	  /* getWarningphrase(i); */
	}
      else
	{
	  local_body=NULL;
	}
    }
  
  if (local_body!=NULL) /* only sdp is allowed in the test app.... */
    apptype = sgetcopy("application/sdp");
  else
    apptype = NULL;

  if (1==msg_makereply(code,reason,
		       tr->lastrequest,
		       response,
		       local_body,
		       apptype,0)) 
    {
      msg_setcontact(response, getsipconf("contact"));
      osip_ul_sendmsg(tr,response);
    }
  else
    {
      if (local_body!=NULL)
	sfree(local_body);
      TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,NULL,"<example_agentlogic.c> Could not create response for current status\n"));
      sfree(response);
      sfree(reason);
      if (apptype!=NULL)
	sfree(apptype);
      return ;
    }
  if (local_body!=NULL)
    sfree(local_body);
  sfree(reason);
  if (apptype!=NULL)
    sfree(apptype);
}


int
sip_invite(session_t **session)
{
  static int only_once = 0;
#ifdef OSIP_MT
  transaction_mt_t *transaction_mt;
#endif
  sip_t         *invite;
  transaction_t *transaction;
  char *tmp;

  msg_init (&invite);


  {/* mandatory */
    to_2char((*session)->to, &tmp);
    msg_setto(invite,tmp);
    sfree(tmp);
  }

  /* this is used for subject and User-Agent header */
  if ((*session)->headers!=NULL)
    {
    int pos = 0;
    while (!list_eol((*session)->headers,pos))
      {
      header_t *header;
      header = (header_t *)list_get((*session)->headers,pos);
      msg_setheader(invite,header->hname,header->hvalue);
      pos++;
      }
    }
  /* do something for proxy ?!*/

  makeinvite(invite);

  if (only_once==0) {
    /* this is not used and not free() by any layer during test! */
    (*session)->callid = (call_id_t *)smalloc(sizeof(call_id_t));
    (*session)->callid->number = sgetcopy(invite->call_id->number);
    (*session)->callid->host   = sgetcopy(invite->call_id->host);
    
    (*session)->cseq = (cseq_t *)smalloc(sizeof(cseq_t));
    (*session)->cseq->number = sgetcopy(invite->cseq->number);
    (*session)->cseq->method = sgetcopy(invite->cseq->method);

    only_once = 1;
  }
  
  transaction = (transaction_t *) smalloc(sizeof(transaction_t));
  transaction_init(myconfig,
		   transaction,
		   invite->to,
		   invite->from,
		   invite->call_id,
		   invite->cseq);
#ifdef OSIP_MT
  transaction_mt_init(&transaction_mt);
  transaction_mt_set_transaction(transaction_mt,transaction);
  transaction_mt_start_transaction(transaction_mt);
#endif

  osip_ul_sendmsg(transaction,invite);  
  
  return transaction->transactionid;
}

int
sip_bye(session_t **session)
{
#ifdef OSIP_MT
  transaction_mt_t *transaction_mt;
#endif
  sip_t         *bye;
  transaction_t *transaction;

  msg_init (&bye);

  makebye(bye,NULL,(*session)->to,NULL
	  ,(*session)->callid,(*session)->cseq);
  fprintf(stdout,"BYE DONE: \n");


  transaction = (transaction_t *) smalloc(sizeof(transaction_t));
  transaction_init(myconfig,
		   transaction,
		   bye->to,
		   bye->from,
		   bye->call_id,
		   bye->cseq);
#ifdef OSIP_MT
  transaction_mt_init(&transaction_mt);
  transaction_mt_set_transaction(transaction_mt,transaction);
  transaction_mt_start_transaction(transaction_mt);
#endif

  osip_ul_sendmsg(transaction,bye);
  
  return transaction->transactionid;
}

void
sip_register(char *expires)
{
#ifdef OSIP_MT
  transaction_mt_t *transaction_mt;
#endif
  sip_t         *sregister;
  transaction_t *transaction;
  char *registrationnumber = NULL;
  static call_id_t *registrationcallid = NULL;
  static int regnumber = 1;
  if (registrationcallid==NULL)
    {
      registrationcallid = (call_id_t *) smalloc(sizeof(call_id_t));
      call_id_setnumber(registrationcallid,sgetcopy("1239874324324"));
      call_id_sethost(registrationcallid,sgetcopy(getsipconf("localip")));
    }
  regnumber++;
  registrationnumber = (char *)smalloc(7*sizeof(char));
  sprintf(registrationnumber,"%i",regnumber);
  
  msg_init (&sregister);
  makeregister(sregister,registrationcallid,registrationnumber,expires);

  sfree(registrationnumber);

  transaction = (transaction_t *) smalloc(sizeof(transaction_t));
  transaction_init(myconfig,
		   transaction,
		   sregister->to,
		   sregister->from,
		   sregister->call_id,
		   sregister->cseq);
#ifdef OSIP_MT
  transaction_mt_init(&transaction_mt);
  transaction_mt_set_transaction(transaction_mt,transaction);
  transaction_mt_start_transaction(transaction_mt);
#endif
  osip_ul_sendmsg(transaction,sregister);

  return ;
}
