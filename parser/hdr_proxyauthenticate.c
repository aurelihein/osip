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

/* fills the proxy-authenticate header of message.               */
/* INPUT :  char *hvalue | value of header.   */
/* OUTPUT: sip_t *sip | structure to save results. */
/* returns -1 on error. */
int
msg_setproxy_authenticate (sip_t * sip, char *hvalue)
{
  proxy_authenticate_t *proxy_authenticate;
  int i;

  i = proxy_authenticate_init (&(proxy_authenticate));
  if (i != 0)
    return -1;
  i = proxy_authenticate_parse (proxy_authenticate, hvalue);
  if (i != 0)
    {
      proxy_authenticate_free (proxy_authenticate);
      sfree (proxy_authenticate);
      return -1;
    }
#ifdef USE_TMP_BUFFER
  sip->message_property = 2;
#endif
  list_add (sip->proxy_authenticates, proxy_authenticate, -1);
  return 0;
}



/* returns the proxy_authenticate header.            */
/* INPUT : sip_t *sip | sip message.   */
/* returns null on error. */
int
msg_getproxy_authenticate (sip_t * sip, int pos, proxy_authenticate_t ** dest)
{
  proxy_authenticate_t *proxy_authenticate;

  *dest = NULL;
  if (list_size (sip->proxy_authenticates) <= pos)
    return -1;			/* does not exist */

  proxy_authenticate = (proxy_authenticate_t *)
    list_get (sip->proxy_authenticates, pos);

  *dest = proxy_authenticate;
  return pos;
}
