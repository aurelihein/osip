
#ifndef _APPUA_H_
#define _APPUA_H_

#include <osip/port.h>
#include <osip/osip.h>

#ifdef OSIP_MT
#include <osip/thread.h>
#endif

void cb_rcvinvite  (sipevent_t *se,transaction_t *tr);
void cb_rcvack     (sipevent_t *se,transaction_t *tr);
void cb_rcvbye     (sipevent_t *se,transaction_t *tr);
void cb_rcvcancel  (sipevent_t *se,transaction_t *tr);
void cb_rcvinfo    (sipevent_t *se,transaction_t *tr);
void cb_rcvoptions (sipevent_t *se,transaction_t *tr);
void cb_rcvregister(sipevent_t *se,transaction_t *tr);
void cb_rcvprack   (sipevent_t *se,transaction_t *tr);
void cb_rcvunkrequest(sipevent_t *se,transaction_t *tr);

void cb_sndinvite  (sipevent_t *se,transaction_t *tr);
void cb_sndack     (sipevent_t *se,transaction_t *tr);
void cb_sndbye     (sipevent_t *se,transaction_t *tr);
void cb_sndcancel  (sipevent_t *se,transaction_t *tr);
void cb_sndinfo    (sipevent_t *se,transaction_t *tr);
void cb_sndoptions (sipevent_t *se,transaction_t *tr);
void cb_sndregister(sipevent_t *se,transaction_t *tr);
void cb_sndprack   (sipevent_t *se,transaction_t *tr);
void cb_sndunkrequest(sipevent_t *se,transaction_t *tr);

void cb_rcv1xx(sipevent_t *se,transaction_t *tr);
void cb_rcv2xx(sipevent_t *se,transaction_t *tr);
void cb_rcv3xx(sipevent_t *se,transaction_t *tr);
void cb_rcv4xx(sipevent_t *se,transaction_t *tr);
void cb_rcv5xx(sipevent_t *se,transaction_t *tr);
void cb_rcv6xx(sipevent_t *se,transaction_t *tr);

void cb_snd1xx(sipevent_t *se,transaction_t *tr);
void cb_snd2xx(sipevent_t *se,transaction_t *tr);
void cb_snd3xx(sipevent_t *se,transaction_t *tr);
void cb_snd4xx(sipevent_t *se,transaction_t *tr);
void cb_snd5xx(sipevent_t *se,transaction_t *tr);
void cb_snd6xx(sipevent_t *se,transaction_t *tr);

void cb_rcvresp_retransmission(transaction_t *tr);
void cb_sndreq_retransmission(transaction_t *tr);
void cb_sndresp_retransmission(transaction_t *tr);
void cb_rcvreq_retransmission(transaction_t *tr);

void cb_killtransaction(transaction_t *tr);
void cb_endoftransaction(transaction_t *tr);

void cb_connection_refused(transaction_t *tr);
void cb_network_error(transaction_t *tr);

#ifdef OSIP_MT
typedef struct _transaction_mt_t {

  transaction_t *transaction;
#ifdef __VXWORKS_OS__
  int thread;
#else
  sthread_t *thread;
#endif
} transaction_mt_t;

int  transaction_mt_init(transaction_mt_t **transaction_mt);
void transaction_mt_set_transaction(transaction_mt_t *transaction_mt,
				   transaction_t *transaction);
int transaction_mt_start_transaction(transaction_mt_t *transaction_mt);
void transaction_mt_stop_transaction(transaction_mt_t *transaction_mt);
#endif

typedef struct _mt_ua_t {

  osip_t *config;

  list_t *transactions_mt;

#ifdef OSIP_MT
#ifdef __VXWORKS_OS__
  int timerthread;
#else
  sthread_t *timerthread;
#endif
#endif

  int port;
} mt_ua_t;


int ua_init(mt_ua_t **mt_ua, int port, osip_t *config);

#ifdef OSIP_MT
int mt_ua_init(mt_ua_t **mt_ua, int port, osip_t *config);
int sipd_start(mt_ua_t *mt_ua);
#endif
int osipudpd_close(mt_ua_t *mt_ua);
int osipudpd_open(mt_ua_t *mt_ua);
int osipudpd_execute(mt_ua_t *mt_ua, int sec_max, int usec_max);

#ifdef OSIP_MT
/* thread managing events for one transaction */
void *transaction_thread(transaction_t *transaction);
/* start a thread which will handle timers */
int timersd_start(mt_ua_t *mt_ua);
#endif

typedef struct _session_t {

  call_id_t *callid;
  cseq_t *cseq;

  from_t *from;
  to_t *to;
  int   transactionid;
  int   use_proxy;
  url_t *proxy;
  list_t *headers; /* list of header_t */
  contact_t *contact;

} session_t;

/* start the stack with this design:                     */
int ua_start_design2(mt_ua_t *mt_ua);

#ifdef OSIP_MT
/* start the stack with this design:                     */
/*    two threads for timers to enable retransamissions. */
/*    one thread listening on a udp port.                */
/*    one thread per transaction.                        */
int mt_ua_start_design1(mt_ua_t *mt_ua);

/* temporary patch targeted to linphone before improvements */
#ifdef LINPHONE
int mt_ua_stop_design1(mt_ua_t *mt_ua);
#endif
#endif

/* Those method names SHOULD be used to initiate event requesting */
/* new transactions. This could be extended on the same model     */
/* shown in "ualogic" directory */

int    sip_invite  (  session_t **session);
int    sip_bye     (  session_t **session);
void   sip_register(  char      *expires);

/******************************************* */
/* callback procedures you MUST implement    */
/* one exemple is provided in "ua",          */
/* "ualogic" and "uamess" directories        */
/* Those methods could be used to implement  */
/* any transactions types.                   */
/******************************************* */


/* this is a facility to annouce YOUR high-level application layer */
/* so it is aware of sessions */
void   uaapp_annouceincinvite(sip_t *invite,transaction_t *tr);
void   uaapp_annouceincrequest(sip_t *request,transaction_t *tr);
void   uaapp_annouceincack(sip_t *request,transaction_t *tr);
void   uaapp_annouceincresponse(sip_t *response,transaction_t *tr);
void   uaapp_annouceoutinvite(sip_t *invite,transaction_t *tr);
void   uaapp_annouceoutrequest(sip_t *request,transaction_t *tr);
void   uaapp_annouceoutresponse(sip_t *response,transaction_t *tr);

/* these are the method's names you SHOULD use to create YOUR answers. */
/* You could extend it. */
void   respondtorequest(transaction_t *tr,int code);
void   respondtoinvite (transaction_t *tr,int code);
void   respondto200forinvite(transaction_t *tr, sip_t *response);

/* these are the method's names you SHOULD use to create YOUR requests */
/* An exemple is given in "uamess" */
/* But you SHOULD define your own way to create messages.*/

int    makeinvite  (sip_t *dest);
int    makeack     (sip_t *dest     ,sip_t    *response);
int    makeregister(sip_t *dest,call_id_t *rcallid,char *rnumber,char *expires);

/*int    makebye(sip_t     *dest   ,sip_t    *invite); */
int    makebye     (sip_t     *dest   ,from_t   *from    ,to_t *to
		    ,url_t    *rquri  ,call_id_t *callid  ,cseq_t *p_cseq );

#endif
