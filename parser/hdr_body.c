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

#include <stdio.h>
#include <stdlib.h>

#include <osip/smsg.h>
#include <osip/port.h>
#include "msg.h"

int
body_init (body_t ** body)
{
  *body = (body_t *) smalloc (sizeof (body_t));
  if (*body == NULL)
    return -1;
  (*body)->body = NULL;
  (*body)->content_type = NULL;

  (*body)->headers = (list_t *) smalloc (sizeof (list_t));
  if ((*body)->headers == NULL)
    {
      sfree (*body);
      *body = NULL;
      return -1;
    }
  list_init ((*body)->headers);
  return 0;
}

/* fill the body of message.                              */
/* INPUT : char *buf | pointer to the start of body.      */
/* OUTPUT: sip_t *sip | structure to save results.        */
/* returns -1 on error. */
int
msg_setbody (sip_t * sip, char *buf)
{
  body_t *body;
  int i;

  i = body_init (&body);
  if (i != 0)
    return -1;
  i = body_parse (body, buf);
  if (i != 0)
    {
      body_free (body);
      sfree (body);
      return -1;
    }
#ifdef USE_TMP_BUFFER
  sip->message_property = 2;
#endif
  list_add (sip->bodies, body, -1);
  return 0;
}

/* returns the pos of body header.                   */
/* INPUT : int pos | pos of body header.             */
/* INPUT : sip_t *sip | sip message.                 */
/* OUTPUT: body_t *body | structure to save results. */
/* returns -1 on error. */
int
msg_getbody (sip_t * sip, int pos, body_t ** dest)
{
  body_t *body;

  *dest = NULL;
  if (list_size (sip->bodies) <= pos)
    return -1;			/* does not exist */
  body = (body_t *) list_get (sip->bodies, pos);
  *dest = body;
  return pos;
}

int
body_setcontenttype (body_t * body, char *hvalue)
{
  int i;

  if (body == NULL)
    return -1;
  if (hvalue == NULL)
    return -1;

  i = content_type_init (&(body->content_type));
  if (i != 0)
    return -1;
  i = content_type_parse (body->content_type, hvalue);
  if (i != 0)
    {
      content_type_free (body->content_type);
      sfree (body->content_type);
      body->content_type = NULL;
      return -1;
    }
  return 0;
}

int
body_setheader (body_t * body, char *hname, char *hvalue)
{
  header_t *h;
  int i;

  if (body == NULL)
    return -1;
  if (hname == NULL)
    return -1;
  if (hvalue == NULL)
    return -1;

  i = header_init (&h);
  if (i != 0)
    return -1;

  h->hname = sgetcopy (hname);
  h->hvalue = sgetcopy (hvalue);

  list_add (body->headers, h, -1);
  return 0;
}

/* fill the body of message.                              */
/* INPUT : char *buf | pointer to the start of body.      */
/* OUTPUT: sip_t *sip | structure to save results.        */
/* returns -1 on error. */
int
msg_setbody_mime (sip_t * sip, char *buf)
{
  body_t *body;
  int i;

  i = body_init (&body);
  if (i != 0)
    return -1;
  i = body_parse_mime (body, buf);
  if (i != 0)
    {
      body_free (body);
      sfree (body);
      return -1;
    }
#ifdef USE_TMP_BUFFER
  sip->message_property = 2;
#endif
  list_add (sip->bodies, body, -1);
  return 0;
}

int
body_parse_header (body_t * body, char *start_of_body_header,
		   char **next_body)
{
  char *start_of_line;
  char *end_of_line;
  char *colon_index;
  char *hname;
  char *hvalue;
  int i;

  *next_body = NULL;
  start_of_line = start_of_body_header;
  for (;;)
    {
      i = find_next_crlf (start_of_line, &end_of_line);
      if (i == -1)
	return -1;		/* error case: no end of body found */

      /* find the headere name */
      colon_index = strchr (start_of_line, ':');
      if (colon_index == NULL)
	return -1;		/* this is also an error case */

      if (colon_index - start_of_line + 1 < 2)
	return -1;
      hname = (char *) smalloc (colon_index - start_of_line + 1);
      if (hname == NULL)
	return -1;
      sstrncpy (hname, start_of_line, colon_index - start_of_line);
      sclrspace (hname);
      stolowercase (hname);

      if ((end_of_line - 2) - colon_index < 2)
	return -1;
      hvalue = (char *) smalloc ((end_of_line - 2) - colon_index);
      if (hvalue == NULL)
	{
	  sfree (hname);
	  return -1;
	}
      sstrncpy (hvalue, colon_index + 1, (end_of_line - 2) - colon_index - 1);
      sclrspace (hvalue);

      /* really store the header in the sip structure */
      /* i = msg_set_header(sip, hname, hvalue); */
      if (strncmp (hname, "content-type", 12) == 0)
	i = body_setcontenttype (body, hvalue);
      else
	i = body_setheader (body, hname, hvalue);
      sfree (hname);
      sfree (hvalue);
      if (i == -1)
	return -1;

      if (strncmp (end_of_line, CRLF, 2) == 0
	  || strncmp (end_of_line, "\n", 1) == 0
	  || strncmp (end_of_line, "\r", 1) == 0)
	{
	  *next_body = end_of_line;
	  return 0;
	}
      start_of_line = end_of_line;
    }
}

int
body_parse (body_t * body, char *start_of_body)
{
  int i;

  if (body == NULL)
    return -1;
  if (start_of_body == NULL)
    return -1;
  if (body->headers == NULL)
    return -1;

  /*  i = find_next_crlfcrlf(start_of_body, &end_of_body);
     if (i==-1) return -1; 
     body->body = (char *)smalloc(end_of_body-start_of_body+1);
     sstrncpy(body->body, start_of_body, end_of_body-start_of_body);
   */

  i = strlen (start_of_body);
  body->body = (char *) smalloc (i + 1);
  if (body->body == NULL)
    return -1;
  sstrncpy (body->body, start_of_body, i);
  return 0;
}

int
body_parse_mime (body_t * body, char *start_of_body)
{
  char *end_of_body_header;
  char *start_of_body_header;
  int i;

  if (body == NULL)
    return -1;
  if (start_of_body == NULL)
    return -1;
  if (body->headers == NULL)
    return -1;

  start_of_body_header = start_of_body;

  i = body_parse_header (body, start_of_body_header, &end_of_body_header);
  if (i == -1)
    return -1;

  start_of_body_header = end_of_body_header;
  /* set the real start of body */
  if (strncmp (start_of_body_header, CRLF, 2) == 0)
    start_of_body_header = start_of_body_header + 2;
  else
    {
      if ((strncmp (start_of_body_header, "\n", 1) == 0)
	  || (strncmp (start_of_body_header, "\r", 1) == 0))
	start_of_body_header = start_of_body_header + 1;
      else
	return -1;		/* message does not end with CRLFCRLF, CRCR or LFLF */
    }

  end_of_body_header = end_of_body_header + strlen (end_of_body_header);
  body->body =
    (char *) smalloc (end_of_body_header - start_of_body_header + 1);
  if (body->body == NULL)
    return -1;
  sstrncpy (body->body, start_of_body_header,
	    end_of_body_header - start_of_body_header);

  return 0;

}

/* returns the body as a string.          */
/* INPUT : body_t *body | body.  */
/* returns null on error. */
int
body_2char (body_t * body, char **dest)
{
  char *tmp_body;
  char *tmp;
  char *ptr;
  int pos;
  int i;
  unsigned length;

  *dest = NULL;
  if (body == NULL)
    return -1;
  if (body->body == NULL)
    return -1;
  if (body->headers == NULL)
    return -1;

  length = strlen (body->body) + (list_size (body->headers) * 40);
  tmp_body = (char *) smalloc (length);
  if (tmp_body == NULL)
    return -1;
  ptr = tmp_body;		/* save the initial address of the string */

  if (body->content_type != NULL)
    {
      sstrncpy (tmp_body, "content-type: ", 14);
      tmp_body = tmp_body + strlen (tmp_body);
      i = content_type_2char (body->content_type, &tmp);
      if (i == -1)
	{
	  sfree (tmp_body);
	  return -1;
	}
      if (length < tmp_body - ptr + strlen (tmp) + 4)
	{
	  int len;

	  len = tmp_body - ptr;
	  length = length + strlen (tmp) + 4;
	  ptr = realloc (ptr, length);
	  tmp_body = ptr + len;
	}

      sstrncpy (tmp_body, tmp, strlen (tmp));
      sfree (tmp);
      tmp_body = tmp_body + strlen (tmp_body);
      sstrncpy (tmp_body, CRLF, 2);
      tmp_body = tmp_body + 2;
    }

  pos = 0;
  while (!list_eol (body->headers, pos))
    {
      header_t *header;

      header = (header_t *) list_get (body->headers, pos);
      i = header_2char (header, &tmp);
      if (i == -1)
	{
	  sfree (tmp_body);
	  return -1;
	}
      if (length < tmp_body - ptr + strlen (tmp) + 4)
	{
	  int len;

	  len = tmp_body - ptr;
	  length = length + strlen (tmp) + 4;
	  ptr = realloc (ptr, length);
	  tmp_body = ptr + len;
	}
      sstrncpy (tmp_body, tmp, strlen (tmp));
      sfree (tmp);
      tmp_body = tmp_body + strlen (tmp_body);
      sstrncpy (tmp_body, CRLF, 2);
      tmp_body = tmp_body + 2;
      pos++;
    }

  if ((list_size (body->headers) > 0) || (body->content_type != NULL))
    {
      sstrncpy (tmp_body, CRLF, 2);
      tmp_body = tmp_body + 2;
    }
  if (length < tmp_body - ptr + strlen (body->body) + 4)
    {
      int len;

      len = tmp_body - ptr;
      length = length + strlen (body->body) + 4;
      ptr = realloc (ptr, length);
      tmp_body = ptr + len;
    }
  sstrncpy (tmp_body, body->body, strlen (body->body));
  tmp_body = tmp_body + strlen (body->body);

  /* end of this body */

  *dest = ptr;
  return 0;

}

/* deallocates a body structure.  */
/* INPUT : body_t *body | body.  */
void
body_free (body_t * body)
{
  if (body == NULL)
    return;
  sfree (body->body);
  if (body->content_type != NULL)
    {
      content_type_free (body->content_type);
      sfree (body->content_type);
    }

  {
    header_t *header;

    while (!list_eol (body->headers, 0))
      {
	header = (header_t *) list_get (body->headers, 0);
	list_remove (body->headers, 0);
	header_free (header);
	sfree (header);
      }
    sfree (body->headers);
  }
}
