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

#include <osipparser2/osip_const.h>

#include <time.h>

#ifdef WIN32
#include <Winsock.h>
#endif

#ifdef __sun
#include <sys/types.h>
#endif

#include <osipparser2/osip_parser.h>
#include <osip2/osip_fifo.h>

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
 * (osip_event_t) that can occur. EVENTS can be either TIMEOUT events and
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
 *  osip_set_cb_send_message(osip, &application_cb_snd_message);
 *
 * // here is the long list of callback that you can register. Some
 * // of this callbacks are very useless (announcing a retransmission,
 * // or announcing that you have sent a SIP message which you may already
 * // know...).
 *
 * // those callbacks are mandatory. They are called when oSIP has decided
 * // that this transaction MUST no longer be handled by oSIP. (This is
 * // called in both successful or error cases scenario)
  osip_set_kill_transaction_callback(osip ,OSIP_ICT_KILL_TRANSACTION,
                                 &cb_ict_kill_transaction);
  osip_set_kill_transaction_callback(osip ,OSIP_NIST_KILL_TRANSACTION,
                                 &cb_ist_kill_transaction);
  osip_set_kill_transaction_callback(osip ,OSIP_NICT_KILL_TRANSACTION,
                                 &cb_nict_kill_transaction);
  osip_set_kill_transaction_callback(osip ,OSIP_NIST_KILL_TRANSACTION,
                                 &cb_nist_kill_transaction);
 *
 * // those callbacks are optional. The purpose is to announce retransmissions
 * // of SIP message decided by the oSIP stack. (They can be used for statistics?)
  osip_set_message_callback(osip ,OSIP_ICT_STATUS_2XX_RECEIVED_AGAIN,
                        &cb_rcvresp_retransmission);
  osip_set_message_callback(osip ,OSIP_ICT_STATUS_3456XX_RECEIVED_AGAIN,
                        &cb_rcvresp_retransmission);
  osip_set_message_callback(osip ,OSIP_ICT_INVITE_SENT_AGAIN,
                        &cb_sndreq_retransmission);
  osip_set_message_callback(osip ,OSIP_IST_STATUS_2XX_SENT_AGAIN,
                        &cb_sndresp_retransmission);
  osip_set_message_callback(osip ,OSIP_IST_STATUS_3456XX_SENT_AGAIN,
                        &cb_sndresp_retransmission);
  osip_set_message_callback(osip ,OSIP_IST_INVITE_RECEIVED_AGAIN,
                        &cb_rcvreq_retransmission);
  osip_set_message_callback(osip ,OSIP_NICT_STATUS_2XX_RECEIVED_AGAIN,
                        &cb_rcvresp_retransmission);
  osip_set_message_callback(osip ,OSIP_NICT_STATUS_3456XX_RECEIVED_AGAIN,
                        &cb_rcvresp_retransmission);
  osip_set_message_callback(osip ,OSIP_NICT_REQUEST_SENT_AGAIN,
                        &cb_sndreq_retransmission);
  osip_set_message_callback(osip ,OSIP_NIST_STATUS_2XX_SENT_AGAIN,
                        &cb_sndresp_retransmission);
  osip_set_message_callback(osip ,OSIP_NIST_STATUS_3456XX_SENT_AGAIN,
                        &cb_sndresp_retransmission);
  osip_set_message_callback(osip ,OSIP_NIST_REQUEST_RECEIVED_AGAIN,
                        &cb_rcvreq_retransmission);
 *
 * // those callbacks are mandatory. They are used to announce network related
 * // errors (the return code of the network callback if it was not 0)
  osip_set_transport_error_callback(osip ,OSIP_ICT_TRANSPORT_ERROR,
                                    &cb_transport_error);
  osip_set_transport_error_callback(osip ,OSIP_IST_TRANSPORT_ERROR,
                                    &cb_transport_error);
  osip_set_transport_error_callback(osip ,OSIP_NICT_TRANSPORT_ERROR,
                                    &cb_transport_error);
  osip_set_transport_error_callback(osip ,OSIP_NIST_TRANSPORT_ERROR,
                                    &cb_transport_error);
 *  
 * // those callbacks are optional. They are used to announce the initial
 * // request sent for a newly created transaction.
  osip_set_message_callback(osip ,OSIP_ICT_INVITE_SENT,     &cb_sndinvite);
  osip_set_message_callback(osip ,OSIP_ICT_ACK_SENT,        &cb_sndack);
  osip_set_message_callback(osip ,OSIP_NICT_REGISTER_SENT,  &cb_sndregister);
  osip_set_message_callback(osip ,OSIP_NICT_BYE_SENT,       &cb_sndbye);
  osip_set_message_callback(osip ,OSIP_NICT_CANCEL_SENT,    &cb_sndcancel);
  osip_set_message_callback(osip ,OSIP_NICT_INFO_SENT,      &cb_sndinfo);
  osip_set_message_callback(osip ,OSIP_NICT_OPTIONS_SENT,   &cb_sndoptions);
  osip_set_message_callback(osip ,OSIP_NICT_SUBSCRIBE_SENT, &cb_sndsubscribe);
  osip_set_message_callback(osip ,OSIP_NICT_NOTIFY_SENT,    &cb_sndnotify);
  osip_set_message_callback(osip ,OSIP_NICT_UNKNOWN_REQUEST_SENT, &cb_sndunkrequest);
 *
 * // those callbacks are mandatory. They are used to announce the initial
 * // response received for a transaction. (for SIP response between 100 and 199,
 * // all responses are announced because this is not a retransmission case)
  osip_set_message_callback(osip ,OSIP_ICT_STATUS_1XX_RECEIVED, &cb_rcv1xx);
  osip_set_message_callback(osip ,OSIP_ICT_STATUS_2XX_RECEIVED, &cb_rcv2xx);
  osip_set_message_callback(osip ,OSIP_ICT_STATUS_3XX_RECEIVED, &cb_rcv3xx);
  osip_set_message_callback(osip ,OSIP_ICT_STATUS_4XX_RECEIVED, &cb_rcv4xx);
  osip_set_message_callback(osip ,OSIP_ICT_STATUS_5XX_RECEIVED, &cb_rcv5xx);
  osip_set_message_callback(osip ,OSIP_ICT_STATUS_6XX_RECEIVED, &cb_rcv6xx);
 *  
 * // those callbacks are optional. They are used to announce the initial
 * // response sent for a transaction. (for SIP response between 100 and 199,
 * // all responses are announced because this is not a retransmission case)
  
  osip_set_message_callback(osip ,OSIP_IST_STATUS_1XX_SENT, &cb_snd1xx);
  osip_set_message_callback(osip ,OSIP_IST_STATUS_2XX_SENT, &cb_snd2xx);
  osip_set_message_callback(osip ,OSIP_IST_STATUS_3XX_SENT, &cb_snd3xx);
  osip_set_message_callback(osip ,OSIP_IST_STATUS_4XX_SENT, &cb_snd4xx);
  osip_set_message_callback(osip ,OSIP_IST_STATUS_5XX_SENT, &cb_snd5xx);
  osip_set_message_callback(osip ,OSIP_IST_STATUS_6XX_SENT, &cb_snd6xx);
 *
 * // those callbacks are mandatory. They are used to announce the initial
 * // response received for a transaction. (for SIP response between 100 and 199,
 * // all responses are announced because this is not a retransmission case)  
  osip_set_message_callback(osip ,OSIP_NICT_STATUS_1XX_RECEIVED, &cb_rcv1xx);
  osip_set_message_callback(osip ,OSIP_NICT_STATUS_2XX_RECEIVED, &cb_rcv2xx);
  osip_set_message_callback(osip ,OSIP_NICT_STATUS_3XX_RECEIVED, &cb_rcv3xx);
  osip_set_message_callback(osip ,OSIP_NICT_STATUS_4XX_RECEIVED, &cb_rcv4xx);
  osip_set_message_callback(osip ,OSIP_NICT_STATUS_5XX_RECEIVED, &cb_rcv5xx);
  osip_set_message_callback(osip ,OSIP_NICT_STATUS_6XX_RECEIVED, &cb_rcv6xx);
 *
 * // those callbacks are optional. They are used to announce the initial
 * // response sent for a transaction. (for SIP response between 100 and 199,
 * // all responses are announced because this is not a retransmission case)
  osip_set_message_callback(osip ,OSIP_NIST_STATUS_1XX_SENT, &cb_snd1xx);
  osip_set_message_callback(osip ,OSIP_NIST_STATUS_2XX_SENT, &cb_snd2xx);
  osip_set_message_callback(osip ,OSIP_NIST_STATUS_3XX_SENT, &cb_snd3xx);
  osip_set_message_callback(osip ,OSIP_NIST_STATUS_4XX_SENT, &cb_snd4xx);
  osip_set_message_callback(osip ,OSIP_NIST_STATUS_5XX_SENT, &cb_snd5xx);
  osip_set_message_callback(osip ,OSIP_NIST_STATUS_6XX_SENT, &cb_snd6xx);
 *
 * // those callbacks are mandatory. They are used to announce the initial
 * // request received for a transaction. It is not useless to notice that
 * // a special behaviour exist for the 200 OK and the ACK in the case of
 * // a successful INVITE transaction. This will be discussed later.
  osip_set_message_callback(osip ,OSIP_IST_INVITE_RECEIVED,     &cb_rcvinvite);
  osip_set_message_callback(osip ,OSIP_IST_ACK_RECEIVED,        &cb_rcvack);
  osip_set_message_callback(osip ,OSIP_IST_ACK_RECEIVED_AGAIN,  &cb_rcvack2);
  osip_set_message_callback(osip ,OSIP_NIST_REGISTER_RECEIVED,  &cb_rcvregister);
  osip_set_message_callback(osip ,OSIP_NIST_BYE_RECEIVED,       &cb_rcvbye);
  osip_set_message_callback(osip ,OSIP_NIST_CANCEL_RECEIVED,    &cb_rcvcancel);
  osip_set_message_callback(osip ,OSIP_NIST_INFO_RECEIVED,      &cb_rcvinfo);
  osip_set_message_callback(osip ,OSIP_NIST_OPTIONS_RECEIVED,   &cb_rcvoptions);
  osip_set_message_callback(osip ,OSIP_NIST_SUBSCRIBE_RECEIVED, &cb_rcvsubscribe);
  osip_set_message_callback(osip ,OSIP_NIST_NOTIFY_RECEIVED,    &cb_rcvnotify);
  osip_set_message_callback(osip ,OSIP_NIST_UNKNOWN_REQUEST_RECEIVED, &cb_sndunkrequest);
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
 * osip_t *osip       = your_global_osip_context;
 * osip_transaction_t *transaction;
 * osip_message_t     *sip_register_message;
 * osip_event_t       *sipevent;
 *
 * application_build_register(&sip_register_message);
 * osip_transaction_init(&transaction,
 *		   NICT, //a REGISTER is a Non-Invite-Client-Transaction
 *		   osip,
 *		   sip_register_message);
 *
 * // If you have a special context that you want to associate to that
 * // transaction, you can use a special method that associate your context
 * // to the transaction context.
 *
 * osip_transaction_set_your_instance(transaction, any_pointer);
 *
 * // at this point, the transaction context exists in oSIP but you still have
 * // to give the SIP message to the finite state machine.   
 * sipevent = osip_new_outgoing_sipmessage(msg);
 * sipevent->transactionid =  transaction->transactionid;
 * osip_transaction_add_event(transaction, sipevent);
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
 * that when your are calling osip_transaction_execute(), it is forbidden to call
 * this method again with the same transaction context until the first call
 * has returned. In a multi threaded application, if one thread handles one
 * transaction, the code will be the following:
 * <BR><pre><code>
 *   while (1)
 *     {
 *       se = (osip_event_t *)osip_fifo_get(transaction->transactionff);
 *       if (se==NULL)
 * 	  osip_thread_exit();
 *       if (osip_transaction_execute(transaction,se)<1)  // deletion asked
 * 	  osip_thread_exit();
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
 * Those states are extracted from the diagram found in rfc3261.txt
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
    IST_PRE_PROCEEDING,
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

/**
 * Enumeration for transaction type.
 * A transaction can be either of:
 *  ICT,
 *  IST,
 *  NICT,
 *  NIST,
 */
  typedef enum osip_fsm_type_t
  {
    ICT, /**< Invite Client (outgoing) Transaction */
    IST, /**< Invite Server (incoming) Transaction */
    NICT,/**< Non-Invite Client (outgoing) Transaction */
    NIST /**< Non-Invite Server (incoming) Transaction */
  }
  osip_fsm_type_t;

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
 * @defvar osip_ict_t
 */
  typedef struct osip_ict osip_ict_t;

  struct osip_ict
  {
    /* state machine is implied... */
    int timer_a_length;		/* A=T1, A=2xT1... (unreliable tr only)  */
    struct timeval timer_a_start;
    int timer_b_length;		/* B = 64* T1                            */
    struct timeval timer_b_start;	/* fire when transaction timeouts        */
    int timer_d_length;		/* D >= 32s for unreliable tr (or 0)     */
    struct timeval timer_d_start;	/* should be equal to timer H            */
    char *destination;		/* url used to send requests         */
    int port;			/* port of next hop                  */

  };

/**
 * Structure for NON-INVITE CLIENT TRANSACTION (outgoing NON-INVITE transaction).
 * @defvar osip_nict_t
 */
  typedef struct osip_nict osip_nict_t;

  struct osip_nict
  {
    /* state machine is implied... */

    int timer_e_length;		/* A=T1, A=2xT1... (unreliable tr. only) */
    struct timeval timer_e_start;	/*  (else = -1) not active               */
    int timer_f_length;		/* B = 64* T1                            */
    struct timeval timer_f_start;	/* fire when transaction timeouts        */
    int timer_k_length;		/* K = T4 (else = 0)                     */
    struct timeval timer_k_start;
    char *destination;		/* url used to send requests         */
    int port;			/* port of next hop                  */

  };

/**
 * Structure for INVITE SERVER TRANSACTION (incoming INVITE transaction).
 * @defvar osip_ist_t
 */
  typedef struct osip_ist osip_ist_t;

  struct osip_ist
  {
    int timer_g_length;		/* G=MIN(T1*2,T2) for unreliable trans.  */
    struct timeval timer_g_start;	/* 0 when reliable transport is used!    */
    int timer_h_length;		/* H = 64* T1                            */
    struct timeval timer_h_start;	/* fire when if no ACK is received       */
    int timer_i_length;		/* I = T4 for unreliable transport (or 0) */
    struct timeval timer_i_start;	/* absorb all ACK                        */
  };

/**
 * Structure for NON-INVITE SERVER TRANSACTION (incoming SERVER transaction).
 * @defvar osip_nist_t
 */
  typedef struct osip_nist osip_nist_t;

  struct osip_nist
  {
    int timer_j_length;		/* J = 64*T1 (else 0) */
    struct timeval timer_j_start;
  };

/**
 * Structure for transaction handling.
 * @defvar osip_transaction_t
 */
  typedef struct osip_transaction osip_transaction_t;

  struct osip_transaction
  {

    void *your_instance;	/* add whatever you want here.       */
    int transactionid;		/* simple id used to identify the tr. */
    osip_fifo_t *transactionff;	/* events must be added in this fifo */

    osip_via_t *topvia;		/* CALL-LEG definition */
    osip_from_t *from;		/* CALL-LEG definition */
    osip_to_t *to;
    osip_call_id_t *callid;
    osip_cseq_t *cseq;

    osip_message_t *orig_request;	/* last request sent                 */
    osip_message_t *last_response;	/* last response received            */
    osip_message_t *ack;	/* ack request sent                  */

    state_t state;		/* state of transaction              */

    time_t birth_time;		/* birth_date of transaction         */
    time_t completed_time;	/* end   date of transaction         */

    /* RESPONSE are received on this socket */
    int in_socket;
    /* REQUESTS are sent on this socket */
    int out_socket;

    void *config;		/* transaction is managed by config  */

    osip_fsm_type_t ctx_type;
    osip_ict_t *ict_context;
    osip_ist_t *ist_context;
    osip_nict_t *nict_context;
    osip_nist_t *nist_context;
  };


/**
 * Enumeration for callback type.
 */
  typedef enum osip_message_callback_type
  {
    OSIP_ICT_INVITE_SENT = 0,               /**< INVITE MESSAGE SENT */
    OSIP_ICT_INVITE_SENT_AGAIN,             /**< INVITE MESSAGE RETRANSMITTED */
    OSIP_ICT_ACK_SENT,                      /**< ACK MESSAGE SENT */
    OSIP_ICT_ACK_SENT_AGAIN,                /**< ACK MESSAGE RETRANSMITTED */
    OSIP_ICT_STATUS_1XX_RECEIVED,           /**< 1XX FOR INVITE RECEIVED */
    OSIP_ICT_STATUS_2XX_RECEIVED,           /**< 2XX FOR INVITE RECEIVED */
    OSIP_ICT_STATUS_2XX_RECEIVED_AGAIN,     /**< 2XX FOR INVITE RECEIVED AGAIN */
    OSIP_ICT_STATUS_3XX_RECEIVED,           /**< 3XX FOR INVITE RECEIVED */
    OSIP_ICT_STATUS_4XX_RECEIVED,           /**< 4XX FOR INVITE RECEIVED */
    OSIP_ICT_STATUS_5XX_RECEIVED,           /**< 5XX FOR INVITE RECEIVED */
    OSIP_ICT_STATUS_6XX_RECEIVED,           /**< 6XX FOR INVITE RECEIVED */
    OSIP_ICT_STATUS_3456XX_RECEIVED_AGAIN,  /**< RESPONSE RECEIVED AGAIN */

    OSIP_IST_INVITE_RECEIVED,               /**< INVITE MESSAGE RECEIVED */
    OSIP_IST_INVITE_RECEIVED_AGAIN,         /**< INVITE MESSAGE RECEIVED AGAN */
    OSIP_IST_ACK_RECEIVED,                  /**< ACK MESSAGE RECEIVED */
    OSIP_IST_ACK_RECEIVED_AGAIN,            /**< ACK MESSAGE RECEIVED AGAIN */
    OSIP_IST_STATUS_1XX_SENT,               /**< 1XX FOR INVITE SENT */
    OSIP_IST_STATUS_2XX_SENT,               /**< 2XX FOR INVITE SENT */
    OSIP_IST_STATUS_2XX_SENT_AGAIN,         /**< 2XX FOR INVITE RETRANSMITTED */
    OSIP_IST_STATUS_3XX_SENT,               /**< 3XX FOR INVITE SENT */
    OSIP_IST_STATUS_4XX_SENT,               /**< 4XX FOR INVITE SENT */
    OSIP_IST_STATUS_5XX_SENT,               /**< 5XX FOR INVITE SENT */
    OSIP_IST_STATUS_6XX_SENT,               /**< 6XX FOR INVITE SENT */
    OSIP_IST_STATUS_3456XX_SENT_AGAIN,      /**< RESPONSE RETRANSMITTED */

    OSIP_NICT_REGISTER_SENT,                /**< REGISTER MESSAGE SENT */
    OSIP_NICT_BYE_SENT,                     /**< BYE MESSAGE SENT */
    OSIP_NICT_OPTIONS_SENT,                 /**< OPTIONS MESSAGE SENT */
    OSIP_NICT_INFO_SENT,                    /**< INFO MESSAGE SENT */
    OSIP_NICT_CANCEL_SENT,                  /**< CANCEL MESSAGE SENT */
    OSIP_NICT_NOTIFY_SENT,                  /**< NOTIFY MESSAGE SENT */
    OSIP_NICT_SUBSCRIBE_SENT,               /**< SUBSCRIBE MESSAGE SENT */
    OSIP_NICT_UNKNOWN_REQUEST_SENT,         /**< UNKNOWN REQUEST MESSAGE SENT */
    OSIP_NICT_REQUEST_SENT_AGAIN,           /**< REQUEST MESSAGE RETRANMITTED */
    OSIP_NICT_STATUS_1XX_RECEIVED,          /**< 1XX FOR MESSAGE RECEIVED */
    OSIP_NICT_STATUS_2XX_RECEIVED,          /**< 2XX FOR MESSAGE RECEIVED */
    OSIP_NICT_STATUS_2XX_RECEIVED_AGAIN,    /**< 2XX FOR MESSAGE RECEIVED AGAIN */
    OSIP_NICT_STATUS_3XX_RECEIVED,          /**< 3XX FOR MESSAGE RECEIVED */
    OSIP_NICT_STATUS_4XX_RECEIVED,          /**< 4XX FOR MESSAGE RECEIVED */
    OSIP_NICT_STATUS_5XX_RECEIVED,          /**< 5XX FOR MESSAGE RECEIVED */
    OSIP_NICT_STATUS_6XX_RECEIVED,          /**< 6XX FOR MESSAGE RECEIVED */
    OSIP_NICT_STATUS_3456XX_RECEIVED_AGAIN, /**< RESPONSE RECEIVED AGAIN */

    OSIP_NIST_REGISTER_RECEIVED,            /**< REGISTER RECEIVED */
    OSIP_NIST_BYE_RECEIVED,                 /**< BYE RECEIVED */
    OSIP_NIST_OPTIONS_RECEIVED,             /**< OPTIONS RECEIVED */
    OSIP_NIST_INFO_RECEIVED,                /**< INFO RECEIVED */
    OSIP_NIST_CANCEL_RECEIVED,              /**< CANCEL RECEIVED */
    OSIP_NIST_NOTIFY_RECEIVED,              /**< NOTIFY RECEIVED */
    OSIP_NIST_SUBSCRIBE_RECEIVED,           /**< SUBSCRIBE RECEIVED */

    OSIP_NIST_UNKNOWN_REQUEST_RECEIVED,     /**< UNKNWON REQUEST RECEIVED */
    OSIP_NIST_REQUEST_RECEIVED_AGAIN,       /**< UNKNWON REQUEST RECEIVED AGAIN */
    OSIP_NIST_STATUS_1XX_SENT,              /**< 1XX FOR MESSAGE SENT */
    OSIP_NIST_STATUS_2XX_SENT,              /**< 2XX FOR MESSAGE SENT */
    OSIP_NIST_STATUS_2XX_SENT_AGAIN,        /**< 2XX FOR MESSAGE RETRANSMITTED */
    OSIP_NIST_STATUS_3XX_SENT,              /**< 3XX FOR MESSAGE SENT */
    OSIP_NIST_STATUS_4XX_SENT,              /**< 4XX FOR MESSAGE SENT */
    OSIP_NIST_STATUS_5XX_SENT,              /**< 5XX FOR MESSAGE SENT */
    OSIP_NIST_STATUS_6XX_SENT,              /**< 6XX FOR MESSAGE SENT */
    OSIP_NIST_STATUS_3456XX_SENT_AGAIN,     /**< RESPONSE RETRANSMITTED */

    OSIP_MESSAGE_CALLBACK_COUNT             /**< END OF ENUM */
  } osip_message_callback_type_t;

/**
 * Enumeration for callback type used when transaction is over.
 */
  typedef enum osip_kill_callback_type
  {
    OSIP_ICT_KILL_TRANSACTION,      /**< end of Client INVITE transaction */
    OSIP_IST_KILL_TRANSACTION,      /**< end of Server INVITE transaction */
    OSIP_NICT_KILL_TRANSACTION,     /**< end of Client Non-INVITE transaction */
    OSIP_NIST_KILL_TRANSACTION,     /**< end of Server Non-INVITE transaction */

    OSIP_KILL_CALLBACK_COUNT        /**< END OF ENUM */
  } osip_kill_callback_type_t;

/**
 * Enumeration for callback type used when a transport error is detected.
 */
  typedef enum osip_transport_error_callback_type
  {
    OSIP_ICT_TRANSPORT_ERROR,             /**< transport error for ICT */
    OSIP_IST_TRANSPORT_ERROR,             /**< transport error for IST */
    OSIP_NICT_TRANSPORT_ERROR,            /**< transport error for NICT */
    OSIP_NIST_TRANSPORT_ERROR,            /**< transport error for NIST */

    OSIP_TRANSPORT_ERROR_CALLBACK_COUNT   /**< END OF ENUM */
  } osip_transport_error_callback_type_t;

  typedef void (*osip_message_cb_t) (int type, osip_transaction_t *,
				     osip_message_t *);
  typedef void (*osip_kill_transaction_cb_t) (int type, osip_transaction_t *);
  typedef void (*osip_transport_error_cb_t) (int type, osip_transaction_t *,
					     int error);


#ifdef OSIP_RETRANSMIT_2XX
  struct osip_dialog;

  typedef struct ixt_t ixt_t;

  struct ixt_t
  {
    /* any ACK received that match this context will set counter to -1 */
    struct osip_dialog *dialog;
    osip_message_t *msg2xx;	/* copy of string to retransmit */
    osip_message_t *ack;	/* useless for ist */
    time_t start;
    int interval;		/* between each retransmission, in ms */
    char *dest;
    int port;
    int sock;
    int counter;		/* start at 7 */
  };

#endif

/**
 * Structure for osip handling.
 * In order to use osip, you have to manage at least one global instance
 * of an osip_t element. Then, you'll register a set of required callbacks
 * and a set of optional ones.
 * @defvar osip_t
 */
  typedef struct osip osip_t;

  struct osip
  {

    void *application_context;	/* a pointer for your personnal usage */

    /* list of transactions for ict, ist, nict, nist */
    osip_list_t *osip_ict_transactions;
    osip_list_t *osip_ist_transactions;
    osip_list_t *osip_nict_transactions;
    osip_list_t *osip_nist_transactions;

    osip_list_t *ixt_retransmissions;	/* for retransmission of 2xx & ACK for INVITE */

    osip_message_cb_t msg_callbacks[OSIP_MESSAGE_CALLBACK_COUNT];
    osip_kill_transaction_cb_t kill_callbacks[OSIP_KILL_CALLBACK_COUNT];
    osip_transport_error_cb_t
      tp_error_callbacks[OSIP_TRANSPORT_ERROR_CALLBACK_COUNT];

    /* callbacks for sending messages */
    int (*cb_send_message) (osip_transaction_t *, osip_message_t *, char *,
			    int, int);
  };

/**
 * Set a callback for each transaction operation. 
 * @param osip The element to work on.
 * @param type The event type to hook on.
 * @param cb The method to be called upon the event.
 */
  int osip_set_message_callback (osip_t *osip, int type, osip_message_cb_t cb);

/**
 * Set a callback for transaction operation related to the end of transactions. 
 * @param osip The element to work on.
 * @param type The event type to hook on.
 * @param cb The method to be called upon the event.
 */
  int osip_set_kill_transaction_callback (osip_t *osip, int type,
					  osip_kill_transaction_cb_t cb);

/**
 * Set a callback for each transaction operation related to network error.
 * @param osip The element to work on.
 * @param type The event type to hook on.
 * @param cb The method to be called upon the event.
 */
  int osip_set_transport_error_callback (osip_t *osip, int type,
					 osip_transport_error_cb_t cb);

/**
 * Structure for sipevent handling.
 * A osip_event_t element will have a type and will be related
 * to a transaction. In the general case, it is used by the
 * application layer to give SIP messages to the oSIP finite
 * state machine.
 * @defvar osip_event_t
 */
  typedef struct osip_event osip_event_t;

  struct osip_event
  {
    type_t type;
    int transactionid;
    osip_message_t *sip;
  };



/**
 * Allocate an osip_transaction_t element.
 * @param transaction The element to allocate.
 * @param ctx_type The type of transaction. (ICT, IST, NICT, NIST)
 * @param osip The global instance of oSIP.
 * @param request The SIP request that initiate the transaction.
 */
  int osip_transaction_init (osip_transaction_t ** transaction,
			     osip_fsm_type_t ctx_type, osip_t * osip,
			     osip_message_t * request);
/**
 * Free all resource in a osip_transaction_t element.
 * @param transaction The element to free.
 */
  int osip_transaction_free (osip_transaction_t * transaction);
/**
 * Free all resource in a osip_transaction_t element.
 * This method does the same than osip_transaction_free() but it assumes
 * that the transaction is already removed from the list of transaction
 * in the osip stack. (to remove it use osip_xixt_remove(osip, transaction);
 * @param transaction The element to free.
 */
  int osip_transaction_free2 (osip_transaction_t * transaction);

/**
 * Set the host and port destination used for sending the SIP message.
 * This can be useful for an application with 'DIRECT ROOTING MODE'
 * NOTE: Instead, you should use the 'Route' header facility which
 * leads to the same behaviour.
 * @param ict The element to work on.
 * @param destination The destination host.
 * @param port The destination port.
 */
  int osip_ict_set_destination (osip_ict_t * ict, char *destination,
				int port);

/**
 * Set the host and port destination used for sending the SIP message.
 * This can be useful for an application with 'DIRECT ROOTING MODE'
 * NOTE: Instead, you should use the 'Route' header facility which
 * leads to the same behaviour.
 * @param nict The element to work on.
 * @param destination The destination host.
 * @param port The destination port.
 */
  int osip_nict_set_destination (osip_nict_t * nict, char *destination,
				 int port);

/**
 * Add a SIP event in the fifo of a osip_transaction_t element.
 * @param transaction The element to work on.
 * @param evt The event to add.
 */
  int osip_transaction_add_event (osip_transaction_t * transaction,
				  osip_event_t * evt);
/**
 * Consume one osip_event_t element previously added in the fifo.
 * NOTE: This method MUST NEVER be called within another call
 * of this method. (For example, you can't call osip_transaction_execute()
 * in a callback registered in the osip_t element.)
 * @param transaction The element to free.
 * @param evt The element to consume.
 */
  int osip_transaction_execute (osip_transaction_t * transaction,
				osip_event_t * evt);
/**
 * Set a pointer to your personal context associated with this transaction.
 * NOTE: this is a very useful method that allow you to avoid searching
 * for your personal context inside the registered callbacks.
 * You can initialise this pointer to your context right after
 * the creation of the osip_transaction_t element. Then, you'll be
 * able to get the address of your context by calling
 * osip_transaction_get_your_instance().
 * @param transaction The element to work on.
 * @param instance The address of your context.
 */
  int osip_transaction_set_your_instance (osip_transaction_t * transaction,
					  void *instance);
/**
 * Get a pointer to your personal context associated with this transaction.
 * @param transaction The element to work on.
 */
  void *osip_transaction_get_your_instance (osip_transaction_t * transaction);

/**
 * Get target ip and port for this request.
 * (automaticly set by osip_transaction_init() for ict and nict)
 * @param transaction The element to work on.
 * @param ip The ip of host where to send initial request.
 * @param port The port where to send initial request.
 */
  int osip_transaction_get_destination (osip_transaction_t * transaction,
					char **ip, int *port);

#ifndef DOXYGEN

/**
 * Set the socket for incoming message.
 * NOTE: THIS HAS NEVER TESTED! Please send feedback.
 * @param transaction The element to work on.
 * @param sock The socket for incoming message.
 */
  int osip_transaction_set_in_socket (osip_transaction_t * transaction,
				      int sock);
/**
 * Set the socket for outgoing message.
 * NOTE: THIS HAS NEVER TESTED! Please send feedback.
 * @param transaction The element to work on.
 * @param sock The socket for outgoing message.
 */
  int osip_transaction_set_out_socket (osip_transaction_t * transaction,
				       int sock);


#if 0


/**
 * Check if the first 2 parameters match the other ones.
 * NOTE: THIS IS AN INTERNAL METHOD ONLY
 * @param to1 The initial to header.
 * @param from1 The initial from header.
 * @param to2 The new to header.
 * @param from2 The new from header.
 */
  int callleg_match (osip_to_t * to1, osip_from_t * from1, osip_to_t * to2,
		     osip_from_t * from2);

#endif

#endif				/* endif DOXYGEN */


/** 
 * Allocate an osip_t element.
 * @param osip the element to allocate.
 */
  int osip_init (osip_t ** osip);
/**
 * Free all resource in a osip_t element.
 * @param osip The element to release.
 */
  void osip_release (osip_t * osip);

/**
 * Set a pointer in a osip_t element.
 * This help to find your application layer in callbacks.
 * @param osip The element to work on.
 * @param pointer The element to set.
 */
  void osip_set_application_context (osip_t * osip, void *pointer);

/**
 * Get a pointer in a osip_t element.
 * This help to find your application layer in callbacks.
 * @param osip The element to work on.
 */
  void *osip_get_application_context (osip_t * osip);


/**
 * Remove a transaction from the osip stack.
 * @param osip The element to work on.
 * @param ict The transaction to add.
 */
  int osip_remove_transaction (osip_t * osip, osip_transaction_t * ict);


/**
 * Consume ALL pending osip_event_t previously added in the fifos of ict transactions.
 * @param osip The element to work on.
 */
  int osip_ict_execute (osip_t * osip);
/**
 * Consume ALL pending osip_event_t previously added in the fifos of ist transactions.
 * @param osip The element to work on.
 */
  int osip_ist_execute (osip_t * osip);
/**
 * Consume ALL pending osip_event_t previously added in the fifos of nict transactions.
 * @param osip The element to work on.
 */
  int osip_nict_execute (osip_t * osip);
/**
 * Consume ALL pending osip_event_t previously added in the fifos of nist transactions.
 * @param osip The element to work on.
 */
  int osip_nist_execute (osip_t * osip);

/**
 * Retreive the minimum timer value to be used by an application
 * so that the osip_timer_*_execute method don't have to be called
 * often.
 * 
 * @param osip The element to work on.
 * @param lower_tv The minimum timer when the application should wake up.
 */
  void osip_timers_gettimeout (osip_t * osip, struct timeval *lower_tv);

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

/* Take care of mutlithreading issuewhile using this method */
/**
 * Search for a transaction that match this event (MUST be a MESSAGE event).
 * @param transactions The list of transactions to work on.
 * @param evt The element representing the SIP MESSAGE.
 */
  osip_transaction_t *osip_transaction_find (osip_list_t * transactions,
					     osip_event_t * evt);


#ifndef DOXYGEN
/**
 * Some race conditions can happen in multi threaded applications.
 * Use this method carefully.
 * <BR>Search for a transaction that match this event (MUST be a MESSAGE event).
 * @param osip The element to work on.
 * @param evt The element representing the SIP MESSAGE.
 */
#ifndef OSIP_MT
  osip_transaction_t *osip_find_transaction (osip_t * osip,
					     osip_event_t * evt);
#endif

  osip_transaction_t *__osip_find_transaction (osip_t * osip,
					       osip_event_t * evt,
					       int consume);
#endif

/**
 * Search for a transaction that match this event (MUST be a MESSAGE event)
 * and add this event if a transaction is found..
 * @param osip The element to work on.
 * @param evt The element representing the SIP MESSAGE.
 */
  int osip_find_transaction_and_add_event (osip_t * osip, osip_event_t * evt);

/**
 * Create a transaction for this event (MUST be a SIP REQUEST event).
 * @param osip The element to work on.
 * @param evt The element representing the new SIP REQUEST.
 */
  osip_transaction_t *osip_create_transaction (osip_t * osip,
					       osip_event_t * evt);

/**
 * Create a sipevent from a SIP message string.
 * @param buf The SIP message as a string.
 */
  osip_event_t *osip_parse (char *buf);


#ifdef OSIP_RETRANSMIT_2XX

  void osip_retransmissions_execute (osip_t * osip);

/**
 * Start out of fsm 200 Ok retransmissions. This is usefull for user-agents.
 * @param osip The osip_t structure.
 * @param dialog The dialog the 200 Ok is part of.
 * @param msg200ok The 200 ok response.
 * @param sock The socket to be used to send the message. (optional).
 */
  void osip_start_200ok_retransmissions (osip_t * osip,
					 struct osip_dialog *dialog,
					 osip_message_t * msg200ok, int sock);

/**
 * Start out of fsm ACK retransmissions. This is usefull for user-agents.
 * @param osip The osip_t structure.
 * @param dialog The dialog the ACK is part of.
 * @param ack The ACK that has just been sent in response to a 200 Ok.
 * @param dest The destination host.
 * @param sock The destination port.
 * @param sock The socket to be used to send the message. (optional).
 */
  void osip_start_ack_retransmissions (osip_t * osip,
				       struct osip_dialog *dialog,
				       osip_message_t * ack, char *dest,
				       int port, int sock);

/**
 * Stop the out of fsm 200 Ok retransmissions matching an incoming ACK.
 * @param osip The osip_t structure.
 * @param ack  The ack that has just been received.
 */
  void osip_stop_200ok_retransmissions (osip_t * osip, osip_message_t * ack);

/**
 * Stop out of fsm retransmissions (ACK or 200 Ok) associated to a given dialog.
 * This function must be called before freeing a dialog if out of fsm retransmissions
 * have been scheduled.
 * @param osip The osip_t structure
 * @param dialog The dialog.
 */
  void osip_stop_retransmissions_from_dialog (osip_t * osip,
					      struct osip_dialog *dialog);

#endif

/**
 * Allocate a sipevent (we know this message is an OUTGOING SIP message).
 * @param sip The SIP message we want to send.
 */
  osip_event_t *osip_new_outgoing_sipmessage (osip_message_t * sip);

/**
 * Free all ressource in a sipevent.
 * @param event The event to free.
 */
  void osip_event_free (osip_event_t * event);

/**
 * Register the callback used to send SIP message.
 * @param cf The osip element attached to the transaction.
 * @param cb The method we want to register.
 */
  void osip_set_cb_send_message (osip_t * cf,
				 int (*cb) (osip_transaction_t *,
					    osip_message_t *, char *,
					    int, int));

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

#ifdef __cplusplus
}
#endif


/** @} */

#endif
