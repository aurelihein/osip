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

#include <stdlib.h>
#include <stdio.h>

#include "fsm.h"
#include <osip/port.h>
#include <osip/ua.h>

static statemachine_t *uac_4inv;



statemachine_t *
fsm_getfsm_uac4inv()
{
      return uac_4inv;
}

int 
fsm_load_uac4inv() {

  transition_t *transition;

  uac_4inv       = (statemachine_t *) smalloc(sizeof(statemachine_t));
  uac_4inv->transitions = (list_t *)  smalloc(sizeof(list_t));
  list_init(uac_4inv->transitions);

  transition         = (transition_t *) smalloc(sizeof(transition_t));
  transition->state  = INITIAL;
  transition->type  = SND_REQINVITE;
  transition->method = (void(*)(void *,void *))&uac_i_sendinvite;
  list_add(uac_4inv->transitions,transition,-1);

  transition         = (transition_t *) smalloc(sizeof(transition_t));
  transition->state  = CALLING;
  transition->type  = TIMEOUT;
  transition->method = (void(*)(void *,void *))&uac_i_retransmitinvite;
  list_add(uac_4inv->transitions,transition,-1);

  transition         = (transition_t *) smalloc(sizeof(transition_t));
  transition->state  = CALLING;
  transition->type  = RCV_STATUS_1XX;
  transition->method = (void(*)(void *,void *))&uac_rcv1XX;
  list_add(uac_4inv->transitions,transition,-1);

  transition         = (transition_t *) smalloc(sizeof(transition_t));
  transition->state  = CALLING;
  transition->type   = RCV_STATUS_23456XX;
  transition->method = (void(*)(void *,void *))&uac_rcv23456XX;
  list_add(uac_4inv->transitions,transition,-1);

  transition         = (transition_t *) smalloc(sizeof(transition_t));
  transition->state  = PROCEEDING;
  transition->type  = RCV_STATUS_1XX;
  transition->method = (void(*)(void *,void *))&uac_rcv1XX;
  list_add(uac_4inv->transitions,transition,-1);

  transition         = (transition_t *) smalloc(sizeof(transition_t));
  transition->state  = PROCEEDING;
  transition->type  = RCV_STATUS_23456XX;
  transition->method = (void(*)(void *,void *))&uac_rcv23456XX;
  list_add(uac_4inv->transitions,transition,-1);

  transition         = (transition_t *) smalloc(sizeof(transition_t));
  transition->state  = WAITACK;
  transition->type   = SND_REQACK;
  transition->method = (void(*)(void *,void *))&uac_i_sendack;
  list_add(uac_4inv->transitions,transition,-1);

  transition         = (transition_t *) smalloc(sizeof(transition_t));
  transition->state  = COMPLETED;
  transition->type   = RCV_STATUS_23456XX;
  transition->method = (void(*)(void *,void *))&uac_i_retransmitack;
  list_add(uac_4inv->transitions,transition,-1);
  return 0;
}

/* Called when STATE=INITIAL and EVENT=SND_REQINVITE */
void
uac_i_sendinvite(sipevent_t *sipevent,transaction_t *transaction)
{
  ua_sendrequest(sipevent,transaction);

  /* always inform the timer of new transactions */
  transaction->retransmissioncounter = 1;
  fifo_add(((osip_t *)transaction->config)->uac_timerff,transaction);
}

/* Called when STATE=CALLING and EVENT=TIMEOUT */
void
uac_i_retransmitinvite(sipevent_t *sipevent,transaction_t *transaction)
{
  ua_retransmitrequest(sipevent,transaction);
}

/* Called when STATE=COMPLETED and EVENT=RCV_STATUS_2XX||RCV_STATUS_3456XX */
void
uac_i_retransmitack(sipevent_t *sipevent,transaction_t *transaction)
{
  if (!MSG_IS_ACK(transaction->lastrequest))
    { /* upper layer must give the ACK before... */
      TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,NULL,"warning: Upper layer must provide an ACK request (quickly:-) %s %s!\n",transaction->callid->number,transaction->lastrequest->strtline->sipmethod));
    }
  else
    {
      int i;
      if (((osip_t *)transaction->config)->cb_rcvresp_retransmission != NULL)
	((osip_t *)transaction->config)->cb_rcvresp_retransmission(transaction);
      i = udp_send_request(transaction->lastrequest,
			   transaction->proxy);
      if (i==-1)
	{
	  transaction->state=NETWORK_ERROR;
	  /* invoke the right callback! */
	  if (((osip_t *)transaction->config)->cb_network_error != NULL)
	    ((osip_t *)transaction->config)->cb_network_error(transaction);

	  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,NULL,"ack retransmission failed.\n"));
	}
      else
	{
	  /* invoke the right callback! */
	  if (((osip_t *)transaction->config)->cb_sndreq_retransmission != NULL)
	    ((osip_t *)transaction->config)->cb_sndreq_retransmission(transaction);
	  /* update time */
	  transaction->completed_time = time(NULL);
	  transaction->state = COMPLETED; /* we are yet in this state! */
	}
    }

  /* free useless data (retransmission of last response) */
  msg_free(sipevent->sip);
  sfree(sipevent->sip);
  return; /* OK */
}

/* Called when STATE=WAITACK and EVENT=SND_REQACK */
void
uac_i_sendack(sipevent_t *sipevent,transaction_t *transaction)
{
  int i;
  i = udp_send_request(sipevent->sip,
		       transaction->proxy);
  if (i==-1)
    {
    TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,NULL,"ack not sent.\n"));

    msg_free(sipevent->sip);
    sfree(sipevent->sip);
    return;
    }
  
  if (transaction->lastrequest!=NULL)
    { /* it is the invite! */
      msg_free(transaction->lastrequest);
      sfree(transaction->lastrequest);
    }
  transaction->lastrequest = sipevent->sip;

  /* invoke the right callback! */
  if (((osip_t *)transaction->config)->cb_sndack != NULL)
    ((osip_t *)transaction->config)->cb_sndack(sipevent,transaction);
  transaction->completed_time = time(NULL);
  transaction->state = COMPLETED;

  return; /* OK */
}
