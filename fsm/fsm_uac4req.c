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

static statemachine_t *uac_4req;


statemachine_t *
fsm_getfsm_uac4req()
{
  return uac_4req;
}

int 
fsm_load_uac4req() {

  transition_t *transition;

  uac_4req       = (statemachine_t *) smalloc(sizeof(statemachine_t));
  uac_4req->transitions = (list_t *)  smalloc(sizeof(list_t));
  list_init(uac_4req->transitions);

  transition         = (transition_t *) smalloc(sizeof(transition_t));
  transition->state  = INITIAL;
  transition->type   = SND_REQUEST;
  transition->method = (void(*)(void *,void *))&uac_r_sendrequest;
  list_add(uac_4req->transitions,transition,-1);

  transition         = (transition_t *) smalloc(sizeof(transition_t));
  transition->state  = CALLING;
  transition->type  = TIMEOUT;
  transition->method = (void(*)(void *,void *))&uac_r_retransmitrequest;
  list_add(uac_4req->transitions,transition,-1);

  transition         = (transition_t *) smalloc(sizeof(transition_t));
  transition->state  = CALLING;
  transition->type  = RCV_STATUS_1XX;
  transition->method = (void(*)(void *,void *))&uac_rcv1XX;
  list_add(uac_4req->transitions,transition,-1);

  transition         = (transition_t *) smalloc(sizeof(transition_t));
  transition->state  = CALLING;
  transition->type  = RCV_STATUS_23456XX;
  transition->method = (void(*)(void *,void *))&uac_rcv23456XX;
  list_add(uac_4req->transitions,transition,-1);

  transition         = (transition_t *) smalloc(sizeof(transition_t));
  transition->state  = PROCEEDING;
  transition->type  = RCV_STATUS_1XX;
  transition->method = (void(*)(void *,void *))&uac_rcv1XX;
  list_add(uac_4req->transitions,transition,-1);

  transition         = (transition_t *) smalloc(sizeof(transition_t));
  transition->state  = PROCEEDING;
  transition->type  = RCV_STATUS_23456XX;
  transition->method = (void(*)(void *,void *))&uac_rcv23456XX;
  list_add(uac_4req->transitions,transition,-1);

  return 0;
}



/* Called when STATE=INITIAL and EVENT=SND_REQINVITE */
void
uac_r_sendrequest(sipevent_t *sipevent,transaction_t *transaction)
{
  ua_sendrequest(sipevent,transaction);

  /* always inform the timer of new transactions */
  transaction->retransmissioncounter = 1;
  fifo_add(((osip_t *)transaction->config)->uac_timerff,transaction);
  return ;    
}

/* Called when STATE=CALLING and EVENT=TIMEOUT */
void
uac_r_retransmitrequest(sipevent_t *sipevent,transaction_t *transaction)
{
  ua_retransmitrequest(sipevent,transaction);
}

/* Called when STATE=CALLING and EVENT=RCV_STATUS_1XX */
void
uac_rcv1XX(sipevent_t *sipevent,transaction_t *transaction)
{
  ua_rcvresponse(sipevent,transaction);

  transaction->state = PROCEEDING;
  /* invoke the right callback! */
#ifndef EXTENDED_CALLBACKS
  if (((osip_t *)transaction->config)->cb_rcv1xx != NULL)
    ((osip_t *)transaction->config)->cb_rcv1xx(sipevent,transaction);
#else
  if (MSG_IS_RESPONSEFOR(sipevent->sip,"INVITE")) {
    if (((osip_t *)transaction->config)->cb_rcvinvite1xx != NULL)
      ((osip_t *)transaction->config)->cb_rcvinvite1xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"BYE")) {
    if (((osip_t *)transaction->config)->cb_rcvbye1xx != NULL)
      ((osip_t *)transaction->config)->cb_rcvbye1xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"CANCEL")) {
    if (((osip_t *)transaction->config)->cb_rcvcancel1xx != NULL)
      ((osip_t *)transaction->config)->cb_rcvcancel1xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"INFO")) {
    if (((osip_t *)transaction->config)->cb_rcvinfo1xx != NULL)
      ((osip_t *)transaction->config)->cb_rcvinfo1xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"OPTIONS")) {
    if (((osip_t *)transaction->config)->cb_rcvoptions1xx != NULL)
      ((osip_t *)transaction->config)->cb_rcvoptions1xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"REGISTER")) {
    if (((osip_t *)transaction->config)->cb_rcvregister1xx != NULL)
      ((osip_t *)transaction->config)->cb_rcvregister1xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"PRACK")) {
    if (((osip_t *)transaction->config)->cb_rcvprack1xx != NULL)
      ((osip_t *)transaction->config)->cb_rcvprack1xx(sipevent,transaction);
  } else {
    if (((osip_t *)transaction->config)->cb_rcvunkrequest1xx != NULL)
      ((osip_t *)transaction->config)->cb_rcvunkrequest1xx(sipevent,transaction);
  }
#endif

  return ; /* OK */
}

/* Called when STATE=CALLING||PROCEEDING and EVENT=RCV_STATUS_3456XX */
void
uac_rcv23456XX(sipevent_t *sipevent,transaction_t *transaction)
{
  ua_rcvresponse(sipevent,transaction);
  
  /* AMD/ bug: fixed 26/09/2001 */ 
  if (MSG_IS_INVITE(transaction->lastrequest))
      transaction->state = WAITACK;
  else /* for other request, it's all done! */
    {
      transaction->completed_time = time(NULL);
      transaction->state = COMPLETED;
    }

  /* invoke the right callback! */
  if (MSG_IS_STATUS_2XX(sipevent->sip))
    {
#ifndef EXTENDED_CALLBACKS
  if (((osip_t *)transaction->config)->cb_rcv2xx != NULL)
    ((osip_t *)transaction->config)->cb_rcv2xx(sipevent,transaction);
#else
  if (MSG_IS_RESPONSEFOR(sipevent->sip,"INVITE")) {
    if (((osip_t *)transaction->config)->cb_rcvinvite2xx != NULL)
      ((osip_t *)transaction->config)->cb_rcvinvite2xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"BYE")) {
    if (((osip_t *)transaction->config)->cb_rcvbye2xx != NULL)
      ((osip_t *)transaction->config)->cb_rcvbye2xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"CANCEL")) {
    if (((osip_t *)transaction->config)->cb_rcvcancel2xx != NULL)
      ((osip_t *)transaction->config)->cb_rcvcancel2xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"INFO")) {
    if (((osip_t *)transaction->config)->cb_rcvinfo2xx != NULL)
      ((osip_t *)transaction->config)->cb_rcvinfo2xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"OPTIONS")) {
    if (((osip_t *)transaction->config)->cb_rcvoptions2xx != NULL)
      ((osip_t *)transaction->config)->cb_rcvoptions2xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"REGISTER")) {
    if (((osip_t *)transaction->config)->cb_rcvregister2xx != NULL)
      ((osip_t *)transaction->config)->cb_rcvregister2xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"PRACK")) {
    if (((osip_t *)transaction->config)->cb_rcvprack2xx != NULL)
      ((osip_t *)transaction->config)->cb_rcvprack2xx(sipevent,transaction);
  } else {
    if (((osip_t *)transaction->config)->cb_rcvunkrequest2xx != NULL)
      ((osip_t *)transaction->config)->cb_rcvunkrequest2xx(sipevent,transaction);
  }
#endif
    }
  else if (MSG_IS_STATUS_3XX(sipevent->sip))
    {
#ifndef EXTENDED_CALLBACKS
  if (((osip_t *)transaction->config)->cb_rcv3xx != NULL)
    ((osip_t *)transaction->config)->cb_rcv3xx(sipevent,transaction);
#else
  if (MSG_IS_RESPONSEFOR(sipevent->sip,"INVITE")) {
    if (((osip_t *)transaction->config)->cb_rcvinvite3xx != NULL)
      ((osip_t *)transaction->config)->cb_rcvinvite3xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"BYE")) {
    if (((osip_t *)transaction->config)->cb_rcvbye3xx != NULL)
      ((osip_t *)transaction->config)->cb_rcvbye3xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"CANCEL")) {
    if (((osip_t *)transaction->config)->cb_rcvcancel3xx != NULL)
      ((osip_t *)transaction->config)->cb_rcvcancel3xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"INFO")) {
    if (((osip_t *)transaction->config)->cb_rcvinfo3xx != NULL)
      ((osip_t *)transaction->config)->cb_rcvinfo3xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"OPTIONS")) {
    if (((osip_t *)transaction->config)->cb_rcvoptions3xx != NULL)
      ((osip_t *)transaction->config)->cb_rcvoptions3xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"REGISTER")) {
    if (((osip_t *)transaction->config)->cb_rcvregister3xx != NULL)
      ((osip_t *)transaction->config)->cb_rcvregister3xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"PRACK")) {
    if (((osip_t *)transaction->config)->cb_rcvprack3xx != NULL)
      ((osip_t *)transaction->config)->cb_rcvprack3xx(sipevent,transaction);
  } else {
    if (((osip_t *)transaction->config)->cb_rcvunkrequest3xx != NULL)
      ((osip_t *)transaction->config)->cb_rcvunkrequest3xx(sipevent,transaction);
  }
#endif
    }
  else if (MSG_IS_STATUS_4XX(sipevent->sip))
    {
#ifndef EXTENDED_CALLBACKS
  if (((osip_t *)transaction->config)->cb_rcv4xx != NULL)
    ((osip_t *)transaction->config)->cb_rcv4xx(sipevent,transaction);
#else
  if (MSG_IS_RESPONSEFOR(sipevent->sip,"INVITE")) {
    if (((osip_t *)transaction->config)->cb_rcvinvite4xx != NULL)
      ((osip_t *)transaction->config)->cb_rcvinvite4xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"BYE")) {
    if (((osip_t *)transaction->config)->cb_rcvbye4xx != NULL)
      ((osip_t *)transaction->config)->cb_rcvbye4xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"CANCEL")) {
    if (((osip_t *)transaction->config)->cb_rcvcancel4xx != NULL)
      ((osip_t *)transaction->config)->cb_rcvcancel4xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"INFO")) {
    if (((osip_t *)transaction->config)->cb_rcvinfo4xx != NULL)
      ((osip_t *)transaction->config)->cb_rcvinfo4xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"OPTIONS")) {
    if (((osip_t *)transaction->config)->cb_rcvoptions4xx != NULL)
      ((osip_t *)transaction->config)->cb_rcvoptions4xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"REGISTER")) {
    if (((osip_t *)transaction->config)->cb_rcvregister4xx != NULL)
      ((osip_t *)transaction->config)->cb_rcvregister4xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"PRACK")) {
    if (((osip_t *)transaction->config)->cb_rcvprack4xx != NULL)
      ((osip_t *)transaction->config)->cb_rcvprack4xx(sipevent,transaction);
  } else {
    if (((osip_t *)transaction->config)->cb_rcvunkrequest4xx != NULL)
      ((osip_t *)transaction->config)->cb_rcvunkrequest4xx(sipevent,transaction);
  }
#endif
    }
  else if (MSG_IS_STATUS_5XX(sipevent->sip))
    {
#ifndef EXTENDED_CALLBACKS
  if (((osip_t *)transaction->config)->cb_rcv5xx != NULL)
    ((osip_t *)transaction->config)->cb_rcv5xx(sipevent,transaction);
#else
  if (MSG_IS_RESPONSEFOR(sipevent->sip,"INVITE")) {
    if (((osip_t *)transaction->config)->cb_rcvinvite5xx != NULL)
      ((osip_t *)transaction->config)->cb_rcvinvite5xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"BYE")) {
    if (((osip_t *)transaction->config)->cb_rcvbye5xx != NULL)
      ((osip_t *)transaction->config)->cb_rcvbye5xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"CANCEL")) {
    if (((osip_t *)transaction->config)->cb_rcvcancel5xx != NULL)
      ((osip_t *)transaction->config)->cb_rcvcancel5xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"INFO")) {
    if (((osip_t *)transaction->config)->cb_rcvinfo5xx != NULL)
      ((osip_t *)transaction->config)->cb_rcvinfo5xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"OPTIONS")) {
    if (((osip_t *)transaction->config)->cb_rcvoptions5xx != NULL)
      ((osip_t *)transaction->config)->cb_rcvoptions5xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"REGISTER")) {
    if (((osip_t *)transaction->config)->cb_rcvregister5xx != NULL)
      ((osip_t *)transaction->config)->cb_rcvregister5xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"PRACK")) {
    if (((osip_t *)transaction->config)->cb_rcvprack5xx != NULL)
      ((osip_t *)transaction->config)->cb_rcvprack5xx(sipevent,transaction);
  } else {
    if (((osip_t *)transaction->config)->cb_rcvunkrequest5xx != NULL)
      ((osip_t *)transaction->config)->cb_rcvunkrequest5xx(sipevent,transaction);
  }
#endif
    }
  else if (MSG_IS_STATUS_6XX(sipevent->sip))
    {
#ifndef EXTENDED_CALLBACKS
  if (((osip_t *)transaction->config)->cb_rcv6xx != NULL)
    ((osip_t *)transaction->config)->cb_rcv6xx(sipevent,transaction);
#else
  if (MSG_IS_RESPONSEFOR(sipevent->sip,"INVITE")) {
    if (((osip_t *)transaction->config)->cb_rcvinvite6xx != NULL)
      ((osip_t *)transaction->config)->cb_rcvinvite6xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"BYE")) {
    if (((osip_t *)transaction->config)->cb_rcvbye6xx != NULL)
      ((osip_t *)transaction->config)->cb_rcvbye6xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"CANCEL")) {
    if (((osip_t *)transaction->config)->cb_rcvcancel6xx != NULL)
      ((osip_t *)transaction->config)->cb_rcvcancel6xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"INFO")) {
    if (((osip_t *)transaction->config)->cb_rcvinfo6xx != NULL)
      ((osip_t *)transaction->config)->cb_rcvinfo6xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"OPTIONS")) {
    if (((osip_t *)transaction->config)->cb_rcvoptions6xx != NULL)
      ((osip_t *)transaction->config)->cb_rcvoptions6xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"REGISTER")) {
    if (((osip_t *)transaction->config)->cb_rcvregister6xx != NULL)
      ((osip_t *)transaction->config)->cb_rcvregister6xx(sipevent,transaction);
  } else if (MSG_IS_RESPONSEFOR(sipevent->sip,"PRACK")) {
    if (((osip_t *)transaction->config)->cb_rcvprack6xx != NULL)
      ((osip_t *)transaction->config)->cb_rcvprack6xx(sipevent,transaction);
  } else {
    if (((osip_t *)transaction->config)->cb_rcvunkrequest6xx != NULL)
      ((osip_t *)transaction->config)->cb_rcvunkrequest6xx(sipevent,transaction);
  }
#endif
    }  

  return ; /* OK */
}

void
ua_sendrequest(sipevent_t *sipevent,transaction_t *transaction)
{
  int i;
  i = udp_send_request(sipevent->sip,
		       transaction->proxy);
  if (i==-1)
    { 
      transaction->state=NETWORK_ERROR;
      /* invoke the right callback! */
      /* err must be tested.... NOT USABLE
	 if (code==SIP_ECONNREFUSED)
	 {
	 if (((osip_t *)transaction->config)->cb_connection_refused != NULL)
	 ((osip_t *)transaction->config)->cb_connection_refused(transaction);
	 }

	 else */
	{
	  if (((osip_t *)transaction->config)->cb_network_error != NULL)
	    ((osip_t *)transaction->config)->cb_network_error(transaction);
	}
      
      msg_free(sipevent->sip);
      sfree(sipevent->sip);
      return ;
    }
  
  transaction->lastrequest = sipevent->sip;
  transaction->state = CALLING;

  /* invoke the right callback! */
  if (MSG_IS_INVITE(sipevent->sip))
    {
      if (((osip_t *)transaction->config)->cb_sndinvite != NULL)
	((osip_t *)transaction->config)->cb_sndinvite(sipevent,transaction);
    }
  else if (MSG_IS_BYE(sipevent->sip))
    {
      if (((osip_t *)transaction->config)->cb_sndbye != NULL)
	((osip_t *)transaction->config)->cb_sndbye(sipevent,transaction);
    }
  else if (MSG_IS_REGISTER(sipevent->sip))
    {
      if (((osip_t *)transaction->config)->cb_sndregister != NULL)
	((osip_t *)transaction->config)->cb_sndregister(sipevent,transaction);
    }
  else if (MSG_IS_CANCEL(sipevent->sip))
    {
      if (((osip_t *)transaction->config)->cb_sndcancel != NULL)
	((osip_t *)transaction->config)->cb_sndcancel(sipevent,transaction);
    }
  else if (MSG_IS_INFO(sipevent->sip))
    {
      if (((osip_t *)transaction->config)->cb_sndinfo != NULL)
	((osip_t *)transaction->config)->cb_sndinfo(sipevent,transaction);
    }
  else if (MSG_IS_OPTIONS(sipevent->sip))
    {
      if (((osip_t *)transaction->config)->cb_sndoptions != NULL)
	((osip_t *)transaction->config)->cb_sndoptions(sipevent,transaction);
    }
  else if (MSG_IS_PRACK(sipevent->sip))
    {
      if (((osip_t *)transaction->config)->cb_sndprack != NULL)
	((osip_t *)transaction->config)->cb_sndprack(sipevent,transaction);
    }
  else
    {
      if (((osip_t *)transaction->config)->cb_sndunkrequest != NULL)
	((osip_t *)transaction->config)->cb_sndunkrequest(sipevent,transaction);
    }
  return;
}

void
ua_retransmitrequest(sipevent_t *sipevent,transaction_t *transaction)
{
  int i;
  i =   udp_send_request(transaction->lastrequest,
			 transaction->proxy);
  if (i==-1)
    {
      transaction->state=NETWORK_ERROR;
      /*
	if (code==SIP_ECONNREFUSED)
	{
	if (((osip_t *)transaction->config)->cb_connection_refused != NULL)
	((osip_t *)transaction->config)->cb_connection_refused(transaction);
	}
	else
	{
	TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,NULL,"retransmission of request failed.\n")); */
      if (((osip_t *)transaction->config)->cb_network_error != NULL)
	((osip_t *)transaction->config)->cb_network_error(transaction);
	/*} */
      return ;
    }

  /* invoke the right callback! */
  if (((osip_t *)transaction->config)->cb_sndreq_retransmission != NULL)
    ((osip_t *)transaction->config)->cb_sndreq_retransmission(transaction);

  return; /* OK */
}

void
ua_rcvresponse(sipevent_t *sipevent,transaction_t *transaction)
{
  if (transaction->lastresponse!=NULL)
    {
    msg_free(transaction->lastresponse);
    sfree(transaction->lastresponse);
    } /* else this is the first response received */

  transaction->lastresponse = sipevent->sip;
  DEBUG(msg_logresponse(transaction->lastresponse,"RCV %s %s (%s) f:%s t:%s cseq:%s callid:%s\n"));

  return ; /* OK */
}



