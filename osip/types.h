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

#ifndef _SIP_H_
#define _SIP_H_

#include <stdio.h>

#include <osip/port.h>

#include <osip/const.h>
#include <osip/urls.h>
#include <osip/smsg.h>

/**********************************************/
/*     start OF  STRUCT AND API FOR           */
/*               SIP TRANSACTION-LEVEL API    */
/**********************************************/

/* A call leg is identified by the combination of the */
/* Call-ID header field and the addr-spec and tag of the To */
/* and From header fields */
typedef struct _callleg_t {

  /* this is a call leg */
  to_t *to;
  from_t *from;
  /* call_id_t *callid; */

  /* transactions associated with callleg */
  list_t *incomingtransactions;
  list_t *outgoingtransactions;

  /* all events for this transaction arrive here */
  fifo_t *calllegff;
} callleg_t;




/* A call consists of all participants in a conference */
/* invited by a common source. */
typedef struct _sipcall_t {

  call_id_t     *callid;
  fifo_t        *sipcallff;

  /* could have many call legs */
  list_t *calllegs;
} sipcall_t;



#endif




