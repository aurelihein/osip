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

statemachine_t *nist_fsm;

statemachine_t *
nist_get_fsm ()
{
  return nist_fsm;
}

void
nist_unload_fsm ()
{
  transition_t *transition;
  statemachine_t *statemachine = nist_get_fsm ();

  while (!list_eol (statemachine->transitions, 0))
    {
      transition = (transition_t *) list_get (statemachine->transitions, 0);
      list_remove (statemachine->transitions, 0);
      sfree (transition);
    }
  sfree (statemachine->transitions);
  sfree (statemachine);
}


void
nist_load_fsm ()
{
  transition_t *transition;

  nist_fsm = (statemachine_t *) smalloc (sizeof (statemachine_t));
  nist_fsm->transitions = (list_t *) smalloc (sizeof (list_t));
  list_init (nist_fsm->transitions);

  transition = (transition_t *) smalloc (sizeof (transition_t));
  transition->state = NIST_PRE_TRYING;
  transition->type = RCV_REQUEST;
  transition->method = (void (*)(void *, void *)) &nist_rcv_request;
  list_add (nist_fsm->transitions, transition, -1);

  /* This can be used to announce request but is useless, as
     the transaction cannot send any response yet!
     transition         = (transition_t *) smalloc(sizeof(transition_t));
     transition->state  = NIST_TRYING;
     transition->type   = RCV_REQUEST;
     transition->method = (void(*)(void *,void *))&nist_rcv_request;
     list_add(nist_fsm->transitions,transition,-1);
   */

  transition = (transition_t *) smalloc (sizeof (transition_t));
  transition->state = NIST_TRYING;
  transition->type = SND_STATUS_1XX;
  transition->method = (void (*)(void *, void *)) &nist_snd_1xx;
  list_add (nist_fsm->transitions, transition, -1);

  transition = (transition_t *) smalloc (sizeof (transition_t));
  transition->state = NIST_TRYING;
  transition->type = SND_STATUS_2XX;
  transition->method = (void (*)(void *, void *)) &nist_snd_23456xx;
  list_add (nist_fsm->transitions, transition, -1);

  transition = (transition_t *) smalloc (sizeof (transition_t));
  transition->state = NIST_TRYING;
  transition->type = SND_STATUS_3456XX;
  transition->method = (void (*)(void *, void *)) &nist_snd_23456xx;
  list_add (nist_fsm->transitions, transition, -1);

  transition = (transition_t *) smalloc (sizeof (transition_t));
  transition->state = NIST_PROCEEDING;
  transition->type = SND_STATUS_1XX;
  transition->method = (void (*)(void *, void *)) &nist_snd_1xx;
  list_add (nist_fsm->transitions, transition, -1);

  transition = (transition_t *) smalloc (sizeof (transition_t));
  transition->state = NIST_PROCEEDING;
  transition->type = SND_STATUS_2XX;
  transition->method = (void (*)(void *, void *)) &nist_snd_23456xx;
  list_add (nist_fsm->transitions, transition, -1);

  transition = (transition_t *) smalloc (sizeof (transition_t));
  transition->state = NIST_PROCEEDING;
  transition->type = SND_STATUS_3456XX;
  transition->method = (void (*)(void *, void *)) &nist_snd_23456xx;
  list_add (nist_fsm->transitions, transition, -1);

  transition = (transition_t *) smalloc (sizeof (transition_t));
  transition->state = NIST_PROCEEDING;
  transition->type = RCV_REQUEST;
  transition->method = (void (*)(void *, void *)) &nist_rcv_request;
  list_add (nist_fsm->transitions, transition, -1);

  transition = (transition_t *) smalloc (sizeof (transition_t));
  transition->state = NIST_COMPLETED;
  transition->type = TIMEOUT_J;
  transition->method = (void (*)(void *, void *)) &nist_timeout_j_event;
  list_add (nist_fsm->transitions, transition, -1);

  transition = (transition_t *) smalloc (sizeof (transition_t));
  transition->state = NIST_COMPLETED;
  transition->type = RCV_REQUEST;
  transition->method = (void (*)(void *, void *)) &nist_rcv_request;
  list_add (nist_fsm->transitions, transition, -1);

}

void
nist_rcv_request (transaction_t * nist, sipevent_t * evt)
{
  int i;
  osip_t *osip = (osip_t *) nist->config;

  if (nist->state == NIST_PRE_TRYING)	/* announce new REQUEST */
    {
      /* Here we have ist->orig_request == NULL */
      nist->orig_request = evt->sip;

      if (MSG_IS_REGISTER (evt->sip))
	{
	  osip->cb_nist_register_received (nist, nist->orig_request);
	}
      else if (MSG_IS_BYE (evt->sip))
	{
	  osip->cb_nist_bye_received (nist, nist->orig_request);
	}
      else if (MSG_IS_OPTIONS (evt->sip))
	{
	  osip->cb_nist_options_received (nist, nist->orig_request);
	}
      else if (MSG_IS_INFO (evt->sip))
	{
	  osip->cb_nist_info_received (nist, nist->orig_request);
	}
      else if (MSG_IS_CANCEL (evt->sip))
	{
	  osip->cb_nist_cancel_received (nist, nist->orig_request);
	}
      else if (MSG_IS_NOTIFY (evt->sip))
	{
	  osip->cb_nist_notify_received (nist, nist->orig_request);
	}
      else if (MSG_IS_SUBSCRIBE (evt->sip))
	{
	  osip->cb_nist_subscribe_received (nist, nist->orig_request);
	}
      else
	{
	  osip->cb_nist_unknown_received (nist, nist->orig_request);
	}
    }
  else				/* NIST_PROCEEDING or NIST_COMPLETED */
    {
      /* delete retransmission */
      msg_free (evt->sip);
      sfree (evt->sip);

      if (osip->cb_nist_request_received2 != NULL)
	osip->cb_nist_request_received2 (nist, nist->orig_request);
      if (nist->last_response != NULL)	/* retransmit last response */
	{
	  via_t *via;

	  via = (via_t *) list_get (nist->last_response->vias, 0);
	  if (via)
	    {
	      char *host;
	      int port;
	      generic_param_t *maddr;
	      generic_param_t *received;
	      generic_param_t *rport;
	      via_param_getbyname (via, "maddr", &maddr);
	      via_param_getbyname (via, "received", &received);
	      via_param_getbyname (via, "rport", &rport);
	      /* 1: user should not use the provided information
	         (host and port) if they are using a reliable
	         transport. Instead, they should use the already
	         open socket attached to this transaction. */
	      /* 2: check maddr and multicast usage */
	      if (maddr != NULL)
		host = maddr->gvalue;
	      /* we should check if this is a multicast address and use
	         set the "ttl" in this case. (this must be done in the
	         UDP message (not at the SIP layer) */
	      else if (received != NULL)
		host = received->gvalue;
	      else
		host = via->host;

	      if (rport == NULL || rport->gvalue == NULL)
		{
		  if (via->port != NULL)
		    port = satoi (via->port);
		  else
		    port = 5060;
		}
	      else
		port = satoi (rport->gvalue);

	      i = osip->cb_send_message (nist, nist->last_response, host,
					 port, nist->out_socket);
	    }
	  else
	    i = -1;
	  if (i != 0)
	    {
	      osip->cb_nist_transport_error (nist, i);
	      transaction_set_state (nist, NIST_TERMINATED);
	      osip->cb_nist_kill_transaction (nist);
	      /* MUST BE DELETED NOW */
	      return;
	    }
	  else
	    {
	      if (MSG_IS_STATUS_1XX (nist->last_response))
		osip->cb_nist_1xx_sent (nist, nist->last_response);
	      else if (MSG_IS_STATUS_2XX (nist->last_response))
		osip->cb_nist_2xx_sent2 (nist, nist->last_response);
	      else
		osip->cb_nist_3456xx_sent2 (nist, nist->last_response);
	      return;
	    }
	}
      /* we are already in the proper state */
      return;
    }

  /* we come here only if it was the first REQUEST received */
  transaction_set_state (nist, NIST_TRYING);
}

void
nist_snd_1xx (transaction_t * nist, sipevent_t * evt)
{
  int i;
  via_t *via;
  osip_t *osip = (osip_t *) nist->config;

  if (nist->last_response != NULL)
    {
      msg_free (nist->last_response);
      sfree (nist->last_response);
    }
  nist->last_response = evt->sip;

  via = (via_t *) list_get (nist->last_response->vias, 0);
  if (via)
    {
      char *host;
      int port;
      generic_param_t *maddr;
      generic_param_t *received;
      generic_param_t *rport;
      via_param_getbyname (via, "maddr", &maddr);
      via_param_getbyname (via, "received", &received);
      via_param_getbyname (via, "rport", &rport);
      /* 1: user should not use the provided information
         (host and port) if they are using a reliable
         transport. Instead, they should use the already
         open socket attached to this transaction. */
      /* 2: check maddr and multicast usage */
      if (maddr != NULL)
	host = maddr->gvalue;
      /* we should check if this is a multicast address and use
         set the "ttl" in this case. (this must be done in the
         UDP message (not at the SIP layer) */
      else if (received != NULL)
	host = received->gvalue;
      else
	host = via->host;

      if (rport == NULL || rport->gvalue == NULL)
	{
	  if (via->port != NULL)
	    port = satoi (via->port);
	  else
	    port = 5060;
	}
      else
	port = satoi (rport->gvalue);
      i = osip->cb_send_message (nist, nist->last_response, host,
				 port, nist->out_socket);
    }
  else
    i = -1;
  if (i != 0)
    {
      osip->cb_nist_transport_error (nist, i);
      transaction_set_state (nist, NIST_TERMINATED);
      osip->cb_nist_kill_transaction (nist);
      /* MUST BE DELETED NOW */
      return;
    }
  else
    osip->cb_nist_1xx_sent (nist, nist->last_response);

  transaction_set_state (nist, NIST_PROCEEDING);
}

void
nist_snd_23456xx (transaction_t * nist, sipevent_t * evt)
{
  int i;
  via_t *via;
  osip_t *osip = (osip_t *) nist->config;

  if (nist->last_response != NULL)
    {
      msg_free (nist->last_response);
      sfree (nist->last_response);
    }
  nist->last_response = evt->sip;

  via = (via_t *) list_get (nist->last_response->vias, 0);
  if (via)
    {
      char *host;
      int port;
      generic_param_t *maddr;
      generic_param_t *received;
      generic_param_t *rport;
      via_param_getbyname (via, "maddr", &maddr);
      via_param_getbyname (via, "received", &received);
      via_param_getbyname (via, "rport", &rport);
      /* 1: user should not use the provided information
         (host and port) if they are using a reliable
         transport. Instead, they should use the already
         open socket attached to this transaction. */
      /* 2: check maddr and multicast usage */
      if (maddr != NULL)
	host = maddr->gvalue;
      /* we should check if this is a multicast address and use
         set the "ttl" in this case. (this must be done in the
         UDP message (not at the SIP layer) */
      else if (received != NULL)
	host = received->gvalue;
      else
	host = via->host;

      if (rport == NULL || rport->gvalue == NULL)
	{
	  if (via->port != NULL)
	    port = satoi (via->port);
	  else
	    port = 5060;
	}
      else
	port = satoi (rport->gvalue);
      i = osip->cb_send_message (nist, nist->last_response, host,
				 port, nist->out_socket);
    }
  else
    i = -1;
  if (i != 0)
    {
      osip->cb_nist_transport_error (nist, i);
      transaction_set_state (nist, NIST_TERMINATED);
      osip->cb_nist_kill_transaction (nist);
      /* MUST BE DELETED NOW */
      return;
    }
  else
    {
      if (EVT_IS_SND_STATUS_2XX (evt))
	osip->cb_nist_2xx_sent (nist, nist->last_response);
      else if (MSG_IS_STATUS_3XX (nist->last_response))
	osip->cb_nist_3xx_sent (nist, nist->last_response);
      else if (MSG_IS_STATUS_4XX (nist->last_response))
	osip->cb_nist_4xx_sent (nist, nist->last_response);
      else if (MSG_IS_STATUS_5XX (nist->last_response))
	osip->cb_nist_5xx_sent (nist, nist->last_response);
      else
	osip->cb_nist_6xx_sent (nist, nist->last_response);
    }

  if (nist->state != NIST_COMPLETED)	/* start J timer */
    nist->nist_context->timer_j_start = time (NULL);

  transaction_set_state (nist, NIST_COMPLETED);
}


void
nist_timeout_j_event (transaction_t * nist, sipevent_t * evt)
{
  osip_t *osip = (osip_t *) nist->config;

  nist->nist_context->timer_j_length = -1;
  nist->nist_context->timer_j_start = -1;

  transaction_set_state (nist, NIST_TERMINATED);
  osip->cb_nist_kill_transaction (nist);
}
