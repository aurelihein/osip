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

#ifdef OSIP_MT
static smutex_t *ict_fastmutex;
static smutex_t *ist_fastmutex;
static smutex_t *nict_fastmutex;
static smutex_t *nist_fastmutex;
#endif

int
osip_global_init ()
{
  /* load the fsm configuration */
  ict_load_fsm ();
  ist_load_fsm ();
  nict_load_fsm ();
  nist_load_fsm ();

  /* load the parser configuration */
  parser_init ();

  /* give a seed for all calls to rand() */
  srand (time (NULL));
  init_random_number ();

#ifdef OSIP_MT
  ict_fastmutex = smutex_init ();
  ist_fastmutex = smutex_init ();
  nict_fastmutex = smutex_init ();
  nist_fastmutex = smutex_init ();
#endif
  return 0;
}

void
osip_global_free ()
{
  ict_unload_fsm ();
  ist_unload_fsm ();
  nict_unload_fsm ();
  nist_unload_fsm ();

#ifdef OSIP_MT
  smutex_destroy (ict_fastmutex);
  sfree (ict_fastmutex);
  smutex_destroy (ist_fastmutex);
  sfree (ist_fastmutex);
  smutex_destroy (nict_fastmutex);
  sfree (nict_fastmutex);
  smutex_destroy (nist_fastmutex);
  sfree (nist_fastmutex);
#endif
}

int
osip_ict_lock (osip_t * osip)
{
#ifdef OSIP_MT
  return smutex_lock (ict_fastmutex);
#else
  return 0;
#endif
}

int
osip_ict_unlock (osip_t * osip)
{
#ifdef OSIP_MT
  return smutex_unlock (ict_fastmutex);
#else
  return 0;
#endif
}

int
osip_ist_lock (osip_t * osip)
{
#ifdef OSIP_MT
  return smutex_lock (ist_fastmutex);
#else
  return 0;
#endif
}

int
osip_ist_unlock (osip_t * osip)
{
#ifdef OSIP_MT
  return smutex_unlock (ist_fastmutex);
#else
  return 0;
#endif
}

int
osip_nict_lock (osip_t * osip)
{
#ifdef OSIP_MT
  return smutex_lock (nict_fastmutex);
#else
  return 0;
#endif
}

int
osip_nict_unlock (osip_t * osip)
{
#ifdef OSIP_MT
  return smutex_unlock (nict_fastmutex);
#else
  return 0;
#endif
}

int
osip_nist_lock (osip_t * osip)
{
#ifdef OSIP_MT
  return smutex_lock (nist_fastmutex);
#else
  return 0;
#endif
}

int
osip_nist_unlock (osip_t * osip)
{
#ifdef OSIP_MT
  return smutex_unlock (nist_fastmutex);
#else
  return 0;
#endif
}

int
osip_add_ict (osip_t * osip, transaction_t * ict)
{
#ifdef OSIP_MT
  smutex_lock (ict_fastmutex);
#endif
  list_add (osip->ict_transactions, ict, -1);
#ifdef OSIP_MT
  smutex_unlock (ict_fastmutex);
#endif
  return 0;
}

int
osip_add_ist (osip_t * osip, transaction_t * ist)
{
#ifdef OSIP_MT
  smutex_lock (ist_fastmutex);
#endif
  list_add (osip->ist_transactions, ist, -1);
#ifdef OSIP_MT
  smutex_unlock (ist_fastmutex);
#endif
  return 0;
}

int
osip_add_nict (osip_t * osip, transaction_t * nict)
{
#ifdef OSIP_MT
  smutex_lock (nict_fastmutex);
#endif
  list_add (osip->nict_transactions, nict, -1);
#ifdef OSIP_MT
  smutex_unlock (nict_fastmutex);
#endif
  return 0;
}

int
osip_add_nist (osip_t * osip, transaction_t * nist)
{
#ifdef OSIP_MT
  smutex_lock (nist_fastmutex);
#endif
  list_add (osip->nist_transactions, nist, -1);
#ifdef OSIP_MT
  smutex_unlock (nist_fastmutex);
#endif
  return 0;
}

int
osip_remove_ict (osip_t * osip, transaction_t * ict)
{
  int pos = 0;
  transaction_t *tmp;

#ifdef OSIP_MT
  smutex_lock (ict_fastmutex);
#endif
  while (!list_eol (osip->ict_transactions, pos))
    {
      tmp = list_get (osip->ict_transactions, pos);
      if (tmp->transactionid == ict->transactionid)
	{
	  list_remove (osip->ict_transactions, pos);
#ifdef OSIP_MT
	  smutex_unlock (ict_fastmutex);
#endif
	  return 0;
	}
      pos++;
    }
#ifdef OSIP_MT
  smutex_unlock (ict_fastmutex);
#endif
  return -1;
}

int
osip_remove_ist (osip_t * osip, transaction_t * ist)
{
  int pos = 0;
  transaction_t *tmp;

#ifdef OSIP_MT
  smutex_lock (ist_fastmutex);
#endif
  while (!list_eol (osip->ist_transactions, pos))
    {
      tmp = list_get (osip->ist_transactions, pos);
      if (tmp->transactionid == ist->transactionid)
	{
	  list_remove (osip->ist_transactions, pos);
#ifdef OSIP_MT
	  smutex_unlock (ist_fastmutex);
#endif
	  return 0;
	}
      pos++;
    }
#ifdef OSIP_MT
  smutex_unlock (ist_fastmutex);
#endif
  return -1;
}

int
osip_remove_nict (osip_t * osip, transaction_t * nict)
{
  int pos = 0;
  transaction_t *tmp;

#ifdef OSIP_MT
  smutex_lock (nict_fastmutex);
#endif
  while (!list_eol (osip->nict_transactions, pos))
    {
      tmp = list_get (osip->nict_transactions, pos);
      if (tmp->transactionid == nict->transactionid)
	{
	  list_remove (osip->nict_transactions, pos);
#ifdef OSIP_MT
	  smutex_unlock (nict_fastmutex);
#endif
	  return 0;
	}
      pos++;
    }
#ifdef OSIP_MT
  smutex_unlock (nict_fastmutex);
#endif
  return -1;
}

int
osip_remove_nist (osip_t * osip, transaction_t * nist)
{
  int pos = 0;
  transaction_t *tmp;

#ifdef OSIP_MT
  smutex_lock (nist_fastmutex);
#endif
  while (!list_eol (osip->nist_transactions, pos))
    {
      tmp = list_get (osip->nist_transactions, pos);
      if (tmp->transactionid == nist->transactionid)
	{
	  list_remove (osip->nist_transactions, pos);
#ifdef OSIP_MT
	  smutex_unlock (nist_fastmutex);
#endif
	  return 0;
	}
      pos++;
    }
#ifdef OSIP_MT
  smutex_unlock (nist_fastmutex);
#endif
  return -1;
}

#if 0
/* this method is made obsolete because it contains bugs and is also
   too much limited.
   any call to this method should be replace this way:

   //osip_distribute(osip, evt);
   transaction_t *transaction = osip_find_transaction_and_add_event(osip, evt);

   if (i!=0) // in case it's a new request
     {
        if (evt is an ACK)
            evt could be an ACK for INVITE (not handled by oSIP)
        else if ( evt is a 200 for INVITE)
           evt could be a retransmission of a 200 for INVITE (not handled by oSIP)
        else if (evt is a new request)  == not a ACK and not a response
	  {
           transaction = osip_create_transaction(osip, evt);
           if (transaction==NULL)
             printf("failed to create a transaction\");
          }
    }
    else
    {
    // here, the message as been taken by the stack.
    }
*/


/* finds the transaction context and add the sipevent in its fifo. */
/* USED ONLY BY THE TRANSPORT LAYER.                               */
/* INPUT : osip_t *osip | osip. contains the list of tr. context*/
/* INPUT : sipevent_t* sipevent | event to dispatch.               */
transaction_t *
osip_distribute_event (osip_t * osip, sipevent_t * evt)
{
  transaction_t *transaction = NULL;
  int i;
  context_type_t ctx_type;

  if (EVT_IS_INCOMINGMSG (evt))
    {
      /* event is for ict */
      if (MSG_IS_REQUEST (evt->sip))
	{
	  if (0 == strcmp (evt->sip->cseq->method, "INVITE")
	      || 0 == strcmp (evt->sip->cseq->method, "ACK"))
	    {
#ifdef OSIP_MT
	      smutex_lock (ist_fastmutex);
#endif
	      transaction =
		osip_transaction_find (osip->ist_transactions, evt);
#ifdef OSIP_MT
	      smutex_unlock (ist_fastmutex);
#endif
	    }
	  else
	    {
#ifdef OSIP_MT
	      smutex_lock (nist_fastmutex);
#endif
	      transaction =
		osip_transaction_find (osip->nist_transactions, evt);
#ifdef OSIP_MT
	      smutex_unlock (nist_fastmutex);
#endif
	    }
	}
      else
	{
	  if (0 == strcmp (evt->sip->cseq->method, "INVITE")
	      || 0 == strcmp (evt->sip->cseq->method, "ACK"))
	    {
#ifdef OSIP_MT
	      smutex_lock (ict_fastmutex);
#endif
	      transaction =
		osip_transaction_find (osip->ict_transactions, evt);
#ifdef OSIP_MT
	      smutex_unlock (ict_fastmutex);
#endif
	    }
	  else
	    {
#ifdef OSIP_MT
	      smutex_lock (nict_fastmutex);
#endif
	      transaction =
		osip_transaction_find (osip->nict_transactions, evt);
#ifdef OSIP_MT
	      smutex_unlock (nict_fastmutex);
#endif
	    }
	}
      if (transaction == NULL)
	{
	  if (EVT_IS_RCV_STATUS_1XX (evt)
	      || EVT_IS_RCV_STATUS_2XX (evt)
	      || EVT_IS_RCV_STATUS_3456XX (evt) || EVT_IS_RCV_ACK (evt))
	    {			/* event MUST be ignored! */
	      /* EXCEPT FOR 2XX THAT MUST BE GIVEN TO THE CORE LAYER!!! */

	      /* TODO */

	      OSIP_TRACE (osip_trace
			  (__FILE__, __LINE__, OSIP_WARNING, NULL,
			   "transaction does not yet exist... %x callid:%s\n",
			   evt, evt->sip->call_id->number));
	      msg_free (evt->sip);
	      sfree (evt->sip);
	      sfree (evt);	/* transaction thread will not delete it */
	      return NULL;
	    }

	  /* we create a new context for this incoming request */
	  if (0 == strcmp (evt->sip->cseq->method, "INVITE"))
	    ctx_type = IST;
	  else
	    ctx_type = NIST;

	  i = transaction_init (&transaction, ctx_type, osip, evt->sip);
	  if (i == -1)
	    {
	      msg_free (evt->sip);
	      sfree (evt->sip);
	      sfree (evt);	/* transaction thread will not delete it */
	      return NULL;
	    }
	}
      evt->transactionid = transaction->transactionid;

      evt->transactionid = transaction->transactionid;
      fifo_add (transaction->transactionff, evt);
      return transaction;
    }
  else
    {
      OSIP_TRACE (osip_trace
		  (__FILE__, __LINE__, OSIP_BUG, NULL,
		   "wrong event type %x\n", evt));
      return NULL;
    }
}
#endif

int
osip_find_transaction_and_add_event (osip_t * osip, sipevent_t * evt)
{
  transaction_t *transaction = __osip_find_transaction (osip, evt, 1);

  if (transaction == NULL)
    return -1;
  return 0;
}

transaction_t *
osip_find_transaction (osip_t * osip, sipevent_t * evt)
{
#ifdef OSIP_MT
  OSIP_TRACE (osip_trace
	      (__FILE__, __LINE__, OSIP_BUG, NULL,
	       "\n\n\n\nYou are using a multithreaded application, but this method is not allowed! Use osip_find_transaction_add_add_event() instead.\n\n\\n"));
#endif
  return __osip_find_transaction (osip, evt, 0);
}

transaction_t *
__osip_find_transaction (osip_t * osip, sipevent_t * evt, int consume)
{
  transaction_t *transaction = NULL;
  list_t *transactions = NULL;

#ifdef OSIP_MT
  smutex_t *mut = NULL;
#endif

  if (evt == NULL || evt->sip == NULL || evt->sip->cseq == NULL)
    return NULL;

  if (EVT_IS_INCOMINGMSG (evt))
    {
      if (MSG_IS_REQUEST (evt->sip))
	{
	  if (0 == strcmp (evt->sip->cseq->method, "INVITE")
	      || 0 == strcmp (evt->sip->cseq->method, "ACK"))
	    {
	      transactions = osip->ist_transactions;
#ifdef OSIP_MT
	      mut = ist_fastmutex;
#endif
	    }
	  else
	    {
	      transactions = osip->nist_transactions;
#ifdef OSIP_MT
	      mut = nist_fastmutex;
#endif
	    }
	}
      else
	{
	  if (0 == strcmp (evt->sip->cseq->method, "INVITE")
	      || 0 == strcmp (evt->sip->cseq->method, "ACK"))
	    {
	      transactions = osip->ict_transactions;
#ifdef OSIP_MT
	      mut = ict_fastmutex;
#endif
	    }
	  else
	    {
	      transactions = osip->nict_transactions;
#ifdef OSIP_MT
	      mut = nict_fastmutex;
#endif
	    }
	}
    }
  else if (EVT_IS_OUTGOINGMSG (evt))
    {
      if (MSG_IS_RESPONSE (evt->sip))
	{
	  if (0 == strcmp (evt->sip->cseq->method, "INVITE")
	      || 0 == strcmp (evt->sip->cseq->method, "ACK"))
	    {
	      transactions = osip->ist_transactions;
#ifdef OSIP_MT
	      mut = ist_fastmutex;
#endif
	    }
	  else
	    {
	      transactions = osip->nist_transactions;
#ifdef OSIP_MT
	      mut = nist_fastmutex;
#endif
	    }
	}
      else
	{
	  if (0 == strcmp (evt->sip->cseq->method, "INVITE")
	      || 0 == strcmp (evt->sip->cseq->method, "ACK"))
	    {
	      transactions = osip->ict_transactions;
#ifdef OSIP_MT
	      mut = ict_fastmutex;
#endif
	    }
	  else
	    {
	      transactions = osip->nict_transactions;
#ifdef OSIP_MT
	      mut = nict_fastmutex;
#endif
	    }
	}
    }
  if (transactions == NULL)
    return NULL;		/* not a message??? */

#ifdef OSIP_MT
  smutex_lock (mut);
#endif
  transaction = osip_transaction_find (transactions, evt);
  if (consume == 1)
    {				/* we add the event before releasing the mutex!! */
      if (transaction != NULL)
	{
	  transaction_add_event (transaction, evt);
#ifdef OSIP_MT
	  smutex_unlock (mut);
#endif
	  return transaction;
	}
    }
#ifdef OSIP_MT
  smutex_unlock (mut);
#endif

  return transaction;
}

transaction_t *
osip_create_transaction (osip_t * osip, sipevent_t * evt)
{
  transaction_t *transaction;
  int i;
  context_type_t ctx_type;

  if (evt==NULL)
    return NULL;
  if (evt->sip==NULL)
    return NULL;

  /* make sure the request's method reflect the cseq value. */
  if (MSG_IS_REQUEST (evt->sip))
    {
      /* delete request where cseq method does not match
	 the method in request-line */
      if (evt->sip->cseq==NULL || evt->sip->strtline==NULL
	  || evt->sip->cseq->method==NULL || evt->sip->strtline->sipmethod==NULL)
	{
	  return NULL;
	}
      if (0 != strcmp (evt->sip->cseq->method, evt->sip->strtline->sipmethod))
	{
	  OSIP_TRACE (osip_trace
		      (__FILE__, __LINE__, OSIP_WARNING, NULL,
		       "core module: Discard invalid message with method!=cseq!\n"));
	  return NULL;
	}
    }

  if (MSG_IS_ACK (evt->sip)) /* ACK never create transactions */
    return NULL;

  if (EVT_IS_INCOMINGREQ (evt))
    {
      /* we create a new context for this incoming request */
      if (0 == strcmp (evt->sip->cseq->method, "INVITE"))
	ctx_type = IST;
      else
	ctx_type = NIST;
    }
  else if (EVT_IS_OUTGOINGREQ (evt))
    {
      if (0 == strcmp (evt->sip->cseq->method, "INVITE"))
	ctx_type = ICT;
      else
	ctx_type = NICT;
    }
  else
    {
      OSIP_TRACE (osip_trace
		  (__FILE__, __LINE__, OSIP_ERROR, NULL,
		   "Cannot build a transction for this message!\n"));
      return NULL;
    }

  i = transaction_init (&transaction, ctx_type, osip, evt->sip);
  if (i == -1)
    {
      return NULL;
    }
  evt->transactionid = transaction->transactionid;
  return transaction;
}

transaction_t *
osip_transaction_find (list_t * transactions, sipevent_t * evt)
{
  int pos = 0;
  transaction_t *transaction;

  if (EVT_IS_INCOMINGREQ (evt))
    {
      while (!list_eol (transactions, pos))
	{
	  transaction = (transaction_t *) list_get (transactions, pos);
	  if (0 ==
	      transaction_matching_request_to_xist_17_2_3 (transaction,
							   evt->sip))
	    return transaction;
	  pos++;
	}
    }
  else if (EVT_IS_INCOMINGRESP (evt))
    {
      while (!list_eol (transactions, pos))
	{
	  transaction = (transaction_t *) list_get (transactions, pos);
	  if (0 ==
	      transaction_matching_response_to_xict_17_1_3 (transaction,
							    evt->sip))
	    return transaction;
	  pos++;
	}
    }
  else				/* handle OUTGOING message */
    {				/* THE TRANSACTION ID MUST BE SET */
      while (!list_eol (transactions, pos))
	{
	  transaction = (transaction_t *) list_get (transactions, pos);
	  if (transaction->transactionid == evt->transactionid)
	    return transaction;
	  pos++;
	}
    }
  return NULL;
}


int
osip_init (osip_t ** osip)
{
  *osip = (osip_t *) smalloc (sizeof (osip_t));
  if (*osip == NULL)
    return -1;			/* allocation failed */

#ifndef DISABLE_MEMSET
  memset (*osip, 0, sizeof (osip_t));
#else

  (*osip)->cb_ict_kill_transaction = NULL;
  (*osip)->cb_ict_invite_sent = NULL;
  (*osip)->cb_ict_invite_sent2 = NULL;
  (*osip)->cb_ict_ack_sent = NULL;
  (*osip)->cb_ict_ack_sent2 = NULL;
  (*osip)->cb_ict_1xx_received = NULL;
  (*osip)->cb_ict_2xx_received = NULL;
  (*osip)->cb_ict_2xx_received2 = NULL;
  (*osip)->cb_ict_3456xx_received = NULL;
  (*osip)->cb_ict_3456xx_received2 = NULL;
  (*osip)->cb_ict_transport_error = NULL;

  /* callbacks for ist */
  (*osip)->cb_ist_kill_transaction = NULL;
  (*osip)->cb_ist_invite_received = NULL;
  (*osip)->cb_ist_invite_received2 = NULL;
  (*osip)->cb_ist_ack_received = NULL;
  (*osip)->cb_ist_ack_received2 = NULL;
  (*osip)->cb_ist_1xx_sent = NULL;
  (*osip)->cb_ist_1xx_sent2 = NULL;
  (*osip)->cb_ist_2xx_sent = NULL;
  (*osip)->cb_ist_2xx_sent2 = NULL;
  (*osip)->cb_ist_3456xx_sent = NULL;
  (*osip)->cb_ist_3456xx_sent2 = NULL;
  (*osip)->cb_ist_transport_error = NULL;


  /* callbacks for nict */
  (*osip)->cb_nict_kill_transaction = NULL;
  (*osip)->cb_nict_register_sent = NULL;
  (*osip)->cb_nict_bye_sent = NULL;
  (*osip)->cb_nict_options_sent = NULL;
  (*osip)->cb_nict_info_sent = NULL;
  (*osip)->cb_nict_cancel_sent = NULL;
  (*osip)->cb_nict_notify_sent = NULL;
  (*osip)->cb_nict_subscribe_sent = NULL;
  (*osip)->cb_nict_unknown_sent = NULL;
  (*osip)->cb_nict_request_sent2 = NULL;
  (*osip)->cb_nict_1xx_received = NULL;
  (*osip)->cb_nict_2xx_received = NULL;
  (*osip)->cb_nict_2xx_received2 = NULL;
  (*osip)->cb_nict_3456xx_received = NULL;
  (*osip)->cb_nict_3456xx_received2 = NULL;
  (*osip)->cb_nict_transport_error = NULL;

  /* callbacks for nist */
  (*osip)->cb_nist_kill_transaction = NULL;
  (*osip)->cb_nist_register_received = NULL;
  (*osip)->cb_nist_bye_received = NULL;
  (*osip)->cb_nist_options_received = NULL;
  (*osip)->cb_nist_info_received = NULL;
  (*osip)->cb_nist_cancel_received = NULL;
  (*osip)->cb_nist_notify_received = NULL;
  (*osip)->cb_nist_subscribe_received = NULL;
  /* ... TO BE ADDED: All known and used method extansions */
  (*osip)->cb_nist_unknown_received = NULL;
  (*osip)->cb_nist_request_received2 = NULL;
  (*osip)->cb_nist_1xx_sent = NULL;
  (*osip)->cb_nist_2xx_sent = NULL;
  (*osip)->cb_nist_2xx_sent2 = NULL;
  (*osip)->cb_nist_3456xx_sent = NULL;
  (*osip)->cb_nist_3456xx_sent2 = NULL;
  (*osip)->cb_nist_transport_error = NULL;
#endif

  (*osip)->ict_transactions = (list_t *) smalloc (sizeof (list_t));
  list_init ((*osip)->ict_transactions);
  (*osip)->ist_transactions = (list_t *) smalloc (sizeof (list_t));
  list_init ((*osip)->ist_transactions);
  (*osip)->nict_transactions = (list_t *) smalloc (sizeof (list_t));
  list_init ((*osip)->nict_transactions);
  (*osip)->nist_transactions = (list_t *) smalloc (sizeof (list_t));
  list_init ((*osip)->nist_transactions);

  return 0;
}

void
osip_free (osip_t * osip)
{
  sfree (osip->ict_transactions);
  sfree (osip->ist_transactions);
  sfree (osip->nict_transactions);
  sfree (osip->nist_transactions);
}


void
osip_set_application_context (osip_t * osip, void *pointer)
{
  osip->application_context = pointer;
}

void *
osip_get_application_context (osip_t * osip)
{
  if (osip == NULL)
    return NULL;
  return osip->application_context;
}

int
osip_ict_execute (osip_t * osip)
{
  transaction_t *transaction;
  sipevent_t *se;
  int more_event;
  int tr;

  tr = 0;
  while (!list_eol (osip->ict_transactions, tr))
    {
      transaction = list_get (osip->ict_transactions, tr);
      tr++;
      more_event = 1;
      do
	{
	  se = (sipevent_t *) fifo_tryget (transaction->transactionff);
	  if (se == NULL)	/* no more event for this transaction */
	    more_event = 0;
	  else
	    transaction_execute (transaction, se);
	}
      while (more_event == 1);
    }
  return 0;
}

int
osip_ist_execute (osip_t * osip)
{
  transaction_t *transaction;
  sipevent_t *se;
  int more_event;
  int tr;

  tr = 0;
  while (!list_eol (osip->ist_transactions, tr))
    {
      transaction = list_get (osip->ist_transactions, tr);
      tr++;
      more_event = 1;
      do
	{
	  se = (sipevent_t *) fifo_tryget (transaction->transactionff);
	  if (se == NULL)	/* no more event for this transaction */
	    more_event = 0;
	  else
	    transaction_execute (transaction, se);
	}
      while (more_event == 1);
    }
  return 0;
}

int
osip_nict_execute (osip_t * osip)
{
  transaction_t *transaction;
  sipevent_t *se;
  int more_event;
  int tr;

  tr = 0;
  while (!list_eol (osip->nict_transactions, tr))
    {
      transaction = list_get (osip->nict_transactions, tr);
      tr++;
      more_event = 1;
      do
	{
	  se = (sipevent_t *) fifo_tryget (transaction->transactionff);
	  if (se == NULL)	/* no more event for this transaction */
	    more_event = 0;
	  else
	    transaction_execute (transaction, se);
	}
      while (more_event == 1);
    }
  return 0;
}

int
osip_nist_execute (osip_t * osip)
{
  transaction_t *transaction;
  sipevent_t *se;
  int more_event;
  int tr;

  tr = 0;
  while (!list_eol (osip->nist_transactions, tr))
    {
      transaction = list_get (osip->nist_transactions, tr);
      tr++;
      more_event = 1;
      do
	{
	  se = (sipevent_t *) fifo_tryget (transaction->transactionff);
	  if (se == NULL)	/* no more event for this transaction */
	    more_event = 0;
	  else
	    transaction_execute (transaction, se);
	}
      while (more_event == 1);
    }
  return 0;
}


void
osip_timers_ict_execute (osip_t * osip)
{
  transaction_t *tr;
  int pos = 0;

#ifdef OSIP_MT
  smutex_lock (ict_fastmutex);
#endif
  /* handle ict timers */
  while (!list_eol (osip->ict_transactions, pos))
    {
      sipevent_t *evt;

      tr = (transaction_t *) list_get (osip->ict_transactions, pos);

      if (1 <= fifo_size (tr->transactionff))
	{
	  OSIP_TRACE (osip_trace
		      (__FILE__, __LINE__, OSIP_INFO4, NULL,
		       "1 Pending event already in transaction !\n"));
	}
      else
	{
	  evt =
	    ict_need_timer_b_event (tr->ict_context, tr->state,
				    tr->transactionid);
	  if (evt != NULL)
	    fifo_add (tr->transactionff, evt);
	  else
	    {
	      evt =
		ict_need_timer_a_event (tr->ict_context, tr->state,
					tr->transactionid);
	      if (evt != NULL)
		fifo_add (tr->transactionff, evt);
	      else
		{
		  evt = ict_need_timer_d_event (tr->ict_context, tr->state,
						tr->transactionid);
		  if (evt != NULL)
		    fifo_add (tr->transactionff, evt);
		}
	    }
	}
      pos++;
    }
#ifdef OSIP_MT
  smutex_unlock (ict_fastmutex);
#endif
}

void
osip_timers_ist_execute (osip_t * osip)
{
  transaction_t *tr;
  int pos = 0;

#ifdef OSIP_MT
  smutex_lock (ist_fastmutex);
#endif
  /* handle ist timers */
  while (!list_eol (osip->ist_transactions, pos))
    {
      sipevent_t *evt;

      tr = (transaction_t *) list_get (osip->ist_transactions, pos);

      evt =
	ist_need_timer_i_event (tr->ist_context, tr->state,
				tr->transactionid);
      if (evt != NULL)
	fifo_add (tr->transactionff, evt);
      else
	{
	  evt = ist_need_timer_h_event (tr->ist_context, tr->state,
					tr->transactionid);
	  if (evt != NULL)
	    fifo_add (tr->transactionff, evt);
	  else
	    {
	      evt = ist_need_timer_g_event (tr->ist_context, tr->state,
					    tr->transactionid);
	      if (evt != NULL)
		fifo_add (tr->transactionff, evt);
	    }
	}
      pos++;
    }
#ifdef OSIP_MT
  smutex_unlock (ist_fastmutex);
#endif
}

void
osip_timers_nict_execute (osip_t * osip)
{
  transaction_t *tr;
  int pos = 0;

#ifdef OSIP_MT
  smutex_lock (nict_fastmutex);
#endif
  /* handle nict timers */
  while (!list_eol (osip->nict_transactions, pos))
    {
      sipevent_t *evt;

      tr = (transaction_t *) list_get (osip->nict_transactions, pos);

      evt =
	nict_need_timer_k_event (tr->nict_context, tr->state,
				 tr->transactionid);
      if (evt != NULL)
	fifo_add (tr->transactionff, evt);
      else
	{
	  evt =
	    nict_need_timer_f_event (tr->nict_context, tr->state,
				     tr->transactionid);
	  if (evt != NULL)
	    fifo_add (tr->transactionff, evt);
	  else
	    {
	      evt = nict_need_timer_e_event (tr->nict_context, tr->state,
					     tr->transactionid);
	      if (evt != NULL)
		fifo_add (tr->transactionff, evt);
	    }
	}
      pos++;
    }
#ifdef OSIP_MT
  smutex_unlock (nict_fastmutex);
#endif
}


void
osip_timers_nist_execute (osip_t * osip)
{
  transaction_t *tr;
  int pos = 0;

#ifdef OSIP_MT
  smutex_lock (nist_fastmutex);
#endif
  /* handle nist timers */
  while (!list_eol (osip->nist_transactions, pos))
    {
      sipevent_t *evt;

      tr = (transaction_t *) list_get (osip->nist_transactions, pos);

      evt =
	nist_need_timer_j_event (tr->nist_context, tr->state,
				 tr->transactionid);
      if (evt != NULL)
	fifo_add (tr->transactionff, evt);
      pos++;
    }
#ifdef OSIP_MT
  smutex_unlock (nist_fastmutex);
#endif
}

void
osip_setcb_send_message (osip_t * cf,
			 int (*cb) (transaction_t *, sip_t *, char *, int,
				    int))
{
  cf->cb_send_message = cb;
}

void
osip_setcb_ict_kill_transaction (osip_t * cf, void (*cb) (transaction_t *))
{
  cf->cb_ict_kill_transaction = cb;
}

void
osip_setcb_ict_invite_sent (osip_t * cf,
			    void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_ict_invite_sent = cb;
}

void
osip_setcb_ict_invite_sent2 (osip_t * cf,
			     void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_ict_invite_sent2 = cb;
}

void
osip_setcb_ict_ack_sent (osip_t * cf, void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_ict_ack_sent = cb;
}

void
osip_setcb_ict_ack_sent2 (osip_t * cf, void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_ict_ack_sent2 = cb;
}

void
osip_setcb_ict_1xx_received (osip_t * cf,
			     void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_ict_1xx_received = cb;
}

void
osip_setcb_ict_2xx_received (osip_t * cf,
			     void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_ict_2xx_received = cb;
}

void
osip_setcb_ict_2xx_received2 (osip_t * cf,
			      void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_ict_2xx_received2 = cb;
}

void
osip_setcb_ict_3xx_received (osip_t * cf,
			     void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_ict_3xx_received = cb;
}

void
osip_setcb_ict_4xx_received (osip_t * cf,
			     void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_ict_4xx_received = cb;
}

void
osip_setcb_ict_5xx_received (osip_t * cf,
			     void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_ict_5xx_received = cb;
}

void
osip_setcb_ict_6xx_received (osip_t * cf,
			     void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_ict_6xx_received = cb;
}

void
osip_setcb_ict_3456xx_received2 (osip_t * cf,
				 void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_ict_3456xx_received2 = cb;
}

void
osip_setcb_ict_transport_error (osip_t * cf,
				void (*cb) (transaction_t *, int error))
{
  cf->cb_ict_transport_error = cb;
}

/* callbacks for ist */
void
osip_setcb_ist_kill_transaction (osip_t * cf, void (*cb) (transaction_t *))
{
  cf->cb_ist_kill_transaction = cb;
}

void
osip_setcb_ist_invite_received (osip_t * cf,
				void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_ist_invite_received = cb;
}

void
osip_setcb_ist_invite_received2 (osip_t * cf,
				 void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_ist_invite_received2 = cb;
}

void
osip_setcb_ist_ack_received (osip_t * cf,
			     void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_ist_ack_received = cb;
}

void
osip_setcb_ist_ack_received2 (osip_t * cf,
			      void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_ist_ack_received2 = cb;
}

void
osip_setcb_ist_1xx_sent (osip_t * cf, void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_ist_1xx_sent = cb;
}

void
osip_setcb_ist_2xx_sent (osip_t * cf, void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_ist_2xx_sent = cb;
}

void
osip_setcb_ist_2xx_sent2 (osip_t * cf, void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_ist_2xx_sent2 = cb;
}

void
osip_setcb_ist_3xx_sent (osip_t * cf, void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_ist_3xx_sent = cb;
}

void
osip_setcb_ist_4xx_sent (osip_t * cf, void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_ist_4xx_sent = cb;
}

void
osip_setcb_ist_5xx_sent (osip_t * cf, void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_ist_5xx_sent = cb;
}

void
osip_setcb_ist_6xx_sent (osip_t * cf, void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_ist_6xx_sent = cb;
}

void
osip_setcb_ist_3456xx_sent2 (osip_t * cf,
			     void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_ist_3456xx_sent2 = cb;
}

void
osip_setcb_ist_transport_error (osip_t * cf,
				void (*cb) (transaction_t *, int error))
{
  cf->cb_ist_transport_error = cb;
}


/* callbacks for nict */
void
osip_setcb_nict_kill_transaction (osip_t * cf, void (*cb) (transaction_t *))
{
  cf->cb_nict_kill_transaction = cb;
}

void
osip_setcb_nict_register_sent (osip_t * cf,
			       void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_nict_register_sent = cb;
}

void
osip_setcb_nict_bye_sent (osip_t * cf, void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_nict_bye_sent = cb;
}

void
osip_setcb_nict_options_sent (osip_t * cf,
			      void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_nict_options_sent = cb;
}

void
osip_setcb_nict_info_sent (osip_t * cf, void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_nict_info_sent = cb;
}

void
osip_setcb_nict_cancel_sent (osip_t * cf,
			     void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_nict_cancel_sent = cb;
}

void
osip_setcb_nict_notify_sent (osip_t * cf,
			     void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_nict_notify_sent = cb;
}

void
osip_setcb_nict_subscribe_sent (osip_t * cf,
				void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_nict_subscribe_sent = cb;
}

void
osip_setcb_nict_unknown_sent (osip_t * cf,
			      void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_nict_unknown_sent = cb;
}

void
osip_setcb_nict_request_sent2 (osip_t * cf,
			       void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_nict_request_sent2 = cb;
}

void
osip_setcb_nict_1xx_received (osip_t * cf,
			      void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_nict_1xx_received = cb;
}

void
osip_setcb_nict_2xx_received (osip_t * cf,
			      void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_nict_2xx_received = cb;
}

void
osip_setcb_nict_2xx_received2 (osip_t * cf,
			       void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_nict_2xx_received2 = cb;
}

void
osip_setcb_nict_3xx_received (osip_t * cf,
			      void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_nict_3xx_received = cb;
}

void
osip_setcb_nict_4xx_received (osip_t * cf,
			      void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_nict_4xx_received = cb;
}

void
osip_setcb_nict_5xx_received (osip_t * cf,
			      void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_nict_5xx_received = cb;
}

void
osip_setcb_nict_6xx_received (osip_t * cf,
			      void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_nict_6xx_received = cb;
}

void
osip_setcb_nict_3456xx_received2 (osip_t * cf,
				  void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_nict_3456xx_received2 = cb;
}

void
osip_setcb_nict_transport_error (osip_t * cf,
				 void (*cb) (transaction_t *, int error))
{
  cf->cb_nict_transport_error = cb;
}

  /* callbacks for nist */
void
osip_setcb_nist_kill_transaction (osip_t * cf, void (*cb) (transaction_t *))
{
  cf->cb_nist_kill_transaction = cb;
}

void
osip_setcb_nist_register_received (osip_t * cf,
				   void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_nist_register_received = cb;
}

void
osip_setcb_nist_bye_received (osip_t * cf,
			      void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_nist_bye_received = cb;
}

void
osip_setcb_nist_options_received (osip_t * cf,
				  void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_nist_options_received = cb;
}

void
osip_setcb_nist_info_received (osip_t * cf,
			       void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_nist_info_received = cb;
}

void
osip_setcb_nist_cancel_received (osip_t * cf,
				 void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_nist_cancel_received = cb;
}

void
osip_setcb_nist_notify_received (osip_t * cf,
				 void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_nist_notify_received = cb;
}

void
osip_setcb_nist_subscribe_received (osip_t * cf,
				    void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_nist_subscribe_received = cb;
}

void
osip_setcb_nist_unknown_received (osip_t * cf,
				  void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_nist_unknown_received = cb;
}

  /* ... TO BE ADDED: All known and used method extansions */

void
osip_setcb_nist_request_received2 (osip_t * cf,
				   void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_nist_request_received2 = cb;
}

void
osip_setcb_nist_1xx_sent (osip_t * cf, void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_nist_1xx_sent = cb;
}

void
osip_setcb_nist_2xx_sent (osip_t * cf, void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_nist_2xx_sent = cb;
}

void
osip_setcb_nist_2xx_sent2 (osip_t * cf, void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_nist_2xx_sent2 = cb;
}

void
osip_setcb_nist_3xx_sent (osip_t * cf, void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_nist_3xx_sent = cb;
}

void
osip_setcb_nist_4xx_sent (osip_t * cf, void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_nist_4xx_sent = cb;
}

void
osip_setcb_nist_5xx_sent (osip_t * cf, void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_nist_5xx_sent = cb;
}

void
osip_setcb_nist_6xx_sent (osip_t * cf, void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_nist_6xx_sent = cb;
}

void
osip_setcb_nist_3456xx_sent2 (osip_t * cf,
			      void (*cb) (transaction_t *, sip_t *))
{
  cf->cb_nist_3456xx_sent2 = cb;
}

void
osip_setcb_nist_transport_error (osip_t * cf,
				 void (*cb) (transaction_t *, int error))
{
  cf->cb_nist_transport_error = cb;
}
