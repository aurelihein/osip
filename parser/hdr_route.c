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

#ifdef __VXWORKS_OS__
int
route_init2 (route_t ** route)
#else
int
route_init (route_t ** route)
#endif
{
  return from_init ((from_t **) route);
}

/* adds the route header to message.         */
/* INPUT : const char *hvalue | value of header.    */
/* OUTPUT: sip_t *sip | structure to save results.  */
/* returns -1 on error. */
int
msg_setroute (sip_t * sip, char *hvalue)
{
  route_t *route;
  int i;

#ifdef __VXWORKS_OS__
  i = route_init2 (&route);
#else
  i = route_init (&route);
#endif
  if (i != 0)
    return -1;
  i = route_parse (route, hvalue);
  if (i != 0)
    {
      route_free (route);
      sfree (route);
      return -1;
    }
#ifdef USE_TMP_BUFFER
  sip->message_property = 2;
#endif
  list_add (sip->routes, route, -1);
  return 0;
}

/* returns the route header.    */
/* INPUT : sip_t *sip | sip message.   */
/* returns null on error. */
int
msg_getroute (sip_t * sip, int pos, route_t ** dest)
{
  route_t *route;

  *dest = NULL;
  if (list_size (sip->routes) <= pos)
    return -1;			/* does not exist */
  route = (route_t *) list_get (sip->routes, pos);
  *dest = route;
  return pos;
}

int
route_parse (route_t * route, char *hvalue)
{
  return from_parse ((from_t *) route, hvalue);
}

/* returns the route header as a string.          */
/* INPUT : route_t *route | route header.  */
/* returns null on error. */
int
route_2char (route_t * route, char **dest)
{
  /* we can't use from_2char(): route and record_route */
  /* always use brackets. */
  return record_route_2char ((record_route_t *) route, dest);
}

/* deallocates a route_t structure.  */
/* INPUT : route_t *route | route header. */
void
route_free (route_t * route)
{
  from_free ((from_t *) route);
}
