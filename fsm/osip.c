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

#include <osip/osip.h>
#include "fsm.h"
#include <osip/port.h>


int
osip_init(osip_t **osip)
{
  *osip = (osip_t *)smalloc(sizeof(osip_t));
  if (*osip==NULL)
    return -1; /* allocation failed */

  osip_init_timers(*osip);

  /* list of current transactions */
  (*osip)->transactions    = (list_t *) smalloc(sizeof(list_t));
  list_init((*osip)->transactions);

  /* default behavior: do not use a proxy */
  (*osip)->proxy = NULL;

  (*osip)->cb_rcvinvite = NULL;
  (*osip)->cb_rcvack = NULL;
  (*osip)->cb_rcvbye = NULL;
  (*osip)->cb_rcvcancel = NULL;
  (*osip)->cb_rcvinfo = NULL;
  (*osip)->cb_rcvoptions = NULL;
  (*osip)->cb_rcvregister = NULL;
  (*osip)->cb_rcvprack = NULL;
  (*osip)->cb_rcvunkrequest = NULL;

  (*osip)->cb_sndinvite = NULL;
  (*osip)->cb_sndack = NULL;
  (*osip)->cb_sndbye = NULL;
  (*osip)->cb_sndcancel = NULL;
  (*osip)->cb_sndinfo = NULL;
  (*osip)->cb_sndoptions = NULL;
  (*osip)->cb_sndregister = NULL;
  (*osip)->cb_sndprack = NULL;
  (*osip)->cb_sndunkrequest = NULL;

#ifndef EXTENDED_CALLBACKS
  (*osip)->cb_snd1xx = NULL;
  (*osip)->cb_snd2xx = NULL;
  (*osip)->cb_snd3xx = NULL;
  (*osip)->cb_snd4xx = NULL;
  (*osip)->cb_snd5xx = NULL;
  (*osip)->cb_snd6xx = NULL;

  (*osip)->cb_rcv1xx = NULL;
  (*osip)->cb_rcv2xx = NULL;
  (*osip)->cb_rcv3xx = NULL;
  (*osip)->cb_rcv4xx = NULL;
  (*osip)->cb_rcv5xx = NULL;
  (*osip)->cb_rcv6xx = NULL;
#else
  (*osip)->cb_sndinvite1xx = NULL;
  (*osip)->cb_sndinvite2xx = NULL;
  (*osip)->cb_sndinvite3xx = NULL;
  (*osip)->cb_sndinvite4xx = NULL;
  (*osip)->cb_sndinvite5xx = NULL;
  (*osip)->cb_sndinvite6xx = NULL;

  (*osip)->cb_rcvinvite1xx = NULL;
  (*osip)->cb_rcvinvite2xx = NULL;
  (*osip)->cb_rcvinvite3xx = NULL;
  (*osip)->cb_rcvinvite4xx = NULL;
  (*osip)->cb_rcvinvite5xx = NULL;
  (*osip)->cb_rcvinvite6xx = NULL;

  (*osip)->cb_sndack1xx = NULL;
  (*osip)->cb_sndack2xx = NULL;
  (*osip)->cb_sndack3xx = NULL;
  (*osip)->cb_sndack4xx = NULL;
  (*osip)->cb_sndack5xx = NULL;
  (*osip)->cb_sndack6xx = NULL;

  (*osip)->cb_rcvack1xx = NULL;
  (*osip)->cb_rcvack2xx = NULL;
  (*osip)->cb_rcvack3xx = NULL;
  (*osip)->cb_rcvack4xx = NULL;
  (*osip)->cb_rcvack5xx = NULL;
  (*osip)->cb_rcvack6xx = NULL;

  (*osip)->cb_sndbye1xx = NULL;
  (*osip)->cb_sndbye2xx = NULL;
  (*osip)->cb_sndbye3xx = NULL;
  (*osip)->cb_sndbye4xx = NULL;
  (*osip)->cb_sndbye5xx = NULL;
  (*osip)->cb_sndbye6xx = NULL;

  (*osip)->cb_rcvbye1xx = NULL;
  (*osip)->cb_rcvbye2xx = NULL;
  (*osip)->cb_rcvbye3xx = NULL;
  (*osip)->cb_rcvbye4xx = NULL;
  (*osip)->cb_rcvbye5xx = NULL;
  (*osip)->cb_rcvbye6xx = NULL;

  (*osip)->cb_sndcancel1xx = NULL;
  (*osip)->cb_sndcancel2xx = NULL;
  (*osip)->cb_sndcancel3xx = NULL;
  (*osip)->cb_sndcancel4xx = NULL;
  (*osip)->cb_sndcancel5xx = NULL;
  (*osip)->cb_sndcancel6xx = NULL;

  (*osip)->cb_rcvcancel1xx = NULL;
  (*osip)->cb_rcvcancel2xx = NULL;
  (*osip)->cb_rcvcancel3xx = NULL;
  (*osip)->cb_rcvcancel4xx = NULL;
  (*osip)->cb_rcvcancel5xx = NULL;
  (*osip)->cb_rcvcancel6xx = NULL;

  (*osip)->cb_sndinfo1xx = NULL;
  (*osip)->cb_sndinfo2xx = NULL;
  (*osip)->cb_sndinfo3xx = NULL;
  (*osip)->cb_sndinfo4xx = NULL;
  (*osip)->cb_sndinfo5xx = NULL;
  (*osip)->cb_sndinfo6xx = NULL;

  (*osip)->cb_rcvinfo1xx = NULL;
  (*osip)->cb_rcvinfo2xx = NULL;
  (*osip)->cb_rcvinfo3xx = NULL;
  (*osip)->cb_rcvinfo4xx = NULL;
  (*osip)->cb_rcvinfo5xx = NULL;
  (*osip)->cb_rcvinfo6xx = NULL;

  (*osip)->cb_sndoptions1xx = NULL;
  (*osip)->cb_sndoptions2xx = NULL;
  (*osip)->cb_sndoptions3xx = NULL;
  (*osip)->cb_sndoptions4xx = NULL;
  (*osip)->cb_sndoptions5xx = NULL;
  (*osip)->cb_sndoptions6xx = NULL;

  (*osip)->cb_rcvoptions1xx = NULL;
  (*osip)->cb_rcvoptions2xx = NULL;
  (*osip)->cb_rcvoptions3xx = NULL;
  (*osip)->cb_rcvoptions4xx = NULL;
  (*osip)->cb_rcvoptions5xx = NULL;
  (*osip)->cb_rcvoptions6xx = NULL;

  (*osip)->cb_sndregister1xx = NULL;
  (*osip)->cb_sndregister2xx = NULL;
  (*osip)->cb_sndregister3xx = NULL;
  (*osip)->cb_sndregister4xx = NULL;
  (*osip)->cb_sndregister5xx = NULL;
  (*osip)->cb_sndregister6xx = NULL;

  (*osip)->cb_rcvregister1xx = NULL;
  (*osip)->cb_rcvregister2xx = NULL;
  (*osip)->cb_rcvregister3xx = NULL;
  (*osip)->cb_rcvregister4xx = NULL;
  (*osip)->cb_rcvregister5xx = NULL;
  (*osip)->cb_rcvregister6xx = NULL;

  (*osip)->cb_sndprack1xx = NULL;
  (*osip)->cb_sndprack2xx = NULL;
  (*osip)->cb_sndprack3xx = NULL;
  (*osip)->cb_sndprack4xx = NULL;
  (*osip)->cb_sndprack5xx = NULL;
  (*osip)->cb_sndprack6xx = NULL;

  (*osip)->cb_rcvprack1xx = NULL;
  (*osip)->cb_rcvprack2xx = NULL;
  (*osip)->cb_rcvprack3xx = NULL;
  (*osip)->cb_rcvprack4xx = NULL;
  (*osip)->cb_rcvprack5xx = NULL;
  (*osip)->cb_rcvprack6xx = NULL;

  (*osip)->cb_sndunkrequest1xx = NULL;
  (*osip)->cb_sndunkrequest2xx = NULL;
  (*osip)->cb_sndunkrequest3xx = NULL;
  (*osip)->cb_sndunkrequest4xx = NULL;
  (*osip)->cb_sndunkrequest5xx = NULL;
  (*osip)->cb_sndunkrequest6xx = NULL;

  (*osip)->cb_rcvunkrequest1xx = NULL;
  (*osip)->cb_rcvunkrequest2xx = NULL;
  (*osip)->cb_rcvunkrequest3xx = NULL;
  (*osip)->cb_rcvunkrequest4xx = NULL;
  (*osip)->cb_rcvunkrequest5xx = NULL;
  (*osip)->cb_rcvunkrequest6xx = NULL;

#endif

  (*osip)->cb_rcvresp_retransmission = NULL;
  (*osip)->cb_sndreq_retransmission = NULL;
  (*osip)->cb_sndresp_retransmission = NULL;
  (*osip)->cb_rcvreq_retransmission = NULL;

  (*osip)->cb_killtransaction = NULL;
  (*osip)->cb_endoftransaction = NULL;

  (*osip)->cb_connection_refused = NULL;
  (*osip)->cb_network_error = NULL;

  return 0;
}

int
osip_init_timers(osip_t *osip)
{
  /* timers for uac and uas transactions. */
  osip->uac_timerff   = (fifo_t *)smalloc(sizeof(fifo_t));
  osip->uas_timerff   = (fifo_t *)smalloc(sizeof(fifo_t));
  fifo_init(osip->uac_timerff);
  fifo_init(osip->uas_timerff);

  /* timers for uac and uas transactions. */
  osip->uas_transactions  = (list_t *)smalloc(sizeof(list_t));
  list_init(osip->uas_transactions);
  osip->uac_transactions  = (list_t *)smalloc(sizeof(list_t));
  list_init(osip->uac_transactions);
  return 0;
}

void
osip_init_proxy(osip_t *osip, url_t *prox)
{
  osip->proxy = prox;
}

int
osip_execute(osip_t *config)
{
  transaction_t *transaction;
  sipevent_t *se;
  int more_event;
  int tr;
  tr=0;
  while (!list_eol(config->transactions,tr))
    {
      transaction = list_get(config->transactions,tr);
      tr++;
      more_event = 1;
      do
	{
	  se = (sipevent_t *)fifo_tryget(transaction->transactionff);
	  
	  if (se==NULL) /* no more event for this transaction */
	    {
	      /* continue with next transaction, and its first event */
	      more_event=0;   
	    }
	  else
	    {
	      transaction_execute(transaction,se);
	    }
	} while (more_event==1);
    }
  return 0;
}


void osip_setcb_killtransaction  (osip_t *cf,void (*cb)(transaction_t*))
{
  cf->cb_killtransaction = cb;
}

void osip_setcb_endoftransaction  (osip_t *cf,void (*cb)(transaction_t*))
{
  cf->cb_endoftransaction = cb;
}

void osip_setcb_connection_refused  (osip_t *cf,void (*cb)(transaction_t*))
{
  cf->cb_connection_refused = cb;
}

void osip_setcb_network_error  (osip_t *cf,void (*cb)(transaction_t*))
{
  cf->cb_network_error = cb;
}

void osip_setcb_rcvreq_retransmission  (osip_t *cf,void (*cb)(transaction_t*))
{
  cf->cb_rcvreq_retransmission = cb;
}

void osip_setcb_sndreq_retransmission (osip_t *cf,void (*cb)(transaction_t*))
{
  cf->cb_sndreq_retransmission = cb;
}

void osip_setcb_sndresp_retransmission     (osip_t *cf,void (*cb)(transaction_t*))
{
  cf->cb_sndresp_retransmission = cb;
}

void osip_setcb_rcvresp_retransmission(osip_t *cf,void (*cb)(transaction_t*))
{
  cf->cb_rcvresp_retransmission = cb;
}

void osip_setcb_rcvinvite  (osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvinvite = cb;
}

void osip_setcb_rcvack     (osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvack = cb;
}

void osip_setcb_rcvbye     (osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvbye = cb;
}

void osip_setcb_rcvcancel  (osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvcancel = cb;
}

void osip_setcb_rcvinfo    (osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvinfo = cb;
}
void osip_setcb_rcvoptions (osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvoptions = cb;
}
void osip_setcb_rcvregister(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvregister = cb;
}
void osip_setcb_rcvprack   (osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvprack = cb;
}
void osip_setcb_rcvunkrequest(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvunkrequest = cb;
}

void osip_setcb_sndinvite  (osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndinvite = cb;
}
void osip_setcb_sndack     (osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndack = cb;
}
void osip_setcb_sndbye     (osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndbye = cb;
}
void osip_setcb_sndcancel  (osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndcancel = cb;
}
void osip_setcb_sndinfo    (osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndinfo = cb;
}
void osip_setcb_sndoptions (osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndoptions = cb;
}
void osip_setcb_sndregister(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndregister = cb;
}
void osip_setcb_sndprack   (osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndprack = cb;
}
void osip_setcb_sndunkrequest(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndunkrequest = cb;
}

#ifndef EXTENDED_CALLBACKS

void osip_setcb_rcv1xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcv1xx = cb;
}
void osip_setcb_rcv2xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcv2xx = cb;
}
void osip_setcb_rcv3xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcv3xx = cb;
}
void osip_setcb_rcv4xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcv4xx = cb;
}
void osip_setcb_rcv5xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcv5xx = cb;
}

void osip_setcb_rcv6xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcv6xx = cb;
}

void osip_setcb_snd1xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_snd1xx = cb;
}
void osip_setcb_snd2xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_snd2xx = cb;
}
void osip_setcb_snd3xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_snd3xx = cb;
}
void osip_setcb_snd4xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_snd4xx = cb;
}
void osip_setcb_snd5xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_snd5xx = cb;
}
void osip_setcb_snd6xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_snd6xx = cb;
}

#else

void osip_setcb_rcvinvite1xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvinvite1xx = cb;
}
void osip_setcb_rcvinvite2xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvinvite2xx = cb;
}
void osip_setcb_rcvinvite3xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvinvite3xx = cb;
}
void osip_setcb_rcvinvite4xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvinvite4xx = cb;
}
void osip_setcb_rcvinvite5xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvinvite5xx = cb;
}

void osip_setcb_rcvinvite6xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvinvite6xx = cb;
}

void osip_setcb_sndinvite1xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndinvite1xx = cb;
}
void osip_setcb_sndinvite2xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndinvite2xx = cb;
}
void osip_setcb_sndinvite3xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndinvite3xx = cb;
}
void osip_setcb_sndinvite4xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndinvite4xx = cb;
}
void osip_setcb_sndinvite5xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndinvite5xx = cb;
}
void osip_setcb_sndinvite6xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndinvite6xx = cb;
}

void osip_setcb_rcvack1xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvack1xx = cb;
}
void osip_setcb_rcvack2xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvack2xx = cb;
}
void osip_setcb_rcvack3xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvack3xx = cb;
}
void osip_setcb_rcvack4xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvack4xx = cb;
}
void osip_setcb_rcvack5xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvack5xx = cb;
}

void osip_setcb_rcvack6xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvack6xx = cb;
}

void osip_setcb_sndack1xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndack1xx = cb;
}
void osip_setcb_sndack2xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndack2xx = cb;
}
void osip_setcb_sndack3xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndack3xx = cb;
}
void osip_setcb_sndack4xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndack4xx = cb;
}
void osip_setcb_sndack5xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndack5xx = cb;
}
void osip_setcb_sndack6xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndack6xx = cb;
}

void osip_setcb_rcvbye1xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvbye1xx = cb;
}
void osip_setcb_rcvbye2xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvbye2xx = cb;
}
void osip_setcb_rcvbye3xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvbye3xx = cb;
}
void osip_setcb_rcvbye4xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvbye4xx = cb;
}
void osip_setcb_rcvbye5xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvbye5xx = cb;
}

void osip_setcb_rcvbye6xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvbye6xx = cb;
}

void osip_setcb_sndbye1xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndbye1xx = cb;
}
void osip_setcb_sndbye2xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndbye2xx = cb;
}
void osip_setcb_sndbye3xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndbye3xx = cb;
}
void osip_setcb_sndbye4xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndbye4xx = cb;
}
void osip_setcb_sndbye5xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndbye5xx = cb;
}
void osip_setcb_sndbye6xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndbye6xx = cb;
}

void osip_setcb_rcvcancel1xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvcancel1xx = cb;
}
void osip_setcb_rcvcancel2xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvcancel2xx = cb;
}
void osip_setcb_rcvcancel3xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvcancel3xx = cb;
}
void osip_setcb_rcvcancel4xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvcancel4xx = cb;
}
void osip_setcb_rcvcancel5xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvcancel5xx = cb;
}

void osip_setcb_rcvcancel6xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvcancel6xx = cb;
}

void osip_setcb_sndcancel1xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndcancel1xx = cb;
}
void osip_setcb_sndcancel2xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndcancel2xx = cb;
}
void osip_setcb_sndcancel3xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndcancel3xx = cb;
}
void osip_setcb_sndcancel4xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndcancel4xx = cb;
}
void osip_setcb_sndcancel5xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndcancel5xx = cb;
}
void osip_setcb_sndcancel6xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndcancel6xx = cb;
}

void osip_setcb_rcvinfo1xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvinfo1xx = cb;
}
void osip_setcb_rcvinfo2xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvinfo2xx = cb;
}
void osip_setcb_rcvinfo3xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvinfo3xx = cb;
}
void osip_setcb_rcvinfo4xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvinfo4xx = cb;
}
void osip_setcb_rcvinfo5xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvinfo5xx = cb;
}

void osip_setcb_rcvinfo6xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvinfo6xx = cb;
}

void osip_setcb_sndinfo1xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndinfo1xx = cb;
}
void osip_setcb_sndinfo2xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndinfo2xx = cb;
}
void osip_setcb_sndinfo3xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndinfo3xx = cb;
}
void osip_setcb_sndinfo4xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndinfo4xx = cb;
}
void osip_setcb_sndinfo5xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndinfo5xx = cb;
}
void osip_setcb_sndinfo6xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndinfo6xx = cb;
}

void osip_setcb_rcvoptions1xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvoptions1xx = cb;
}
void osip_setcb_rcvoptions2xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvoptions2xx = cb;
}
void osip_setcb_rcvoptions3xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvoptions3xx = cb;
}
void osip_setcb_rcvoptions4xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvoptions4xx = cb;
}
void osip_setcb_rcvoptions5xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvoptions5xx = cb;
}

void osip_setcb_rcvoptions6xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvoptions6xx = cb;
}

void osip_setcb_sndoptions1xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndoptions1xx = cb;
}
void osip_setcb_sndoptions2xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndoptions2xx = cb;
}
void osip_setcb_sndoptions3xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndoptions3xx = cb;
}
void osip_setcb_sndoptions4xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndoptions4xx = cb;
}
void osip_setcb_sndoptions5xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndoptions5xx = cb;
}
void osip_setcb_sndoptions6xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndoptions6xx = cb;
}

void osip_setcb_rcvregister1xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvregister1xx = cb;
}
void osip_setcb_rcvregister2xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvregister2xx = cb;
}
void osip_setcb_rcvregister3xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvregister3xx = cb;
}
void osip_setcb_rcvregister4xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvregister4xx = cb;
}
void osip_setcb_rcvregister5xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvregister5xx = cb;
}

void osip_setcb_rcvregister6xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvregister6xx = cb;
}

void osip_setcb_sndregister1xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndregister1xx = cb;
}
void osip_setcb_sndregister2xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndregister2xx = cb;
}
void osip_setcb_sndregister3xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndregister3xx = cb;
}
void osip_setcb_sndregister4xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndregister4xx = cb;
}
void osip_setcb_sndregister5xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndregister5xx = cb;
}
void osip_setcb_sndregister6xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndregister6xx = cb;
}

void osip_setcb_rcvprack1xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvprack1xx = cb;
}
void osip_setcb_rcvprack2xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvprack2xx = cb;
}
void osip_setcb_rcvprack3xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvprack3xx = cb;
}
void osip_setcb_rcvprack4xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvprack4xx = cb;
}
void osip_setcb_rcvprack5xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvprack5xx = cb;
}

void osip_setcb_rcvprack6xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvprack6xx = cb;
}

void osip_setcb_sndprack1xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndprack1xx = cb;
}
void osip_setcb_sndprack2xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndprack2xx = cb;
}
void osip_setcb_sndprack3xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndprack3xx = cb;
}
void osip_setcb_sndprack4xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndprack4xx = cb;
}
void osip_setcb_sndprack5xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndprack5xx = cb;
}
void osip_setcb_sndprack6xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndprack6xx = cb;
}

void osip_setcb_rcvunkrequest1xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvunkrequest1xx = cb;
}
void osip_setcb_rcvunkrequest2xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvunkrequest2xx = cb;
}
void osip_setcb_rcvunkrequest3xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvunkrequest3xx = cb;
}
void osip_setcb_rcvunkrequest4xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvunkrequest4xx = cb;
}
void osip_setcb_rcvunkrequest5xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvunkrequest5xx = cb;
}
void osip_setcb_rcvunkrequest6xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_rcvunkrequest6xx = cb;
}

void osip_setcb_sndunkrequest1xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndunkrequest1xx = cb;
}
void osip_setcb_sndunkrequest2xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndunkrequest2xx = cb;
}
void osip_setcb_sndunkrequest3xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndunkrequest3xx = cb;
}
void osip_setcb_sndunkrequest4xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndunkrequest4xx = cb;
}
void osip_setcb_sndunkrequest5xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndunkrequest5xx = cb;
}
void osip_setcb_sndunkrequest6xx(osip_t *cf,void (*cb)(sipevent_t*,transaction_t*))
{
  cf->cb_sndunkrequest6xx = cb;
}


#endif
