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

#include <stdlib.h>
#include <stdio.h>

#include <osip/port.h>
#include <osip/smsg.h>
#include "msg.h"



/* adds the accept header to message.              */
/* INPUT : char *hvalue | value of header.    */
/* OUTPUT: sip_t *sip | structure to save results.  */
/* returns -1 on error. */
int
msg_setaccept (sip_t * sip, char *hvalue)
{
  accept_t *accept;
  int i;

  i = accept_init (&accept);
  if (i != 0)
    return -1;
  i = accept_parse (accept, hvalue);
  if (i != 0)
    {
      accept_free (accept);
      sfree (accept);
      return -1;
    }
#ifdef USE_TMP_BUFFER
  sip->message_property = 2;
#endif

  list_add (sip->accepts, accept, -1);
  return 0;
}


int
msg_getaccept (sip_t * sip, int pos, accept_t ** dest)
{
  accept_t *accept;

  *dest = NULL;
  if (list_size (sip->accepts) <= pos)
    return -1;			/* does not exist */
  accept = (accept_t *) list_get (sip->accepts, pos);
  *dest = accept;
  return pos;
}
