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

#ifndef _UA_H_
#define _UA_H_

#include <osip/osip.h>

extern int OnEvent_EndOfTransaction(transaction_t *transaction);
extern int OnEvent_TransactionKilled(transaction_t *transaction);

extern int OnEvent_connection_refused(transaction_t *tr);
extern int OnEvent_network_error(transaction_t *tr);

/* this is how YOUR mid-level application layer is aware of session   */

extern int OnEvent_sndresp_retransmission(transaction_t *tr);
extern int OnEvent_rcvreq_retransmission(transaction_t *tr);
extern int OnEvent_New_IncomingInvite(sipevent_t *se,transaction_t *tr);
extern int OnEvent_New_IncomingAck(sipevent_t *se,transaction_t *tr);
extern int OnEvent_New_IncomingBye(sipevent_t *se,transaction_t *tr);
extern int OnEvent_New_IncomingCancel(sipevent_t *se,transaction_t *tr);
extern int OnEvent_New_IncomingInfo(sipevent_t *se,transaction_t *tr);
extern int OnEvent_New_IncomingOptions(sipevent_t *se,transaction_t *tr);
extern int OnEvent_New_IncomingRegister(sipevent_t *se,transaction_t *tr);
extern int OnEvent_New_IncomingPrack(sipevent_t *se,transaction_t *tr);
extern int OnEvent_New_IncomingUnknownRequest(sipevent_t *se,transaction_t *tr);
extern int OnEvent_New_Outgoing1xxResponse(sipevent_t *se,transaction_t *tr);
extern int OnEvent_New_Outgoing2xxResponse(sipevent_t *se,transaction_t *tr);
extern int OnEvent_New_Outgoing3xxResponse(sipevent_t *se,transaction_t *tr);
extern int OnEvent_New_Outgoing4xxResponse(sipevent_t *se,transaction_t *tr);
extern int OnEvent_New_Outgoing5xxResponse(sipevent_t *se,transaction_t *tr);
extern int OnEvent_New_Outgoing6xxResponse(sipevent_t *se,transaction_t *tr);

extern int OnEvent_sndreq_retransmission(transaction_t *tr);
extern int OnEvent_rcvresp_retransmission(transaction_t *tr);
extern int OnEvent_New_OutgoingInvite(sipevent_t *se,transaction_t *tr);
extern int OnEvent_New_OutgoingAck(sipevent_t *se,transaction_t *tr);
extern int OnEvent_New_OutgoingBye(sipevent_t *se,transaction_t *tr);
extern int OnEvent_New_OutgoingCancel(sipevent_t *se,transaction_t *tr);
extern int OnEvent_New_OutgoingRegister(sipevent_t *se,transaction_t *tr);
extern int OnEvent_New_OutgoingInfo(sipevent_t *se,transaction_t *tr);
extern int OnEvent_New_OutgoingOptions(sipevent_t *se,transaction_t *tr);
extern int OnEvent_New_OutgoingPrack(sipevent_t *se,transaction_t *tr);
extern int OnEvent_New_OutgoingUnknownRequest(sipevent_t *se,transaction_t *tr);
extern int OnEvent_New_Incoming1xxResponse(sipevent_t *se,transaction_t *tr);
extern int OnEvent_New_Incoming2xxResponse(sipevent_t *se,transaction_t *tr);
extern int OnEvent_New_Incoming3xxResponse(sipevent_t *se,transaction_t *tr);
extern int OnEvent_New_Incoming4xxResponse(sipevent_t *se,transaction_t *tr);
extern int OnEvent_New_Incoming5xxResponse(sipevent_t *se,transaction_t *tr);
extern int OnEvent_New_Incoming6xxResponse(sipevent_t *se,transaction_t *tr);

#endif
