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

#ifndef _FSM_H_
#define _FSM_H_

#include <osip/smsg.h>
#include <osip/osip.h>

/* This is for internal use only.                      */
type_t evt_settype_incoming_sipmessage (sip_t * sip);
type_t evt_settype_outgoing_sipmessage (sip_t * sip);

/* send a request on UDP.                       */
int udp_send_request (sip_t * request, url_t * proxy);

/* send a response on UDP.                       */
int udp_send_response (sip_t * response);

void ict_load_fsm ();
void ist_load_fsm ();
void nict_load_fsm ();
void nist_load_fsm ();

void ict_unload_fsm ();
void ist_unload_fsm ();
void nict_unload_fsm ();
void nist_unload_fsm ();

/* load state machine structure for (UAC || UAS) for (INVITE || REQUEST) */
statemachine_t *ict_get_fsm ();
statemachine_t *ist_get_fsm ();

statemachine_t *nict_get_fsm ();
statemachine_t *nist_get_fsm ();

typedef struct _transition_t
{
  state_t state;
  type_t type;
  void (*method) (void *, void *);
}
transition_t;

int fsm_callmethod (type_t type, state_t state,
		    statemachine_t * statemachine,
		    void *sipevent, void *transaction);


/*!! THESE ARE FOR INTERNAL USE ONLY!! */
/* These methods are the "exection method" for the finite */
/* state machine.                                         */

/************************/
/* FSM  ---- > ICT      */
/************************/

void ict_snd_invite (transaction_t * ict, sipevent_t * evt);
void ict_timeout_a_event (transaction_t * ict, sipevent_t * evt);
void ict_timeout_b_event (transaction_t * ict, sipevent_t * evt);
void ict_timeout_d_event (transaction_t * ict, sipevent_t * evt);
void ict_rcv_1xx (transaction_t * ict, sipevent_t * evt);
void ict_rcv_2xx (transaction_t * ict, sipevent_t * evt);
sip_t *ict_create_ack (transaction_t * ict, sip_t * response);
void ict_rcv_3456xx (transaction_t * ict, sipevent_t * evt);
void ict_retransmit_ack (transaction_t * ict, sipevent_t * evt);

/************************/
/* FSM  ---- > IST      */
/************************/

sip_t *ist_create_resp_100 (transaction_t * ist, sip_t * request);
void ist_rcv_invite (transaction_t * ist, sipevent_t * evt);
void ist_timeout_g_event (transaction_t * ist, sipevent_t * evt);
void ist_timeout_h_event (transaction_t * ist, sipevent_t * evt);
void ist_timeout_i_event (transaction_t * ist, sipevent_t * evt);
void ist_snd_1xx (transaction_t * ist, sipevent_t * evt);
void ist_snd_2xx (transaction_t * ist, sipevent_t * evt);
void ist_snd_3456xx (transaction_t * ist, sipevent_t * evt);
void ist_rcv_ack (transaction_t * ist, sipevent_t * evt);

/***********************/
/* FSM  ---- > NICT    */
/***********************/

void nict_snd_request (transaction_t * nict, sipevent_t * evt);
void nict_timeout_e_event (transaction_t * nict, sipevent_t * evt);
void nict_timeout_f_event (transaction_t * nict, sipevent_t * evt);
void nict_timeout_k_event (transaction_t * nict, sipevent_t * evt);
void nict_rcv_1xx (transaction_t * nict, sipevent_t * evt);
void nict_rcv_23456xx (transaction_t * nict, sipevent_t * evt);

/* void nict_rcv_23456xx2(transaction_t *nict, sipevent_t *evt); */

/************************/
/* FSM  ---- > NIST     */
/************************/

void nist_rcv_request (transaction_t * nist, sipevent_t * evt);
void nist_snd_1xx (transaction_t * nist, sipevent_t * evt);
void nist_snd_23456xx (transaction_t * nist, sipevent_t * evt);
void nist_timeout_j_event (transaction_t * nist, sipevent_t * evt);


#endif
