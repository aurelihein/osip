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


#ifndef _OSIP_H_
#define _OSIP_H_

#include <time.h>

#ifdef __sun
#include <sys/types.h>
#endif

#include <osip/const.h>
#include <osip/smsg.h>
#include <osip/fifo.h>


#define NO_RESPONSE_TIMEOUT 60
#define END_TRANSACTION_TIMEOUT 10
#define SIP_MESSAGE_MAX_LENGTH 20000


typedef enum _state_t {
  /* STATES for invite client transaction */
  ICT_PRE_CALLING,
  ICT_CALLING,
  ICT_PROCEEDING,
  ICT_COMPLETED,
  ICT_TERMINATED,

  /* STATES for invite server transaction */
  IST_PRE_PROCEEDING, /* this is used to only annouce once the INVITE */
  IST_PROCEEDING,
  IST_COMPLETED,
  IST_CONFIRMED,
  IST_TERMINATED,

  /* STATES for NON-invite client transaction */
  NICT_PRE_TRYING,
  NICT_TRYING,
  NICT_PROCEEDING,
  NICT_COMPLETED,
  NICT_TERMINATED,

  /* STATES for NON-invite server transaction */
  NIST_PRE_TRYING,
  NIST_TRYING,
  NIST_PROCEEDING,
  NIST_COMPLETED,
  NIST_TERMINATED,

  DIALOG_EARLY,
  DIALOG_CONFIRMED,
  DIALOG_CLOSE    /* ?? */
} state_t;

typedef enum _type_t {
  /* TIMEOUT EVENTS for ICT */
  TIMEOUT_A,
  TIMEOUT_B,
  TIMEOUT_D,

  /* TIMEOUT EVENTS for NICT */
  TIMEOUT_E,
  TIMEOUT_F,
  TIMEOUT_K,

  /* TIMEOUT EVENTS for IST */
  TIMEOUT_G,
  TIMEOUT_H,
  TIMEOUT_I,

  /* TIMEOUT EVENTS for NIST */
  TIMEOUT_J,

  /* FOR INCOMING MESSAGE */
  RCV_REQINVITE,
  RCV_REQACK,
  RCV_REQUEST,
  RCV_STATUS_1XX,
  RCV_STATUS_2XX,
  RCV_STATUS_3456XX,

  /* FOR OUTGOING MESSAGE */
  SND_REQINVITE,
  SND_REQACK,
  SND_REQUEST,
  SND_STATUS_1XX,
  SND_STATUS_2XX,
  SND_STATUS_3456XX,

  KILL_TRANSACTION,
  UNKNOWN_EVT
} type_t;

typedef struct _statemachine_t {
  list_t *transitions;
} statemachine_t;

typedef enum _context_type_t {
  ICT,
  IST,
  NICT,
  NIST
} context_type_t;

#ifndef DEFAULT_T1
#define DEFAULT_T1 500  /* 500 ms */
#endif
#ifndef DEFAULT_T2
#define DEFAULT_T2 4000 /* 4s */
#endif
#ifndef DEFAULT_T4
#define DEFAULT_T4 5000 /* 4s */
#endif

typedef struct _ict_t {
  /* state machine is implied... */

  int timer_a_length;        /* A=T1, A=2xT1... (unreliable transport only)  */
  time_t timer_a_start;
  int timer_b_length;        /* B = 64* T1                                   */
  time_t timer_b_start;      /* fire when transaction timeouts               */
  int timer_d_length;        /* D >= 32s for unreliable transport (else = 0) */
  time_t timer_d_start;      /* should be equal to timer H */

  char *destination;         /* url used to send requests         */    
  int port;                  /* port of next hop                  */

} ict_t;

typedef struct _nict_t {
  /* state machine is implied... */

  int timer_e_length;        /* A=T1, A=2xT1... (unreliable transport only)  */
  time_t timer_e_start;      /*  (else = -1) not active                      */
  int timer_f_length;        /* B = 64* T1                                   */
  time_t timer_f_start;      /* fire when transaction timeouts               */
  int timer_k_length;        /* K = T4 (else = 0)                            */
  time_t timer_k_start;

  char *destination;         /* url used to send requests         */    
  int port;                  /* port of next hop                  */

} nict_t;

typedef struct _ist_t {

  int timer_g_length;        /* G=MIN(T1*2,T2) for unreliable transport (else=0)*/
  time_t timer_g_start;      /* else = 0 when reliable transport is used!       */
  int timer_h_length;        /* H = 64* T1                                      */
  time_t timer_h_start;      /* fire when if no ACK is received                 */
  int timer_i_length;        /* I = T4 for unreliable transport (else = 0)      */
  time_t timer_i_start;      /* absorb all ACK                                  */

  int auto_send_100;  /* set to 0 for automatic 100 replies (0 is default) */

} ist_t;

typedef struct _nist_t {

  int timer_j_length;        /* J = 64*T1 (else 0) */
  time_t timer_j_start;

} nist_t;

typedef struct _transaction_t {

  void *your_instance;       /* add whatever you want here.       */
  int transactionid;         /* simple id used to identify the tr.*/
  fifo_t *transactionff;     /* events must be added in this fifo */

  from_t    *from;           /* CALL-LEG definition */
  to_t      *to;
  call_id_t *callid;
  cseq_t    *cseq;

  sip_t *orig_request;       /* last request sent                 */
  sip_t *last_response;      /* last response received            */
  sip_t *ack;                /* ack request sent                  */

  state_t state;             /* state of transaction              */

  time_t birth_time;         /* birth_date of transaction         */
  time_t completed_time;     /* end   date of transaction         */

  /* RESPONSE are received on this socket */
  int in_socket;
  /* REQUESTS are sent on this socket */
  int out_socket;

  void *config;              /* transaction is managed by config  */

  context_type_t ctx_type;
  ict_t *ict_context;
  ist_t *ist_context;
  nict_t *nict_context;
  nist_t *nist_context;
} transaction_t;

typedef struct _osip_t {

  /* list of transactions for ict, ist, nict, nist */
  list_t *ict_transactions;
  list_t *ist_transactions;
  list_t *nict_transactions;
  list_t *nist_transactions;

  /* callbacks for sending messages */
  int (*cb_send_message)(transaction_t *, sip_t*, char*, int, int);

  /* callbacks for ict */
  void (*cb_ict_kill_transaction)(transaction_t*);
  void (*cb_ict_invite_sent)(transaction_t*, sip_t*);
  void (*cb_ict_invite_sent2)(transaction_t*, sip_t*);
  void (*cb_ict_ack_sent)(transaction_t*, sip_t*);
  void (*cb_ict_ack_sent2)(transaction_t*, sip_t*);
  void (*cb_ict_1xx_received)(transaction_t*, sip_t*);
  void (*cb_ict_2xx_received)(transaction_t*, sip_t*);
  void (*cb_ict_2xx_received2)(transaction_t*, sip_t*);
  void (*cb_ict_3xx_received)(transaction_t*, sip_t*);
  void (*cb_ict_4xx_received)(transaction_t*, sip_t*);
  void (*cb_ict_5xx_received)(transaction_t*, sip_t*);
  void (*cb_ict_6xx_received)(transaction_t*, sip_t*);
  void (*cb_ict_3456xx_received2)(transaction_t*, sip_t*);

  void (*cb_ict_transport_error)(transaction_t*, int error);

  /* callbacks for ist */
  void (*cb_ist_kill_transaction)(transaction_t*);
  void (*cb_ist_invite_received)(transaction_t*, sip_t*);
  void (*cb_ist_invite_received2)(transaction_t*, sip_t*);
  void (*cb_ist_ack_received)(transaction_t*, sip_t*);
  void (*cb_ist_ack_received2)(transaction_t*, sip_t*);
  void (*cb_ist_1xx_sent)(transaction_t*, sip_t*);
  void (*cb_ist_2xx_sent)(transaction_t*, sip_t*);
  void (*cb_ist_2xx_sent2)(transaction_t*, sip_t*);
  void (*cb_ist_3xx_sent)(transaction_t*, sip_t*);
  void (*cb_ist_4xx_sent)(transaction_t*, sip_t*);
  void (*cb_ist_5xx_sent)(transaction_t*, sip_t*);
  void (*cb_ist_6xx_sent)(transaction_t*, sip_t*);
  void (*cb_ist_3456xx_sent2)(transaction_t*, sip_t*);

  void (*cb_ist_transport_error)(transaction_t*, int error);


  /* callbacks for nict */
  void (*cb_nict_kill_transaction)(transaction_t*);
  void (*cb_nict_register_sent)(transaction_t*, sip_t*);
  void (*cb_nict_bye_sent)(transaction_t*, sip_t*);
  void (*cb_nict_options_sent)(transaction_t*, sip_t*);
  void (*cb_nict_info_sent)(transaction_t*, sip_t*);
  void (*cb_nict_cancel_sent)(transaction_t*, sip_t*);
  void (*cb_nict_notify_sent)(transaction_t*, sip_t*);
  void (*cb_nict_subscribe_sent)(transaction_t*, sip_t*);
  void (*cb_nict_unknown_sent)(transaction_t*, sip_t*);
  void (*cb_nict_request_sent2)(transaction_t*, sip_t*);
  void (*cb_nict_1xx_received)(transaction_t*, sip_t*);
  void (*cb_nict_2xx_received)(transaction_t*, sip_t*);
  void (*cb_nict_2xx_received2)(transaction_t*, sip_t*);
  void (*cb_nict_3xx_received)(transaction_t*, sip_t*);
  void (*cb_nict_4xx_received)(transaction_t*, sip_t*);
  void (*cb_nict_5xx_received)(transaction_t*, sip_t*);
  void (*cb_nict_6xx_received)(transaction_t*, sip_t*);
  void (*cb_nict_3456xx_received2)(transaction_t*, sip_t*);
  void (*cb_nict_transport_error)(transaction_t*, int error);

  /* callbacks for nist */
  void (*cb_nist_kill_transaction)(transaction_t*);
  void (*cb_nist_register_received)(transaction_t*, sip_t*);
  void (*cb_nist_bye_received)(transaction_t*, sip_t*);
  void (*cb_nist_options_received)(transaction_t*, sip_t*);
  void (*cb_nist_info_received)(transaction_t*, sip_t*);
  void (*cb_nist_cancel_received)(transaction_t*, sip_t*);
  void (*cb_nist_notify_received)(transaction_t*, sip_t*);
  void (*cb_nist_subscribe_received)(transaction_t*, sip_t*);
  /* ... TO BE ADDED: All known and used method extansions */
  void (*cb_nist_unknown_received)(transaction_t*, sip_t*);
  void (*cb_nist_request_received2)(transaction_t*, sip_t*);
  void (*cb_nist_1xx_sent)(transaction_t*, sip_t*);
  void (*cb_nist_2xx_sent)(transaction_t*, sip_t*);
  void (*cb_nist_2xx_sent2)(transaction_t*, sip_t*);
  void (*cb_nist_3xx_sent)(transaction_t*, sip_t*);
  void (*cb_nist_4xx_sent)(transaction_t*, sip_t*);
  void (*cb_nist_5xx_sent)(transaction_t*, sip_t*);
  void (*cb_nist_6xx_sent)(transaction_t*, sip_t*);
  void (*cb_nist_3456xx_sent2)(transaction_t*, sip_t*);
  void (*cb_nist_transport_error)(transaction_t*, int error);

} osip_t;


typedef struct _sipevent_t {
  type_t type;
  int transactionid;
  sip_t *sip;
} sipevent_t;

int ict_init(ict_t **ict, osip_t *osip, sip_t *invite);
int ict_free(ict_t *ict);
int ict_set_destination(ict_t *ict, char *destination, int port);
sipevent_t *ict_need_timer_a_event(ict_t *ict, state_t state, int transactionid);
sipevent_t *ict_need_timer_b_event(ict_t *ict, state_t state, int transactionid);
sipevent_t *ict_need_timer_d_event(ict_t *ict, state_t state, int transactionid);

int nict_init(nict_t **nict, osip_t *osip, sip_t *invite);
int nict_free(nict_t *nict);
int nict_set_destination(nict_t *nict, char *destination, int port);
sipevent_t *nict_need_timer_e_event(nict_t *nict,state_t state,int transactionid);
sipevent_t *nict_need_timer_f_event(nict_t *nict,state_t state,int transactionid);
sipevent_t *nict_need_timer_k_event(nict_t *nict,state_t state,int transactionid);

int ist_init(ist_t **ist, osip_t *osip, sip_t *invite);
int ist_free(ist_t *ist);
int ist_set_auto_send_100(ist_t *ist, int bool);
sipevent_t *ist_need_timer_g_event(ist_t *ist, state_t state, int transactionid);
sipevent_t *ist_need_timer_h_event(ist_t *ist, state_t state, int transactionid);
sipevent_t *ist_need_timer_i_event(ist_t *ist, state_t state, int transactionid);

int nist_init(nist_t **nist, osip_t *osip, sip_t *invite);
int nist_free(nist_t *nist);
sipevent_t *nist_need_timer_j_event(nist_t *nist,state_t state,int transactionid);

int transaction_init(transaction_t **transaction, context_type_t ctx_type,
		     osip_t *osip, sip_t *invite);
int transaction_free(transaction_t *transaction);
int transaction_execute(transaction_t *transaction, sipevent_t *se);
int transaction_set_your_instance(transaction_t *transaction, void *instance);
int transaction_set_from(transaction_t *transaction, from_t *from);
int transaction_set_to(transaction_t *transaction, to_t *to);
int transaction_set_call_id(transaction_t *transaction, call_id_t *call_id);
int transaction_set_cseq(transaction_t *transaction, cseq_t *cseq);
int transaction_set_orig_request(transaction_t *transaction, sip_t *request);
int transaction_set_last_response(transaction_t *transaction, sip_t *last_response);
int transaction_set_ack(transaction_t *transaction, sip_t *ack);
int transaction_set_state(transaction_t *transaction, state_t state);
int transaction_set_in_socket(transaction_t *transaction, int sock);
int transaction_set_out_socket(transaction_t *transaction, int sock);
int transaction_set_config(transaction_t *transaction, osip_t *osip);

int callleg_match(to_t *to1,from_t *from1,to_t *to2,from_t *from2);
int call_id_match(call_id_t *callid1,call_id_t *callid2);
int cseq_match(cseq_t *cseq1,cseq_t *cseq2);

/* start the oSIP stack */
int  osip_global_init();

/* create a context for a SIP Agent */
int  osip_init(osip_t **osip);
int  osip_ict_lock(osip_t *osip);
int  osip_ict_unlock(osip_t *osip);
int  osip_ist_lock(osip_t *osip);
int  osip_ist_unlock(osip_t *osip);
int  osip_nict_lock(osip_t *osip);
int  osip_nict_unlock(osip_t *osip);
int  osip_nist_lock(osip_t *osip);
int  osip_nist_unlock(osip_t *osip);
int  osip_add_ict(osip_t *osip, transaction_t *ict);
int  osip_add_ist(osip_t *osip, transaction_t *ist);
int  osip_add_nict(osip_t *osip, transaction_t *nict);
int  osip_add_nist(osip_t *osip, transaction_t *nist);

int  osip_remove_ict(osip_t *osip, transaction_t *ict);
int  osip_remove_ist(osip_t *osip, transaction_t *ist);
int  osip_remove_nict(osip_t *osip, transaction_t *nict);
int  osip_remove_nist(osip_t *osip, transaction_t *nist);

int  osip_ict_execute(osip_t *osip);
int  osip_ist_execute(osip_t *osip);
int  osip_nict_execute(osip_t *osip);
int  osip_nist_execute(osip_t *osip);

void  osip_timers_ict_execute(osip_t *osip);
void  osip_timers_ist_execute(osip_t *osip);
void  osip_timers_nict_execute(osip_t *osip);
void  osip_timers_nist_execute(osip_t *osip);
/* TODO: void osip_free(osip_t *osip); */

transaction_t *osip_distribute_event(osip_t *osip,sipevent_t* sipevent);
transaction_t *osip_transaction_find(list_t *transactions, sipevent_t *se);

sipevent_t    *osip_parse(char *buf);
sipevent_t    *osip_new_event(type_t type,int transactionid);
sipevent_t    *osip_new_outgoing_sipmessage(sip_t *sip);
sipevent_t    *osip_new_incoming_sipmessage(sip_t *sip);


void osip_setcb_send_message(osip_t *cf,
			     int (*cb)(transaction_t *, sip_t*, char*, int, int));
/* callbacks for ict */
void osip_setcb_ict_kill_transaction(osip_t *cf,void (*cb)(transaction_t*));
void osip_setcb_ict_invite_sent(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_ict_invite_sent2(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_ict_ack_sent(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_ict_ack_sent2(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_ict_1xx_received(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_ict_2xx_received(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_ict_2xx_received2(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_ict_3xx_received(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_ict_4xx_received(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_ict_5xx_received(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_ict_6xx_received(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_ict_3456xx_received2(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_ict_transport_error(osip_t *cf,void (*cb)(transaction_t*, int error));

/* callbacks for ist */
void osip_setcb_ist_kill_transaction(osip_t *cf,void (*cb)(transaction_t*));
void osip_setcb_ist_invite_received(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_ist_invite_received2(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_ist_ack_received(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_ist_ack_received2(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_ist_1xx_sent(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_ist_1xx_sent2(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_ist_2xx_sent(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_ist_2xx_sent2(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_ist_3xx_sent(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_ist_4xx_sent(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_ist_5xx_sent(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_ist_6xx_sent(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_ist_3456xx_sent2(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_ist_transport_error(osip_t *cf,void (*cb)(transaction_t*, int error));


/* callbacks for nict */
void osip_setcb_nict_kill_transaction(osip_t *cf,void (*cb)(transaction_t*));
void osip_setcb_nict_register_sent(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_nict_bye_sent(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_nict_options_sent(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_nict_info_sent(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_nict_cancel_sent(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_nict_notify_sent(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_nict_subscribe_sent(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_nict_unknown_sent(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_nict_request_sent2(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_nict_1xx_received(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_nict_2xx_received(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_nict_2xx_received2(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_nict_3xx_received(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_nict_4xx_received(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_nict_5xx_received(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_nict_6xx_received(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_nict_3456xx_received2(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_nict_transport_error(osip_t *cf,void (*cb)(transaction_t*, int error));

  /* callbacks for nist */
void osip_setcb_nist_kill_transaction(osip_t *cf,void (*cb)(transaction_t*));
void osip_setcb_nist_register_received(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_nist_bye_received(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_nist_options_received(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_nist_info_received(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_nist_cancel_received(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_nist_notify_received(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_nist_subscribe_received(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_nist_unknown_received(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
  /* ... TO BE ADDED: All known and used method extansions */
void osip_setcb_nist_request_received2  (osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_nist_1xx_sent(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_nist_2xx_sent(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_nist_2xx_sent2(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_nist_3xx_sent(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_nist_4xx_sent(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_nist_5xx_sent(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_nist_6xx_sent(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_nist_3456xx_sent2(osip_t *cf,void (*cb)(transaction_t*, sip_t*));
void osip_setcb_nist_transport_error(osip_t *cf,void (*cb)(transaction_t*, int error));


/* FOR INCOMING TRANSACTION */
#define EVT_IS_RCV_INVITE(event)       (event->type==RCV_REQINVITE)
#define EVT_IS_RCV_ACK(event)          (event->type==RCV_REQACK)
#define EVT_IS_RCV_REQUEST(event)      (event->type==RCV_REQUEST)
#define EVT_IS_RCV_STATUS_1XX(event)   (event->type==RCV_STATUS_1XX)
#define EVT_IS_RCV_STATUS_2XX(event)   (event->type==RCV_STATUS_2XX)
#define EVT_IS_RCV_STATUS_3456XX(event)   (event->type==RCV_STATUS_3456XX)


/* FOR OUTGOING TRANSACTION */
#define EVT_IS_SND_INVITE(event)       (event->type==SND_REQINVITE)
#define EVT_IS_SND_ACK(event)          (event->type==SND_REQACK)
#define EVT_IS_SND_REQUEST(event)      (event->type==SND_REQUEST)
#define EVT_IS_SND_STATUS_1XX(event)   (event->type==SND_STATUS_1XX)
#define EVT_IS_SND_STATUS_2XX(event)   (event->type==SND_STATUS_2XX)
#define EVT_IS_SND_STATUS_3456XX(event)   (event->type==SND_STATUS_3456XX)
#define EVT_IS_INCOMINGMSG(event)      (event->type>=RCV_REQINVITE \
                	               &&event->type<=RCV_STATUS_3456XX)
#define EVT_IS_INCOMINGREQ(event)      (EVT_IS_RCV_INVITE(event) \
                                       ||EVT_IS_RCV_ACK(event) \
                                       ||EVT_IS_RCV_REQUEST(event))
#define EVT_IS_INCOMINGRESP(event)     (EVT_IS_RCV_STATUS_1XX(event) \
                                       ||EVT_IS_RCV_STATUS_2XX(event) \
				       ||EVT_IS_RCV_STATUS_3456XX(event))
#define EVT_IS_OUTGOINGMSG(event)      (event->type>=SND_REQINVITE \
                	               &&event->type<=SND_STATUS_3456XX)
#define EVT_IS_OUTGOINGREQ(event)      (EVT_IS_SND_INVITE(event) \
                                       ||EVT_IS_SND_ACK(event) \
                                       ||EVT_IS_SND_REQUEST(event))
#define EVT_IS_OUTGOINGRESP(event)     (EVT_IS_SND_STATUS_1XX(event) \
                                       ||EVT_IS_SND_STATUS_2XX(event) \
				       ||EVT_IS_SND_STATUS_3456XX(event))

#define EVT_IS_MSG(event)              (event->type>=RCV_REQINVITE \
                	               &&event->type<=SND_STATUS_3456XX)
#define EVT_IS_KILL_TRANSACTION(event) (event->type==KILL_TRANSACTION)
#define EVT_IS_UNKNOWN_EVT(event)      (event->type==UNKNOWN_EVT)
#define EVT_IS_TIMEOUT(event)          (event->type==TIMEOUT)


#endif
