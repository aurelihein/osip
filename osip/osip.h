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


#ifndef _OSIP_H_
#define _OSIP_H_

#include <osip/const.h>

#include <time.h>

#ifdef __sun
#include <sys/types.h>
#endif

#include <osip/smsg.h>
#include <osip/fifo.h>

/**
 * @file osip.h
 * @brief oSIP fsm Routines
 *
 * <H2>Introduction.</H2>
 * fsm stands for 'finite state machine'. The possible STATE of the state
 * machines are defined in the enum state. In oSIP, you can actually find
 * 4 different state machines. Those state machines definitions are directly
 * related to the definitions of transactions from the SIP specifications.
 * (See section: 17.1.1,  17.1.2,  17.2.1,  17.2.2). In the 4 drawings shown
 * in those sections, you'll find the possible STATES and the possible EVENTS
 * (sipevent_t) that can occur. EVENTS can be either TIMEOUT events and
 * SIP message (incoming and outgoing) events.
 * <H2>Why 4 finite state machines.</H2>
 * <BR>SIP has two different kind of transaction: INVITE and NON-INVITE ones.
 * Also, a SIP User Agent can act as a server and as a client. This simply
 * leads to 4 transactions state machines.
 * <BR>
 * <H2>Step 1: oSIP initialisation</H2>
 * <BR>To use oSIP, a program MUST first initialise internal elements in the
 * stack. The initialisation is shown below:
 * <pre><code>
 * osip_t *osip;
 * // initialise internal element first
 * if (0!=osip_global_init())
 *   return -1;
 * // allocate a global osip element.
 * if (0!=osip_init(&osip))
 *   return -1;
 *
 * // the next step is the initialisation of the callbacks used by the
 * // oSIP stack to announce events (when a transition occurs in the fsm)
 *
 * // This callback is somewhat special and is used by oSIP to inform
 * // the application that a message has to be sent. The message is
 * // sent by your application! oSIP has no ways to send it alone.
 * // Also, the method you supply will be called with default values where
 * // you should send the SIP message. You are not mandated to send the
 * // SIP message by using those default values.
 * // the callback MUST return 0 on success, 1 on ECONNREFUSED, -1 on error.
 *  osip_setcb_send_message(osip, &application_cb_snd_message);
 *
 * // here is the long list of callback that you can register. Some
 * // of this callbacks are very useless (announcing a retransmission,
 * // or announcing that you have sent a SIP message which you may already
 * // know...).
 *
 * // those callbacks are mandatory. They are called when oSIP has decided
 * // that this transaction MUST no longer be handled by oSIP. (This is
 * // called in both successful or error cases scenario)
 *  osip_setcb_ict_kill_transaction(osip,&application_cb_ict_kill_transaction);
 *  osip_setcb_ist_kill_transaction(osip,&application_cb_ist_kill_transaction);
 *  osip_setcb_nict_kill_transaction(osip,&application_cb_nict_kill_transaction);
 *  osip_setcb_nist_kill_transaction(osip,&application_cb_nist_kill_transaction);
 *
 * // those callbacks are optional. The purpose is to announce retransmissions
 * // of SIP message decided by the oSIP stack. (They can be used for statistics?)
 *  osip_setcb_ict_2xx_received2(osip,&application_cb_rcvresp_retransmission);
 *  osip_setcb_ict_3456xx_received2(osip,&application_cb_rcvresp_retransmission);
 *  osip_setcb_ict_invite_sent2(osip,&application_cb_sndreq_retransmission);
 *  osip_setcb_ist_2xx_sent2(osip,&application_cb_sndresp_retransmission);
 *  osip_setcb_ist_3456xx_sent2(osip,&application_cb_sndresp_retransmission);
 *  osip_setcb_ist_invite_received2(osip,&application_cb_rcvreq_retransmission);
 *  osip_setcb_nict_2xx_received2(osip,&application_cb_rcvresp_retransmission);
 *  osip_setcb_nict_3456xx_received2(osip,&application_cb_rcvresp_retransmission);
 *  osip_setcb_nict_request_sent2(osip,&application_cb_sndreq_retransmission);
 *  osip_setcb_nist_2xx_sent2(osip,&application_cb_sndresp_retransmission);
 *  osip_setcb_nist_3456xx_sent2(osip,&application_cb_sndresp_retransmission);
 *  osip_setcb_nist_request_received2(osip,&application_cb_rcvreq_retransmission);
 *
 * // those callbacks are mandatory. They are used to announce network related
 * // errors (the return code of the network callback if it was not 0)
 *  osip_setcb_ict_transport_error(osip,&application_cb_transport_error);
 *  osip_setcb_ist_transport_error(osip,&application_cb_transport_error);
 *  osip_setcb_nict_transport_error(osip,&application_cb_transport_error);
 *  osip_setcb_nist_transport_error(osip,&application_cb_transport_error);
 *  
 * // those callbacks are optional. They are used to announce the initial
 * // request sent for a newly created transaction.
 *  osip_setcb_ict_invite_sent  (osip,&application_cb_sndinvite);
 *  osip_setcb_ict_ack_sent     (osip,&application_cb_sndack);
 *  osip_setcb_nict_register_sent(osip,&application_cb_sndregister);
 *  osip_setcb_nict_bye_sent     (osip,&application_cb_sndbye);
 *  osip_setcb_nict_cancel_sent  (osip,&application_cb_sndcancel);
 *  osip_setcb_nict_info_sent    (osip,&application_cb_sndinfo);
 *  osip_setcb_nict_options_sent (osip,&application_cb_sndoptions);
 *  osip_setcb_nict_subscribe_sent (osip,&application_cb_sndoptions);
 *  osip_setcb_nict_notify_sent (osip,&application_cb_sndoptions);
 *  osip_setcb_nict_unknown_sent(osip,&application_cb_sndunkrequest);
 *
 * // those callbacks are mandatory. They are used to announce the initial
 * // response received for a transaction. (for SIP response between 100 and 199,
 * // all responses are announced because this is not a retransmission case)
 *  osip_setcb_ict_1xx_received(osip,&application_cb_rcv1xx);
 *  osip_setcb_ict_2xx_received(osip,&application_cb_rcv2xx);
 *  osip_setcb_ict_3xx_received(osip,&application_cb_rcv3xx);
 *  osip_setcb_ict_4xx_received(osip,&application_cb_rcv4xx);
 *  osip_setcb_ict_5xx_received(osip,&application_cb_rcv5xx);
 *  osip_setcb_ict_6xx_received(osip,&application_cb_rcv6xx);
 *  
 * // those callbacks are optional. They are used to announce the initial
 * // response sent for a transaction. (for SIP response between 100 and 199,
 * // all responses are announced because this is not a retransmission case)
 *  osip_setcb_ist_1xx_sent(osip,&application_cb_snd1xx);
 *  osip_setcb_ist_2xx_sent(osip,&application_cb_snd2xx);
 *  osip_setcb_ist_3xx_sent(osip,&application_cb_snd3xx);
 *  osip_setcb_ist_4xx_sent(osip,&application_cb_snd4xx);
 *  osip_setcb_ist_5xx_sent(osip,&application_cb_snd5xx);
 *  osip_setcb_ist_6xx_sent(osip,&application_cb_snd6xx);
 *
 * // those callbacks are mandatory. They are used to announce the initial
 * // response received for a transaction. (for SIP response between 100 and 199,
 * // all responses are announced because this is not a retransmission case)
 *  osip_setcb_nict_1xx_received(osip,&application_cb_rcv1xx);
 *  osip_setcb_nict_2xx_received(osip,&application_cb_rcv2xx);
 *  osip_setcb_nict_3xx_received(osip,&application_cb_rcv3xx);
 *  osip_setcb_nict_4xx_received(osip,&application_cb_rcv4xx);
 *  osip_setcb_nict_5xx_received(osip,&application_cb_rcv5xx);
 *  osip_setcb_nict_6xx_received(osip,&application_cb_rcv6xx);
 *
 * // those callbacks are optional. They are used to announce the initial
 * // response sent for a transaction. (for SIP response between 100 and 199,
 * // all responses are announced because this is not a retransmission case)
 *  osip_setcb_nist_1xx_sent(osip,&application_cb_snd1xx);
 *  osip_setcb_nist_2xx_sent(osip,&application_cb_snd2xx);
 *  osip_setcb_nist_3xx_sent(osip,&application_cb_snd3xx);
 *  osip_setcb_nist_4xx_sent(osip,&application_cb_snd4xx);
 *  osip_setcb_nist_5xx_sent(osip,&application_cb_snd5xx);
 *  osip_setcb_nist_6xx_sent(osip,&application_cb_snd6xx);
 *
 * // those callbacks are mandatory. They are used to announce the initial
 * // request received for a transaction. It is not useless to notice that
 * // a special behaviour exist for the 200 OK and the ACK in the case of
 * // a successful INVITE transaction. This will be discussed later.
 *  osip_setcb_ist_invite_received   (osip,&application_cb_rcvinvite);
 *  osip_setcb_ist_ack_received      (osip,&application_cb_rcvack);
 * // this callback is optional
 *  osip_setcb_ist_ack_received2     (osip,&application_cb_rcvack2);
 *  osip_setcb_nist_register_received(osip,&application_cb_rcvregister);
 *  osip_setcb_nist_bye_received     (osip,&application_cb_rcvbye);
 *  osip_setcb_nist_cancel_received  (osip,&application_cb_rcvcancel);
 *  osip_setcb_nist_info_received    (osip,&application_cb_rcvinfo);
 *  osip_setcb_nist_options_received (osip,&application_cb_rcvoptions);
 *  osip_setcb_nist_subscribe_received(osip,&application_cb_rcvoptions);
 *  osip_setcb_nist_notify_received  (osip,&application_cb_rcvoptions);
 *  osip_setcb_nist_unknown_received (osip,&application_cb_rcvunkrequest);
 *
 * </code></pre>
 * <P>
 * <H2>Step 2: Initialising a new transaction.</H2>
 * <BR>Let's assume you want to implement a User Agent and you want to
 * start a REGISTER transaction. Using the parser library, you will first
 * have to build a SIP compliant message. (oSIP, as a low layer library
 * provides an interface to build SIP messages, but it's up to you to
 * correctly fill all the required fields.)
 * As soon as you have build the SIP message, you are ready to start a new
 * transaction. Here is the code:
 * <BR><pre><code>
 * osip_t *osip = your_global_osip_context;
 * transaction_t *transaction;
 * sip_t         *sip_register_message;
 * sipevent_t *sipevent;
 *
 * application_build_register(&sip_register_message);
 * transaction_init(&transaction,
 *		   NICT,          //a REGISTER is a Non-Invite-Client-Transaction
 *		   osip,
 *		   sip_register_message);
 *
 * // If you have a special context that you want to associate to that
 * // transaction, you can use a special method that associate your context
 * // to the transaction context.
 *
 * transaction_set_your_instance(transaction, my_context);
 *
 * // at this point, the transaction context exists in oSIP but you still have
 * // to give the SIP message to the finite state machine.   
 * sipevent = osip_new_outgoing_sipmessage(msg);
 * sipevent->transactionid =  transaction->transactionid;
 * transaction_add_event(transaction, sipevent);
 * // at this point, the event will be handled by oSIP. (The memory resource will
 * // also be handled by oSIP). Note that no action is taken there. 
 * </pre></code>
 * <P>Adding new events in the fsm is made with similar code.
 * <P>
 * <H2>Step 3: Consuming events.</H2>
 * <BR>The previous step show how to create a transaction and one possible way
 * to add a new event. (Note, that some events -the TIMEOUT_* ones- will be
 * added by oSIP not by the application). In this step, we describe how the
 * oSIP stack will consume events. In fact, this is very simple, but you
 * should be aware that it's not always allowed to consume an event at any time!
 * The fsm MUST consume events sequentially within a transaction. This means
 * that when your are calling transaction_execute(), it is forbidden to call
 * this method again with the same transaction context until the first call
 * has returned. In a multi threaded application, if one thread handles one
 * transaction, the code will be the following:
 * <BR><pre><code>
 *   while (1)
 *     {
 *       se = (sipevent_t *)fifo_get(transaction->transactionff);
 *       if (se==NULL)
 * 	  sthread_exit();
 *       if (transaction_execute(transaction,se)<1)  // deletion asked
 * 	  sthread_exit();
 *   }
 * </pre></code>
 * <P>
 * <H2>Step 4: How the stack will announce the events</H2>
 * Looking at the case of a usual outgoing REGISTER transaction, this behaviour
 * is expected.
 * <BR>When an event is seen as useful for the fsm, it means that a transition
 * from one state to another has to be done on the transaction context. If the
 * event is SND_REQUEST (this is the case for an outgoing REGISTER), the
 * callback previously registered to announce this action will be called. This
 * callback is useless for the application as no action has to be taken at this
 * step. A more interesting announcement will be made when consuming the
 * first final response received. If the callbacks associated to 2xx message
 * is called, then the transaction has succeeded. Inside this callback, you
 * will probably inform the user of the success of the registration if you want
 * to do so...
 * If the final response is not a 2xx, or the network callback is called, you'll
 * probably want to take some actions. For example, if you receive a 302, you'll
 * probably want to retry a registration at the new location. All that decision
 * is up to you.
 */

/**
 * @defgroup oSIP_FSM oSIP fsm Handling
 * @ingroup oSIP
 * @{
 */

#ifdef __cplusplus
extern "C"
{
#endif


/**
 * Enumeration for transaction state.
 * <BR>
 * <BR>Here is the list of possible values for transactions:
 * <BR>   ICT_PRE_CALLING,
 * <BR>   ICT_CALLING,
 * <BR>   ICT_PROCEEDING,
 * <BR>   ICT_COMPLETED,
 * <BR>   ICT_TERMINATED,
 * <BR>
 * <BR>   IST_PRE_PROCEEDING,
 * <BR>   IST_PROCEEDING,
 * <BR>   IST_COMPLETED,
 * <BR>   IST_CONFIRMED,
 * <BR>   IST_TERMINATED,
 *<BR>
 * <BR>   NICT_PRE_TRYING,
 * <BR>   NICT_TRYING,
 * <BR>   NICT_PROCEEDING,
 * <BR>   NICT_COMPLETED,
 * <BR>   NICT_TERMINATED,
 *<BR>
 * <BR>   NIST_PRE_TRYING,
 * <BR>   NIST_TRYING,
 * <BR>   NIST_PROCEEDING,
 * <BR>   NIST_COMPLETED,
 * <BR>   NIST_TERMINATED,
 *
 */
  typedef enum _state_t
  {
    /* STATES for invite client transaction */
    ICT_PRE_CALLING,
    ICT_CALLING,
    ICT_PROCEEDING,
    ICT_COMPLETED,
    ICT_TERMINATED,

    /* STATES for invite server transaction */
    IST_PRE_PROCEEDING,		/* this is used to only announce once the INVITE */
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

#ifndef DOXYGEN
    DIALOG_EARLY,
    DIALOG_CONFIRMED,
    DIALOG_CLOSE		/* ?? */
#endif
  }
  state_t;

/**
 * Enumeration for event type.
 * <BR>The list of values that you need to know is reduced to this:
 * <BR> RCV_REQINVITE,
 * <BR> RCV_REQACK,
 * <BR> RCV_REQUEST,
 * <BR> RCV_STATUS_1XX,
 * <BR> RCV_STATUS_2XX,
 * <BR> RCV_STATUS_3456XX,
 *<BR>
 * <BR> SND_REQINVITE,
 * <BR> SND_REQACK,
 * <BR> SND_REQUEST,
 * <BR> SND_STATUS_1XX,
 * <BR> SND_STATUS_2XX,
 * <BR> SND_STATUS_3456XX,
 */
  typedef enum type_t
  {
    /* TIMEOUT EVENTS for ICT */
    TIMEOUT_A,	 /**< Timer A */
    TIMEOUT_B,	 /**< Timer B */
    TIMEOUT_D,	 /**< Timer D */

    /* TIMEOUT EVENTS for NICT */
    TIMEOUT_E,	 /**< Timer E */
    TIMEOUT_F,	 /**< Timer F */
    TIMEOUT_K,	 /**< Timer K */

    /* TIMEOUT EVENTS for IST */
    TIMEOUT_G,	 /**< Timer G */
    TIMEOUT_H,	 /**< Timer H */
    TIMEOUT_I,	 /**< Timer I */

    /* TIMEOUT EVENTS for NIST */
    TIMEOUT_J,	 /**< Timer J */

    /* FOR INCOMING MESSAGE */
    RCV_REQINVITE,    /**< Event is an incoming INVITE request */
    RCV_REQACK,	      /**< Event is an incoming ACK request */
    RCV_REQUEST,      /**< Event is an incoming NON-INVITE and NON-ACK request */
    RCV_STATUS_1XX,   /**< Event is an incoming informational response */
    RCV_STATUS_2XX,   /**< Event is an incoming 2XX response */
    RCV_STATUS_3456XX,/**< Event is an incoming final response (not 2XX) */

    /* FOR OUTGOING MESSAGE */
    SND_REQINVITE,    /**< Event is an outgoing INVITE request */
    SND_REQACK,	      /**< Event is an outgoing ACK request */
    SND_REQUEST,      /**< Event is an outgoing NON-INVITE and NON-ACK request */
    SND_STATUS_1XX,   /**< Event is an outgoing informational response */
    SND_STATUS_2XX,   /**< Event is an outgoing 2XX response */
    SND_STATUS_3456XX,/**< Event is an outgoing final response (not 2XX) */

    KILL_TRANSACTION, /**< Event to 'kill' the transaction before termination */
    UNKNOWN_EVT
  }
  type_t;

#ifndef DOXYGEN
  typedef struct statemachine_t statemachine_t;

  struct statemachine_t
  {
    list_t *transitions;
  };
#endif

/**
 * Enumeration for transaction type.
 * A transaction can be either of:
 *  ICT,
 *  IST,
 *  NICT,
 *  NIST,
 */
  typedef enum context_type_t
  {
    ICT, /**< Invite Client (outgoing) Transaction */
    IST, /**< Invite Server (incoming) Transaction */
    NICT,/**< Non-Invite Client (outgoing) Transaction */
    NIST /**< Non-Invite Server (incoming) Transaction */
  }
  context_type_t;

#ifndef DEFAULT_T1
/**
 * You can re-define the default value for T1. (T1 is defined in rfcxxxx)
 * The default value is 500ms.
 */
#define DEFAULT_T1 500		/* 500 ms */
#endif
#ifndef DEFAULT_T2
/**
 * You can re-define the default value for T2. (T2 is defined in rfcxxxx)
 * The default value is 4000ms.
 */
#define DEFAULT_T2 4000		/* 4s */
#endif
#ifndef DEFAULT_T4
/**
 * You can re-define the default value for T4. (T1 is defined in rfcxxxx)
 * The default value is 5000ms.
 */
#define DEFAULT_T4 5000		/* 5s */
#endif


/**
 * Structure for INVITE CLIENT TRANSACTION (outgoing INVITE transaction).
 * @defvar ict_t
 */
  typedef struct ict_t ict_t;

  struct ict_t
  {
    /* state machine is implied... */

    int timer_a_length;		/* A=T1, A=2xT1... (unreliable transport only)  */
    time_t timer_a_start;
    int timer_b_length;		/* B = 64* T1                                   */
    time_t timer_b_start;	/* fire when transaction timeouts               */
    int timer_d_length;		/* D >= 32s for unreliable transport (else = 0) */
    time_t timer_d_start;	/* should be equal to timer H */

    char *destination;		/* url used to send requests         */
    int port;			/* port of next hop                  */

  };

/**
 * Structure for NON-INVITE CLIENT TRANSACTION (outgoing NON-INVITE transaction).
 * @defvar nict_t
 */
  typedef struct nict_t nict_t;

  struct nict_t
  {
    /* state machine is implied... */

    int timer_e_length;		/* A=T1, A=2xT1... (unreliable transport only)  */
    time_t timer_e_start;	/*  (else = -1) not active                      */
    int timer_f_length;		/* B = 64* T1                                   */
    time_t timer_f_start;	/* fire when transaction timeouts               */
    int timer_k_length;		/* K = T4 (else = 0)                            */
    time_t timer_k_start;

    char *destination;		/* url used to send requests         */
    int port;			/* port of next hop                  */

  };

/**
 * Structure for INVITE SERVER TRANSACTION (incoming INVITE transaction).
 * @defvar ist_t
 */
  typedef struct ist_t ist_t;

  struct ist_t
  {

    int timer_g_length;		/* G=MIN(T1*2,T2) for unreliable transport (else=0) */
    time_t timer_g_start;	/* else = 0 when reliable transport is used!       */
    int timer_h_length;		/* H = 64* T1                                      */
    time_t timer_h_start;	/* fire when if no ACK is received                 */
    int timer_i_length;		/* I = T4 for unreliable transport (else = 0)      */
    time_t timer_i_start;	/* absorb all ACK                                  */

    int auto_send_100;		/* set to 0 for automatic 100 replies (0 is default) */

  };

/**
 * Structure for NON-INVITE SERVER TRANSACTION (incoming SERVER transaction).
 * @defvar nist_t
 */
  typedef struct nist_t nist_t;

  struct nist_t
  {

    int timer_j_length;		/* J = 64*T1 (else 0) */
    time_t timer_j_start;

  };

/**
 * Structure for transaction handling.
 * @defvar transaction_t
 */
  typedef struct transaction_t transaction_t;

  struct transaction_t
  {

    void *your_instance;	/* add whatever you want here.       */
    int transactionid;		/* simple id used to identify the tr. */
    fifo_t *transactionff;	/* events must be added in this fifo */

    via_t *topvia;		/* CALL-LEG definition */
    from_t *from;		/* CALL-LEG definition */
    to_t *to;
    call_id_t *callid;
    cseq_t *cseq;

    sip_t *orig_request;	/* last request sent                 */
    sip_t *last_response;	/* last response received            */
    sip_t *ack;			/* ack request sent                  */

    state_t state;		/* state of transaction              */

    time_t birth_time;		/* birth_date of transaction         */
    time_t completed_time;	/* end   date of transaction         */

    /* RESPONSE are received on this socket */
    int in_socket;
    /* REQUESTS are sent on this socket */
    int out_socket;

    void *config;		/* transaction is managed by config  */

    context_type_t ctx_type;
    ict_t *ict_context;
    ist_t *ist_context;
    nict_t *nict_context;
    nist_t *nist_context;
  };

/**
 * Structure for osip handling.
 * In order to use osip, you have to manage at least one global instance
 * of an osip_t element. Then, you'll register a set of required callbacks
 * and a set of optional ones.
 * @defvar osip_t
 */
  typedef struct osip_t osip_t;

  struct osip_t
  {

    void *application_context;	/* a pointer for your personnal usage */

    /* list of transactions for ict, ist, nict, nist */
    list_t *ict_transactions;
    list_t *ist_transactions;
    list_t *nict_transactions;
    list_t *nist_transactions;

    /* callbacks for sending messages */
    int (*cb_send_message) (transaction_t *, sip_t *, char *, int, int);

    /* callbacks for ict */
    void (*cb_ict_kill_transaction) (transaction_t *);
    void (*cb_ict_invite_sent) (transaction_t *, sip_t *);
    void (*cb_ict_invite_sent2) (transaction_t *, sip_t *);
    void (*cb_ict_ack_sent) (transaction_t *, sip_t *);
    void (*cb_ict_ack_sent2) (transaction_t *, sip_t *);
    void (*cb_ict_1xx_received) (transaction_t *, sip_t *);
    void (*cb_ict_2xx_received) (transaction_t *, sip_t *);
    void (*cb_ict_2xx_received2) (transaction_t *, sip_t *);
    void (*cb_ict_3xx_received) (transaction_t *, sip_t *);
    void (*cb_ict_4xx_received) (transaction_t *, sip_t *);
    void (*cb_ict_5xx_received) (transaction_t *, sip_t *);
    void (*cb_ict_6xx_received) (transaction_t *, sip_t *);
    void (*cb_ict_3456xx_received2) (transaction_t *, sip_t *);

    void (*cb_ict_transport_error) (transaction_t *, int error);

    /* callbacks for ist */
    void (*cb_ist_kill_transaction) (transaction_t *);
    void (*cb_ist_invite_received) (transaction_t *, sip_t *);
    void (*cb_ist_invite_received2) (transaction_t *, sip_t *);
    void (*cb_ist_ack_received) (transaction_t *, sip_t *);
    void (*cb_ist_ack_received2) (transaction_t *, sip_t *);
    void (*cb_ist_1xx_sent) (transaction_t *, sip_t *);
    void (*cb_ist_2xx_sent) (transaction_t *, sip_t *);
    void (*cb_ist_2xx_sent2) (transaction_t *, sip_t *);
    void (*cb_ist_3xx_sent) (transaction_t *, sip_t *);
    void (*cb_ist_4xx_sent) (transaction_t *, sip_t *);
    void (*cb_ist_5xx_sent) (transaction_t *, sip_t *);
    void (*cb_ist_6xx_sent) (transaction_t *, sip_t *);
    void (*cb_ist_3456xx_sent2) (transaction_t *, sip_t *);

    void (*cb_ist_transport_error) (transaction_t *, int error);


    /* callbacks for nict */
    void (*cb_nict_kill_transaction) (transaction_t *);
    void (*cb_nict_register_sent) (transaction_t *, sip_t *);
    void (*cb_nict_bye_sent) (transaction_t *, sip_t *);
    void (*cb_nict_options_sent) (transaction_t *, sip_t *);
    void (*cb_nict_info_sent) (transaction_t *, sip_t *);
    void (*cb_nict_cancel_sent) (transaction_t *, sip_t *);
    void (*cb_nict_notify_sent) (transaction_t *, sip_t *);
    void (*cb_nict_subscribe_sent) (transaction_t *, sip_t *);
    void (*cb_nict_unknown_sent) (transaction_t *, sip_t *);
    void (*cb_nict_request_sent2) (transaction_t *, sip_t *);
    void (*cb_nict_1xx_received) (transaction_t *, sip_t *);
    void (*cb_nict_2xx_received) (transaction_t *, sip_t *);
    void (*cb_nict_2xx_received2) (transaction_t *, sip_t *);
    void (*cb_nict_3xx_received) (transaction_t *, sip_t *);
    void (*cb_nict_4xx_received) (transaction_t *, sip_t *);
    void (*cb_nict_5xx_received) (transaction_t *, sip_t *);
    void (*cb_nict_6xx_received) (transaction_t *, sip_t *);
    void (*cb_nict_3456xx_received2) (transaction_t *, sip_t *);
    void (*cb_nict_transport_error) (transaction_t *, int error);

    /* callbacks for nist */
    void (*cb_nist_kill_transaction) (transaction_t *);
    void (*cb_nist_register_received) (transaction_t *, sip_t *);
    void (*cb_nist_bye_received) (transaction_t *, sip_t *);
    void (*cb_nist_options_received) (transaction_t *, sip_t *);
    void (*cb_nist_info_received) (transaction_t *, sip_t *);
    void (*cb_nist_cancel_received) (transaction_t *, sip_t *);
    void (*cb_nist_notify_received) (transaction_t *, sip_t *);
    void (*cb_nist_subscribe_received) (transaction_t *, sip_t *);
    /* ... TO BE ADDED: All known and used method extensions */
    void (*cb_nist_unknown_received) (transaction_t *, sip_t *);
    void (*cb_nist_request_received2) (transaction_t *, sip_t *);
    void (*cb_nist_1xx_sent) (transaction_t *, sip_t *);
    void (*cb_nist_2xx_sent) (transaction_t *, sip_t *);
    void (*cb_nist_2xx_sent2) (transaction_t *, sip_t *);
    void (*cb_nist_3xx_sent) (transaction_t *, sip_t *);
    void (*cb_nist_4xx_sent) (transaction_t *, sip_t *);
    void (*cb_nist_5xx_sent) (transaction_t *, sip_t *);
    void (*cb_nist_6xx_sent) (transaction_t *, sip_t *);
    void (*cb_nist_3456xx_sent2) (transaction_t *, sip_t *);
    void (*cb_nist_transport_error) (transaction_t *, int error);

  };


/**
 * Structure for sipevent handling.
 * A sipevent_t element will have a type and will be related
 * to a transaction. In the general case, it is used by the
 * application layer to give SIP messages to the oSIP finite
 * state machine.
 * @defvar sipevent_t
 */
  typedef struct sipevent_t sipevent_t;

  struct sipevent_t
  {
    type_t type;
    int transactionid;
    sip_t *sip;
  };

#ifndef DOXYGEN

/**
 * Allocate an ict_t element. (for outgoing INVITE transaction)
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param ict The element to allocate.
 * @param osip The global instance of oSIP.
 * @param invite The SIP request that initiate the transaction.
 */
  int ict_init (ict_t ** ict, osip_t * osip, sip_t * invite);
/**
 * Free all resource in a ict_t element.
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param ict The element to free.
 */
  int ict_free (ict_t * ict);

#endif

/**
 * Set the host and port destination used for sending the SIP message.
 * This can be useful for an application with 'DIRECT ROOTING MODE'
 * NOTE: Instead, you should use the 'Route' header facility which
 * leads to the same behaviour.
 * @param ict The element to work on.
 * @param destination The destination host.
 * @param port The destination port.
 */
  int ict_set_destination (ict_t * ict, char *destination, int port);

#ifndef DOXYGEN
/**
 * Check if this transaction needs a TIMEOUT_A event 
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param ict The element to work on.
 * @param state The actual state of the transaction.
 * @param transactionid The transaction id.
 */
  sipevent_t *ict_need_timer_a_event (ict_t * ict, state_t state,
				      int transactionid);
/**
 * Check if this transaction needs a TIMEOUT_B event 
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param ict The element to work on.
 * @param state The actual state of the transaction.
 * @param transactionid The transaction id.
 */
  sipevent_t *ict_need_timer_b_event (ict_t * ict, state_t state,
				      int transactionid);
/**
 * Check if this transaction needs a TIMEOUT_D event 
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param ict The element to work on.
 * @param state The actual state of the transaction.
 * @param transactionid The transaction id.
 */
  sipevent_t *ict_need_timer_d_event (ict_t * ict, state_t state,
				      int transactionid);

/**
 * Allocate an nict_t element. (for outgoing NON-INVITE transaction)
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param nict The element to allocate.
 * @param osip The global instance of oSIP.
 * @param request The SIP request that initiate the transaction.
 */
  int nict_init (nict_t ** nict, osip_t * osip, sip_t * request);
/**
 * Free all resource in an nict_t element.
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param nict The element to free.
 */
  int nict_free (nict_t * nict);

#endif

/**
 * Set the host and port destination used for sending the SIP message.
 * This can be useful for an application with 'DIRECT ROOTING MODE'
 * NOTE: Instead, you should use the 'Route' header facility which
 * leads to the same behaviour.
 * @param nict The element to work on.
 * @param destination The destination host.
 * @param port The destination port.
 */
  int nict_set_destination (nict_t * nict, char *destination, int port);

#ifndef DOXYGEN

/**
 * Check if this transaction needs a TIMEOUT_E event 
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param nict The element to work on.
 * @param state The actual state of the transaction.
 * @param transactionid The transaction id.
 */
  sipevent_t *nict_need_timer_e_event (nict_t * nict, state_t state,
				       int transactionid);
/**
 * Check if this transaction needs a TIMEOUT_F event 
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param nict The element to work on.
 * @param state The actual state of the transaction.
 * @param transactionid The transaction id.
 */
  sipevent_t *nict_need_timer_f_event (nict_t * nict, state_t state,
				       int transactionid);
/**
 * Check if this transaction needs a TIMEOUT_K event 
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param nict The element to work on.
 * @param state The actual state of the transaction.
 * @param transactionid The transaction id.
 */
  sipevent_t *nict_need_timer_k_event (nict_t * nict, state_t state,
				       int transactionid);

/**
 * Allocate an ist_t element. (for incoming INVITE transaction)
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param ist The element to allocate.
 * @param osip The global instance of oSIP.
 * @param invite The SIP invite that initiate the transaction.
 */
  int ist_init (ist_t ** ist, osip_t * osip, sip_t * invite);
/**
 * Free all resource in a ist_t element.
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param ist The element to free.
 */
  int ist_free (ist_t * ist);
  int ist_set_auto_send_100 (ist_t * ist, int abool);
/**
 * Check if this transaction needs a TIMEOUT_G event 
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param ist The element to work on.
 * @param state The actual state of the transaction.
 * @param transactionid The transaction id.
 */
  sipevent_t *ist_need_timer_g_event (ist_t * ist, state_t state,
				      int transactionid);
/**
 * Check if this transaction needs a TIMEOUT_H event 
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param ist The element to work on.
 * @param state The actual state of the transaction.
 * @param transactionid The transaction id.
 */
  sipevent_t *ist_need_timer_h_event (ist_t * ist, state_t state,
				      int transactionid);
/**
 * Check if this transaction needs a TIMEOUT_I event 
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param ist The element to work on.
 * @param state The actual state of the transaction.
 * @param transactionid The transaction id.
 */
  sipevent_t *ist_need_timer_i_event (ist_t * ist, state_t state,
				      int transactionid);

/**
 * Allocate an nist_t element. (for incoming NON-INVITE transaction)
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param nist The element to allocate.
 * @param osip The global instance of oSIP.
 * @param request The SIP request that initiate the transaction.
 */
  int nist_init (nist_t ** nist, osip_t * osip, sip_t * request);
/**
 * Free all resource in a nist_t element.
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param nist The element to free.
 */
  int nist_free (nist_t * nist);

#endif

/**
 * Check if this transaction needs a TIMEOUT_J event 
 * @param nist The element to work on.
 * @param state The actual state of the transaction.
 * @param transactionid The transaction id.
 */
  sipevent_t *nist_need_timer_j_event (nist_t * nist, state_t state,
				       int transactionid);

/**
 * Allocate an transaction_t element.
 * @param transaction The element to allocate.
 * @param ctx_type The type of transaction. (ICT, IST, NICT, NIST)
 * @param osip The global instance of oSIP.
 * @param request The SIP request that initiate the transaction.
 */
  int transaction_init (transaction_t ** transaction, context_type_t ctx_type,
			osip_t * osip, sip_t * request);
/**
 * Free all resource in a transaction_t element.
 * @param transaction The element to free.
 */
  int transaction_free (transaction_t * transaction);
/**
 * Free all resource in a transaction_t element.
 * This method does the same than transaction_free() but it assumes
 * that the transaction is already removed from the list of transaction
 * in the osip stack. (to remove it use osip_xixt_remove(osip, transaction);
 * @param transaction The element to free.
 */
/*  */
  int transaction_free2 (transaction_t * transaction);

/**
 * Add a SIP event in the fifo of a transaction_t element.
 * @param transaction The element to work on.
 * @param evt The event to add.
 */
  int transaction_add_event (transaction_t * transaction, sipevent_t * evt);
/**
 * Consume one sipevent_t element previously added in the fifo.
 * NOTE: This method MUST NEVER be called within another call
 * of this method. (For example, you can't call transaction_execute()
 * in a callback registered in the osip_t element.)
 * @param transaction The element to free.
 * @param evt The element to consume.
 */
  int transaction_execute (transaction_t * transaction, sipevent_t * evt);
/**
 * Set a pointer to your personal context associated with this transaction.
 * NOTE: this is a very useful method that allow you to avoid searching
 * for your personal context inside the registered callbacks.
 * You can initialise this pointer to your context right after
 * the creation of the transaction_t element. Then, you'll be
 * able to get the address of your context by calling
 * transaction_get_your_instance().
 * @param transaction The element to work on.
 * @param instance The address of your context.
 */
  int transaction_set_your_instance (transaction_t * transaction,
				     void *instance);
/**
 * Get a pointer to your personal context associated with this transaction.
 * @param transaction The element to work on.
 */
  void *transaction_get_your_instance (transaction_t * transaction);

/**
 * Get target ip and port for this request.
 * (automaticly set by transaction_init() for ict and nict)
 * @param transaction The element to work on.
 * @param The host where to send initial request.
 * @param The port where to send initial request.
 */
  int transaction_get_destination(transaction_t * transaction, char **ip, int *port);

#ifndef DOXYGEN

/**
 * Set the Top Via value associated to this transaction.
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param transaction The element to work on.
 * @param topvia The topvia header.
 */
  int transaction_set_topvia (transaction_t * transaction, via_t * topvia);
/**
 * Set the from value associated to this transaction.
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param transaction The element to work on.
 * @param from The from header.
 */
  int transaction_set_from (transaction_t * transaction, from_t * from);
/**
 * Set the to value associated to this transaction.
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param transaction The element to work on.
 * @param to The to header.
 */
  int transaction_set_to (transaction_t * transaction, to_t * to);
/**
 * Set the Call-Id value associated to this transaction.
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param transaction The element to work on.
 * @param call_id The Call-Id header.
 */
  int transaction_set_call_id (transaction_t * transaction,
			       call_id_t * call_id);
/**
 * Set the cseq value associated to this transaction.
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param transaction The element to work on.
 * @param cseq The cseq header.
 */
  int transaction_set_cseq (transaction_t * transaction, cseq_t * cseq);
/**
 * Set the initial request associated to this transaction.
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param transaction The element to work on.
 * @param request The initial request.
 */
  int transaction_set_orig_request (transaction_t * transaction,
				    sip_t * request);
/**
 * Set the last RESPONSE associated to this transaction.
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param transaction The element to work on.
 * @param last_response The last RESPONSE.
 */
  int transaction_set_last_response (transaction_t * transaction,
				     sip_t * last_response);
/**
 * Set the ACK message associated to this transaction.
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param transaction The element to work on.
 * @param ack The ACK MESSAGE.
 */
  int transaction_set_ack (transaction_t * transaction, sip_t * ack);
/**
 * Set the state of the transaction.
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param transaction The element to work on.
 * @param state The new state.
 */
  int transaction_set_state (transaction_t * transaction, state_t state);
/**
 * Set the socket for incoming message.
 * NOTE: THIS HAS NEVER TESTED! Please send feedback.
 * @param transaction The element to work on.
 * @param sock The socket for incoming message.
 */
  int transaction_set_in_socket (transaction_t * transaction, int sock);
/**
 * Set the from value associated to this transaction.
 * NOTE: THIS HAS NEVER TESTED! Please send feedback.
 * @param transaction The element to work on.
 * @param sock The socket for outgoing message.
 */
  int transaction_set_out_socket (transaction_t * transaction, int sock);
/**
 * Set the from value associated to this transaction.
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param transaction The element to work on.
 * @param osip The osip_t element.
 */
  int transaction_set_config (transaction_t * transaction, osip_t * osip);

/**
 * Check if the response match a server transaction.
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param tr The transaction.
 * @param resp The SIP response received.
 */
  int transaction_matching_response_to_xict_17_1_3 (transaction_t * tr,
						    sip_t * resp);
/**
 * Check if the request match a client transaction.
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param tr The transaction.
 * @param request The SIP request received.
 */
  int transaction_matching_request_to_xist_17_2_3 (transaction_t * tr,
						   sip_t * request);
/**
 * Check if the tags in the From headers match.
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param from1 The first From header.
 * @param from2 The second From header.
 */
  int from_tag_match (from_t * from1, from_t * from2);
/**
 * Check if the tags in the To headers match.
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param to1 The first To header.
 * @param to2 The second To header.
 */
  int to_tag_match (to_t * to1, to_t * to2);
/**
 * Check if the Via headers match.
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param via1 The first Via header.
 * @param via2 The second Via header.
 */
  int via_match (via_t * via1, via_t * via2);
/**
 * Check if the first 2 parameters match the other ones.
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param to1 The initial to header.
 * @param from1 The initial from header.
 * @param to2 The new to header.
 * @param from2 The new from header.
 */
  int callleg_match (to_t * to1, from_t * from1, to_t * to2, from_t * from2);
/**
 * Check if the Call-Id headers match.
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param callid1 The initial Call-Id header.
 * @param callid2 The new Call-Id header.
 */
  int call_id_match (call_id_t * callid1, call_id_t * callid2);
/**
 * Check if the CSeq headers match.
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param cseq1 The initial CSeq header.
 * @param cseq2 The new CSeq header.
 */
  int cseq_match (cseq_t * cseq1, cseq_t * cseq2);

#endif				/* endif DOXYGEN */

/**
 * Initialise the global oSIP stack elements.
 * This method initialise the parser and load the fsm.
 * This method MUST be called before any call to oSIP is made.
 */
  int osip_global_init ();
/**
 * Free all global resource hold by the oSIP stack.
 * This can only be called after all osip_t element has been "stopped".
 */
  void osip_global_free ();
/** 
 * Allocate an osip_t element.
 * @param osip the element to allocate.
 */
  int osip_init (osip_t ** osip);
/**
 * Free all resource in a osip_t element.
 * @param osip The element to free.
 */
/**
 * Free all resource in a osip_t element.
 * @param osip The element to free.
 */
  void osip_free (osip_t * osip);

/**
 * Set a pointer in a osip_t element.
 * This help to find your application layer in callbacks.
 * @param osip The element to free.
 */
  void osip_set_application_context (osip_t * osip, void *pointer);

/**
 * Get a pointer in a osip_t element.
 * This help to find your application layer in callbacks.
 * @param osip The element to free.
 */
  void *osip_get_application_context (osip_t * osip);

#ifndef DOXYGEN

/**
 * Lock access to the list of ict transactions.
 * @param osip The element to work on.
 */
  int osip_ict_lock (osip_t * osip);
/**
 * Unlock access to the list of ict transactions.
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param osip The element to work on.
 */
  int osip_ict_unlock (osip_t * osip);
/**
 * Lock access to the list of ist transactions.
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param osip The element to work on.
 */
  int osip_ist_lock (osip_t * osip);
/**
 * Unlock access to the list of ist transactions.
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param osip The element to work on.
 */
  int osip_ist_unlock (osip_t * osip);
/**
 * Lock access to the list of nict transactions.
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param osip The element to work on.
 */
  int osip_nict_lock (osip_t * osip);
/**
 * Unlock access to the list of nict transactions.
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param osip The element to work on.
 */
  int osip_nict_unlock (osip_t * osip);
/**
 * Lock access to the list of nist transactions.
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param osip The element to work on.
 */
  int osip_nist_lock (osip_t * osip);
/**
 * Unlock access to the list of nist transactions.
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param osip The element to work on.
 */
  int osip_nist_unlock (osip_t * osip);
/**
 * Add a ict transaction in the ict list of transaction.
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param osip The element to work on.
 * @param ict The transaction to add.
 */
  int osip_add_ict (osip_t * osip, transaction_t * ict);
/**
 * Add a ist transaction in the ist list of transaction.
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param osip The element to work on.
 * @param ist The transaction to add.
 */
  int osip_add_ist (osip_t * osip, transaction_t * ist);
/**
 * Add a nict transaction in the nict list of transaction.
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param osip The element to work on.
 * @param nict The transaction to add.
 */
  int osip_add_nict (osip_t * osip, transaction_t * nict);
/**
 * Add a nist transaction in the nist list of transaction.
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param osip The element to work on.
 * @param nist The transaction to add.
 */
  int osip_add_nist (osip_t * osip, transaction_t * nist);
/**
 * Remove a ict transaction from the ict list of transaction.
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param osip The element to work on.
 * @param ict The transaction to add.
 */
  int osip_remove_ict (osip_t * osip, transaction_t * ict);
/**
 * Remove a ist transaction from the ist list of transaction.
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param osip The element to work on.
 * @param ist The transaction to add.
 */
  int osip_remove_ist (osip_t * osip, transaction_t * ist);
/**
 * Remove a nict transaction from the nict list of transaction.
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param osip The element to work on.
 * @param nict The transaction to add.
 */
  int osip_remove_nict (osip_t * osip, transaction_t * nict);
/**
 * Remove a nist transaction from the nist list of transaction.
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param osip The element to work on.
 * @param nist The transaction to add.
 */
  int osip_remove_nist (osip_t * osip, transaction_t * nist);

#endif

/**
 * Consume ALL pending sipevent_t previously added in the fifos of ict transactions.
 * @param osip The element to work on.
 */
  int osip_ict_execute (osip_t * osip);
/**
 * Consume ALL pending sipevent_t previously added in the fifos of ist transactions.
 * @param osip The element to work on.
 */
  int osip_ist_execute (osip_t * osip);
/**
 * Consume ALL pending sipevent_t previously added in the fifos of nict transactions.
 * @param osip The element to work on.
 */
  int osip_nict_execute (osip_t * osip);
/**
 * Consume ALL pending sipevent_t previously added in the fifos of nist transactions.
 * @param osip The element to work on.
 */
  int osip_nist_execute (osip_t * osip);

/**
 * Check if an ict transactions needs a timer event.
 * @param osip The element to work on.
 */
  void osip_timers_ict_execute (osip_t * osip);
/**
 * Check if an ist transactions needs a timer event.
 * @param osip The element to work on.
 */
  void osip_timers_ist_execute (osip_t * osip);
/**
 * Check if a nict transactions needs a timer event.
 * @param osip The element to work on.
 */
  void osip_timers_nict_execute (osip_t * osip);
/**
 * Check if a nist transactions needs a timer event.
 * @param osip The element to work on.
 */
  void osip_timers_nist_execute (osip_t * osip);

/* obsolete in 0.8.4: see comments in fsm/osip.c */
/* transaction_t *osip_distribute_event(osip_t *osip,sipevent_t* sipevent); */

/* obsolete: use osip_find_transaction + osip_create_transaction */
/**
 * Search for a transaction that match this event (MUST be a MESSAGE event).
 * @param transactions The list of transactions to work on.
 * @param evt The element representing the SIP MESSAGE.
 */
  transaction_t *osip_transaction_find (list_t * transactions,
					sipevent_t * evt);

/*
  BUG!!!
  BUG!!! there is a possible race conditions with this new method!!!
  BUG!!!
*/

/**
 * OBSOLETE!! -> some race conditions can happen in multi threaded applications.
 * <BR>Search for a transaction that match this event (MUST be a MESSAGE event).
 * @param osip The element to work on.
 * @param evt The element representing the SIP MESSAGE.
 */
#ifndef OSIP_MT
  transaction_t *osip_find_transaction (osip_t * osip, sipevent_t * evt);
#endif

  transaction_t *__osip_find_transaction (osip_t * osip, sipevent_t * evt,
					  int consume);

/**
 * Search for a transaction that match this event (MUST be a MESSAGE event)
 * and add this event if a transaction is found..
 * @param osip The element to work on.
 * @param evt The element representing the SIP MESSAGE.
 */
  int osip_find_transaction_and_add_event (osip_t * osip, sipevent_t * evt);

/**
 * Create a transaction for this event (MUST be a SIP REQUEST event).
 * @param osip The element to work on.
 * @param evt The element representing the new SIP REQUEST.
 */
  transaction_t *osip_create_transaction (osip_t * osip, sipevent_t * evt);

/**
 * Create a sipevent from a SIP message string.
 * @param buf The SIP message as a string.
 */
  sipevent_t *osip_parse (char *buf);

#ifndef DOXYGEN

/**
 * Allocate a sipevent.
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param type The type of the event.
 * @param transactionid The transaction id for this event.
 */
  sipevent_t *osip_new_event (type_t type, int transactionid);
/**
 * Allocate a sipevent (we know this message is an INCOMING SIP message).
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param type The type of the event.
 * @param transactionid The transaction id for this event.
 */
  sipevent_t *osip_new_incoming_sipmessage (sip_t * sip);

#endif

/**
 * Allocate a sipevent (we know this message is an OUTGOING SIP message).
 * @param sip The SIP message we want to send.
 */
  sipevent_t *osip_new_outgoing_sipmessage (sip_t * sip);


/**
 * Register the callback used to send SIP message.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_send_message (osip_t * cf,
				int (*cb) (transaction_t *, sip_t *, char *,
					   int, int));
/* callbacks for ict */
/**
 * Register the callback called when the transaction is deleted.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_ict_kill_transaction (osip_t * cf,
					void (*cb) (transaction_t *));
/**
 * Register the callback called when an INVITE is sent.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_ict_invite_sent (osip_t * cf,
				   void (*cb) (transaction_t *, sip_t *));
/**
 * Register the callback called when an INVITE is retransmitted.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_ict_invite_sent2 (osip_t * cf,
				    void (*cb) (transaction_t *, sip_t *));
/**
 * Register the callback called when an ACK is sent.
 * NOTE: This method is only called if the final response was not a 2xx
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_ict_ack_sent (osip_t * cf,
				void (*cb) (transaction_t *, sip_t *));
/**
 * Register the callback called when an ACK is retransmitted.
 * NOTE: This method is only called if the final response was not a 2xx
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_ict_ack_sent2 (osip_t * cf,
				 void (*cb) (transaction_t *, sip_t *));
/**
 * Register the callback called when a 1xx SIP message is received.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_ict_1xx_received (osip_t * cf,
				    void (*cb) (transaction_t *, sip_t *));
/**
 * Register the callback called when a 2xx SIP message is received.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_ict_2xx_received (osip_t * cf,
				    void (*cb) (transaction_t *, sip_t *));
/**
 * Register the callback called when a 2xx SIP message is received again.
 * NOTE: obsolete... THIS IS NEVER CALLED! as the transaction is destroyed
 * when the first 200 is received.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_ict_2xx_received2 (osip_t * cf,
				     void (*cb) (transaction_t *, sip_t *));
/**
 * Register the callback called when a 3xx SIP message is received.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_ict_3xx_received (osip_t * cf,
				    void (*cb) (transaction_t *, sip_t *));
/**
 * Register the callback called when a 4xx SIP message is received.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_ict_4xx_received (osip_t * cf,
				    void (*cb) (transaction_t *, sip_t *));
/**
 * Register the callback called when a 5xx SIP message is received.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_ict_5xx_received (osip_t * cf,
				    void (*cb) (transaction_t *, sip_t *));
/**
 * Register the callback called when a 6xx SIP message is received.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_ict_6xx_received (osip_t * cf,
				    void (*cb) (transaction_t *, sip_t *));
/**
 * Register the callback called when a retransmission of a final response is received.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_ict_3456xx_received2 (osip_t * cf,
					void (*cb) (transaction_t *,
						    sip_t *));
/**
 * Register the callback called when a transport error happens.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_ict_transport_error (osip_t * cf,
				       void (*cb) (transaction_t *,
						   int error));

/* callbacks for ist */
/**
 * Register the callback called when the transaction is deleted.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_ist_kill_transaction (osip_t * cf,
					void (*cb) (transaction_t *));
/**
 * Register the callback called when an INVITE is received.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_ist_invite_received (osip_t * cf,
				       void (*cb) (transaction_t *, sip_t *));
/**
 * Register the callback called when an INVITE is received again.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_ist_invite_received2 (osip_t * cf,
					void (*cb) (transaction_t *,
						    sip_t *));
/**
 * Register the callback called when an ACK is received.
 * NOTE: This method is only called if the final response was not a 2xx
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_ist_ack_received (osip_t * cf,
				    void (*cb) (transaction_t *, sip_t *));
/**
 * Register the callback called when an ACK is received again.
 * NOTE: This method is only called if the final response was not a 2xx
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_ist_ack_received2 (osip_t * cf,
				     void (*cb) (transaction_t *, sip_t *));
/**
 * Register the callback called when a 1xx SIP message is sent.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_ist_1xx_sent (osip_t * cf,
				void (*cb) (transaction_t *, sip_t *));
/**
 * Register the callback called when a 1xx SIP message is sent again.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_ist_1xx_sent2 (osip_t * cf,
				 void (*cb) (transaction_t *, sip_t *));
/**
 * Register the callback called when a 2xx SIP message is sent.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_ist_2xx_sent (osip_t * cf,
				void (*cb) (transaction_t *, sip_t *));
/**
 * Register the callback called when a 2xx SIP message is sent again.
 * NOTE: This method is never called because the transaction is destroyed
 * right after the first 200 OK is sent.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_ist_2xx_sent2 (osip_t * cf,
				 void (*cb) (transaction_t *, sip_t *));
/**
 * Register the callback called when a 3xx SIP message is sent.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_ist_3xx_sent (osip_t * cf,
				void (*cb) (transaction_t *, sip_t *));
/**
 * Register the callback called when a 4xx SIP message is sent.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_ist_4xx_sent (osip_t * cf,
				void (*cb) (transaction_t *, sip_t *));
/**
 * Register the callback called when a 5xx SIP message is sent.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_ist_5xx_sent (osip_t * cf,
				void (*cb) (transaction_t *, sip_t *));
/**
 * Register the callback called when a 6xx SIP message is sent.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_ist_6xx_sent (osip_t * cf,
				void (*cb) (transaction_t *, sip_t *));
/**
 * Register the callback called when a final response (not 200) is sent again.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_ist_3456xx_sent2 (osip_t * cf,
				    void (*cb) (transaction_t *, sip_t *));
/**
 * Register the callback called when a transport error happens.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_ist_transport_error (osip_t * cf,
				       void (*cb) (transaction_t *,
						   int error));


/* callbacks for nict */
/**
 * Register the callback called when the transaction is deleted.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_nict_kill_transaction (osip_t * cf,
					 void (*cb) (transaction_t *));
/**
 * Register the callback called when an REGISTER is sent.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_nict_register_sent (osip_t * cf,
				      void (*cb) (transaction_t *, sip_t *));
/**
 * Register the callback called when an BYE is sent.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_nict_bye_sent (osip_t * cf,
				 void (*cb) (transaction_t *, sip_t *));
/**
 * Register the callback called when an OPTIONS is sent.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_nict_options_sent (osip_t * cf,
				     void (*cb) (transaction_t *, sip_t *));
/**
 * Register the callback called when an INFO is sent.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_nict_info_sent (osip_t * cf,
				  void (*cb) (transaction_t *, sip_t *));
/**
 * Register the callback called when an CANCEL is sent.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_nict_cancel_sent (osip_t * cf,
				    void (*cb) (transaction_t *, sip_t *));
/**
 * Register the callback called when an NOTIFY is sent.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_nict_notify_sent (osip_t * cf,
				    void (*cb) (transaction_t *, sip_t *));
/**
 * Register the callback called when an SUBSCRIBE is sent.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_nict_subscribe_sent (osip_t * cf,
				       void (*cb) (transaction_t *, sip_t *));
/**
 * Register the callback called when an UNKNOWN REQUEST is sent.
 * NOTE: All SIP request that do not have specific callback
 * will use this one.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_nict_unknown_sent (osip_t * cf,
				     void (*cb) (transaction_t *, sip_t *));
/**
 * Register the callback called when an REQUEST is sent again.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_nict_request_sent2 (osip_t * cf,
				      void (*cb) (transaction_t *, sip_t *));
/**
 * Register the callback called when a 1xx SIP message is received.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_nict_1xx_received (osip_t * cf,
				     void (*cb) (transaction_t *, sip_t *));
/**
 * Register the callback called when a 2xx SIP message is received.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_nict_2xx_received (osip_t * cf,
				     void (*cb) (transaction_t *, sip_t *));
/**
 * Register the callback called when a 2xx SIP message is received again.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_nict_2xx_received2 (osip_t * cf,
				      void (*cb) (transaction_t *, sip_t *));
/**
 * Register the callback called when a 3xx SIP message is received.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_nict_3xx_received (osip_t * cf,
				     void (*cb) (transaction_t *, sip_t *));
/**
 * Register the callback called when a 4xx SIP message is received.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_nict_4xx_received (osip_t * cf,
				     void (*cb) (transaction_t *, sip_t *));
/**
 * Register the callback called when a 5xx SIP message is received.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_nict_5xx_received (osip_t * cf,
				     void (*cb) (transaction_t *, sip_t *));
/**
 * Register the callback called when a 6xx SIP message is received.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_nict_6xx_received (osip_t * cf,
				     void (*cb) (transaction_t *, sip_t *));
/**
 * Register the callback called when a final response (not 200) is received again.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_nict_3456xx_received2 (osip_t * cf,
					 void (*cb) (transaction_t *,
						     sip_t *));
/**
 * Register the callback called when a transport error happens.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_nict_transport_error (osip_t * cf,
					void (*cb) (transaction_t *,
						    int error));

  /* callbacks for nist */
/**
 * Register the callback called when the transaction is deleted.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_nist_kill_transaction (osip_t * cf,
					 void (*cb) (transaction_t *));
/**
 * Register the callback called when an REGISTER is received.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_nist_register_received (osip_t * cf,
					  void (*cb) (transaction_t *,
						      sip_t *));
/**
 * Register the callback called when an BYE is received.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_nist_bye_received (osip_t * cf,
				     void (*cb) (transaction_t *, sip_t *));
/**
 * Register the callback called when an OPTIONS is received.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_nist_options_received (osip_t * cf,
					 void (*cb) (transaction_t *,
						     sip_t *));
/**
 * Register the callback called when an INFO is received.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_nist_info_received (osip_t * cf,
				      void (*cb) (transaction_t *, sip_t *));
/**
 * Register the callback called when an CANCEL is received.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_nist_cancel_received (osip_t * cf,
					void (*cb) (transaction_t *,
						    sip_t *));
/**
 * Register the callback called when an NOTIFY is received.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_nist_notify_received (osip_t * cf,
					void (*cb) (transaction_t *,
						    sip_t *));
/**
 * Register the callback called when an SUBSCRIBE is received.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_nist_subscribe_received (osip_t * cf,
					   void (*cb) (transaction_t *,
						       sip_t *));
/**
 * Register the callback called when an unknown REQUEST is received.
 * NOTE: When the message does not have a specific callback, this
 * callback is used instead.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_nist_unknown_received (osip_t * cf,
					 void (*cb) (transaction_t *,
						     sip_t *));
  /* ... TO BE ADDED: All known and used method extensions */
/**
 * Register the callback called when a REQUEST is received again.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_nist_request_received2 (osip_t * cf,
					  void (*cb) (transaction_t *,
						      sip_t *));
/**
 * Register the callback called when a 1xx SIP message is sent.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_nist_1xx_sent (osip_t * cf,
				 void (*cb) (transaction_t *, sip_t *));
/**
 * Register the callback called when a 2xx SIP message is sent.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_nist_2xx_sent (osip_t * cf,
				 void (*cb) (transaction_t *, sip_t *));
/**
 * Register the callback called when a 2xx SIP message is sent.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_nist_2xx_sent2 (osip_t * cf,
				  void (*cb) (transaction_t *, sip_t *));
/**
 * Register the callback called when a 3xx SIP message is sent.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_nist_3xx_sent (osip_t * cf,
				 void (*cb) (transaction_t *, sip_t *));
/**
 * Register the callback called when a 4xx SIP message is sent.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_nist_4xx_sent (osip_t * cf,
				 void (*cb) (transaction_t *, sip_t *));
/**
 * Register the callback called when a 5xx SIP message is sent.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_nist_5xx_sent (osip_t * cf,
				 void (*cb) (transaction_t *, sip_t *));
/**
 * Register the callback called when a 6xx SIP message is sent.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_nist_6xx_sent (osip_t * cf,
				 void (*cb) (transaction_t *, sip_t *));
/**
 * Register the callback called when a final response is sent again.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_nist_3456xx_sent2 (osip_t * cf,
				     void (*cb) (transaction_t *, sip_t *));
/**
 * Register the callback called when a transport error happens.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_setcb_nist_transport_error (osip_t * cf,
					void (*cb) (transaction_t *,
						    int error));


/* FOR INCOMING TRANSACTION */
/**
 * Check if the sipevent is of type RCV_REQINVITE.
 * @param event the event to check.
 */
#define EVT_IS_RCV_INVITE(event)       (event->type==RCV_REQINVITE)
/**
 * Check if the sipevent is of type RCV_REQACK.
 * @param event the event to check.
 */
#define EVT_IS_RCV_ACK(event)          (event->type==RCV_REQACK)
/**
 * Check if the sipevent is of type RCV_REQUEST.
 * @param event the event to check.
 */
#define EVT_IS_RCV_REQUEST(event)      (event->type==RCV_REQUEST)
/**
 * Check if the sipevent is of type RCV_STATUS_1XX.
 * @param event the event to check.
 */
#define EVT_IS_RCV_STATUS_1XX(event)   (event->type==RCV_STATUS_1XX)
/**
 * Check if the sipevent is of type RCV_STATUS_2XX.
 * @param event the event to check.
 */
#define EVT_IS_RCV_STATUS_2XX(event)   (event->type==RCV_STATUS_2XX)
/**
 * Check if the sipevent is of type RCV_STATUS_3456XX.
 * @param event the event to check.
 */
#define EVT_IS_RCV_STATUS_3456XX(event)   (event->type==RCV_STATUS_3456XX)


/* FOR OUTGOING TRANSACTION */
/**
 * Check if the sipevent is of type SND_REQINVITE.
 * @param event the event to check.
 */
#define EVT_IS_SND_INVITE(event)       (event->type==SND_REQINVITE)
/**
 * Check if the sipevent is of type SND_REQACK.
 * @param event the event to check.
 */
#define EVT_IS_SND_ACK(event)          (event->type==SND_REQACK)
/**
 * Check if the sipevent is of type SND_REQUEST.
 * @param event the event to check.
 */
#define EVT_IS_SND_REQUEST(event)      (event->type==SND_REQUEST)
/**
 * Check if the sipevent is of type SND_STATUS_1XX.
 * @param event the event to check.
 */
#define EVT_IS_SND_STATUS_1XX(event)   (event->type==SND_STATUS_1XX)
/**
 * Check if the sipevent is of type SND_STATUS_2XX.
 * @param event the event to check.
 */
#define EVT_IS_SND_STATUS_2XX(event)   (event->type==SND_STATUS_2XX)
/**
 * Check if the sipevent is of type SND_STATUS_3456XX.
 * @param event the event to check.
 */
#define EVT_IS_SND_STATUS_3456XX(event)   (event->type==SND_STATUS_3456XX)
/**
 * Check if the sipevent is of an incoming SIP MESSAGE.
 * @param event the event to check.
 */
#define EVT_IS_INCOMINGMSG(event)      (event->type>=RCV_REQINVITE \
                	               &&event->type<=RCV_STATUS_3456XX)
/**
 * Check if the sipevent is of an incoming SIP REQUEST.
 * @param event the event to check.
 */
#define EVT_IS_INCOMINGREQ(event)      (EVT_IS_RCV_INVITE(event) \
                                       ||EVT_IS_RCV_ACK(event) \
                                       ||EVT_IS_RCV_REQUEST(event))
/**
 * Check if the sipevent is of an incoming SIP RESPONSE.
 * @param event the event to check.
 */
#define EVT_IS_INCOMINGRESP(event)     (EVT_IS_RCV_STATUS_1XX(event) \
                                       ||EVT_IS_RCV_STATUS_2XX(event) \
				       ||EVT_IS_RCV_STATUS_3456XX(event))
/**
 * Check if the sipevent is of an outgoing SIP MESSAGE.
 * @param event the event to check.
 */
#define EVT_IS_OUTGOINGMSG(event)      (event->type>=SND_REQINVITE \
                	               &&event->type<=SND_STATUS_3456XX)
/**
 * Check if the sipevent is of an outgoing SIP REQUEST.
 * @param event the event to check.
 */
#define EVT_IS_OUTGOINGREQ(event)      (EVT_IS_SND_INVITE(event) \
                                       ||EVT_IS_SND_ACK(event) \
                                       ||EVT_IS_SND_REQUEST(event))
/**
 * Check if the sipevent is of an outgoing SIP RESPONSE.
 * @param event the event to check.
 */
#define EVT_IS_OUTGOINGRESP(event)     (EVT_IS_SND_STATUS_1XX(event) \
                                       ||EVT_IS_SND_STATUS_2XX(event) \
				       ||EVT_IS_SND_STATUS_3456XX(event))

/**
 * Check if the sipevent is a SIP MESSAGE.
 * @param event the event to check.
 */
#define EVT_IS_MSG(event)              (event->type>=RCV_REQINVITE \
                	               &&event->type<=SND_STATUS_3456XX)
/**
 * Check if the sipevent is of type KILL_TRANSACTION.
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param event the event to check.
 */
#define EVT_IS_KILL_TRANSACTION(event) (event->type==KILL_TRANSACTION)

/*
 * Check if the sipevent is of type UNKNOWN_EVT.
 * NOTE: obsolete! This never happen.
 * @param event the event to check.
 */
/* #define EVT_IS_UNKNOWN_EVT(event)      (event->type==UNKNOWN_EVT) */
/*
 * Check if the sipevent is of type UNKNOWN_EVT.
 * NOTE: obsolete!
 * @param event the event to check.
 */
/* #define EVT_IS_TIMEOUT(event)          (event->type==TIMEOUT) */


#ifdef __cplusplus
}
#endif


/** @} */

#endif
