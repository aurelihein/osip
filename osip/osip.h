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
  /* states for UAC/UAS behavior */
  INITIAL,
  CALLING,
  PROCEEDING,
  WAITACK,
  ANSWERED,
  COMPLETED,

  /* these are the states for PROXY-STATEFULL */
  REQUEST_FORWARDED,
  RESPONSE_FORWARDED,
  ACK_FORWARDED,

  /* Possible reasons of ERRORS */
  NETWORK_ERROR,
  SYNTAX_ERROR,
  GLOBAL_ERROR
} state_t;

typedef enum _type_t {
  TIMEOUT,

  /* FOR INCOMING TRANSACTION */
  RCV_REQINVITE,
  RCV_REQACK,
  RCV_REQUEST,
  RCV_STATUS_1XX,
  RCV_STATUS_23456XX,

  /* FOR OUTGOING TRANSACTION */
  SND_REQINVITE,
  SND_REQACK,
  SND_REQUEST,
  SND_STATUS_1XX,
  SND_STATUS_23456XX,

  KILL_TRANSACTION,
  UNKNOWN_EVT
} type_t;

typedef struct _statemachine_t {
  list_t *transitions;
} statemachine_t;

typedef struct _transaction_t {
  
  void *your_instance;       /* add whatever you want here.       */
  int transactionid;         /* simple id used to identify the tr.*/
  fifo_t *transactionff;     /* events must be added in this fifo */

  from_t    *from;           /* CALL-LEG definition */
  to_t      *to;
  call_id_t *callid;
  cseq_t    *cseq;
  sip_t *lastrequest;        /* last request received or sent     */
  sip_t *lastresponse;       /* last response received or sent    */

  state_t state;             /* state of transaction              */
  statemachine_t *statemachine; /* state-machine of transaction   */

  time_t birth_time;         /* birth_date of transaction         */
  time_t completed_time;     /* end   date of transaction         */
  int retransmissioncounter; /* facilities for internal timer     */

  url_t *proxy;              /* url used to send requests         */    
  void *config;              /* transaction is managed by config  */
} transaction_t;

typedef struct _sipevent_t {
  type_t type;
  int transactionid;
  sip_t *sip;
} sipevent_t;

typedef struct _osip_t {

  list_t *transactions;      /* this is the list of transactions */

  /* when proxy is not set, the req-URI is used */
  url_t *proxy;              /* url used to send requests */

  /* callbacks for state machines */
  void (*cb_rcvinvite)(sipevent_t*,transaction_t*);
  void (*cb_rcvack)(sipevent_t*,transaction_t*);
  void (*cb_rcvbye)(sipevent_t*,transaction_t*);
  void (*cb_rcvcancel)(sipevent_t*,transaction_t*);
  void (*cb_rcvinfo)(sipevent_t*,transaction_t*);
  void (*cb_rcvoptions)(sipevent_t*,transaction_t*);
  void (*cb_rcvregister)(sipevent_t*,transaction_t*);
  void (*cb_rcvprack)(sipevent_t*,transaction_t*);
  void (*cb_rcvunkrequest)(sipevent_t*,transaction_t*);

#ifndef EXTENDED_CALLBACKS

  void (*cb_snd1xx)(sipevent_t*,transaction_t*);
  void (*cb_snd2xx)(sipevent_t*,transaction_t*);
  void (*cb_snd3xx)(sipevent_t*,transaction_t*);
  void (*cb_snd4xx)(sipevent_t*,transaction_t*);
  void (*cb_snd5xx)(sipevent_t*,transaction_t*);
  void (*cb_snd6xx)(sipevent_t*,transaction_t*);

#else

  void (*cb_sndinvite1xx)(sipevent_t*,transaction_t*);
  void (*cb_sndinvite2xx)(sipevent_t*,transaction_t*);
  void (*cb_sndinvite3xx)(sipevent_t*,transaction_t*);
  void (*cb_sndinvite4xx)(sipevent_t*,transaction_t*);
  void (*cb_sndinvite5xx)(sipevent_t*,transaction_t*);
  void (*cb_sndinvite6xx)(sipevent_t*,transaction_t*);

  void (*cb_sndack1xx)(sipevent_t*,transaction_t*);
  void (*cb_sndack2xx)(sipevent_t*,transaction_t*);
  void (*cb_sndack3xx)(sipevent_t*,transaction_t*);
  void (*cb_sndack4xx)(sipevent_t*,transaction_t*);
  void (*cb_sndack5xx)(sipevent_t*,transaction_t*);
  void (*cb_sndack6xx)(sipevent_t*,transaction_t*);

  void (*cb_sndbye1xx)(sipevent_t*,transaction_t*);
  void (*cb_sndbye2xx)(sipevent_t*,transaction_t*);
  void (*cb_sndbye3xx)(sipevent_t*,transaction_t*);
  void (*cb_sndbye4xx)(sipevent_t*,transaction_t*);
  void (*cb_sndbye5xx)(sipevent_t*,transaction_t*);
  void (*cb_sndbye6xx)(sipevent_t*,transaction_t*);

  void (*cb_sndcancel1xx)(sipevent_t*,transaction_t*);
  void (*cb_sndcancel2xx)(sipevent_t*,transaction_t*);
  void (*cb_sndcancel3xx)(sipevent_t*,transaction_t*);
  void (*cb_sndcancel4xx)(sipevent_t*,transaction_t*);
  void (*cb_sndcancel5xx)(sipevent_t*,transaction_t*);
  void (*cb_sndcancel6xx)(sipevent_t*,transaction_t*);

  void (*cb_sndinfo1xx)(sipevent_t*,transaction_t*);
  void (*cb_sndinfo2xx)(sipevent_t*,transaction_t*);
  void (*cb_sndinfo3xx)(sipevent_t*,transaction_t*);
  void (*cb_sndinfo4xx)(sipevent_t*,transaction_t*);
  void (*cb_sndinfo5xx)(sipevent_t*,transaction_t*);
  void (*cb_sndinfo6xx)(sipevent_t*,transaction_t*);

  void (*cb_sndoptions1xx)(sipevent_t*,transaction_t*);
  void (*cb_sndoptions2xx)(sipevent_t*,transaction_t*);
  void (*cb_sndoptions3xx)(sipevent_t*,transaction_t*);
  void (*cb_sndoptions4xx)(sipevent_t*,transaction_t*);
  void (*cb_sndoptions5xx)(sipevent_t*,transaction_t*);
  void (*cb_sndoptions6xx)(sipevent_t*,transaction_t*);

  void (*cb_sndregister1xx)(sipevent_t*,transaction_t*);
  void (*cb_sndregister2xx)(sipevent_t*,transaction_t*);
  void (*cb_sndregister3xx)(sipevent_t*,transaction_t*);
  void (*cb_sndregister4xx)(sipevent_t*,transaction_t*);
  void (*cb_sndregister5xx)(sipevent_t*,transaction_t*);
  void (*cb_sndregister6xx)(sipevent_t*,transaction_t*);

  void (*cb_sndprack1xx)(sipevent_t*,transaction_t*);
  void (*cb_sndprack2xx)(sipevent_t*,transaction_t*);
  void (*cb_sndprack3xx)(sipevent_t*,transaction_t*);
  void (*cb_sndprack4xx)(sipevent_t*,transaction_t*);
  void (*cb_sndprack5xx)(sipevent_t*,transaction_t*);
  void (*cb_sndprack6xx)(sipevent_t*,transaction_t*);

  void (*cb_sndunkrequest1xx)(sipevent_t*,transaction_t*);
  void (*cb_sndunkrequest2xx)(sipevent_t*,transaction_t*);
  void (*cb_sndunkrequest3xx)(sipevent_t*,transaction_t*);
  void (*cb_sndunkrequest4xx)(sipevent_t*,transaction_t*);
  void (*cb_sndunkrequest5xx)(sipevent_t*,transaction_t*);
  void (*cb_sndunkrequest6xx)(sipevent_t*,transaction_t*);

#endif /* EXTENDED_CALLBACKS */

  void (*cb_sndinvite)(sipevent_t*,transaction_t*);
  void (*cb_sndack)(sipevent_t*,transaction_t*);
  void (*cb_sndbye)(sipevent_t*,transaction_t*);
  void (*cb_sndcancel)(sipevent_t*,transaction_t*);
  void (*cb_sndinfo)(sipevent_t*,transaction_t*);
  void (*cb_sndoptions)(sipevent_t*,transaction_t*);
  void (*cb_sndregister)(sipevent_t*,transaction_t*);
  void (*cb_sndprack)(sipevent_t*,transaction_t*);
  void (*cb_sndunkrequest)(sipevent_t*,transaction_t*);

#ifndef EXTENDED_CALLBACKS

  void (*cb_rcv1xx)(sipevent_t*,transaction_t*);
  void (*cb_rcv2xx)(sipevent_t*,transaction_t*);
  void (*cb_rcv3xx)(sipevent_t*,transaction_t*);
  void (*cb_rcv4xx)(sipevent_t*,transaction_t*);
  void (*cb_rcv5xx)(sipevent_t*,transaction_t*);
  void (*cb_rcv6xx)(sipevent_t*,transaction_t*);

#else

  void (*cb_rcvinvite1xx)(sipevent_t*,transaction_t*);
  void (*cb_rcvinvite2xx)(sipevent_t*,transaction_t*);
  void (*cb_rcvinvite3xx)(sipevent_t*,transaction_t*);
  void (*cb_rcvinvite4xx)(sipevent_t*,transaction_t*);
  void (*cb_rcvinvite5xx)(sipevent_t*,transaction_t*);
  void (*cb_rcvinvite6xx)(sipevent_t*,transaction_t*);

  void (*cb_rcvack1xx)(sipevent_t*,transaction_t*);
  void (*cb_rcvack2xx)(sipevent_t*,transaction_t*);
  void (*cb_rcvack3xx)(sipevent_t*,transaction_t*);
  void (*cb_rcvack4xx)(sipevent_t*,transaction_t*);
  void (*cb_rcvack5xx)(sipevent_t*,transaction_t*);
  void (*cb_rcvack6xx)(sipevent_t*,transaction_t*);

  void (*cb_rcvbye1xx)(sipevent_t*,transaction_t*);
  void (*cb_rcvbye2xx)(sipevent_t*,transaction_t*);
  void (*cb_rcvbye3xx)(sipevent_t*,transaction_t*);
  void (*cb_rcvbye4xx)(sipevent_t*,transaction_t*);
  void (*cb_rcvbye5xx)(sipevent_t*,transaction_t*);
  void (*cb_rcvbye6xx)(sipevent_t*,transaction_t*);

  void (*cb_rcvcancel1xx)(sipevent_t*,transaction_t*);
  void (*cb_rcvcancel2xx)(sipevent_t*,transaction_t*);
  void (*cb_rcvcancel3xx)(sipevent_t*,transaction_t*);
  void (*cb_rcvcancel4xx)(sipevent_t*,transaction_t*);
  void (*cb_rcvcancel5xx)(sipevent_t*,transaction_t*);
  void (*cb_rcvcancel6xx)(sipevent_t*,transaction_t*);

  void (*cb_rcvinfo1xx)(sipevent_t*,transaction_t*);
  void (*cb_rcvinfo2xx)(sipevent_t*,transaction_t*);
  void (*cb_rcvinfo3xx)(sipevent_t*,transaction_t*);
  void (*cb_rcvinfo4xx)(sipevent_t*,transaction_t*);
  void (*cb_rcvinfo5xx)(sipevent_t*,transaction_t*);
  void (*cb_rcvinfo6xx)(sipevent_t*,transaction_t*);

  void (*cb_rcvoptions1xx)(sipevent_t*,transaction_t*);
  void (*cb_rcvoptions2xx)(sipevent_t*,transaction_t*);
  void (*cb_rcvoptions3xx)(sipevent_t*,transaction_t*);
  void (*cb_rcvoptions4xx)(sipevent_t*,transaction_t*);
  void (*cb_rcvoptions5xx)(sipevent_t*,transaction_t*);
  void (*cb_rcvoptions6xx)(sipevent_t*,transaction_t*);

  void (*cb_rcvregister1xx)(sipevent_t*,transaction_t*);
  void (*cb_rcvregister2xx)(sipevent_t*,transaction_t*);
  void (*cb_rcvregister3xx)(sipevent_t*,transaction_t*);
  void (*cb_rcvregister4xx)(sipevent_t*,transaction_t*);
  void (*cb_rcvregister5xx)(sipevent_t*,transaction_t*);
  void (*cb_rcvregister6xx)(sipevent_t*,transaction_t*);

  void (*cb_rcvprack1xx)(sipevent_t*,transaction_t*);
  void (*cb_rcvprack2xx)(sipevent_t*,transaction_t*);
  void (*cb_rcvprack3xx)(sipevent_t*,transaction_t*);
  void (*cb_rcvprack4xx)(sipevent_t*,transaction_t*);
  void (*cb_rcvprack5xx)(sipevent_t*,transaction_t*);
  void (*cb_rcvprack6xx)(sipevent_t*,transaction_t*);

  void (*cb_rcvunkrequest1xx)(sipevent_t*,transaction_t*);
  void (*cb_rcvunkrequest2xx)(sipevent_t*,transaction_t*);
  void (*cb_rcvunkrequest3xx)(sipevent_t*,transaction_t*);
  void (*cb_rcvunkrequest4xx)(sipevent_t*,transaction_t*);
  void (*cb_rcvunkrequest5xx)(sipevent_t*,transaction_t*);
  void (*cb_rcvunkrequest6xx)(sipevent_t*,transaction_t*);

#endif /* EXTENDED_CALLBACKS */

  void (*cb_rcvresp_retransmission)(transaction_t*);
  void (*cb_sndreq_retransmission)(transaction_t*);
  void (*cb_sndresp_retransmission)(transaction_t*);
  void (*cb_rcvreq_retransmission)(transaction_t*);

  void (*cb_killtransaction)(transaction_t*);
  void (*cb_endoftransaction)(transaction_t*);

  void (*cb_connection_refused)(transaction_t*);
  void (*cb_network_error)(transaction_t*);


  /* When application request a timer for a transaction   */
  /* a pointer to this transaction is added in those fifo */
  fifo_t *uas_timerff;
  fifo_t *uac_timerff;
  /* The timers method MAY use those list to store the       */
  /* actual list of transactions that needs retransmissions. */
  list_t *uas_transactions;
  list_t *uac_transactions;

} osip_t;

/* start the oSIP stack */
int  osip_global_init();

/* create a context for a SIP Agent */
int  osip_init(osip_t **osip);
void osip_init_proxy(osip_t *osip, url_t *prox);
int  osip_execute(osip_t *osip);
/* TODO: void osip_free(osip_t *osip); */

void osip_setcb_rcvinvite  (osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcvack     (osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcvbye     (osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcvcancel  (osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcvinfo    (osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcvoptions (osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcvregister(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcvprack   (osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcvunkrequest(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));

void osip_setcb_sndinvite  (osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_sndack     (osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_sndbye     (osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_sndcancel  (osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_sndinfo    (osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_sndoptions (osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_sndregister(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_sndprack   (osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_sndunkrequest(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));

#ifndef EXTENDED_CALLBACKS

void osip_setcb_rcv1xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcv2xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcv3xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcv4xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcv5xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcv6xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));

void osip_setcb_snd1xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_snd2xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_snd3xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_snd4xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_snd5xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_snd6xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));

#else

void osip_setcb_rcvinvite1xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcvinvite2xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcvinvite3xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcvinvite4xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcvinvite5xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcvinvite6xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));

void osip_setcb_sndinvite1xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_sndinvite2xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_sndinvite3xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_sndinvite4xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_sndinvite5xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_sndinvite6xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));

void osip_setcb_rcvack1xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcvack2xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcvack3xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcvack4xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcvack5xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcvack6xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));

void osip_setcb_sndack1xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_sndack2xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_sndack3xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_sndack4xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_sndack5xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_sndack6xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));

void osip_setcb_rcvbye1xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcvbye2xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcvbye3xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcvbye4xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcvbye5xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcvbye6xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));

void osip_setcb_sndbye1xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_sndbye2xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_sndbye3xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_sndbye4xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_sndbye5xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_sndbye6xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));

void osip_setcb_rcvcancel1xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcvcancel2xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcvcancel3xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcvcancel4xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcvcancel5xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcvcancel6xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));

void osip_setcb_sndcancel1xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_sndcancel2xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_sndcancel3xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_sndcancel4xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_sndcancel5xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_sndcancel6xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));

void osip_setcb_rcvinfo1xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcvinfo2xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcvinfo3xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcvinfo4xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcvinfo5xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcvinfo6xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));

void osip_setcb_sndinfo1xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_sndinfo2xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_sndinfo3xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_sndinfo4xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_sndinfo5xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_sndinfo6xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));

void osip_setcb_rcvoptions1xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcvoptions2xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcvoptions3xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcvoptions4xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcvoptions5xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcvoptions6xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));

void osip_setcb_sndoptions1xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_sndoptions2xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_sndoptions3xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_sndoptions4xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_sndoptions5xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_sndoptions6xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));

void osip_setcb_rcvregister1xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcvregister2xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcvregister3xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcvregister4xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcvregister5xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcvregister6xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));

void osip_setcb_sndregister1xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_sndregister2xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_sndregister3xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_sndregister4xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_sndregister5xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_sndregister6xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));

void osip_setcb_rcvprack1xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcvprack2xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcvprack3xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcvprack4xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcvprack5xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcvprack6xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));

void osip_setcb_sndprack1xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_sndprack2xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_sndprack3xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_sndprack4xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_sndprack5xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_sndprack6xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));

void osip_setcb_rcvunkrequest1xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcvunkrequest2xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcvunkrequest3xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcvunkrequest4xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcvunkrequest5xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_rcvunkrequest6xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));

void osip_setcb_sndunkrequest1xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_sndunkrequest2xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_sndunkrequest3xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_sndunkrequest4xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_sndunkrequest5xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));
void osip_setcb_sndunkrequest6xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*));

#endif /* EXTENDED_CALLBACKS */

void osip_setcb_rcvresp_retransmission(osip_t *cf,void (*cb)(transaction_t*));
void osip_setcb_sndreq_retransmission (osip_t *cf,void (*cb)(transaction_t*));
void osip_setcb_sndresp_retransmission(osip_t *cf,void (*cb)(transaction_t*));
void osip_setcb_rcvreq_retransmission (osip_t *cf,void (*cb)(transaction_t*));

void osip_setcb_killtransaction   (osip_t *cf,void (*cb)(transaction_t*));
void osip_setcb_endoftransaction  (osip_t *cf,void (*cb)(transaction_t*));
void osip_setcb_connection_refused(osip_t *cf,void (*cb)(transaction_t*));
void osip_setcb_network_error     (osip_t *cf,void (*cb)(transaction_t*));

transaction_t *osip_distribute_event(osip_t *config,sipevent_t* sipevent);
sipevent_t    *osip_parse(char *buf);
sipevent_t    *osip_new_event(type_t type,int transactionid);
sipevent_t    *osip_new_outgoing_sipmessage(sip_t *sip);
sipevent_t    *osip_new_incoming_sipmessage(sip_t *sip);

transaction_t *osip_find_asincomingmessage(osip_t *osip, sipevent_t *se);
transaction_t *osip_find_asoutgoingmessage(osip_t *osip, sipevent_t *se);
transaction_t *osip_find_byid(osip_t *osip, int transactionid);
int            osip_remove_byid(osip_t *osip, int transactionid);
int            transaction_init(osip_t *osip, transaction_t *dest,
			       to_t *to, from_t *from, call_id_t *callid,
			       cseq_t *cseq);
void           transaction_free(transaction_t *transaction);
int            transaction_execute(transaction_t *transaction, sipevent_t *se);

/* FOR INCOMING TRANSACTION */
#define EVT_IS_RCV_INVITE(event)       (event->type==RCV_REQINVITE)
#define EVT_IS_RCV_ACK(event)          (event->type==RCV_REQACK)
#define EVT_IS_RCV_REQUEST(event)      (event->type==RCV_REQUEST)
#define EVT_IS_RCV_STATUS_1XX(event)   (event->type==RCV_STATUS_1XX)
#define EVT_IS_RCV_STATUS_23456XX(event)   (event->type==RCV_STATUS_23456XX)


/* FOR OUTGOING TRANSACTION */
#define EVT_IS_SND_INVITE(event)       (event->type==SND_REQINVITE)
#define EVT_IS_SND_ACK(event)          (event->type==SND_REQACK)
#define EVT_IS_SND_REQUEST(event)      (event->type==SND_REQUEST)
#define EVT_IS_SND_STATUS_1XX(event)   (event->type==SND_STATUS_1XX)
#define EVT_IS_SND_STATUS_23456XX(event)   (event->type==SND_STATUS_23456XX)
#define EVT_IS_INCOMINGMSG(event)      (event->type>=RCV_REQINVITE \
                	               &&event->type<=RCV_STATUS_23456XX)
#define EVT_IS_INCOMINGREQ(event)      (EVT_IS_RCV_INVITE(event) \
                                       ||EVT_IS_RCV_ACK(event) \
                                       ||EVT_IS_RCV_REQUEST(event))
#define EVT_IS_INCOMINGRESP(event)     (EVT_IS_RCV_STATUS_1XX(event) \
                                       ||EVT_IS_RCV_STATUS_23456XX(event))
#define EVT_IS_OUTGOINGMSG(event)      (event->type>=SND_REQINVITE \
                	               &&event->type<=SND_STATUS_23456XX)
#define EVT_IS_OUTGOINGREQ(event)      (EVT_IS_SND_INVITE(event) \
                                       ||EVT_IS_SND_ACK(event) \
                                       ||EVT_IS_SND_REQUEST(event))
#define EVT_IS_OUTGOINGRESP(event)     (EVT_IS_SND_STATUS_1XX(event) \
                                       ||EVT_IS_SND_STATUS_23456XX(event))

#define EVT_IS_MSG(event)              (event->type>=RCV_REQINVITE \
                	               &&event->type<=SND_STATUS_23456XX)
#define EVT_IS_KILL_TRANSACTION(event) (event->type==KILL_TRANSACTION)
#define EVT_IS_UNKNOWN_EVT(event)      (event->type==UNKNOWN_EVT)
#define EVT_IS_TIMEOUT(event)          (event->type==TIMEOUT)


#endif
