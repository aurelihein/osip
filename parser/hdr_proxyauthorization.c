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


/* fills the proxy_authorization header of message.               */
/* INPUT :  char *hvalue | value of header.   */
/* OUTPUT: sip_t *sip | structure to save results. */
/* returns -1 on error. */
int
msg_setproxy_authorization (sip_t * sip, char *hvalue)
{
  proxy_authorization_t *proxy_authorization;
  int i;

  i = proxy_authorization_init (&proxy_authorization);
  if (i != 0)
    return -1;
  i = proxy_authorization_parse (proxy_authorization, hvalue);
  if (i != 0)
    {
      proxy_authorization_free (proxy_authorization);
      sfree (proxy_authorization);
      return -1;
    }
#ifdef USE_TMP_BUFFER
  sip->message_property = 2;
#endif
  list_add (sip->proxy_authorizations, proxy_authorization, -1);
  return 0;
}

int
msg_getproxy_authorization (sip_t * sip, int pos,
			    proxy_authorization_t ** dest)
{
  proxy_authorization_t *proxy_authorization;

  *dest = NULL;
  if (list_size (sip->proxy_authorizations) <= pos)
    return -1;			/* does not exist */
  proxy_authorization =
    (proxy_authorization_t *) list_get (sip->proxy_authorizations, pos);
  *dest = proxy_authorization;
  return pos;
}
