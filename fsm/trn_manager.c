/*
  The oSIP library implements the Session Initiation Protocol (SIP -rfc2543-)
  Copyright (C) 2001  Aymeric MOIZARD jack@atosc.org
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <time.h>
#include <stdlib.h>
#include <signal.h>

#include <osip/osip.h>
#include <osip/port.h>
#include "fsm.h"


#ifdef OSIP_MT
static smutex_t *fastmutex;
#endif
int
osip_global_init()
{
  /* load the fsm configuration */
  fsm_load_uac4inv();
  fsm_load_uas4inv();
  fsm_load_uac4req();
  fsm_load_uas4req();

  /* load the parser configuration */
  parser_init();

  /* give a seed for all calls to rand() */
  srand(time(NULL));
  init_random_number();

#ifdef OSIP_MT
  fastmutex = smutex_init();  
#endif
  return 0;
}

int
transaction_init(osip_t *config,
	 transaction_t *dest,
	 to_t *to,
	 from_t *from,
	 call_id_t *callid,
	 cseq_t *cseq)
{
  static int current_transactionid = 0;

  char *tmp;
  int i;

  current_transactionid++;
  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL3,NULL,"Creating transaction with id = %i\n",current_transactionid));

  dest->your_instance=NULL; /* put whatever you want here */
  dest->config = config;
  dest->transactionid = current_transactionid;

  /* could be improved by selecting individually wich */
  /* transaction should go through a proxy and wich should not */
  dest->proxy = config->proxy; /* set to NULL if not used */

  i = from_2char(from, &tmp);
  if (i==-1) return -1;
  i = from_init(&(dest->from));
  if (i==-1) return -1;
  i = from_parse(dest->from, tmp);
  sfree(tmp);
  if (i==-1) return -1;

  i = to_2char(to, &tmp);
  if (i==-1) return -1;
  i = to_init(&(dest->to));
  if (i==-1) return -1;
  i = to_parse(dest->to, tmp);
  sfree(tmp);
  if (i==-1) return -1;

  i = call_id_2char(callid, &tmp);
  if (i==-1) return -1;
  i = call_id_init(&(dest->callid));
  if (i==-1) return -1;
  i = call_id_parse(dest->callid, tmp);
  sfree(tmp);
  if (i==-1) return -1;

  i = cseq_2char(cseq, &tmp);
  if (i==-1) return -1;
  i = cseq_init(&(dest->cseq));
  if (i==-1) return -1;
  i = cseq_parse(dest->cseq, tmp);
  sfree(tmp);
  if (i==-1) return -1;

  dest->state         = INITIAL;
  dest->statemachine  = NULL;
  dest->lastrequest   = NULL;
  dest->lastresponse  = NULL;
  dest->retransmissioncounter=0;

  dest->transactionff = (fifo_t *)smalloc(sizeof(fifo_t));
  fifo_init(dest->transactionff);

  dest->birth_time = time(NULL);
  dest->completed_time = 0;


#ifdef OSIP_MT
  smutex_lock(fastmutex);
#endif
  list_add(config->transactions,dest,-1);
#ifdef OSIP_MT
  smutex_unlock(fastmutex);
#endif
  return 0;
}

void
transaction_free(transaction_t *transaction)
{
  if (transaction==NULL)
    return ;

  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL3,stdout,"free transaction ressource %i\n",transaction->transactionid));
  
  /* first: delete transaction from active ones */
  osip_remove_byid(((osip_t *)transaction->config),
		       transaction->transactionid);
  
  /* do not delete value for proxy....  */
  /* it is shared by all transactions   */
  transaction->proxy = NULL;

  from_free(transaction->from);
  sfree(transaction->from);
  to_free(transaction->to);
  sfree(transaction->to);
  call_id_free(transaction->callid);
  sfree(transaction->callid);
  cseq_free(transaction->cseq);
  sfree(transaction->cseq);
  
  if (transaction->lastrequest != NULL)
    {
      sip_t *tmp;
      tmp = transaction->lastrequest;
      transaction->lastrequest=NULL;
      msg_free(tmp);
      sfree(tmp);
    }
  if (transaction->lastresponse != NULL)
    {
      msg_free(transaction->lastresponse);
      sfree(transaction->lastresponse);
     }
  if (transaction->transactionff!= NULL)
    {
      fifo_free(transaction->transactionff);
      sfree(transaction->transactionff);
    }

}

int
transaction_execute(transaction_t *transaction, sipevent_t *se)
{
  /* to kill the process, simply send this type of event. */
  if (EVT_IS_KILL_TRANSACTION(se))
      {
	sfree(se);
	return 0;
      }

  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL3,NULL,"sipevent se->transactionid: %i\n",se->transactionid));
  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL3,NULL,"sipevent tr->state: %i\n",transaction->state));
  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL3,NULL,"sipevent se->type: %i\n",se->type));
  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL3,NULL,"sipevent se->sip: %x\n",se->sip));

  
  /* statemachine is found automaticly... */
  if (transaction->statemachine==NULL)
    {
      if (EVT_IS_SND_INVITE(se))
	transaction->statemachine = fsm_getfsm_uac4inv();
      else
	transaction->statemachine = fsm_getfsm_uac4req();
    }
  
  if (-1==fsm_callmethod(se->type,transaction->state,
			 transaction->statemachine,
			 se,transaction ))
    {
      TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL3,NULL,"sipevent se: USELESS event!\n"));
      /* message is useless. */
      if(EVT_IS_MSG(se))
	{
	  if (se->sip!=NULL)
	    {
	      msg_free(se->sip);
	      sfree(se->sip);
	    }
	}
    }
  else
    {
      TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL3,NULL,"sipevent se: method called!\n"));
    }
  sfree(se); /* this is the ONLY place for freeing event!! */
  return 1;
}


transaction_t *
osip_find_asincomingmessage(osip_t *osip, sipevent_t *se)
{
  transaction_t *transaction;
  int            pos=0;


#ifdef OSIP_MT
  smutex_lock(fastmutex);
#endif
  while (!list_eol(osip->transactions,pos))
    {
      transaction = (transaction_t *)list_get(osip->transactions,pos);
      /* PSM 30/07/2001 */
      /* if (transaction->lastrequest!=NULL)
	 { */
      if (0==call_id_match(transaction->callid,
			   se->sip->call_id)
	  &&0==callleg_match(transaction->to,
			     transaction->from,
			     se->sip->to,se->sip->from)
	  &&0==cseq_match(transaction->cseq
			  ,se->sip->cseq))
	{
	if (0==strncmp(transaction->cseq->method,
		       se->sip->cseq->method,
		       strlen(transaction->cseq->method))
	    ||
	    ((0==strncmp(transaction->cseq->method,"INVITE",6)||
	      0==strncmp(transaction->cseq->method,"ACK",3))
	     &&0==strncmp(se->sip->cseq->method,"ACK",3)))
	  {
#ifdef OSIP_MT
	  smutex_unlock(fastmutex);
#endif
	  return transaction;
	  }
	}
      /* PSM 30/07/2001 } */
      pos++;
    }
   /*not found */
#ifdef OSIP_MT
  smutex_unlock(fastmutex);
#endif

  if (EVT_IS_RCV_STATUS_1XX(se)
      ||EVT_IS_RCV_STATUS_23456XX(se)
      ||EVT_IS_RCV_ACK(se))
    return NULL;

  transaction = (transaction_t *) smalloc(sizeof(transaction_t));
  {
    int i;
    i = transaction_init(osip,
			 transaction,
			 se->sip->to,
			 se->sip->from,
			 se->sip->call_id,
			 se->sip->cseq);
    
    if (i==-1)
      {
	sfree(transaction);
	return NULL;
      }
  }
  /* Only first requests are not part of a transaction */
  transaction->lastrequest = se->sip;

  if (MSG_IS_RESPONSEFOR(se->sip,"INVITE"))
    transaction->statemachine = fsm_getfsm_uas4inv();
  else 
    transaction->statemachine = fsm_getfsm_uas4req();

  se->transactionid = transaction->transactionid;

  return transaction;
}

/* return NULL when transaction does not exist */
transaction_t *
osip_find_asoutgoingmessage(osip_t *osip,sipevent_t *se)
{
  transaction_t *transaction;
  /* transaction MUST already exist in the list of transaction */

  transaction = osip_find_byid(osip, se->transactionid);
  se->transactionid = transaction->transactionid;

  return transaction;
}

transaction_t *
osip_find_byid(osip_t *osip, int transactionid)
{
  transaction_t *transaction;
  int            pos=0;
    
#ifdef OSIP_MT
  smutex_lock(fastmutex);
#endif  
  while (!list_eol(osip->transactions,pos))
    {
    transaction = (transaction_t *)list_get(osip->transactions,pos);
    
    if (transaction->transactionid==transactionid)
      {
#ifdef OSIP_MT
      smutex_unlock(fastmutex);
#endif
      return transaction;
      }
    pos++;
    }
  /*not found */ 
#ifdef OSIP_MT
  smutex_unlock(fastmutex);
#endif
  return NULL;
}

int
osip_remove_byid(osip_t *osip, int transactionid)
{
  transaction_t *transaction;
  int            pos=0;
  
#ifdef OSIP_MT
  smutex_lock(fastmutex);
#endif  
  while (!list_eol(osip->transactions,pos))
    {
    transaction = (transaction_t *)list_get(osip->transactions,pos);
    
    if (transaction->transactionid==transactionid)
      {
      list_remove(osip->transactions,pos);
#ifdef OSIP_MT
      smutex_unlock(fastmutex);
#endif
      return 0;
      }
    pos++;
    }
  /*not found */ 
  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,NULL,"Error while Deleting transaction ...\n"));

#ifdef OSIP_MT
  smutex_unlock(fastmutex);
#endif
  return -1;
}


int
call_id_match(call_id_t *callid1,call_id_t *callid2){

  if (callid1==NULL||callid2==NULL) return -1;
  if (callid1->number==NULL||callid2->number==NULL) return -1;

  if (0!=strcmp(callid1->number,callid2->number))
    return -1;

  if ((callid1->host==NULL)&&(callid2->host==NULL))
     return 0;
  if ((callid1->host==NULL)&&(callid2->host!=NULL))
     return -1;
  if ((callid1->host!=NULL)&&(callid2->host==NULL))
     return -1;
  if (0!=strcmp(callid1->host,callid2->host))
    return -1;

  return 0;
}

int
callleg_match(to_t *to1,from_t *from1,to_t *to2,from_t *from2)
{
  if (to1==NULL||to2==NULL) return -1;
  if (from1==NULL||from2==NULL) return -1;

  if (0==from_compare((from_t *)to1,(from_t *)to2)
      &&0==from_compare(from1,from2))
    return 0;
  return -1;
}

int
cseq_match(cseq_t *cseq1,cseq_t *cseq2)
{
  if (cseq1==NULL||cseq2==NULL) return -1;
  if (cseq1->number==NULL||cseq2->number==NULL
      ||cseq1->method==NULL||cseq2->method==NULL) return -1;

  if (0==strcmp(cseq1->number,cseq2->number))
    {
      if (strcmp(cseq1->method,"INVITE")||
	  strcmp(cseq1->method,"ACK"))
	return 0;
    }
  return -1;
}
