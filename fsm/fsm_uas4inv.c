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

static statemachine_t *uas_4inv;


statemachine_t *
fsm_getfsm_uas4inv()
{
      return uas_4inv;
}

int
fsm_load_uas4inv() {

  transition_t *transition;

  uas_4inv       = (statemachine_t *) smalloc(sizeof(statemachine_t));
  uas_4inv->transitions = (list_t *)  smalloc(sizeof(list_t));
  list_init(uas_4inv->transitions);

  transition         = (transition_t *) smalloc(sizeof(transition_t));
  transition->state  = INITIAL;
  transition->type  = RCV_REQINVITE;
  transition->method = (void(*)(void *,void *))&uas_rcvrequest;
  list_add(uas_4inv->transitions,transition,-1);

  transition         = (transition_t *) smalloc(sizeof(transition_t));
  transition->state  = PROCEEDING;
  transition->type  = RCV_REQINVITE;
  transition->method = (void(*)(void *,void *))&uas_i_rcvretransmitrequest;
  list_add(uas_4inv->transitions,transition,-1);

  transition         = (transition_t *) smalloc(sizeof(transition_t));
  transition->state  = PROCEEDING;
  transition->type  = SND_STATUS_1XX;
  transition->method = (void(*)(void *,void *))&uas_send1XX;
  list_add(uas_4inv->transitions,transition,-1);

  transition         = (transition_t *) smalloc(sizeof(transition_t));
  transition->state  = PROCEEDING;
  transition->type  = SND_STATUS_23456XX;
  transition->method = (void(*)(void *,void *))&uas_send23456XX;
  list_add(uas_4inv->transitions,transition,-1);

  transition         = (transition_t *) smalloc(sizeof(transition_t));
  transition->state  = ANSWERED;
  transition->type  = RCV_REQINVITE;
  transition->method = (void(*)(void *,void *))&uas_i_rcvretransmitrequest;
  list_add(uas_4inv->transitions,transition,-1);

  transition         = (transition_t *) smalloc(sizeof(transition_t));
  transition->state  = ANSWERED;
  transition->type  = TIMEOUT;
  transition->method = (void(*)(void *,void *))&uas_i_retransmitresponse;
  list_add(uas_4inv->transitions,transition,-1);

  transition         = (transition_t *) smalloc(sizeof(transition_t));
  transition->state  = ANSWERED;
  transition->type   = RCV_REQACK;
  transition->method = (void(*)(void *,void *))&uas_i_rcvack;
  list_add(uas_4inv->transitions,transition,-1);

  return 0;
}


void
uas_i_rcvretransmitrequest(sipevent_t *sipevent,transaction_t *transaction)
{
  ua_retransmitresponse(sipevent,transaction);
}


void
uas_i_retransmitresponse(sipevent_t *sipevent,transaction_t *transaction)
{
  if (udp_send_response(transaction->lastresponse)==-1)
    {
    TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,NULL,"Error while retransmitting final response"));
    if (((osip_t *)transaction->config)->cb_network_error != NULL)
      ((osip_t *)transaction->config)->cb_network_error(transaction);

    return;
    }

  /* invoke the right callback! */
    if (((osip_t *)transaction->config)->cb_sndresp_retransmission != NULL)
      ((osip_t *)transaction->config)->cb_sndresp_retransmission(transaction);
}

void
uas_i_rcvack(sipevent_t *sipevent,transaction_t *transaction)
{
  DEBUG(msg_logrequest(sipevent->sip,"RCV %s f:%s t:%s cseq:%s callid:%s\n"));
  if (transaction->lastrequest!=NULL)
    {
      msg_free(transaction->lastrequest);
      sfree(transaction->lastrequest);
    }
  transaction->lastrequest = sipevent->sip;

  /* invoke the right callback! */
  if (((osip_t *)transaction->config)->cb_rcvack != NULL)
    ((osip_t *)transaction->config)->cb_rcvack(sipevent,transaction);
  transaction->completed_time = time(NULL);
  transaction->state = COMPLETED;

  return ;
}

