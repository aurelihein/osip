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

int
record_route_init (record_route_t ** record_route)
{
  return from_init ((from_t **) record_route);
}

/* adds the record_route header to message.         */
/* INPUT : const char *hvalue | value of header.    */
/* OUTPUT: sip_t *sip | structure to save results.  */
/* returns -1 on error. */
int
msg_setrecord_route (sip_t * sip, char *hvalue)
{
  record_route_t *record_route;
  int i;

  i = record_route_init (&record_route);
  if (i == -1)                  /* allocation failed */
    return -1;
  i = record_route_parse (record_route, hvalue);
  if (i == -1)
    return -1;

#ifdef USE_TMP_BUFFER
  sip->message_property = 2;
#endif
  list_add (sip->record_routes, record_route, -1);
  return 0;
}

/* returns the record_route header.    */
/* INPUT : sip_t *sip | sip message.   */
/* returns null on error. */
int
msg_getrecord_route (sip_t * sip, int pos, record_route_t ** dest)
{
  record_route_t *record_route;

  *dest = NULL;
  if (list_size (sip->record_routes) <= pos)
    return -1;                  /* does not exist */
  record_route = (record_route_t *) list_get (sip->record_routes, pos);
  *dest = record_route;
  return pos;
}

int
record_route_parse (record_route_t * record_route, char *hvalue)
{
  return from_parse ((from_t *) record_route, hvalue);
}

/* returns the record_route header as a string.          */
/* INPUT : record_route_t *record_route | record_route header.  */
/* returns null on error. */
/* returns the from header as a string.  */
/* INPUT : from_t *from | from header.   */
/* returns null on error. */
int
record_route_2char (record_route_t * record_route, char **dest)
{
  char *url;
  char *buf;

  *dest = NULL;
  if ((record_route == NULL) || (record_route->url == NULL))
    return -1;

  buf = (char *) smalloc (200);
  *dest = buf;

  if (url_2char (record_route->url, &url) == -1)
    {
      sfree (buf);
      *dest = NULL;
      return -1;
    }

  /* route and record-route always use brackets */
  if (record_route->displayname != NULL)
    sprintf (buf, "%s <%s>", record_route->displayname, url);
  else
    sprintf (buf, "<%s>", url);
  sfree (url);

  buf = buf + strlen (buf);
  {
    int pos = 0;
    generic_param_t *u_param;

    while (!list_eol (record_route->gen_params, pos))
      {
        u_param = (generic_param_t *) list_get (record_route->gen_params, pos);
        if (u_param->gvalue != NULL)
          sprintf (buf, ";%s=%s", u_param->gname, u_param->gvalue);
        else
          sprintf (buf, ";%s", u_param->gname);
        buf = buf + strlen (buf);
        pos++;
      }
  }
  return 0;
}

/* deallocates a record_route_t structure.  */
/* INPUT : record_route_t *record_route | record_route header. */
void
record_route_free (record_route_t * record_route)
{
  from_free ((from_t *) record_route);
}
