

#include "app.h"
#include <osip/osip.h>

void cb_endoftransaction(transaction_t *tr)
{
#ifdef OSIP_MT
  transaction_mt_stop_transaction((transaction_mt_t *)
				  tr->your_instance);
#endif
}

void cb_killtransaction(transaction_t *tr)
{
#ifdef OSIP_MT
  transaction_mt_stop_transaction((transaction_mt_t *)
				  tr->your_instance);
#endif
}

void cb_sndresp_retransmission(transaction_t *tr)
{
  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,stdout,"cb_sndresp_retransmission!\n"));
}

void cb_rcvreq_retransmission(transaction_t *tr)
{
  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,stdout,"cb_rcvreq_retransmission!\n"));
}

void cb_rcvinvite(sipevent_t *se,transaction_t *tr)
{
  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,stdout,"cb_rcvinvite!\n"));
  uaapp_annouceincinvite(se->sip, tr);
}

void cb_rcvack(sipevent_t *se,transaction_t *tr)
{
  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,stdout,"cb_rcvack!\n"));
  uaapp_annouceincrequest(se->sip, tr);
}

void cb_rcvbye(sipevent_t *se,transaction_t *tr)
{
  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,stdout,"cb_rcvbye!\n"));
  uaapp_annouceincrequest(se->sip, tr);
}

void cb_rcvcancel(sipevent_t *se,transaction_t *tr)
{
  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,stdout,"cb_rcvcancel!\n"));
  uaapp_annouceincrequest(se->sip, tr);
}

void cb_rcvinfo(sipevent_t *se,transaction_t *tr)
{
  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,stdout,"cb_rcvinfo!\n"));
  uaapp_annouceincrequest(se->sip, tr);
}

void cb_rcvoptions(sipevent_t *se,transaction_t *tr)
{
  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,stdout,"cb_rcvoptions!\n"));
  uaapp_annouceincrequest(se->sip, tr);
}

void cb_rcvregister(sipevent_t *se,transaction_t *tr)
{
  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,stdout,"cb_rcvregister!\n"));
  uaapp_annouceincrequest(se->sip, tr);
}

void cb_rcvprack(sipevent_t *se,transaction_t *tr)
{
  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,stdout,"cb_rcvprack!\n"));
  uaapp_annouceincrequest(se->sip, tr);
}

void cb_rcvunkrequest(sipevent_t *se,transaction_t *tr)
{
  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,stdout,"cb_rcvunkrequest!\n"));
  uaapp_annouceincrequest(se->sip, tr);
}

void cb_snd1xx(sipevent_t *se,transaction_t *tr)
{
  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,stdout,"cb_snd1xx!\n"));
  uaapp_annouceoutresponse(se->sip, tr);
}

void cb_snd2xx(sipevent_t *se,transaction_t *tr)
{
  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,stdout,"cb_snd2xx!\n"));
  uaapp_annouceoutresponse(se->sip, tr);
}

void cb_snd3xx(sipevent_t *se,transaction_t *tr)
{
  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,stdout,"cb_snd3xx!\n"));
  uaapp_annouceoutresponse(se->sip, tr);
}

void cb_snd4xx(sipevent_t *se,transaction_t *tr)
{
  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,stdout,"cb_snd4xx!\n"));
  uaapp_annouceoutresponse(se->sip, tr);
}

void cb_snd5xx(sipevent_t *se,transaction_t *tr)
{
  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,stdout,"cb_snd5xx!\n"));
  uaapp_annouceoutresponse(se->sip, tr);
}

void cb_snd6xx(sipevent_t *se,transaction_t *tr)
{
  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,stdout,"cb_snd6xx!\n"));
  uaapp_annouceoutresponse(se->sip, tr);
}
