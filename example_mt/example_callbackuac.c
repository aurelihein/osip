

#include "app.h"
#include <osip/osip.h>

void cb_connection_refused(transaction_t *tr)
{
  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL0,stdout,"on_connection_refused! \n")); 
}

void cb_network_error(transaction_t *tr)
{
  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL0,stdout,"cb_network_error! \n"));
}

void cb_sndreq_retransmission(transaction_t *tr)
{
  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,stdout,"cb_sndreq_retransmission! \n"));
}

void cb_rcvresp_retransmission(transaction_t *tr)
{
  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,stdout,"cb_rcvresp_retransmission! \n"));
}

void cb_sndinvite(sipevent_t *se,transaction_t *tr)
{
  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,stdout,"cb_sndinvite!\n"));
  uaapp_annouceoutinvite(se->sip, tr);
}

void cb_sndack(sipevent_t *se,transaction_t *tr)
{
  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,stdout,"cb_sndack!\n"));
  uaapp_annouceoutrequest(se->sip, tr);
}

void cb_sndbye(sipevent_t *se,transaction_t *tr)
{
  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,stdout,"cb_sndbye!\n"));
  uaapp_annouceoutrequest(se->sip, tr);
}

void cb_sndcancel(sipevent_t *se,transaction_t *tr)
{
  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,stdout,"cb_sndcancel!\n"));
  uaapp_annouceoutrequest(se->sip, tr);
}

void cb_sndinfo(sipevent_t *se,transaction_t *tr)
{
  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,stdout,"cb_sndinfo!\n"));
  uaapp_annouceoutrequest(se->sip, tr);
}

void cb_sndoptions(sipevent_t *se,transaction_t *tr)
{
  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,stdout,"cb_sndoptions!\n"));
  uaapp_annouceoutrequest(se->sip, tr);
}

void cb_sndregister(sipevent_t *se,transaction_t *tr)
{
  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,stdout,"cb_sndregister!\n"));
  uaapp_annouceoutrequest(se->sip, tr);
}

void cb_sndprack(sipevent_t *se,transaction_t *tr)
{
  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,stdout,"cb_sndprack!\n"));
  uaapp_annouceoutrequest(se->sip, tr);
}

void cb_sndunkrequest(sipevent_t *se,transaction_t *tr)
{
  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,stdout,"cb_sndunkrequest!\n"));
  uaapp_annouceoutrequest(se->sip, tr);
}

void cb_rcv1xx(sipevent_t *se,transaction_t *tr)
{
  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,stdout,"cb_rcv1xx!\n"));
  uaapp_annouceincresponse(se->sip, tr);
}

void cb_rcv2xx(sipevent_t *se,transaction_t *tr)
{
  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,stdout,"cb_rcv2xx!\n"));
  uaapp_annouceincresponse(se->sip, tr);
}

void cb_rcv3xx(sipevent_t *se,transaction_t *tr)
{
  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,stdout,"cb_rcv3xx!\n"));
  uaapp_annouceincresponse(se->sip, tr);
}

void cb_rcv4xx(sipevent_t *se,transaction_t *tr)
{
  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,stdout,"cb_rcv4xx!\n"));
  uaapp_annouceincresponse(se->sip, tr);
}

void cb_rcv5xx(sipevent_t *se,transaction_t *tr)
{
  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,stdout,"cb_rcv5xx!\n"));
  uaapp_annouceincresponse(se->sip, tr);
}

void cb_rcv6xx(sipevent_t *se,transaction_t *tr)
{
  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,stdout,"cb_rcv6xx!\n"));
  uaapp_annouceincresponse(se->sip, tr);
}
