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

int
msg_setcall_info (sip_t * sip, char *hvalue)
{
  call_info_t *call_info;
  int i;

  i = call_info_init (&call_info);
  if (i != 0)
    return -1;
  i = call_info_parse (call_info, hvalue);
  if (i != 0)			/* allocation failed */
    {
      call_info_free (call_info);
      sfree (call_info);
      return -1;
    }
#ifdef USE_TMP_BUFFER
  sip->message_property = 2;
#endif
  list_add (sip->call_infos, call_info, -1);
  return 0;
}

int
msg_getcall_info (sip_t * sip, int pos, call_info_t ** dest)
{
  call_info_t *call_info;

  *dest = NULL;
  if (list_size (sip->call_infos) <= pos)
    return -1;			/* does not exist */
  call_info = (call_info_t *) list_get (sip->call_infos, pos);
  *dest = call_info;
  return pos;
}

int
call_info_init (call_info_t ** call_info)
{
  *call_info = (call_info_t *) smalloc (sizeof (call_info_t));
  if (*call_info == NULL)
    return -1;

  (*call_info)->element = NULL;

  (*call_info)->gen_params = (list_t *) smalloc (sizeof (list_t));
  if ((*call_info)->gen_params == NULL)
    {
      sfree (*call_info);
      *call_info = NULL;
      return -1;
    }
  list_init ((*call_info)->gen_params);

  return 0;
}

int
call_info_parse (call_info_t * call_info, char *hvalue)
{
  char *call_info_params;

  call_info_params = strchr (hvalue, '<');
  if (call_info_params == NULL)
    return -1;

  call_info_params = strchr (call_info_params + 1, '>');
  if (call_info_params == NULL)
    return -1;

  call_info_params = strchr (call_info_params + 1, ';');

  if (call_info_params != NULL)
    {
      if (generic_param_parseall (call_info->gen_params, call_info_params) ==
	  -1)
	return -1;
    }
  else
    call_info_params = hvalue + strlen (hvalue);

  if (call_info_params - hvalue + 1 < 2)
    return -1;
  call_info->element = (char *) smalloc (call_info_params - hvalue + 1);
  if (call_info->element == NULL)
    return -1;
  sstrncpy (call_info->element, hvalue, call_info_params - hvalue);
  sclrspace (call_info->element);

  return 0;
}

/* returns the call_info header as a string.  */
/* INPUT : call_info_t *call_info | call_info header.   */
/* returns null on error. */
int
call_info_2char (call_info_t * call_info, char **dest)
{
  char *buf;
  char *tmp;
  int len;
  int plen;

  *dest = NULL;
  if ((call_info == NULL) || (call_info->element == NULL))
    return -1;

  len = strlen (call_info->element) + 2;
  buf = (char *) smalloc (len);
  if (buf == NULL)
    return -1;
  *dest = buf;

  sprintf (buf, "%s", call_info->element);

  {
    int pos = 0;
    generic_param_t *u_param;

    while (!list_eol (call_info->gen_params, pos))
      {
	u_param = (generic_param_t *) list_get (call_info->gen_params, pos);
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


/* deallocates a call_info_t structure.  */
/* INPUT : call_info_t *call_info | call_info. */
void
call_info_free (call_info_t * call_info)
{
  if (call_info == NULL)
    return;
  sfree (call_info->element);

  generic_param_freelist (call_info->gen_params);
  sfree (call_info->gen_params);

  call_info->element = NULL;
  call_info->gen_params = NULL;
}

int
call_info_clone (call_info_t * ctt, call_info_t ** dest)
{
  int i;
  call_info_t *ct;

  *dest = NULL;
  if (ctt == NULL)
    return -1;
  if (ctt->element == NULL)
    return -1;

  i = call_info_init (&ct);
  if (i != 0)			/* allocation failed */
    return -1;
  ct->element = sgetcopy (ctt->element);

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
	    call_info_free (ct);
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
call_info_geturi (call_info_t * ae)
{
  return ae->element;
}

void
call_info_seturi (call_info_t * ae, char *uri)
{
  ae->element = uri;
}
