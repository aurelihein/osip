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

statemachine_t *nict_fsm;

statemachine_t *
nict_get_fsm ()
{
  return nict_fsm;
}

void
nict_unload_fsm ()
{
  transition_t *transition;
  statemachine_t *statemachine = nict_get_fsm ();

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
nict_load_fsm ()
{
  transition_t *transition;

  nict_fsm = (statemachine_t *) smalloc (sizeof (statemachine_t));
  nict_fsm->transitions = (list_t *) smalloc (sizeof (list_t));
  list_init (nict_fsm->transitions);

  /* to avoid race conditions between timers and first request */
  transition = (transition_t *) smalloc (sizeof (transition_t));
  transition->state = NICT_PRE_TRYING;
  transition->type = SND_REQUEST;
  transition->method = (void (*)(void *, void *)) &nict_snd_request;
  list_add (nict_fsm->transitions, transition, -1);
  /*
     transition         = (transition_t *) smalloc(sizeof(transition_t));
     transition->state  = NICT_TRYING;
     transition->type   = SND_REQUEST;
     transition->method = (void(*)(void *,void *))&nict_snd_request;
     list_add(nict_fsm->transitions,transition,-1);
   */
  transition = (transition_t *) smalloc (sizeof (transition_t));
  transition->state = NICT_TRYING;
  transition->type = TIMEOUT_F;
  transition->method = (void (*)(void *, void *)) &nict_timeout_f_event;
  list_add (nict_fsm->transitions, transition, -1);

  transition = (transition_t *) smalloc (sizeof (transition_t));
  transition->state = NICT_TRYING;
  transition->type = TIMEOUT_E;
  transition->method = (void (*)(void *, void *)) &nict_timeout_e_event;
  list_add (nict_fsm->transitions, transition, -1);

  transition = (transition_t *) smalloc (sizeof (transition_t));
  transition->state = NICT_TRYING;
  transition->type = RCV_STATUS_1XX;
  transition->method = (void (*)(void *, void *)) &nict_rcv_1xx;
  list_add (nict_fsm->transitions, transition, -1);

  transition = (transition_t *) smalloc (sizeof (transition_t));
  transition->state = NICT_TRYING;
  transition->type = RCV_STATUS_2XX;
  transition->method = (void (*)(void *, void *)) &nict_rcv_23456xx;
  list_add (nict_fsm->transitions, transition, -1);

  transition = (transition_t *) smalloc (sizeof (transition_t));
  transition->state = NICT_TRYING;
  transition->type = RCV_STATUS_3456XX;
  transition->method = (void (*)(void *, void *)) &nict_rcv_23456xx;
  list_add (nict_fsm->transitions, transition, -1);

  transition = (transition_t *) smalloc (sizeof (transition_t));
  transition->state = NICT_PROCEEDING;
  transition->type = TIMEOUT_F;
  transition->method = (void (*)(void *, void *)) &nict_timeout_f_event;
  list_add (nict_fsm->transitions, transition, -1);

  transition = (transition_t *) smalloc (sizeof (transition_t));
  transition->state = NICT_PROCEEDING;
  transition->type = TIMEOUT_E;
  transition->method = (void (*)(void *, void *)) &nict_timeout_e_event;
  list_add (nict_fsm->transitions, transition, -1);

  transition = (transition_t *) smalloc (sizeof (transition_t));
  transition->state = NICT_PROCEEDING;
  transition->type = RCV_STATUS_1XX;
  transition->method = (void (*)(void *, void *)) &nict_rcv_1xx;
  list_add (nict_fsm->transitions, transition, -1);

  transition = (transition_t *) smalloc (sizeof (transition_t));
  transition->state = NICT_PROCEEDING;
  transition->type = RCV_STATUS_2XX;
  transition->method = (void (*)(void *, void *)) &nict_rcv_23456xx;
  list_add (nict_fsm->transitions, transition, -1);

  transition = (transition_t *) smalloc (sizeof (transition_t));
  transition->state = NICT_PROCEEDING;
  transition->type = RCV_STATUS_3456XX;
  transition->method = (void (*)(void *, void *)) &nict_rcv_23456xx;
  list_add (nict_fsm->transitions, transition, -1);

  transition = (transition_t *) smalloc (sizeof (transition_t));
  transition->state = NICT_COMPLETED;
  transition->type = TIMEOUT_K;
  transition->method = (void (*)(void *, void *)) &nict_timeout_k_event;
  list_add (nict_fsm->transitions, transition, -1);

  /* these ccan be used to announce retransmission of 2xx and 3456xx
     For the state machine, it is completely useless...
     transition         = (transition_t *) smalloc(sizeof(transition_t));
     transition->state  = NICT_COMPLETED;
     transition->type   = RCV_STATUS_2XX;
     transition->method = (void(*)(void *,void *))&nict_rcv_23456xx2;
     list_add(nict_fsm->transitions,transition,-1);

     transition         = (transition_t *) smalloc(sizeof(transition_t));
     transition->state  = NICT_COMPLETED;
     transition->type   = RCV_STATUS_3456XX;
     transition->method = (void(*)(void *,void *))&nict_rcv_23456xx2;
     list_add(nict_fsm->transitions,transition,-1);
   */

}


void
nict_snd_request (transaction_t * nict, sipevent_t * evt)
{
  int i;
  osip_t *osip = (osip_t *) nict->config;

  /* Here we have ict->orig_request == NULL */
  nict->orig_request = evt->sip;

  i = osip->cb_send_message (nict, evt->sip, nict->nict_context->destination,
			     nict->nict_context->port, nict->out_socket);

  if (i == 0)
    {
      /* invoke the right callback! */
      if (MSG_IS_REGISTER (evt->sip))
	{
	  if (osip->cb_nict_register_sent != NULL)
	    osip->cb_nict_register_sent (nict, nict->orig_request);
	}
      else if (MSG_IS_BYE (evt->sip))
	{
	  if (osip->cb_nict_bye_sent != NULL)
	    osip->cb_nict_bye_sent (nict, nict->orig_request);
	}
      else if (MSG_IS_OPTIONS (evt->sip))
	{
	  if (osip->cb_nict_options_sent != NULL)
	    osip->cb_nict_options_sent (nict, nict->orig_request);
	}
      else if (MSG_IS_INFO (evt->sip))
	{
	  if (osip->cb_nict_info_sent != NULL)
	    osip->cb_nict_info_sent (nict, nict->orig_request);
	}
      else if (MSG_IS_CANCEL (evt->sip))
	{
	  if (osip->cb_nict_cancel_sent != NULL)
	    osip->cb_nict_cancel_sent (nict, nict->orig_request);
	}
      else if (MSG_IS_NOTIFY (evt->sip))
	{
	  if (osip->cb_nict_notify_sent != NULL)
	    osip->cb_nict_notify_sent (nict, nict->orig_request);
	}
      else if (MSG_IS_SUBSCRIBE (evt->sip))
	{
	  if (osip->cb_nict_subscribe_sent != NULL)
	    osip->cb_nict_subscribe_sent (nict, nict->orig_request);
	}
      else
	{
	  if (osip->cb_nict_unknown_sent != NULL)
	    osip->cb_nict_unknown_sent (nict, nict->orig_request);
	}

      transaction_set_state (nict, NICT_TRYING);
    }
  else
    {
      osip->cb_nict_transport_error (nict, i);
      transaction_set_state (nict, NICT_TERMINATED);
      osip->cb_nict_kill_transaction (nict);
      /* TODO: MUST BE DELETED NOW */
    }
}

void
nict_timeout_e_event (transaction_t * nict, sipevent_t * evt)
{
  osip_t *osip = (osip_t *) nict->config;
  time_t now = time (NULL);
  int i;

  /* reset timer */
  if (nict->state == NICT_TRYING)
    {
      nict->nict_context->timer_e_length =
	nict->nict_context->timer_e_length * 2;
      if (nict->nict_context->timer_e_length > 4000)
	nict->nict_context->timer_e_length = 4000;
    }
  else				/* in PROCEEDING STATE, TIMER is always 4000 */
    nict->nict_context->timer_e_length = 4000;

  nict->nict_context->timer_e_start = now;

  /* retransmit REQUEST */
  i = osip->cb_send_message (nict, nict->orig_request,
			     nict->nict_context->destination,
			     nict->nict_context->port, nict->out_socket);
  if (i != 0)
    {
      osip->cb_nict_transport_error (nict, i);
      transaction_set_state (nict, NICT_TERMINATED);
      osip->cb_nict_kill_transaction (nict);
      /* TODO: MUST BE DELETED NOW */
      return;
    }
  if (osip->cb_nict_request_sent2 != NULL)
    osip->cb_nict_request_sent2 (nict, nict->orig_request);
}

void
nict_timeout_f_event (transaction_t * nict, sipevent_t * evt)
{
  osip_t *osip = (osip_t *) nict->config;

  nict->nict_context->timer_f_length = -1;
  nict->nict_context->timer_f_start = -1;

  transaction_set_state (nict, NICT_TERMINATED);
  osip->cb_nict_kill_transaction (nict);
}

void
nict_timeout_k_event (transaction_t * nict, sipevent_t * evt)
{
  osip_t *osip = (osip_t *) nict->config;

  nict->nict_context->timer_k_length = -1;
  nict->nict_context->timer_k_start = -1;

  transaction_set_state (nict, NICT_TERMINATED);
  osip->cb_nict_kill_transaction (nict);
}

void
nict_rcv_1xx (transaction_t * nict, sipevent_t * evt)
{
  osip_t *osip = (osip_t *) nict->config;

  /* leave this answer to the core application */

  if (nict->last_response != NULL)
    {
      msg_free (nict->last_response);
      sfree (nict->last_response);
    }
  nict->last_response = evt->sip;
  if (osip->cb_nict_1xx_received != NULL)
    osip->cb_nict_1xx_received (nict, evt->sip);
  transaction_set_state (nict, NICT_PROCEEDING);
}

void
nict_rcv_23456xx (transaction_t * nict, sipevent_t * evt)
{
  osip_t *osip = (osip_t *) nict->config;

  /* leave this answer to the core application */

  if (nict->last_response != NULL)
    {
      msg_free (nict->last_response);
      sfree (nict->last_response);
    }
  nict->last_response = evt->sip;

  if (EVT_IS_RCV_STATUS_2XX (evt))
    osip->cb_nict_2xx_received (nict, nict->last_response);
  else
    {
      if (MSG_IS_STATUS_3XX (nict->last_response))
	osip->cb_nict_3xx_received (nict, nict->last_response);
      else if (MSG_IS_STATUS_4XX (nict->last_response))
	osip->cb_nict_4xx_received (nict, nict->last_response);
      else if (MSG_IS_STATUS_5XX (nict->last_response))
	osip->cb_nict_5xx_received (nict, nict->last_response);
      else
	osip->cb_nict_6xx_received (nict, nict->last_response);
    }

  if (nict->state != NICT_COMPLETED)	/* reset timer K */
    nict->nict_context->timer_k_start = time (NULL);
  transaction_set_state (nict, NICT_COMPLETED);
}

/* See Rq in the state machine definition (above)
void
nict_rcv_23456xx2(transaction_t *nict, sipevent_t *evt)
{
  osip_t *osip = (osip_t*)nict->config;

  if (nict->last_response!=NULL)
    {
      msg_free(nict->last_response);
      sfree(nict->last_response);
    }
  nict->last_response = evt->sip;

  if (EVT_IS_RCV_STATUS_2XX(evt))
    {
      if (osip->cb_nict_2xx_received2!=NULL)
	osip->cb_nict_2xx_received2(nict, evt->sip);
    }
  else
    {
      if (osip->cb_nict_3456xx_received2!=NULL)
	osip->cb_nict_3456xx_received2(nict, evt->sip);
    }
}
*/
