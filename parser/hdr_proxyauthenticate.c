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
  int i;

  if (sip->proxy_authenticate != NULL)
    return -1;
  i = proxy_authenticate_init (&(sip->proxy_authenticate));
  if (i == -1)
    return -1;
#ifdef USE_TMP_BUFFER
  sip->message_property = 2;
#endif
  return proxy_authenticate_parse (sip->proxy_authenticate, hvalue);
}



/* returns the proxy_authenticate header.            */
/* INPUT : sip_t *sip | sip message.   */
/* returns null on error. */
proxy_authenticate_t *
msg_getproxy_authenticate (sip_t * sip)
{
  return sip->proxy_authenticate;
}
