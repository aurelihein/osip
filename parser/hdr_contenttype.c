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
content_type_init (content_type_t ** content_type)
{
  *content_type = (content_type_t *) smalloc (sizeof (content_type_t));
  if (*content_type == NULL)
    return -1;

  (*content_type)->type = NULL;
  (*content_type)->subtype = NULL;

  (*content_type)->gen_params = (list_t *) smalloc (sizeof (list_t));
  if ((*content_type)->gen_params == NULL)
    {
      sfree ((*content_type)->gen_params);
      *content_type = NULL;
      return -1;
    }
  list_init ((*content_type)->gen_params);

  return 0;
}

/* adds the content_type header to message.              */
/* INPUT : char *hvalue | value of header.    */
/* OUTPUT: sip_t *sip | structure to save results.  */
/* returns -1 on error. */
int
msg_setcontent_type (sip_t * sip, char *hvalue)
{
  int i;

  if (sip->content_type != NULL)
    return -1;
  i = content_type_init (&(sip->content_type));
  if (i != 0)
    return -1;
#ifdef USE_TMP_BUFFER
  sip->message_property = 2;
#endif
  i = content_type_parse (sip->content_type, hvalue);
  if (i != 0)
    {
      content_type_free (sip->content_type);
      sfree (sip->content_type);
      sip->content_type = NULL;
    }
  return 0;
}


/* returns the content_type header.            */
/* INPUT : sip_t *sip | sip message.   */
/* returns null on error. */
content_type_t *
msg_getcontent_type (sip_t * sip)
{
  return sip->content_type;
}

/* parses the string as a content_type header.                   */
/* INPUT : const char *string | pointer to a content_type string.*/
/* OUTPUT: content_type_t *content_type | structure to save results.     */
/* returns -1 on error. */
int
content_type_parse (content_type_t * content_type, char *hvalue)
{
  char *subtype;
  char *content_type_params;

  /* How to parse:

     we'll place the pointers:
     subtype              =>  beginning of subtype
     content_type_params  =>  beginning of params

     examples:

     application/multipart ; boundary=
     ^          ^
   */

  subtype = strchr (hvalue, '/');
  content_type_params = strchr (hvalue, ';');

  if (subtype == NULL)
    return -1;			/* do we really mind such an error */

  if (content_type_params != NULL)
    {
      if (generic_param_parseall (content_type->gen_params,
				  content_type_params) == -1)
	return -1;
    }
  else
    content_type_params = subtype + strlen (subtype);

  if (subtype - hvalue + 1 < 2)
    return -1;
  content_type->type = (char *) smalloc (subtype - hvalue + 1);
  if (content_type->type == NULL)
    return -1;
  sstrncpy (content_type->type, hvalue, subtype - hvalue);
  sclrspace (content_type->type);

  if (content_type_params - subtype < 2)
    return -1;
  content_type->subtype = (char *) smalloc (content_type_params - subtype);
  if (content_type->subtype == NULL)
    return -1;
  sstrncpy (content_type->subtype, subtype + 1,
	    content_type_params - subtype - 1);
  sclrspace (content_type->subtype);

  return 0;
}


/* returns the content_type header as a string.  */
/* INPUT : content_type_t *content_type | content_type header.   */
/* returns null on error. */
int
content_type_2char (content_type_t * content_type, char **dest)
{
  char *buf;
  char *tmp;
  int len;

  *dest = NULL;
  if ((content_type == NULL) || (content_type->type == NULL)
      || (content_type->subtype == NULL))
    return -1;

  /* try to guess a long enough length */
  len = strlen (content_type->type) + strlen (content_type->subtype) + 4	/* for '/', ' ', ';' and '\0' */
    + 10 * list_size (content_type->gen_params);

  buf = (char *) smalloc (len);
  tmp = buf;

  sprintf (tmp, "%s/%s", content_type->type, content_type->subtype);

  tmp = tmp + strlen (tmp);
  {
    int pos = 0;
    generic_param_t *u_param;

    if (!list_eol (content_type->gen_params, pos))
      {				/* needed for cannonical form! (authentication issue of rfc2543) */
	sprintf (tmp, " ");
	tmp++;
      }
    while (!list_eol (content_type->gen_params, pos))
      {
	int tmp_len;

	u_param =
	  (generic_param_t *) list_get (content_type->gen_params, pos);
	if (u_param->gvalue == NULL)
	  {
	    sfree (buf);
	    return -1;
	  }
	tmp_len = strlen (buf) + 4 + strlen (u_param->gname)
	  + strlen (u_param->gvalue);
	if (len < tmp_len)
	  {
	    buf = realloc (buf, tmp_len);
	    len = tmp_len;
	    tmp = buf + strlen (buf);
	  }
	sprintf (tmp, ";%s=%s", u_param->gname, u_param->gvalue);
	tmp = tmp + strlen (tmp);
	pos++;
      }
  }
  *dest = buf;
  return 0;
}


/* deallocates a content_type_t structure.  */
/* INPUT : content_type_t *content_type | content_type. */
void
content_type_free (content_type_t * content_type)
{
  if (content_type == NULL)
    return;
  sfree (content_type->type);
  sfree (content_type->subtype);

  generic_param_freelist (content_type->gen_params);
  sfree (content_type->gen_params);

  content_type->type = NULL;
  content_type->subtype = NULL;
  content_type->gen_params = NULL;
}

int
content_type_clone (content_type_t * ctt, content_type_t ** dest)
{
  int i;
  content_type_t *ct;

  *dest = NULL;
  if (ctt == NULL)
    return -1;
  if (ctt->type == NULL)
    return -1;
  if (ctt->subtype == NULL)
    return -1;

  i = content_type_init (&ct);
  if (i != 0)			/* allocation failed */
    return -1;
  ct->type = sgetcopy (ctt->type);
  ct->subtype = sgetcopy (ctt->subtype);

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
	    content_type_free (ct);
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
