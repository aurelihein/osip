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

#ifndef _FSM_H_
#define _FSM_H_

#include <osip/smsg.h>
#include <osip/osip.h>

int  osip_init_timers(osip_t *osip);

int call_id_match(call_id_t *callid1,call_id_t *callid2);
int callleg_match(to_t *to1,from_t *from1,to_t *to2,from_t *from2);
int cseq_match(cseq_t *cseq1,cseq_t *cseq2);

/* This is for internal use only.                      */
type_t 	       evt_settype_incoming_sipmessage   (sip_t *sip);
type_t 	       evt_settype_outgoing_sipmessage   (sip_t *sip);

/* send a request on UDP.                       */
int            udp_send_request(sip_t *request, url_t *proxy);
/* send a response on UDP.                       */
int            udp_send_response(sip_t *response);

int fsm_load_uac4inv();
int fsm_load_uac4req();
int fsm_load_uas4inv();
int fsm_load_uas4req();

/* load state machine structure for (UAC || UAS) for (INVITE || REQUEST) */
statemachine_t* fsm_getfsm_uas4inv();
statemachine_t* fsm_getfsm_uas4req();

statemachine_t* fsm_getfsm_uac4inv();
statemachine_t* fsm_getfsm_uac4req();

typedef struct _transition_t {
  state_t   state;
  type_t    type;
  void       (*method)(void *,void *);
} transition_t;

int  fsm_callmethod(type_t type,state_t state,
		    statemachine_t *statemachine,
		    void *sipevent,void *transaction);


/*!! THESE ARE FOR INTERNAL USE ONLY!! */
/* These methods are the "exection method" for the finite */
/* state machine.                                         */

/************************/
/* FSM  ---- > UAC4INV  */
/************************/

/* Called when STATE=INITIAL and EVENT=SND_REQINVITE */
void            uac_i_sendinvite(sipevent_t *sipevent
			   ,transaction_t *transaction);
/* Called when STATE=CALLING and EVENT=TIMEOUT */
void            uac_i_retransmitinvite(sipevent_t *sipevent
			   ,transaction_t *transaction);
/* Called when STATE=COMPLETED and EVENT=RCV_STATUS_2XX||RCV_STATUS_3456XX */
void            uac_i_retransmitack(sipevent_t *sipevent
			   ,transaction_t *transaction);
/* Called when STATE=FAILURE||SUCCESS and EVENT=SND_REQACK */
void            uac_i_sendack(sipevent_t *sipevent,
			    transaction_t *transaction);

/************************/
/* FSM  ---- > UAC4REQ  */
/************************/

/* Called when STATE=INITIAL and EVENT=SND_REQINVITE */
void            uac_r_sendrequest(sipevent_t *sipevent
			   ,transaction_t *transaction);
/* Called when STATE=CALLING and EVENT=TIMEOUT */
void            uac_r_retransmitrequest(sipevent_t *sipevent
			   ,transaction_t *transaction);

/*********************************/
/* FSM  ---- > UAC4 INV AND REQ  */
/*********************************/

/* Called when STATE=CALLING and EVENT=RCV_STATUS_1XX */
void             uac_rcv1XX(sipevent_t *sipevent
			    ,transaction_t *transaction);
/* Called when STATE=CALLING||PROCEEDING and EVENT=RCV_STATUS_23456XX */
void             uac_rcv23456XX(sipevent_t *sipevent
				 ,transaction_t *transaction);

/* common state machine methods for all request */
void             ua_sendrequest      (sipevent_t *sipevent
				    ,transaction_t *transaction);
void             ua_retransmitrequest(sipevent_t *sipevent
				    ,transaction_t *transaction);
void             ua_rcvresponse      (sipevent_t *sipevent
				    ,transaction_t *transaction);

/************************/
/* FSM  ---- > UAS4INV  */
/************************/

void            uas_rcvrequest(sipevent_t *sipevent
				,transaction_t *transaction);
void            uas_i_rcvretransmitrequest(sipevent_t *sipevent
					      ,transaction_t *transaction);
void            uas_i_retransmitresponse(sipevent_t *sipevent
					 ,transaction_t *transaction);
void            uas_i_rcvack(sipevent_t *sipevent
				,transaction_t *transaction);

/************************/
/* FSM  ---- > UAS4REQ  */
/************************/

void            uas_rcvrequest(sipevent_t *sipevent
				,transaction_t *transaction);
void            uas_r_rcvretransmitrequest(sipevent_t *sipevent
					      ,transaction_t *transaction);
void            uas_r_send23456XX(sipevent_t *sipevent
					,transaction_t *transaction);
void            uas_r_retransmitresponse(sipevent_t *sipevent
					 ,transaction_t *transaction);

/*********************************/
/* FSM  ---- > UAS4 INV AND REQ  */
/*********************************/

/* common state machine methods for all request */
void            ua_retransmitresponse(sipevent_t *sipevent
				      ,transaction_t *transaction);
void            uas_send1XX(sipevent_t *sipevent
			   ,transaction_t *transaction);
int             uas_send23456XX(sipevent_t *sipevent
			      ,transaction_t *transaction);


#endif
