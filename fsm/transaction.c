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

#include <osip/osip.h>
#include <osip/port.h>
#include "fsm.h"

int
transaction_init(transaction_t **transaction, context_type_t ctx_type,
		 osip_t *osip, sip_t *request)
{
  static int transactionid = 0;

  int i;
  time_t now;

  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL3,stdout,"INFO: allocating transaction ressource %i\n",transactionid));

  *transaction = (transaction_t *)smalloc(sizeof(transaction_t));
  if (*transaction==NULL) return -1;

  now = time(NULL);
#ifndef DISABLE_MEMSET
  memset(*transaction, 0, sizeof(transaction_t));
#else

  (*transaction)->your_instance = NULL;
  (*transaction)->last_response = NULL;
  (*transaction)->ack = NULL;
  (*transaction)->completed_time = 0;

  (*transaction)->in_socket = 0;
  (*transaction)->out_socket = 0;

#endif

  (*transaction)->birth_time = now;
  (*transaction)->transactionid = transactionid;
  transactionid++;

  i = transaction_set_from(*transaction, request->from);
  if (i!=0) goto ti_error_1;
  transaction_set_to(*transaction, request->to);
  if (i!=0) goto ti_error_2;
  transaction_set_call_id(*transaction, request->call_id);
  if (i!=0) goto ti_error_3;
  transaction_set_cseq(*transaction, request->cseq);
  if (i!=0) goto ti_error_4;
  (*transaction)->orig_request = request;

  (*transaction)->config = osip;

  (*transaction)->transactionff = (fifo_t*) smalloc(sizeof(fifo_t));
  if ((*transaction)->transactionff==NULL) goto ti_error_5;
  fifo_init((*transaction)->transactionff);

  (*transaction)->ctx_type = ctx_type;
  (*transaction)->ict_context = NULL;
  (*transaction)->ist_context = NULL;
  (*transaction)->nict_context = NULL;
  (*transaction)->nist_context = NULL;
  if (ctx_type==ICT)
    {
      (*transaction)->state = ICT_PRE_CALLING;
      i = ict_init(&((*transaction)->ict_context), osip, request);
      if (i!=0) goto ti_error_6;
      osip_add_ict(osip, *transaction);
    }
  else if  (ctx_type==IST)
    {
      (*transaction)->state = IST_PRE_PROCEEDING;
      i = ist_init(&((*transaction)->ist_context), osip, request);
      if (i!=0) goto ti_error_6;
      osip_add_ist(osip, *transaction);
    }
  else if  (ctx_type==NICT)
    {
      (*transaction)->state = NICT_PRE_TRYING;
      i = nict_init(&((*transaction)->nict_context), osip, request);
      if (i!=0) goto ti_error_6;
      osip_add_nict(osip, *transaction);
    }
  else
    {
      (*transaction)->state = NIST_PRE_TRYING;
      i = nist_init(&((*transaction)->nist_context), osip, request);
      if (i!=0) goto ti_error_6;
      osip_add_nist(osip, *transaction);
    }
  return 0;


 ti_error_6:
  fifo_free((*transaction)->transactionff);
  sfree((*transaction)->transactionff);
 ti_error_5:
  cseq_free((*transaction)->cseq);
  sfree((*transaction)->cseq);
 ti_error_4:
  call_id_free((*transaction)->callid);
  sfree((*transaction)->callid);
 ti_error_3:
  to_free((*transaction)->to);
  sfree((*transaction)->to);
 ti_error_2:
  from_free((*transaction)->from);
  sfree((*transaction)->from);
 ti_error_1:
  sfree(*transaction);
  return -1;
}

int
transaction_free(transaction_t *transaction)
{
  int i;
  if (transaction==NULL) return -1;
  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL3,stdout,"INFO: free transaction ressource %i\n",transaction->transactionid));
  if (transaction->ctx_type==ICT)
    {
      i = osip_remove_ict(transaction->config, transaction);
      ict_free(transaction->ict_context);
      sfree(transaction->ict_context);
    }
  else if (transaction->ctx_type==IST)
    {
      i = osip_remove_ist(transaction->config, transaction);
      ist_free(transaction->ist_context);
      sfree(transaction->ist_context);
    }
  else if (transaction->ctx_type==NICT)
    {
      i = osip_remove_nict(transaction->config, transaction);
      nict_free(transaction->nict_context);
      sfree(transaction->nict_context);
    }
  else
    {
      i = osip_remove_nist(transaction->config, transaction);
      nist_free(transaction->nist_context);
      sfree(transaction->nist_context);
    }

  if (i!=0) /* yet removed ??? */
    {
      TRACE(trace(__FILE__,__LINE__,
		  TRACE_LEVEL3,stdout,
		  "BUG: transaction already removed from list %i!!\n",
		  transaction->transactionid));
    }
  fifo_free(transaction->transactionff);
  sfree(transaction->transactionff);

  msg_free(transaction->orig_request);
  sfree(transaction->orig_request);
  msg_free(transaction->last_response);
  sfree(transaction->last_response);
  msg_free(transaction->ack);
  sfree(transaction->ack);

  from_free(transaction->from);
  sfree(transaction->from);
  to_free(transaction->to);
  sfree(transaction->to);
  call_id_free(transaction->callid);
  sfree(transaction->callid);
  cseq_free(transaction->cseq);
  sfree(transaction->cseq);

  return 0;
}

int
transaction_execute(transaction_t *transaction, sipevent_t *evt)
{
  statemachine_t *statemachine;
  /* to kill the process, simply send this type of event. */
  if (EVT_IS_KILL_TRANSACTION(evt))
      {
	transaction_free(transaction);
	sfree(transaction);
        sfree(evt);
        return 0;
      }

  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL3,NULL,"sipevent evt->transactionid: %i\n",evt->transactionid));
  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL3,NULL,"sipevent tr->state: %i\n",transaction->state));
  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL3,NULL,"sipevent evt->type: %i\n",evt->type));
  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL3,NULL,"sipevent evt->sip: %x\n",evt->sip));

  if (transaction->ctx_type==ICT)
    statemachine = ict_get_fsm();
  else if (transaction->ctx_type==IST)
    statemachine = ist_get_fsm();
  else if (transaction->ctx_type==NICT)
    statemachine = nict_get_fsm();
  else
    statemachine = nist_get_fsm();

  if (-1==fsm_callmethod(evt->type,
			 transaction->state,
                         statemachine,
                         evt,
			 transaction))
    {
      TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL3,NULL,
		  "INFO: USELESS event!\n"));
      /* message is useless. */
      if(EVT_IS_MSG(evt))
        {
          if (evt->sip!=NULL)
            {
              msg_free(evt->sip);
              sfree(evt->sip);
            }
        }
    }
  else
    {
      TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL3,NULL,"sipevent evt: method called!\n"));
    }
  sfree(evt); /* this is the ONLY place for freeing event!! */
  return 1;
}

int
transaction_set_your_instance(transaction_t *transaction, void *instance)
{
  if (transaction==NULL) return -1;
  transaction->your_instance = instance;
  return 0;
}

int
transaction_set_from(transaction_t *transaction, from_t *from)
{
  int i;
  if (transaction==NULL) return -1;
  i = from_clone(from, &(transaction->from));
  if (i==0) return 0;
  transaction->from = NULL;
  return -1;
}

int
transaction_set_to(transaction_t *transaction, to_t *to)
{
  int i;
  if (transaction==NULL) return -1;
  i = to_clone(to, &(transaction->to));
  if (i==0) return 0;
  transaction->to = NULL;
  return -1;
}

int
transaction_set_call_id(transaction_t *transaction, call_id_t *call_id)
{
  int i;
  if (transaction==NULL) return -1;
  i = call_id_clone(call_id, &(transaction->callid));
  if (i==0) return 0;
  transaction->callid = NULL;
  return -1;
}

int
transaction_set_cseq(transaction_t *transaction, cseq_t *cseq)
{
  int i;
  if (transaction==NULL) return -1;
  i = cseq_clone(cseq, &(transaction->cseq));
  if (i==0) return 0;
  transaction->cseq = NULL;
  return -1;
}

int
transaction_set_orig_request(transaction_t *transaction, sip_t *request)
{
  if (transaction==NULL) return -1;
  transaction->orig_request = request;
  return 0;
}

int
transaction_set_last_response(transaction_t *transaction, sip_t *last_response)
{
  if (transaction==NULL) return -1;
  transaction->last_response = last_response;
  return 0;
}

int
transaction_set_ack(transaction_t *transaction, sip_t *ack)
{
  if (transaction==NULL) return -1;
  transaction->ack = ack;
  return 0;
}

int
transaction_set_state(transaction_t *transaction, state_t state)
{
  if (transaction==NULL) return -1;
  transaction->state = state;
  return 0;
}

int
transaction_set_in_socket(transaction_t *transaction, int sock)
{
  if (transaction==NULL) return -1;
  transaction->in_socket = sock;
  return 0;
}

int
transaction_set_out_socket(transaction_t *transaction, int sock)
{
  if (transaction==NULL) return -1;
  transaction->out_socket = sock;
  return 0;
}

int
transaction_set_config(transaction_t *transaction, osip_t *osip)
{
  if (transaction==NULL) return -1;
  transaction->config = (void *)osip;
  return 0;
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
      if (0==strcmp(cseq2->method,"INVITE"))
	{
	  if (0==strcmp(cseq1->method,"INVITE")||
	      0==strcmp(cseq1->method,"ACK"))
	    return 0;
	}
      else
	{
	  if (0==strcmp(cseq1->method, cseq2->method))
	    return 0;
	}
    }
  return -1;
}
