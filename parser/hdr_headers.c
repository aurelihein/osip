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


/* Add a header to a SIP message.                           */
/* INPUT :  char *hname | pointer to a header name.         */
/* INPUT :  char *hvalue | pointer to a header value.       */
/* OUTPUT: sip_t *sip | structure to save results.          */
/* returns -1 on error. */
int
msg_setheader (sip_t * sip, char *hname, char *hvalue)
{
  header_t *h;
  int i;

  if (hname == NULL)
    return -1;

  i = header_init (&h);
  if (i != 0)
    return -1;

  h->hname = (char *) smalloc (strlen (hname) + 1);

  if (h->hname == NULL)
    {
      header_free (h);
      sfree (h);
      return -1;
    }
  sstrncpy (h->hname, hname, strlen (hname));
  sclrspace (h->hname);

  if (hvalue != NULL)
    {				/* some headers can be null ("subject:") */
      h->hvalue = (char *) smalloc (strlen (hvalue) + 1);
      if (h->hvalue == NULL)
	{
	  header_free (h);
	  sfree (h);
	  return -1;
	}
      sstrncpy (h->hvalue, hvalue, strlen (hvalue));
      sclrspace (h->hvalue);
    }
  else
    h->hvalue = NULL;
#ifdef USE_TMP_BUFFER
  sip->message_property = 2;
#endif
  list_add (sip->headers, h, -1);
  return 0;			/* ok */
}

/* Add a header to a SIP message at the top of the list.    */
/* INPUT :  char *hname | pointer to a header name.         */
/* INPUT :  char *hvalue | pointer to a header value.       */
/* OUTPUT: sip_t *sip | structure to save results.          */
/* returns -1 on error. */
int
msg_settopheader (sip_t * sip, char *hname, char *hvalue)
{
  header_t *h;
  int i;

  if (hname == NULL)
    return -1;

  i = header_init (&h);
  if (i != 0)
    return -1;

  h->hname = (char *) smalloc (strlen (hname) + 1);

  if (h->hname == NULL)
    {
      header_free (h);
      sfree (h);
      return -1;
    }
  sstrncpy (h->hname, hname, strlen (hname));
  sclrspace (h->hname);

  if (hvalue != NULL)
    {				/* some headers can be null ("subject:") */
      h->hvalue = (char *) smalloc (strlen (hvalue) + 1);
      if (h->hvalue == NULL)
	{
	  header_free (h);
	  sfree (h);
	  return -1;
	}
      sstrncpy (h->hvalue, hvalue, strlen (hvalue));
      sclrspace (h->hvalue);
    }
  else
    h->hvalue = NULL;
#ifdef USE_TMP_BUFFER
  sip->message_property = 2;
#endif
  list_add (sip->headers, h, 0);
  return 0;			/* ok */
}

/* Get a header in a SIP message.                       */
/* INPUT : int pos | position of number in message.     */
/* OUTPUT: sip_t *sip | structure to scan for a header .*/
/* return null on error. */
int
msg_getheader (sip_t * sip, int pos, header_t ** dest)
{
  *dest = NULL;
  if (list_size (sip->headers) <= pos)
    return -1;			/* NULL */
  *dest = (header_t *) list_get (sip->headers, pos);
  return 0;
}

/* Get a header in a SIP message.                       */
/* INPUT : int pos | position where we start the search */
/* OUTPUT: sip_t *sip | structure to look for header.   */
/* return the current position of the header found      */
/* and -1 on error. */
int
msg_header_getbyname (sip_t * sip, char *hname, int pos, header_t ** dest)
{
  int i;
  header_t *tmp;

  *dest = NULL;
  i = pos;
  if (list_size (sip->headers) <= pos)
    return -1;			/* NULL */
  while (list_size (sip->headers) > i)
    {
      char *tmp2;

      tmp = (header_t *) list_get (sip->headers, i);
      tmp2 = sgetcopy (tmp->hname);
      stolowercase (tmp2);
      if (strcmp (tmp2, hname) == 0)
	{
	  *dest = tmp;
	  sfree (tmp2);
	  return i;
	}
      sfree (tmp2);
      i++;
    }
  return -1;			/* not found */
}

int
header_init (header_t ** header)
{
  *header = (header_t *) smalloc (sizeof (header_t));
  if (*header == NULL)
    return -1;
  (*header)->hname = NULL;
  (*header)->hvalue = NULL;
  return 0;
}

void
header_free (header_t * header)
{
  if (header == NULL)
    return;
  sfree (header->hname);
  sfree (header->hvalue);
  header->hname = NULL;
  header->hvalue = NULL;
}

/* returns the header as a string.    */
/* INPUT : header_t *header | header. */
/* returns null on error. */
int
header_2char (header_t * header, char **dest)
{
  int len;

  *dest = NULL;
  if ((header == NULL) || (header->hname == NULL))
    return -1;

  len = 0;
  if (header->hvalue != NULL)
    len = strlen (header->hvalue);

  *dest = (char *) smalloc (strlen (header->hname) + len + 3);
  if (*dest == NULL)
    return -1;

  if (header->hvalue != NULL)
    sprintf (*dest, "%s: %s", header->hname, header->hvalue);
  else
    sprintf (*dest, "%s: ", header->hname);
  return 0;
}

char *
header_getname (header_t * header)
{
  if (header == NULL)
    return NULL;
  return header->hname;
}

void
header_setname (header_t * header, char *name)
{
  header->hname = name;
}

char *
header_getvalue (header_t * header)
{
  if (header == NULL)
    return NULL;
  return header->hvalue;
}

void
header_setvalue (header_t * header, char *value)
{
  header->hvalue = value;
}

int
header_clone (header_t * header, header_t ** dest)
{
  int i;
  header_t *he;

  *dest = NULL;
  if (header == NULL)
    return -1;
  if (header->hname == NULL)
    return -1;

  i = header_init (&he);
  if (i != 0)
    return -1;
  he->hname = sgetcopy (header->hname);
  if (header->hvalue != NULL)
    he->hvalue = sgetcopy (header->hvalue);

  *dest = he;
  return 0;
}
