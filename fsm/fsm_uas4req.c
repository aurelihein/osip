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

#ifdef __VXWORKS_OS__
#include <time.h>
#endif

static statemachine_t *uas_4req;


statemachine_t *
fsm_getfsm_uas4req()
{
  return uas_4req;
}

int
fsm_load_uas4req() {

  transition_t *transition;

  uas_4req       = (statemachine_t *) smalloc(sizeof(statemachine_t));
  uas_4req->transitions = (list_t *)  smalloc(sizeof(list_t));
  list_init(uas_4req->transitions);

  transition         = (transition_t *) smalloc(sizeof(transition_t));
  transition->state  = INITIAL;
  transition->type  = RCV_REQUEST;
  transition->method = (void(*)(void *,void *))&uas_rcvrequest;
  list_add(uas_4req->transitions,transition,-1);

  transition         = (transition_t *) smalloc(sizeof(transition_t));
  transition->state  = PROCEEDING;
  transition->type  = RCV_REQUEST;
  transition->method = (void(*)(void *,void *))&uas_r_rcvretransmitrequest;
  list_add(uas_4req->transitions,transition,-1);

  transition         = (transition_t *) smalloc(sizeof(transition_t));
  transition->state  = PROCEEDING;
  transition->type  = SND_STATUS_1XX;
  transition->method = (void(*)(void *,void *))&uas_send1XX;
  list_add(uas_4req->transitions,transition,-1);

  transition         = (transition_t *) smalloc(sizeof(transition_t));
  transition->state  = PROCEEDING;
  transition->type  = SND_STATUS_23456XX;
  transition->method = (void(*)(void *,void *))&uas_r_send23456XX;
  list_add(uas_4req->transitions,transition,-1);

  transition         = (transition_t *) smalloc(sizeof(transition_t));
  transition->state  = COMPLETED;
  transition->type  = RCV_REQUEST;
  transition->method = (void(*)(void *,void *))&uas_r_rcvretransmitrequest;
  list_add(uas_4req->transitions,transition,-1);

  return 0;
}


void
uas_rcvrequest(sipevent_t *sipevent,transaction_t *transaction)
{
  DEBUG(msg_logrequest(sipevent->sip,"RCV %s f:%s t:%s cseq:%s callid:%s\n"));

  transaction->lastrequest = sipevent->sip;
  transaction->state = PROCEEDING;

  transaction->retransmissioncounter = 1;
  fifo_add(((osip_t *)transaction->config)->uas_timerff,transaction);

  /* invoke the right callback! */
  if (MSG_IS_INVITE(sipevent->sip))
    {
      if (((osip_t *)transaction->config)->cb_rcvinvite != NULL)
	((osip_t *)transaction->config)->cb_rcvinvite(sipevent,transaction);
    }
  else if (MSG_IS_BYE(sipevent->sip))
    {
      if (((osip_t *)transaction->config)->cb_rcvbye != NULL)
	((osip_t *)transaction->config)->cb_rcvbye(sipevent,transaction);
    }
  else if (MSG_IS_REGISTER(sipevent->sip))
    {
      if (((osip_t *)transaction->config)->cb_rcvregister != NULL)
	((osip_t *)transaction->config)->cb_rcvregister(sipevent,transaction);
    }
  else if (MSG_IS_CANCEL(sipevent->sip))
    {
      if (((osip_t *)transaction->config)->cb_rcvcancel != NULL)
	((osip_t *)transaction->config)->cb_rcvcancel(sipevent,transaction);
    }
  else if (MSG_IS_OPTIONS(sipevent->sip))
    {
      if (((osip_t *)transaction->config)->cb_rcvoptions != NULL)
	((osip_t *)transaction->config)->cb_rcvoptions(sipevent,transaction);
    }
  else if (MSG_IS_INFO(sipevent->sip))
    {
      if (((osip_t *)transaction->config)->cb_rcvinfo != NULL)
	((osip_t *)transaction->config)->cb_rcvinfo(sipevent,transaction);
    }
  else if (MSG_IS_PRACK(sipevent->sip))
    {
      if (((osip_t *)transaction->config)->cb_rcvprack != NULL)
	((osip_t *)transaction->config)->cb_rcvprack(sipevent,transaction);
    }
  else
    {
      if (((osip_t *)transaction->config)->cb_rcvunkrequest != NULL)
	((osip_t *)transaction->config)->cb_rcvunkrequest(sipevent,transaction);
    }
}

void
uas_r_rcvretransmitrequest(sipevent_t *sipevent,transaction_t *transaction)
{
  ua_retransmitresponse(sipevent,transaction);
}

void
uas_r_send23456XX(sipevent_t *sipevent,transaction_t *transaction)
{
  if (uas_send23456XX(sipevent,transaction)==-1)
    return ;

  transaction->completed_time = time(NULL);
  transaction->state = COMPLETED;
}


/* COMMON METHODS FOR incoming INVITE and OTHERREQ */

void
ua_retransmitresponse(sipevent_t *sipevent,transaction_t *transaction)
{
  /* invoke the right callback! */
  if (((osip_t *)transaction->config)->cb_rcvreq_retransmission != NULL)
    ((osip_t *)transaction->config)->cb_rcvreq_retransmission(transaction);
  DEBUG(msg_logrequest(sipevent->sip,"RCV (retransmission) %s f:%s t:%s cseq:%s callid:%s\n"));
  
  if (transaction->lastresponse!=NULL)
    {
      int i;
      i = udp_send_response(transaction->lastresponse);
      if (i==-1)
	{
	  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,NULL,"Error Could not retransmit response.\n"));
	  if (((osip_t *)transaction->config)->cb_network_error != NULL)
	    ((osip_t *)transaction->config)->cb_network_error(transaction);
	}
      else
	/* invoke the right callback! */
	{
	  if (((osip_t *)transaction->config)->cb_sndresp_retransmission != NULL)
	    ((osip_t *)transaction->config)->cb_sndresp_retransmission(transaction);
	}
    }
  else
    /* if no response is given by user, retransmission has no effect */
    {
      TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL4,NULL,"Retransmission has no effect. (Response is not ready?)\n"));
    }

  msg_free(sipevent->sip);
  sfree(sipevent->sip);
}

void
uas_send1XX(sipevent_t *sipevent,transaction_t *transaction)
{
  int i;
  i = udp_send_response(sipevent->sip);
  if (i==-1)
    {
    TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,NULL,"Error Could not send new response.\n"));
    if (((osip_t *)transaction->config)->cb_network_error != NULL)
      ((osip_t *)transaction->config)->cb_network_error(transaction);
    msg_free(sipevent->sip);
    sfree(sipevent->sip);
    return ;
    }
  
  /* invoke the right callback! */
#ifndef EXTENDED_CALLBACKS
  if (((osip_t *)transaction->config)->cb_snd1xx != NULL)
    ((osip_t *)transaction->config)->cb_snd1xx(sipevent,transaction);
#else
  if (MSG_IS_RESPONSEFOR(sipevent->sip,"INVITE")) {
    if (((osip_t *)transaction->config)->cb_sndinvite1xx != NULL)
      ((osip_t *)transaction->config)->cb_sndinvite1xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"BYE")) {
    if (((osip_t *)transaction->config)->cb_sndbye1xx != NULL)
      ((osip_t *)transaction->config)->cb_sndbye1xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"CANCEL")) {
    if (((osip_t *)transaction->config)->cb_sndcancel1xx != NULL)
      ((osip_t *)transaction->config)->cb_sndcancel1xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"INFO")) {
    if (((osip_t *)transaction->config)->cb_sndinfo1xx != NULL)
      ((osip_t *)transaction->config)->cb_sndinfo1xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"OPTIONS")) {
    if (((osip_t *)transaction->config)->cb_sndoptions1xx != NULL)
      ((osip_t *)transaction->config)->cb_sndoptions1xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"REGISTER")) {
    if (((osip_t *)transaction->config)->cb_sndregister1xx != NULL)
      ((osip_t *)transaction->config)->cb_sndregister1xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"PRACK")) {
    if (((osip_t *)transaction->config)->cb_sndprack1xx != NULL)
      ((osip_t *)transaction->config)->cb_sndprack1xx(sipevent,transaction);
  } else {
    if (((osip_t *)transaction->config)->cb_sndunkrequest1xx != NULL)
      ((osip_t *)transaction->config)->cb_sndunkrequest1xx(sipevent,transaction);
  }
#endif

  if (transaction->lastresponse!=NULL)
    {
    msg_free(transaction->lastresponse);
    sfree(transaction->lastresponse);
    }
  transaction->lastresponse = sipevent->sip;
}

int
uas_send23456XX(sipevent_t *sipevent,transaction_t *transaction)
{
  int i;
  i = udp_send_response(sipevent->sip);
  if (i==-1)
    {
    TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,NULL,"response not sent.\n"));
    if (((osip_t *)transaction->config)->cb_network_error != NULL)
      ((osip_t *)transaction->config)->cb_network_error(transaction);
 
    msg_free(sipevent->sip);
    sfree(sipevent->sip);
    return -1;
    }
  
  if (transaction->lastresponse!=NULL)
    {
    msg_free(transaction->lastresponse);
    sfree(transaction->lastresponse);
    }
  transaction->lastresponse = sipevent->sip;

  /* AMD/ bug: fixed 26/09/2001 */
  if (MSG_IS_INVITE(transaction->lastrequest))
      transaction->state = ANSWERED;
  else /* for other requests type, it's all done! */
    {
      transaction->state = COMPLETED;
      transaction->completed_time = time(NULL);
    }

  /* invoke the right callback! */
  if (MSG_IS_STATUS_2XX(sipevent->sip))
    {
#ifndef EXTENDED_CALLBACKS
  if (((osip_t *)transaction->config)->cb_snd2xx != NULL)
    ((osip_t *)transaction->config)->cb_snd2xx(sipevent,transaction);
#else
  if (MSG_IS_RESPONSEFOR(sipevent->sip,"INVITE")) {
    if (((osip_t *)transaction->config)->cb_sndinvite2xx != NULL)
      ((osip_t *)transaction->config)->cb_sndinvite2xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"BYE")) {
    if (((osip_t *)transaction->config)->cb_sndbye2xx != NULL)
      ((osip_t *)transaction->config)->cb_sndbye2xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"CANCEL")) {
    if (((osip_t *)transaction->config)->cb_sndcancel2xx != NULL)
      ((osip_t *)transaction->config)->cb_sndcancel2xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"INFO")) {
    if (((osip_t *)transaction->config)->cb_sndinfo2xx != NULL)
      ((osip_t *)transaction->config)->cb_sndinfo2xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"OPTIONS")) {
    if (((osip_t *)transaction->config)->cb_sndoptions2xx != NULL)
      ((osip_t *)transaction->config)->cb_sndoptions2xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"REGISTER")) {
    if (((osip_t *)transaction->config)->cb_sndregister2xx != NULL)
      ((osip_t *)transaction->config)->cb_sndregister2xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"PRACK")) {
    if (((osip_t *)transaction->config)->cb_sndprack2xx != NULL)
      ((osip_t *)transaction->config)->cb_sndprack2xx(sipevent,transaction);
  } else {
    if (((osip_t *)transaction->config)->cb_sndunkrequest2xx != NULL)
      ((osip_t *)transaction->config)->cb_sndunkrequest2xx(sipevent,transaction);
  }
#endif
    }
  else if (MSG_IS_STATUS_3XX(sipevent->sip))
    {
#ifndef EXTENDED_CALLBACKS
  if (((osip_t *)transaction->config)->cb_snd3xx != NULL)
    ((osip_t *)transaction->config)->cb_snd3xx(sipevent,transaction);
#else
  if (MSG_IS_RESPONSEFOR(sipevent->sip,"INVITE")) {
    if (((osip_t *)transaction->config)->cb_sndinvite3xx != NULL)
      ((osip_t *)transaction->config)->cb_sndinvite3xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"BYE")) {
    if (((osip_t *)transaction->config)->cb_sndbye3xx != NULL)
      ((osip_t *)transaction->config)->cb_sndbye3xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"CANCEL")) {
    if (((osip_t *)transaction->config)->cb_sndcancel3xx != NULL)
      ((osip_t *)transaction->config)->cb_sndcancel3xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"INFO")) {
    if (((osip_t *)transaction->config)->cb_sndinfo3xx != NULL)
      ((osip_t *)transaction->config)->cb_sndinfo3xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"OPTIONS")) {
    if (((osip_t *)transaction->config)->cb_sndoptions3xx != NULL)
      ((osip_t *)transaction->config)->cb_sndoptions3xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"REGISTER")) {
    if (((osip_t *)transaction->config)->cb_sndregister3xx != NULL)
      ((osip_t *)transaction->config)->cb_sndregister3xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"PRACK")) {
    if (((osip_t *)transaction->config)->cb_sndprack3xx != NULL)
      ((osip_t *)transaction->config)->cb_sndprack3xx(sipevent,transaction);
  } else {
    if (((osip_t *)transaction->config)->cb_sndunkrequest3xx != NULL)
      ((osip_t *)transaction->config)->cb_sndunkrequest3xx(sipevent,transaction);
  }
#endif
    }
  else if (MSG_IS_STATUS_4XX(sipevent->sip))
    {
#ifndef EXTENDED_CALLBACKS
  if (((osip_t *)transaction->config)->cb_snd4xx != NULL)
    ((osip_t *)transaction->config)->cb_snd4xx(sipevent,transaction);
#else
  if (MSG_IS_RESPONSEFOR(sipevent->sip,"INVITE")) {
    if (((osip_t *)transaction->config)->cb_sndinvite4xx != NULL)
      ((osip_t *)transaction->config)->cb_sndinvite4xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"BYE")) {
    if (((osip_t *)transaction->config)->cb_sndbye4xx != NULL)
      ((osip_t *)transaction->config)->cb_sndbye4xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"CANCEL")) {
    if (((osip_t *)transaction->config)->cb_sndcancel4xx != NULL)
      ((osip_t *)transaction->config)->cb_sndcancel4xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"INFO")) {
    if (((osip_t *)transaction->config)->cb_sndinfo4xx != NULL)
      ((osip_t *)transaction->config)->cb_sndinfo4xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"OPTIONS")) {
    if (((osip_t *)transaction->config)->cb_sndoptions4xx != NULL)
      ((osip_t *)transaction->config)->cb_sndoptions4xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"REGISTER")) {
    if (((osip_t *)transaction->config)->cb_sndregister4xx != NULL)
      ((osip_t *)transaction->config)->cb_sndregister4xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"PRACK")) {
    if (((osip_t *)transaction->config)->cb_sndprack4xx != NULL)
      ((osip_t *)transaction->config)->cb_sndprack4xx(sipevent,transaction);
  } else {
    if (((osip_t *)transaction->config)->cb_sndunkrequest4xx != NULL)
      ((osip_t *)transaction->config)->cb_sndunkrequest4xx(sipevent,transaction);
  }
#endif
    }
  else if (MSG_IS_STATUS_5XX(sipevent->sip))
    {
#ifndef EXTENDED_CALLBACKS
  if (((osip_t *)transaction->config)->cb_snd5xx != NULL)
    ((osip_t *)transaction->config)->cb_snd5xx(sipevent,transaction);
#else
  if (MSG_IS_RESPONSEFOR(sipevent->sip,"INVITE")) {
    if (((osip_t *)transaction->config)->cb_sndinvite5xx != NULL)
      ((osip_t *)transaction->config)->cb_sndinvite5xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"BYE")) {
    if (((osip_t *)transaction->config)->cb_sndbye5xx != NULL)
      ((osip_t *)transaction->config)->cb_sndbye5xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"CANCEL")) {
    if (((osip_t *)transaction->config)->cb_sndcancel5xx != NULL)
      ((osip_t *)transaction->config)->cb_sndcancel5xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"INFO")) {
    if (((osip_t *)transaction->config)->cb_sndinfo5xx != NULL)
      ((osip_t *)transaction->config)->cb_sndinfo5xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"OPTIONS")) {
    if (((osip_t *)transaction->config)->cb_sndoptions5xx != NULL)
      ((osip_t *)transaction->config)->cb_sndoptions5xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"REGISTER")) {
    if (((osip_t *)transaction->config)->cb_sndregister5xx != NULL)
      ((osip_t *)transaction->config)->cb_sndregister5xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"PRACK")) {
    if (((osip_t *)transaction->config)->cb_sndprack5xx != NULL)
      ((osip_t *)transaction->config)->cb_sndprack5xx(sipevent,transaction);
  } else {
    if (((osip_t *)transaction->config)->cb_sndunkrequest5xx != NULL)
      ((osip_t *)transaction->config)->cb_sndunkrequest5xx(sipevent,transaction);
  }
#endif
    }
  else if (MSG_IS_STATUS_6XX(sipevent->sip))
    {
#ifndef EXTENDED_CALLBACKS
  if (((osip_t *)transaction->config)->cb_snd6xx != NULL)
    ((osip_t *)transaction->config)->cb_snd6xx(sipevent,transaction);
#else
  if (MSG_IS_RESPONSEFOR(sipevent->sip,"INVITE")) {
    if (((osip_t *)transaction->config)->cb_sndinvite6xx != NULL)
      ((osip_t *)transaction->config)->cb_sndinvite6xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"BYE")) {
    if (((osip_t *)transaction->config)->cb_sndbye6xx != NULL)
      ((osip_t *)transaction->config)->cb_sndbye6xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"CANCEL")) {
    if (((osip_t *)transaction->config)->cb_sndcancel6xx != NULL)
      ((osip_t *)transaction->config)->cb_sndcancel6xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"INFO")) {
    if (((osip_t *)transaction->config)->cb_sndinfo6xx != NULL)
      ((osip_t *)transaction->config)->cb_sndinfo6xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"OPTIONS")) {
    if (((osip_t *)transaction->config)->cb_sndoptions6xx != NULL)
      ((osip_t *)transaction->config)->cb_sndoptions6xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"REGISTER")) {
    if (((osip_t *)transaction->config)->cb_sndregister6xx != NULL)
      ((osip_t *)transaction->config)->cb_sndregister6xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"PRACK")) {
    if (((osip_t *)transaction->config)->cb_sndprack6xx != NULL)
      ((osip_t *)transaction->config)->cb_sndprack6xx(sipevent,transaction);
  } else {
    if (((osip_t *)transaction->config)->cb_sndunkrequest6xx != NULL)
      ((osip_t *)transaction->config)->cb_sndunkrequest6xx(sipevent,transaction);
  }
#endif
    }
  else
    { TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,NULL,"Unknown response code?\n"));}
  return 0;
}
