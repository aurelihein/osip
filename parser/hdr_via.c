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

/* adds the via header to message.              */
/* INPUT : const char *hvalue | value of header.    */
/* OUTPUT: sip_t *sip | structure to save results.  */
/* returns -1 on error. */
int
msg_setvia (sip_t * sip, char *hvalue)
{
  via_t *via;
  int i;

  i = via_init (&via);
  if (i != 0)
    return -1;
  i = via_parse (via, hvalue);
  if (i != 0)
    {
      via_free (via);
      sfree (via);
      return -1;
    }
#ifdef USE_TMP_BUFFER
  sip->message_property = 2;
#endif
  list_add (sip->vias, via, -1);
  return 0;
}

/* adds the via header to message in the first position. (to be used by proxy) */
/* INPUT : const char *hvalue | value of header.    */
/* OUTPUT: sip_t *sip | structure to save results.  */
/* returns -1 on error. */
int
msg_appendtopvia (sip_t * sip, char *hvalue)
{
  via_t *via;
  int i;

  i = via_init (&via);
  if (i != 0)
    return -1;
  i = via_parse (via, hvalue);
  if (i != 0)
    {
      via_free (via);
      sfree (via);
      return -1;
    }
#ifdef USE_TMP_BUFFER
  sip->message_property = 2;
#endif
  list_add (sip->vias, via, 0);
  return 0;
}

/* returns the via header.                         */
/* INPUT : int pos | pos of via header.            */
/* INPUT : sip_t *sip | sip message.               */
/* OUTPUT: via_t *via | structure to save results. */
/* returns null on error. */
int
msg_getvia (sip_t * sip, int pos, via_t ** dest)
{
  *dest = NULL;
  if (sip == NULL)
    return -1;
  if (list_size (sip->vias) <= pos)
    return -1;
  *dest = (via_t *) list_get (sip->vias, pos);

  return pos;
}


int
via_init (via_t ** via)
{
  *via = (via_t *) smalloc (sizeof (via_t));
  if (*via == NULL)
    return -1;
  (*via)->version = NULL;
  (*via)->protocol = NULL;
  (*via)->host = NULL;
  (*via)->port = NULL;
  (*via)->comment = NULL;

  (*via)->via_params = (list_t *) smalloc (sizeof (list_t));
  if ((*via)->via_params == NULL)
    {
      sfree (*via);
      *via = NULL;
      return -1;
    }
  list_init ((*via)->via_params);

  return 0;
}

void
via_free (via_t * via)
{
  if (via == NULL)
    return;
  sfree (via->version);
  sfree (via->protocol);
  sfree (via->host);
  sfree (via->port);
  sfree (via->comment);
  generic_param_freelist (via->via_params);
  sfree (via->via_params);

  via->version = NULL;
  via->protocol = NULL;
  via->host = NULL;
  via->port = NULL;
  via->comment = NULL;
  via->via_params = NULL;
}

int
via_parse (via_t * via, char *hvalue)
{
  char *version;
  char *protocol;
  char *host;
  char *ipv6host;
  char *port;
  char *via_params;
  char *comment;

  version = strchr (hvalue, '/');
  if (version == NULL)
    return -1;

  protocol = strchr (version + 1, '/');
  if (protocol == NULL)
    return -1;

  /* set the version */
  if (protocol - version < 2)
    return -1;
  via->version = (char *) smalloc (protocol - version);
  if (via->version == NULL)
    return -1;
  sstrncpy (via->version, version + 1, protocol - version - 1);
  sclrspace (via->version);

  /* Here: we avoid matching an additionnal space */
  host = strchr (protocol + 1, ' ');
  if (host == NULL)
    return -1;			/* fixed in 0.8.4 */
  if (host == protocol + 1)	/* there are extra SPACE characters */
    {
      while (0 == strncmp (host, " ", 1))
	{
	  host++;
	  if (strlen (host) == 1)
	    return -1;		/* via is malformed */
	}
      /* here, we match the real space located after the protocol name */
      host = strchr (host + 1, ' ');
      if (host == NULL)
	return -1;		/* fixed in 0.8.4 */
    }

  /* set the protocol */
  if (host - protocol < 2)
    return -1;
  via->protocol = (char *) smalloc (host - protocol);
  if (via->protocol == NULL)
    return -1;
  sstrncpy (via->protocol, protocol + 1, host - protocol - 1);
  sclrspace (via->protocol);

  /* comments in Via are not allowed any more in the latest draft (09) */
  comment = strchr (host, '(');

  if (comment != NULL)
    {
      char *end_comment;

      end_comment = strchr (host, ')');
      if (end_comment == NULL)
	return -1;		/* if '(' exist ')' MUST exist */
      if (end_comment - comment < 2)
	return -1;
      via->comment = (char *) smalloc (end_comment - comment);
      if (via->comment == NULL)
	return -1;
      sstrncpy (via->comment, comment + 1, end_comment - comment - 1);
      comment--;
    }
  else
    comment = host + strlen (host);

  via_params = strchr (host, ';');

  if ((via_params != NULL) && (via_params < comment))
    /* via params exist */
    {
      char *tmp;

      if (comment - via_params + 1 < 2)
	return -1;
      tmp = (char *) smalloc (comment - via_params + 1);
      if (tmp == NULL)
	return -1;
      sstrncpy (tmp, via_params, comment - via_params);
      generic_param_parseall (via->via_params, tmp);
      sfree (tmp);
    }

  if (via_params == NULL)
    via_params = comment;

  /* add ipv6 support (0.8.4) */
  /* Via: SIP/2.0/UDP [mlke::zeezf:ezfz:zef:zefzf]:port;.... */
  ipv6host = strchr (host, '[');
  if (ipv6host != NULL && ipv6host < via_params)
    {
      port = strchr (ipv6host, ']');
      if (port == NULL || port > via_params)
	return -1;

      if (port - ipv6host < 2)
	return -1;
      via->host = (char *) smalloc (port - ipv6host);
      if (via->host == NULL)
	return -1;
      sstrncpy (via->host, ipv6host + 1, port - ipv6host - 1);
      sclrspace (via->host);

      port = strchr (port, ':');
    }
  else
    {
      port = strchr (host, ':');
      ipv6host = NULL;
    }

  if ((port != NULL) && (port < via_params))
    {
      if (via_params - port < 2)
	return -1;
      via->port = (char *) smalloc (via_params - port);
      if (via->port == NULL)
	return -1;
      sstrncpy (via->port, port + 1, via_params - port - 1);
      sclrspace (via->port);
    }
  else
    port = via_params;

  /* host is already set in the case of ipv6 */
  if (ipv6host != NULL)
    return 0;

  if (port - host < 2)
    return -1;
  via->host = (char *) smalloc (port - host);
  if (via->host == NULL)
    return -1;
  sstrncpy (via->host, host + 1, port - host - 1);
  sclrspace (via->host);

  return 0;
}


/* returns the via header as a string. */
/* INPUT : via_t via* | via header.    */
/* returns null on error. */
int
via_2char (via_t * via, char **dest)
{
  char *buf;
  int len;
  int plen;
  char *tmp;

  *dest = NULL;
  if ((via == NULL) || (via->host == NULL)
      || (via->version == NULL) || (via->protocol == NULL))
    return -1;

  len = strlen (via->version) + 1 + strlen (via->protocol) + 1 + 3 + 2;	/* sip/xxx/xxx */
  len = len + strlen (via->host) + 3 + 1;
  if (via->port != NULL)
    len = len + strlen (via->port) + 2;

  buf = (char *) smalloc (len);
  if (buf == NULL)
    return -1;

  if (strchr (via->host, ':') != NULL)
    {
      if (via->port == NULL)
	sprintf (buf, "SIP/%s/%s [%s]", via->version, via->protocol,
		 via->host);
      else
	sprintf (buf, "SIP/%s/%s [%s]:%s", via->version, via->protocol,
		 via->host, via->port);
    }
  else
    {
      if (via->port == NULL)
	sprintf (buf, "SIP/%s/%s %s", via->version, via->protocol, via->host);
      else
	sprintf (buf, "SIP/%s/%s %s:%s", via->version, via->protocol,
		 via->host, via->port);
    }



  {
    int pos = 0;
    generic_param_t *u_param;

    while (!list_eol (via->via_params, pos))
      {
	u_param = (generic_param_t *) list_get (via->via_params, pos);

	if (u_param->gvalue == NULL)
	  plen = strlen (u_param->gname) + 2;
	else
	  plen = strlen (u_param->gname) + strlen (u_param->gvalue) + 3;
	len = len + plen;
	buf = (char *) realloc (buf, len);
	tmp = buf;
	tmp = tmp + strlen (tmp);
	if (u_param->gvalue == NULL)
	  sprintf (tmp, ";%s", u_param->gname);
	else
	  sprintf (tmp, ";%s=%s", u_param->gname, u_param->gvalue);
	pos++;
      }
  }

  if (via->comment != NULL)
    {
      len = len + strlen (via->comment) + 4;
      buf = (char *) realloc (buf, len);
      tmp = buf;
      tmp = tmp + strlen (tmp);
      sprintf (tmp, " (%s)", via->comment);
    }
  *dest = buf;
  return 0;
}

void
via_setversion (via_t * via, char *version)
{
  via->version = version;
}

char *
via_getversion (via_t * via)
{
  if (via == NULL)
    return NULL;
  return via->version;
}

void
via_setprotocol (via_t * via, char *protocol)
{
  via->protocol = protocol;
}

char *
via_getprotocol (via_t * via)
{
  if (via == NULL)
    return NULL;
  return via->protocol;
}

void
via_sethost (via_t * via, char *host)
{
  via->host = host;
}

char *
via_gethost (via_t * via)
{
  if (via == NULL)
    return NULL;
  return via->host;
}

void
via_setport (via_t * via, char *port)
{
  via->port = port;
}

char *
via_getport (via_t * via)
{
  if (via == NULL)
    return NULL;
  return via->port;
}

void
via_setcomment (via_t * via, char *comment)
{
  via->comment = comment;
}

char *
via_getcomment (via_t * via)
{
  if (via == NULL)
    return NULL;
  return via->comment;
}

int
via_clone (via_t * via, via_t ** dest)
{
  int i;
  via_t *vi;

  *dest = NULL;
  if (via == NULL)
    return -1;
  if (via->version == NULL)
    return -1;
  if (via->protocol == NULL)
    return -1;
  if (via->host == NULL)
    return -1;

  i = via_init (&vi);
  if (i != 0)
    return -1;
  vi->version = sgetcopy (via->version);
  vi->protocol = sgetcopy (via->protocol);
  vi->host = sgetcopy (via->host);
  if (via->port != NULL)
    vi->port = sgetcopy (via->port);
  if (via->comment != NULL)
    vi->comment = sgetcopy (via->comment);

  {
    int pos = 0;
    generic_param_t *u_param;
    generic_param_t *dest_param;

    while (!list_eol (via->via_params, pos))
      {
	u_param = (generic_param_t *) list_get (via->via_params, pos);
	i = generic_param_clone (u_param, &dest_param);
	if (i != 0)
	  {
	    via_free (vi);
	    sfree (vi);
	    return -1;
	  }
	list_add (vi->via_params, dest_param, -1);
	pos++;
      }
  }
  *dest = vi;
  return 0;
}
