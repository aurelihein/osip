/*
  The oSIP library implements the Session Initiation Protocol (SIP -rfc3261-)
  Copyright (C) 2001,2002,2003  Aymeric MOIZARD jack@atosc.org
  
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

#include <osip/port.h>
#include <osip/osip.h>

#include "fsm.h"

int
transaction_init (transaction_t ** transaction, context_type_t ctx_type,
		  osip_t * osip, sip_t * request)
{
  static int transactionid = 1;
  via_t *topvia;

  int i;
  time_t now;

  OSIP_TRACE (osip_trace
	      (__FILE__, __LINE__, OSIP_INFO2, NULL,
	       "allocating transaction ressource %i %s\n", transactionid,
	       request->call_id->number));

  *transaction = (transaction_t *) smalloc (sizeof (transaction_t));
  if (*transaction == NULL)
    return -1;

  now = time (NULL);
#ifndef DISABLE_MEMSET
  memset (*transaction, 0, sizeof (transaction_t));
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

  topvia = list_get (request->vias, 0);
  if (topvia == NULL)
    goto ti_error_1;

  i = transaction_set_topvia (*transaction, topvia);
  if (i != 0)
    goto ti_error_1;

  /* In some situation, some of those informtions might
     be useless. Mostly, I prefer to keep them in all case
     for backward compatibility. */
  i = transaction_set_from (*transaction, request->from);
  if (i != 0)
    goto ti_error_2;
  i = transaction_set_to (*transaction, request->to);
  if (i != 0)
    goto ti_error_3;
  i = transaction_set_call_id (*transaction, request->call_id);
  if (i != 0)
    goto ti_error_4;
  i = transaction_set_cseq (*transaction, request->cseq);
  if (i != 0)
    goto ti_error_5;
  /* RACE conditions can happen for server transactions */
  /* (*transaction)->orig_request = request; */
  (*transaction)->orig_request = NULL;

  (*transaction)->config = osip;

  (*transaction)->transactionff = (fifo_t *) smalloc (sizeof (fifo_t));
  if ((*transaction)->transactionff == NULL)
    goto ti_error_6;
  fifo_init ((*transaction)->transactionff);

  (*transaction)->ctx_type = ctx_type;
  (*transaction)->ict_context = NULL;
  (*transaction)->ist_context = NULL;
  (*transaction)->nict_context = NULL;
  (*transaction)->nist_context = NULL;
  if (ctx_type == ICT)
    {
      (*transaction)->state = ICT_PRE_CALLING;
      i = ict_init (&((*transaction)->ict_context), osip, request);
      if (i != 0)
	goto ti_error_7;
      osip_add_ict (osip, *transaction);
    }
  else if (ctx_type == IST)
    {
      (*transaction)->state = IST_PRE_PROCEEDING;
      i = ist_init (&((*transaction)->ist_context), osip, request);
      if (i != 0)
	goto ti_error_7;
      osip_add_ist (osip, *transaction);
    }
  else if (ctx_type == NICT)
    {
      (*transaction)->state = NICT_PRE_TRYING;
      i = nict_init (&((*transaction)->nict_context), osip, request);
      if (i != 0)
	goto ti_error_7;
      osip_add_nict (osip, *transaction);
    }
  else
    {
      (*transaction)->state = NIST_PRE_TRYING;
      i = nist_init (&((*transaction)->nist_context), osip, request);
      if (i != 0)
	goto ti_error_7;
      osip_add_nist (osip, *transaction);
    }
  return 0;


ti_error_7:
  fifo_free ((*transaction)->transactionff);
  sfree ((*transaction)->transactionff);
ti_error_6:
  cseq_free ((*transaction)->cseq);
  sfree ((*transaction)->cseq);
ti_error_5:
  call_id_free ((*transaction)->callid);
  sfree ((*transaction)->callid);
ti_error_4:
  to_free ((*transaction)->to);
  sfree ((*transaction)->to);
ti_error_3:
  from_free ((*transaction)->from);
  sfree ((*transaction)->from);
ti_error_2:
  via_free ((*transaction)->topvia);
  sfree ((*transaction)->topvia);
ti_error_1:
  sfree (*transaction);
  return -1;
}

/* This method automaticly remove the transaction context from
   the osip stack. This task is required for proper operation
   when a transaction goes in the TERMINATED STATE.
   However the user might want to just take the context out of
   the SIP stack andf keep it for future use without freeing
   all ressource.... This way the transaction context can be
   kept without being used by the oSIP stack.

   new methods that replace this one:
   osip_remove_ict
   osip_remove_nict
   osip_remove_ist
   osip_remove_nist
   +
   transaction_free2();

 */
int
transaction_free (transaction_t * transaction)
{
  sipevent_t *evt;
  int i;

  if (transaction == NULL)
    return -1;
  if (transaction->orig_request != NULL)
    {
      OSIP_TRACE (osip_trace
		  (__FILE__, __LINE__, OSIP_INFO2, NULL,
		   "free transaction ressource %i %s\n",
		   transaction->transactionid,
		   transaction->orig_request->call_id->number));
    }
  if (transaction->ctx_type == ICT)
    {
      i = osip_remove_ict (transaction->config, transaction);
      ict_free (transaction->ict_context);
      sfree (transaction->ict_context);
    }
  else if (transaction->ctx_type == IST)
    {
      i = osip_remove_ist (transaction->config, transaction);
      ist_free (transaction->ist_context);
      sfree (transaction->ist_context);
    }
  else if (transaction->ctx_type == NICT)
    {
      i = osip_remove_nict (transaction->config, transaction);
      nict_free (transaction->nict_context);
      sfree (transaction->nict_context);
    }
  else
    {
      i = osip_remove_nist (transaction->config, transaction);
      nist_free (transaction->nist_context);
      sfree (transaction->nist_context);
    }

  if (i != 0)			/* yet removed ??? */
    {
      OSIP_TRACE (osip_trace (__FILE__, __LINE__, OSIP_BUG, NULL,
			      "transaction already removed from list %i!\n",
			      transaction->transactionid));
    }

  /* empty the fifo */
  evt = fifo_tryget (transaction->transactionff);
  while (evt != NULL)
    {
      msg_free (evt->sip);
      sfree (evt->sip);
      sfree (evt);
      evt = fifo_tryget (transaction->transactionff);
    }
  fifo_free (transaction->transactionff);
  sfree (transaction->transactionff);

  msg_free (transaction->orig_request);
  sfree (transaction->orig_request);
  msg_free (transaction->last_response);
  sfree (transaction->last_response);
  msg_free (transaction->ack);
  sfree (transaction->ack);

  via_free (transaction->topvia);
  sfree (transaction->topvia);
  from_free (transaction->from);
  sfree (transaction->from);
  to_free (transaction->to);
  sfree (transaction->to);
  call_id_free (transaction->callid);
  sfree (transaction->callid);
  cseq_free (transaction->cseq);
  sfree (transaction->cseq);

  return 0;
}

/* same as transaction_free() but assume the transaction is
   already removed from the list of transaction in the osip stack */
int
transaction_free2 (transaction_t * transaction)
{
  sipevent_t *evt;

  if (transaction == NULL)
    return -1;
  if (transaction->orig_request != NULL)
    {
      OSIP_TRACE (osip_trace
		  (__FILE__, __LINE__, OSIP_INFO2, NULL,
		   "free transaction ressource %i %s\n",
		   transaction->transactionid,
		   transaction->orig_request->call_id->number));
    }
  if (transaction->ctx_type == ICT)
    {
      ict_free (transaction->ict_context);
      sfree (transaction->ict_context);
    }
  else if (transaction->ctx_type == IST)
    {
      ist_free (transaction->ist_context);
      sfree (transaction->ist_context);
    }
  else if (transaction->ctx_type == NICT)
    {
      nict_free (transaction->nict_context);
      sfree (transaction->nict_context);
    }
  else
    {
      nist_free (transaction->nist_context);
      sfree (transaction->nist_context);
    }

  /* empty the fifo */
  evt = fifo_tryget (transaction->transactionff);
  while (evt != NULL)
    {
      msg_free (evt->sip);
      sfree (evt->sip);
      sfree (evt);
      evt = fifo_tryget (transaction->transactionff);
    }
  fifo_free (transaction->transactionff);
  sfree (transaction->transactionff);

  msg_free (transaction->orig_request);
  sfree (transaction->orig_request);
  msg_free (transaction->last_response);
  sfree (transaction->last_response);
  msg_free (transaction->ack);
  sfree (transaction->ack);

  via_free (transaction->topvia);
  sfree (transaction->topvia);
  from_free (transaction->from);
  sfree (transaction->from);
  to_free (transaction->to);
  sfree (transaction->to);
  call_id_free (transaction->callid);
  sfree (transaction->callid);
  cseq_free (transaction->cseq);
  sfree (transaction->cseq);

  return 0;
}

int
transaction_add_event (transaction_t * transaction, sipevent_t * evt)
{
  fifo_add (transaction->transactionff, evt);
  return 0;
}

int
transaction_execute (transaction_t * transaction, sipevent_t * evt)
{
  statemachine_t *statemachine;

  /* to kill the process, simply send this type of event. */
  if (EVT_IS_KILL_TRANSACTION (evt))
    {
      /* MAJOR CHANGE!
         TRANSACTION MUST NOW BE RELEASED BY END-USER:
         So Any usefull data can be save and re-used */
      /* transaction_free(transaction);
         sfree(transaction); */
      sfree (evt);
      return 0;
    }

  OSIP_TRACE (osip_trace
	      (__FILE__, __LINE__, OSIP_INFO4, NULL,
	       "sipevent tr->transactionid: %i\n",
	       transaction->transactionid));
  OSIP_TRACE (osip_trace
	      (__FILE__, __LINE__, OSIP_INFO4, NULL,
	       "sipevent tr->state: %i\n", transaction->state));
  OSIP_TRACE (osip_trace
	      (__FILE__, __LINE__, OSIP_INFO4, NULL,
	       "sipevent evt->type: %i\n", evt->type));
  OSIP_TRACE (osip_trace
	      (__FILE__, __LINE__, OSIP_INFO4, NULL,
	       "sipevent evt->sip: %x\n", evt->sip));

  if (transaction->ctx_type == ICT)
    statemachine = ict_get_fsm ();
  else if (transaction->ctx_type == IST)
    statemachine = ist_get_fsm ();
  else if (transaction->ctx_type == NICT)
    statemachine = nict_get_fsm ();
  else
    statemachine = nist_get_fsm ();


  if (-1 == fsm_callmethod (evt->type,
			    transaction->state, statemachine, evt,
			    transaction))
    {
      OSIP_TRACE (osip_trace
		  (__FILE__, __LINE__, OSIP_INFO3, NULL, "USELESS event!\n"));
      /* message is useless. */
      if (EVT_IS_MSG (evt))
	{
	  if (evt->sip != NULL)
	    {
	      msg_free (evt->sip);
	      sfree (evt->sip);
	    }
	}
    }
  else
    {
      OSIP_TRACE (osip_trace
		  (__FILE__, __LINE__, OSIP_INFO4, NULL,
		   "sipevent evt: method called!\n"));
    }
  sfree (evt);			/* this is the ONLY place for freeing event!! */
  return 1;
}

int
transaction_get_destination(transaction_t * transaction, char **ip, int *port)
{
  *ip=NULL;
  *port=0;
  if (transaction==NULL) return -1;
  if (transaction->ict_context!=NULL)
    {
      *ip = transaction->ict_context->destination;
      *port = transaction->ict_context->port;
      return 0;
    }
  else if (transaction->nict_context!=NULL)
    {
      *ip = transaction->nict_context->destination;
      *port = transaction->nict_context->port;
      return 0;
    }
  return -1;
}

int
transaction_set_your_instance (transaction_t * transaction, void *instance)
{
  if (transaction == NULL)
    return -1;
  transaction->your_instance = instance;
  return 0;
}

void *
transaction_get_your_instance (transaction_t * transaction)
{
  if (transaction == NULL)
    return NULL;
  return transaction->your_instance;
}

int
transaction_set_topvia (transaction_t * transaction, via_t * topvia)
{
  int i;

  if (transaction == NULL)
    return -1;
  i = via_clone (topvia, &(transaction->topvia));
  if (i == 0)
    return 0;
  transaction->topvia = NULL;
  return -1;
}

int
transaction_set_from (transaction_t * transaction, from_t * from)
{
  int i;

  if (transaction == NULL)
    return -1;
  i = from_clone (from, &(transaction->from));
  if (i == 0)
    return 0;
  transaction->from = NULL;
  return -1;
}

int
transaction_set_to (transaction_t * transaction, to_t * to)
{
  int i;

  if (transaction == NULL)
    return -1;
  i = to_clone (to, &(transaction->to));
  if (i == 0)
    return 0;
  transaction->to = NULL;
  return -1;
}

int
transaction_set_call_id (transaction_t * transaction, call_id_t * call_id)
{
  int i;

  if (transaction == NULL)
    return -1;
  i = call_id_clone (call_id, &(transaction->callid));
  if (i == 0)
    return 0;
  transaction->callid = NULL;
  return -1;
}

int
transaction_set_cseq (transaction_t * transaction, cseq_t * cseq)
{
  int i;

  if (transaction == NULL)
    return -1;
  i = cseq_clone (cseq, &(transaction->cseq));
  if (i == 0)
    return 0;
  transaction->cseq = NULL;
  return -1;
}

int
transaction_set_orig_request (transaction_t * transaction, sip_t * request)
{
  if (transaction == NULL)
    return -1;
  transaction->orig_request = request;
  return 0;
}

int
transaction_set_last_response (transaction_t * transaction,
			       sip_t * last_response)
{
  if (transaction == NULL)
    return -1;
  transaction->last_response = last_response;
  return 0;
}

int
transaction_set_ack (transaction_t * transaction, sip_t * ack)
{
  if (transaction == NULL)
    return -1;
  transaction->ack = ack;
  return 0;
}

int
transaction_set_state (transaction_t * transaction, state_t state)
{
  if (transaction == NULL)
    return -1;
  transaction->state = state;
  return 0;
}

int
transaction_set_in_socket (transaction_t * transaction, int sock)
{
  if (transaction == NULL)
    return -1;
  transaction->in_socket = sock;
  return 0;
}

int
transaction_set_out_socket (transaction_t * transaction, int sock)
{
  if (transaction == NULL)
    return -1;
  transaction->out_socket = sock;
  return 0;
}

int
transaction_set_config (transaction_t * transaction, osip_t * osip)
{
  if (transaction == NULL)
    return -1;
  transaction->config = (void *) osip;
  return 0;
}

/*
  0.8.7: Oups! Previous release does NOT follow the exact matching rules
  as described in the latest rfc2543bis-09 draft. The previous behavior
  was correct for User Agent but a statefull proxy can't easily fork
  transactions with oSIP.
  This limitation is now removed.
  Another happy side of that it will be easier to handle ACK for 200 to INVITE.
*/

int
transaction_matching_response_to_xict_17_1_3 (transaction_t * tr,
					      sip_t * response)
{
  generic_param_t *b_request;
  generic_param_t *b_response;
  via_t *topvia_response;

  /* some checks to avoid crashing on bad requests */
  if (tr == NULL || (tr->ict_context == NULL && tr->nict_context == NULL) ||
      /* only ict and nict can match a response */
      response == NULL || response->cseq == NULL
      || response->cseq->method == NULL)
    return -1;

  topvia_response = list_get (response->vias, 0);
  if (topvia_response == NULL)
    {
      OSIP_TRACE (osip_trace
		  (__FILE__, __LINE__, OSIP_ERROR, NULL,
		   "Remote UA is not compliant: missing a Via header!\n"));
      return -1;
    }
  via_param_getbyname (tr->topvia, "branch", &b_request);
  if (b_request == NULL)
    {
      OSIP_TRACE (osip_trace
		  (__FILE__, __LINE__, OSIP_BUG, NULL,
		   "You created a transaction without any branch! THIS IS NOT ALLOWED\n"));
      return -1;
    }
  via_param_getbyname (topvia_response, "branch", &b_response);
  if (b_response == NULL)
    {
#ifdef FWDSUPPORT
      // livio01042003: see if other criteria are met.
      // Check that: Call-ID, CSeq, To, and From fields are the same 
      
      // the from tag (unique)
      if (from_tag_match(tr->from, response->from)!=0)
	return -1;
      // the Cseq field
      if (cseq_match(tr->cseq, response->cseq)!=0)
	return -1;
      // the To field
      if (response->to->url->username==NULL && tr->from->url->username!=NULL)
	return -1;
      if (response->to->url->username!=NULL && tr->from->url->username==NULL)
	return -1;
      if (response->to->url->username!=NULL && tr->from->url->username!=NULL)
	{
	  /* if different -> return -1 */
	  if (strcmp(response->to->url->host, tr->from->url->host) ||
	      strcmp(response->to->url->username, tr->from->url->username))
	    return -1;
	}
      else
	{
	  /* if different -> return -1 */
	  if (strcmp(response->to->url->host, tr->from->url->host))
	    return -1;
	}

      // the Call-ID field
      if (call_id_match(tr->callid, response->call_id)!=0)
	return -1;
      return 0;
#else
      OSIP_TRACE (osip_trace
		  (__FILE__, __LINE__, OSIP_BUG, NULL,
		   "Remote UA is not compliant: missing a branch parameter header!\n"));
      return -1;
#endif
    }

  /*
     A response matches a client transaction under two
     conditions:

     1.   If the response has the same value of the branch parameter
     in the top Via header field as the branch parameter in the
     top Via header field of the request that created the
     transaction.
   */
  if (0 != strcmp (b_request->gvalue, b_response->gvalue))
    return -1;
  /*  
     2.   If the method parameter in the CSeq header field matches
     the method of the request that created the transaction. The
     method is needed since a CANCEL request constitutes a
     different transaction, but shares the same value of the
     branch parameter.
     AMD NOTE: cseq->method is ALWAYS the same than the METHOD of the request.
   */
  if (0 == strcmp (response->cseq->method, tr->cseq->method))	/* general case */
    return 0;
  return -1;
}

int
transaction_matching_request_to_xist_17_2_3 (transaction_t * tr,
					     sip_t * request)
{
  generic_param_t *b_origrequest;
  generic_param_t *b_request;
  via_t *topvia_request;
  int length_br;
  int length_br2;

  /* some checks to avoid crashing on bad requests */
  if (tr == NULL || (tr->ist_context == NULL && tr->nist_context == NULL) ||
      /* only ist and nist can match a request */
      request == NULL || request->cseq == NULL
      || request->cseq->method == NULL)
    return -1;

  topvia_request = list_get (request->vias, 0);
  if (topvia_request == NULL)
    {
      OSIP_TRACE (osip_trace
		  (__FILE__, __LINE__, OSIP_ERROR, NULL,
		   "Remote UA is not compliant: missing a Via header!\n"));
      return -1;
    }
  via_param_getbyname (topvia_request, "branch", &b_request);
  via_param_getbyname (tr->topvia, "branch", &b_origrequest);

  if ((b_origrequest == NULL && b_request != NULL) ||
      (b_origrequest != NULL && b_request == NULL))
    return -1;			/* one request is compliant, the other one is not... */

  /* Section 17.2.3 Matching Requests to Server Transactions:
     "The branch parameter in the topmost Via header field of the request
     is examined. If it is present and begins with the magic cookie
     "z9hG4bK", the request was generated by a client transaction
     compliant to this specification."
   */

  if (b_origrequest != NULL && b_request != NULL)	/* case where both request contains
							   a branch */
    {
      length_br = strlen (b_origrequest->gvalue);
      length_br2 = strlen (b_request->gvalue);
      if (length_br != length_br2)
	return -1;		/* can't be the same */
      if (0 == strncmp (b_origrequest->gvalue, "z9hG4bK", 7)
	  && 0 == strncmp (b_request->gvalue, "z9hG4bK", 7))
	{			/* both request comes from a compliant UA */
	  /* The request
	     matches a transaction if the branch parameter in the request is equal
	     to the one in the top Via header field of the request that created
	     the transaction, the sent-by value in the top Via of the request is
	     equal to the one in the request that created the transaction, and in
	     the case of a CANCEL request, the method of the request that created
	     the transaction was also CANCEL.
	   */
	  if (0 != strcmp (b_origrequest->gvalue, b_request->gvalue))
	    return -1;		/* bracnh param does not match */
	  via_param_getbyname (topvia_request, "sent-by", &b_request);
	  via_param_getbyname (tr->topvia, "sent-by", &b_origrequest);
	  if ((b_request != NULL && b_origrequest == NULL) ||
	      (b_request == NULL && b_origrequest != NULL))
	    return -1;		/* sent-by param does not match */
	  if (b_request != NULL	/* useless  && b_origrequest!=NULL */
	      && 0 != strcmp (b_origrequest->gvalue, b_request->gvalue))
	    return -1;
	  if (			/* MSG_IS_CANCEL(request)&& <<-- BUG from the spec?
				   I always check the CSeq */
	       (!(0 == strcmp (tr->cseq->method, "INVITE") &&
		  0 == strcmp (request->cseq->method, "ACK")))
	       && 0 != strcmp (tr->cseq->method, request->cseq->method))
	    return -1;
	  return 0;
	}
    }

  /* Back to the old backward compatibilty mechanism for matching requests */
  if (0 != call_id_match (tr->callid, request->call_id))
    return -1;
  if (MSG_IS_ACK (request))
    {
      generic_param_t *tag_from1;
      generic_param_t *tag_from2;

      from_param_getbyname (tr->to, "tag", &tag_from1);
      from_param_getbyname (request->to, "tag", &tag_from2);
      if (tag_from1 == NULL && tag_from2 != NULL)
	{			/* do not check it as it can be a new tag when the final
				   answer has a tag while an INVITE doesn't have one */
	}
      else if (tag_from1 != NULL && tag_from2 == NULL)
	{
	  return -1;
	}
      else
	{
	  if (0 != to_tag_match (tr->to, request->to))
	    return -1;
	}
    }
  else
    {
      if (0 != to_tag_match (tr->to, request->to))
	return -1;
    }
  if (0 != from_tag_match (tr->from, request->from))
    return -1;
  if (0 != cseq_match (tr->cseq, request->cseq))
    return -1;
  if (0 != via_match (tr->topvia, topvia_request))
    return -1;
  return 0;
}

int
to_tag_match (to_t * to1, to_t * to2)
{
  return from_tag_match ((from_t *) to1, (from_t *) to2);
}

int
from_tag_match (from_t * from1, from_t * from2)
{
  generic_param_t *tag_from1;
  generic_param_t *tag_from2;

  from_param_getbyname (from1, "tag", &tag_from1);
  from_param_getbyname (from2, "tag", &tag_from2);
  if (tag_from1 == NULL && tag_from2 == NULL)
    return 0;
  if ((tag_from1 != NULL && tag_from2 == NULL)
      || (tag_from1 == NULL && tag_from2 != NULL))
    return -1;
  if (0 != strcmp (tag_from1->gvalue, tag_from2->gvalue))
    return -1;
  return 0;
}

int
via_match (via_t * via1, via_t * via2)
{
  /* Can I really compare it this way??
     There exist matching rules for via header, but this method
     should only be used to detect retransmissions so the result should
     be exactly equivalent. (This may not be true if the retransmission
     traverse a different set of proxy...  */
  char *_via1;
  char *_via2;
  int i;

  if (via1 == NULL || via2 == NULL)
    return -1;
  i = via_2char (via1, &_via1);
  if (i != 0)
    return -1;
  i = via_2char (via2, &_via2);
  if (i != 0)
    {
      sfree (_via1);
      return -1;
    }

  i = strcmp (_via1, _via2);
  sfree (_via1);
  sfree (_via2);
  if (i != 0)
    return -1;
  return 0;
}

int
call_id_match (call_id_t * callid1, call_id_t * callid2)
{

  if (callid1 == NULL || callid2 == NULL)
    return -1;
  if (callid1->number == NULL || callid2->number == NULL)
    return -1;

  if (0 != strcmp (callid1->number, callid2->number))
    return -1;

  if ((callid1->host == NULL) && (callid2->host == NULL))
    return 0;
  if ((callid1->host == NULL) && (callid2->host != NULL))
    return -1;
  if ((callid1->host != NULL) && (callid2->host == NULL))
    return -1;
  if (0 != strcmp (callid1->host, callid2->host))
    return -1;

  return 0;
}

int
callleg_match (to_t * to1, from_t * from1, to_t * to2, from_t * from2)
{
  if (to1 == NULL || to2 == NULL)
    return -1;
  if (from1 == NULL || from2 == NULL)
    return -1;

  if (0 == from_compare ((from_t *) to1, (from_t *) to2)
      && 0 == from_compare (from1, from2))
    return 0;
  return -1;
}

int
cseq_match (cseq_t * cseq1, cseq_t * cseq2)
{
  if (cseq1 == NULL || cseq2 == NULL)
    return -1;
  if (cseq1->number == NULL || cseq2->number == NULL
      || cseq1->method == NULL || cseq2->method == NULL)
    return -1;

  if (0 == strcmp (cseq1->number, cseq2->number))
    {
      if (0 == strcmp (cseq2->method, "INVITE")
	  || 0 == strcmp (cseq2->method, "ACK"))
	{
	  if (0 == strcmp (cseq1->method, "INVITE") ||
	      0 == strcmp (cseq1->method, "ACK"))
	    return 0;
	}
      else
	{
	  if (0 == strcmp (cseq1->method, cseq2->method))
	    return 0;
	}
    }
  return -1;
}
