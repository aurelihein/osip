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

statemachine_t *ict_fsm;

statemachine_t *
ict_get_fsm ()
{
  return ict_fsm;
}


void
ict_unload_fsm ()
{
  transition_t *transition;
  statemachine_t *statemachine = ict_get_fsm ();

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
ict_load_fsm ()
{
  transition_t *transition;

  ict_fsm = (statemachine_t *) smalloc (sizeof (statemachine_t));
  ict_fsm->transitions = (list_t *) smalloc (sizeof (list_t));
  list_init (ict_fsm->transitions);

  /* a new state is needed because a race can happen between
     the timer and the timeout event!
     One day to find out this bug  ;-)
   */
  transition = (transition_t *) smalloc (sizeof (transition_t));
  transition->state = ICT_PRE_CALLING;
  transition->type = SND_REQINVITE;
  transition->method = (void (*)(void *, void *)) &ict_snd_invite;
  list_add (ict_fsm->transitions, transition, -1);
  /*
     transition         = (transition_t *) smalloc(sizeof(transition_t));
     transition->state  = ICT_CALLING;
     transition->type   = SND_REQINVITE;
     transition->method = (void(*)(void *,void *))&ict_snd_invite;
     list_add(ict_fsm->transitions,transition,-1);
   */
  transition = (transition_t *) smalloc (sizeof (transition_t));
  transition->state = ICT_CALLING;
  transition->type = TIMEOUT_A;
  transition->method = (void (*)(void *, void *)) &ict_timeout_a_event;
  list_add (ict_fsm->transitions, transition, -1);

  transition = (transition_t *) smalloc (sizeof (transition_t));
  transition->state = ICT_CALLING;
  transition->type = TIMEOUT_B;
  transition->method = (void (*)(void *, void *)) &ict_timeout_b_event;
  list_add (ict_fsm->transitions, transition, -1);

  transition = (transition_t *) smalloc (sizeof (transition_t));
  transition->state = ICT_CALLING;
  transition->type = RCV_STATUS_1XX;
  transition->method = (void (*)(void *, void *)) &ict_rcv_1xx;
  list_add (ict_fsm->transitions, transition, -1);

  transition = (transition_t *) smalloc (sizeof (transition_t));
  transition->state = ICT_CALLING;
  transition->type = RCV_STATUS_2XX;
  transition->method = (void (*)(void *, void *)) &ict_rcv_2xx;
  list_add (ict_fsm->transitions, transition, -1);

  transition = (transition_t *) smalloc (sizeof (transition_t));
  transition->state = ICT_CALLING;
  transition->type = RCV_STATUS_3456XX;
  transition->method = (void (*)(void *, void *)) &ict_rcv_3456xx;
  list_add (ict_fsm->transitions, transition, -1);

  transition = (transition_t *) smalloc (sizeof (transition_t));
  transition->state = ICT_PROCEEDING;
  transition->type = RCV_STATUS_1XX;
  transition->method = (void (*)(void *, void *)) &ict_rcv_1xx;
  list_add (ict_fsm->transitions, transition, -1);

  transition = (transition_t *) smalloc (sizeof (transition_t));
  transition->state = ICT_PROCEEDING;
  transition->type = RCV_STATUS_2XX;
  transition->method = (void (*)(void *, void *)) &ict_rcv_2xx;
  list_add (ict_fsm->transitions, transition, -1);

  transition = (transition_t *) smalloc (sizeof (transition_t));
  transition->state = ICT_PROCEEDING;
  transition->type = RCV_STATUS_3456XX;
  transition->method = (void (*)(void *, void *)) &ict_rcv_3456xx;
  list_add (ict_fsm->transitions, transition, -1);

  transition = (transition_t *) smalloc (sizeof (transition_t));
  transition->state = ICT_COMPLETED;
  transition->type = RCV_STATUS_3456XX;
  transition->method = (void (*)(void *, void *)) &ict_retransmit_ack;
  list_add (ict_fsm->transitions, transition, -1);

  transition = (transition_t *) smalloc (sizeof (transition_t));
  transition->state = ICT_COMPLETED;
  transition->type = TIMEOUT_D;
  transition->method = (void (*)(void *, void *)) &ict_timeout_d_event;
  list_add (ict_fsm->transitions, transition, -1);

}

void
ict_snd_invite (transaction_t * ict, sipevent_t * evt)
{
  int i;
  osip_t *osip = (osip_t *) ict->config;

  /* Here we have ict->orig_request == NULL */
  ict->orig_request = evt->sip;

  i = osip->cb_send_message (ict, evt->sip, ict->ict_context->destination,
			     ict->ict_context->port, ict->out_socket);

  if (i == 0)
    {
      if (osip->cb_ict_invite_sent != NULL)
	osip->cb_ict_invite_sent (ict, ict->orig_request);
      transaction_set_state (ict, ICT_CALLING);
    }
  else
    {
      osip->cb_ict_transport_error (ict, i);
      transaction_set_state (ict, ICT_TERMINATED);
      osip->cb_ict_kill_transaction (ict);
      /* TODO: MUST BE DELETED NOW */
    }
}

void
ict_timeout_a_event (transaction_t * ict, sipevent_t * evt)
{
  osip_t *osip = (osip_t *) ict->config;
  time_t now = time (NULL);
  int i;

  /* reset timer */
  ict->ict_context->timer_a_length = ict->ict_context->timer_a_length * 2;
  ict->ict_context->timer_a_start = now;

  /* retransmit REQUEST */
  i =
    osip->cb_send_message (ict, ict->orig_request,
			   ict->ict_context->destination,
			   ict->ict_context->port, ict->out_socket);
  if (i != 0)
    {
      osip->cb_ict_transport_error (ict, i);
      transaction_set_state (ict, ICT_TERMINATED);
      osip->cb_ict_kill_transaction (ict);
      /* TODO: MUST BE DELETED NOW */
      return;
    }
  if (osip->cb_ict_invite_sent2 != NULL)
    osip->cb_ict_invite_sent2 (ict, ict->orig_request);
}

void
ict_timeout_b_event (transaction_t * ict, sipevent_t * evt)
{
  osip_t *osip = (osip_t *) ict->config;

  ict->ict_context->timer_b_length = -1;
  ict->ict_context->timer_b_start = -1;

  transaction_set_state (ict, ICT_TERMINATED);
  osip->cb_ict_kill_transaction (ict);
}

void
ict_rcv_1xx (transaction_t * ict, sipevent_t * evt)
{
  osip_t *osip = (osip_t *) ict->config;

  /* leave this answer to the core application */

  if (ict->last_response != NULL)
    {
      msg_free (ict->last_response);
      sfree (ict->last_response);
    }
  ict->last_response = evt->sip;
  if (osip->cb_ict_1xx_received != NULL)
    osip->cb_ict_1xx_received (ict, evt->sip);
  transaction_set_state (ict, ICT_PROCEEDING);
}

void
ict_rcv_2xx (transaction_t * ict, sipevent_t * evt)
{
  osip_t *osip = (osip_t *) ict->config;

  /* leave this answer to the core application */

  if (ict->last_response != NULL)
    {
      msg_free (ict->last_response);
      sfree (ict->last_response);
    }
  ict->last_response = evt->sip;
  osip->cb_ict_2xx_received (ict, evt->sip);
  transaction_set_state (ict, ICT_TERMINATED);
  osip->cb_ict_kill_transaction (ict);

}

sip_t *
ict_create_ack (transaction_t * ict, sip_t * response)
{
  int i;
  sip_t *ack;

  i = msg_init (&ack);
  if (i != 0)
    return NULL;

  /* Section 17.1.1.3: Construction of the ACK request: */
  i = from_clone (response->from, &(ack->from));
  if (i != 0)
    goto ica_error;
  i = to_clone (response->to, &(ack->to));	/* include the tag! */
  if (i != 0)
    goto ica_error;
  i = call_id_clone (response->call_id, &(ack->call_id));
  if (i != 0)
    goto ica_error;
  i = cseq_clone (response->cseq, &(ack->cseq));
  if (i != 0)
    goto ica_error;
  sfree (ack->cseq->method);
  ack->cseq->method = sgetcopy ("ACK");

  ack->strtline->sipmethod = (char *) smalloc (5);
  sprintf (ack->strtline->sipmethod, "ACK");
  ack->strtline->sipversion =
    sgetcopy (ict->orig_request->strtline->sipversion);

  ack->strtline->statuscode = NULL;
  ack->strtline->reasonphrase = NULL;

  /* MUST copy REQUEST-URI from Contact header! */
  url_clone (ict->orig_request->strtline->rquri, &(ack->strtline->rquri));

  /* ACK MUST contain only the TOP Via field from original request */
  {
    via_t *via;
    via_t *orig_via;

    msg_getvia (ict->orig_request, 0, &orig_via);
    if (orig_via == NULL)
      goto ica_error;
    via_clone (orig_via, &via);
    list_add (ack->vias, via, -1);
  }

  /* ack MUST contains the ROUTE headers field from the original request */
  /* IS IT TRUE??? */
  /* if the answers contains a set of route (or record route), then it */
  /* should be used instead?? ......May be not..... */
  {
    int pos = 0;
    route_t *route;
    route_t *orig_route;

    while (!list_eol (ict->orig_request->routes, pos))
      {
	orig_route = (route_t *) list_get (ict->orig_request->routes, pos);
	route_clone (orig_route, &route);
	list_add (ack->routes, route, -1);
	pos++;
      }
  }

  /* may be we could add some other headers: */
  /* For example "Max-Forward" */

  return ack;
ica_error:
  msg_free (ack);
  sfree (ack);
  return NULL;
}

void
ict_rcv_3456xx (transaction_t * ict, sipevent_t * evt)
{
  /*  sipevent_t *sipevt; */
  route_t *route;
  int i;
  osip_t *osip = (osip_t *) ict->config;

  /* leave this answer to the core application */

  if (ict->last_response != NULL)
    {
      msg_free (ict->last_response);
      sfree (ict->last_response);
    }
  ict->last_response = evt->sip;
  if (ict->state != ICT_COMPLETED)	/* not a retransmission */
    {
      /* automatic handling of ack! */
      sip_t *ack = ict_create_ack (ict, evt->sip);

      ict->ack = ack;

      if (ict->ack==NULL)
	{
	  /* ERROR! */
	  transaction_set_state (ict, ICT_TERMINATED);
	  osip->cb_ict_kill_transaction (ict);
	  /* TODO: MUST BE DELETED NOW */
	  return;
	}

      /* reset ict->ict_context->destination only if
         it is not yet set. */
      if (ict->ict_context->destination == NULL)
	{
	  msg_getroute (ack, 0, &route);
	  if (route != NULL)
	    {
	      int port = 5060;

	      if (route->url->port != NULL)
		port = satoi (route->url->port);
	      ict_set_destination (ict->ict_context,
				   sgetcopy (route->url->host), port);
	    }
	  else
	    {
	      int port = 5060;

	      if (ack->strtline->rquri->port != NULL)
		port = satoi (ack->strtline->rquri->port);
	      ict_set_destination (ict->ict_context,
				   sgetcopy (ack->strtline->rquri->host),
				   port);
	    }
	}
      i = osip->cb_send_message (ict, ack, ict->ict_context->destination,
				 ict->ict_context->port, ict->out_socket);
      if (i != 0)
	{
	  osip->cb_ict_transport_error (ict, i);
	  transaction_set_state (ict, ICT_TERMINATED);
	  osip->cb_ict_kill_transaction (ict);
	  /* TODO: MUST BE DELETED NOW */
	  return;
	}
      if (MSG_IS_STATUS_3XX (evt->sip))
	osip->cb_ict_3xx_received (ict, evt->sip);
      else if (MSG_IS_STATUS_4XX (evt->sip))
	osip->cb_ict_4xx_received (ict, evt->sip);
      else if (MSG_IS_STATUS_5XX (evt->sip))
	osip->cb_ict_5xx_received (ict, evt->sip);
      else
	osip->cb_ict_6xx_received (ict, evt->sip);
    }

  /* TEST ME:
     sipevt = fifo_tryget(ict->transactionff);
     if (sipevt==NULL)
     {}
     else if (sipevt->sip==NULL)
     fifo_insert(ict->transactionff, sipevt);
     else if (MSG_IS_RESPONSE(sipevt->sip))
     {
     if (0==strcmp(sipevt->sip->strtline->statuscode, evt->sip->strtline->statuscode))
     {
     OSIP_TRACE (osip_trace
     (__FILE__, __LINE__, OSIP_WARNING, NULL,
     "discard this clone of late response... callid:%s\n",
     evt->sip->call_id->number));
     msg_free(sipevt->sip);
     sfree(sipevt->sip);
     sfree(sipevt);
     }
     else
     {
     fifo_insert(ict->transactionff, sipevt);
     }
     }
     else
     fifo_insert(ict->transactionff, sipevt);
   */

  /* start timer D (length is set to MAX (64*DEFAULT_T1 or 32000) */
  ict->ict_context->timer_d_start = time (NULL);
  transaction_set_state (ict, ICT_COMPLETED);
}

void
ict_timeout_d_event (transaction_t * ict, sipevent_t * evt)
{
  osip_t *osip = (osip_t *) ict->config;

  ict->ict_context->timer_d_length = -1;
  ict->ict_context->timer_d_start = -1;

  transaction_set_state (ict, ICT_TERMINATED);
  osip->cb_ict_kill_transaction (ict);
}

void
ict_retransmit_ack (transaction_t * ict, sipevent_t * evt)
{
  int i;
  osip_t *osip = (osip_t *) ict->config;

  /* this could be another 3456xx ??? */
  /* we should make a new ACK and send it!!! */
  /* TODO */

  if (osip->cb_ict_3456xx_received2 != NULL)
    osip->cb_ict_3456xx_received2 (ict, evt->sip);

  msg_free (evt->sip);
  sfree (evt->sip);

  if (ict->ack==NULL)
    {
      /* ERROR! */
      transaction_set_state (ict, ICT_TERMINATED);
      osip->cb_ict_kill_transaction (ict);
      /* TODO: MUST BE DELETED NOW */
      return;
    }

  i = osip->cb_send_message (ict, ict->ack, ict->ict_context->destination,
			     ict->ict_context->port, ict->out_socket);

  if (i == 0)
    {
      if (osip->cb_ict_ack_sent2 != NULL)
	osip->cb_ict_ack_sent2 (ict, ict->ack);
      transaction_set_state (ict, ICT_COMPLETED);
    }
  else
    {
      osip->cb_ict_transport_error (ict, i);
      transaction_set_state (ict, ICT_TERMINATED);
      osip->cb_ict_kill_transaction (ict);
      /* MUST BE DELETED NOW */
    }
}
