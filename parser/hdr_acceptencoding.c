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


/* Accept-Encoding = token
   token possible values are gzip,compress,deflate,identity
*/
int
msg_setaccept_encoding (sip_t * sip, char *hvalue)
{
  accept_encoding_t *accept_encoding;
  int i;

  i = accept_encoding_init (&accept_encoding);
  if (i != 0)
    return -1;
  i = accept_encoding_parse (accept_encoding, hvalue);
  if (i != 0)
    {
      accept_encoding_free (accept_encoding);
      sfree (accept_encoding);
      return -1;
    }
#ifdef USE_TMP_BUFFER
  sip->message_property = 2;
#endif
  list_add (sip->accept_encodings, accept_encoding, -1);
  return 0;
}

int
msg_getaccept_encoding (sip_t * sip, int pos, accept_encoding_t ** dest)
{
  accept_encoding_t *accept_encoding;

  *dest = NULL;
  if (list_size (sip->accept_encodings) <= pos)
    return -1;			/* does not exist */
  accept_encoding =
    (accept_encoding_t *) list_get (sip->accept_encodings, pos);
  *dest = accept_encoding;
  return pos;
}

int
accept_encoding_init (accept_encoding_t ** accept_encoding)
{
  *accept_encoding =
    (accept_encoding_t *) smalloc (sizeof (accept_encoding_t));
  if (*accept_encoding == NULL)
    return -1;
  (*accept_encoding)->element = NULL;

  (*accept_encoding)->gen_params = (list_t *) smalloc (sizeof (list_t));
  if ((*accept_encoding)->gen_params == NULL)
    {
      sfree (*accept_encoding);
      *accept_encoding = NULL;
      return -1;
    }
  list_init ((*accept_encoding)->gen_params);

  return 0;
}

int
accept_encoding_parse (accept_encoding_t * accept_encoding, char *hvalue)
{
  char *accept_encoding_params;

  accept_encoding_params = strchr (hvalue, ';');

  if (accept_encoding_params != NULL)
    {
      if (generic_param_parseall (accept_encoding->gen_params,
				  accept_encoding_params) == -1)
	return -1;
    }
  else
    accept_encoding_params = hvalue + strlen (hvalue);

  if (accept_encoding_params - hvalue + 1 < 2)
    return -1;
  accept_encoding->element =
    (char *) smalloc (accept_encoding_params - hvalue + 1);
  if (accept_encoding->element == NULL)
    return -1;
  sstrncpy (accept_encoding->element, hvalue,
	    accept_encoding_params - hvalue);
  sclrspace (accept_encoding->element);

  return 0;
}

/* returns the accept_encoding header as a string.  */
/* INPUT : accept_encoding_t *accept_encoding | accept_encoding header.   */
/* returns null on error. */
int
accept_encoding_2char (accept_encoding_t * accept_encoding, char **dest)
{
  char *buf;
  char *tmp;
  int len;

  *dest = NULL;
  if ((accept_encoding == NULL) || (accept_encoding->element == NULL))
    return -1;

  len = strlen (accept_encoding->element) + 2;
  buf = (char *) smalloc (len);
  if (buf == NULL)
    return -1;

  sprintf (buf, "%s", accept_encoding->element);
  {
    int pos = 0;
    int plen;
    generic_param_t *u_param;

    while (!list_eol (accept_encoding->gen_params, pos))
      {
	u_param =
	  (generic_param_t *) list_get (accept_encoding->gen_params, pos);
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
  *dest = buf;
  return 0;
}

/* deallocates a accept_encoding_t structure.  */
/* INPUT : accept_encoding_t *accept_encoding | accept_encoding. */
void
accept_encoding_free (accept_encoding_t * accept_encoding)
{
  if (accept_encoding == NULL)
    return;
  sfree (accept_encoding->element);

  generic_param_freelist (accept_encoding->gen_params);
  sfree (accept_encoding->gen_params);

  accept_encoding->element = NULL;
  accept_encoding->gen_params = NULL;
}

int
accept_encoding_clone (accept_encoding_t * ctt, accept_encoding_t ** dest)
{
  int i;
  accept_encoding_t *ct;

  *dest = NULL;
  if (ctt == NULL)
    return -1;
  if (ctt->element == NULL)
    return -1;

  i = accept_encoding_init (&ct);
  if (i != 0)			/* allocation failed */
    return -1;
  ct->element = sgetcopy (ctt->element);
  if (ctt->element != NULL && ct->element == NULL)
    {
      accept_encoding_free (ct);
      sfree (ct);
      return -1;
    }
  {
    int pos = 0;
    generic_param_t *u_param;
    generic_param_t *dest_param;

    while (!list_eol (ctt->gen_params, pos))
      {
	u_param = (generic_param_t *) list_get (ctt->gen_params, pos);
	i = generic_param_clone (u_param, &dest_param);
	if (i != 0)
	  {
	    accept_encoding_free (ct);
	    sfree (ct);
	    return -1;
	  }
	list_add (ct->gen_params, dest_param, -1);
	pos++;
      }
  }
  *dest = ct;
  return 0;
}


char *
accept_encoding_getelement (accept_encoding_t * ae)
{
  return ae->element;
}

void
accept_encoding_setelement (accept_encoding_t * ae, char *element)
{
  ae->element = element;
}
