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

int
content_length_init (content_length_t ** cl)
{
  *cl = (content_length_t *) smalloc (sizeof (content_length_t));
  if (*cl == NULL)
    return -1;
  (*cl)->value = NULL;
  return 0;
}

/* adds the content_length header to message.       */
/* INPUT : const char *hvalue | value of header.    */
/* OUTPUT: sip_t *sip | structure to save results.  */
/* returns -1 on error. */
int
msg_setcontent_length (sip_t * sip, char *hvalue)
{
  int i;

  if (sip->contentlength != NULL)
    return -1;
  i = content_length_init (&(sip->contentlength));
  if (i != 0)
    return -1;
#ifdef USE_TMP_BUFFER
  sip->message_property = 2;
#endif
  i = content_length_parse (sip->contentlength, hvalue);
  if (i != 0)
    {
      content_encoding_free (sip->contentlength);
      sfree (sip->contentlength);
      sip->contentlength = 0;
      return -1;
    }

  return 0;
}

int
content_length_parse (content_length_t * contentlength, char *hvalue)
{
  if (strlen (hvalue) + 1 < 2)
    return -1;
  contentlength->value = (char *) smalloc (strlen (hvalue) + 1);
  if (contentlength->value == NULL)
    return -1;
  sstrncpy (contentlength->value, hvalue, strlen (hvalue));
  return 0;
}

/* returns the content_length header.            */
/* INPUT : sip_t *sip | sip message.   */
/* returns null on error. */
content_length_t *
msg_getcontent_length (sip_t * sip)
{
  return sip->contentlength;
}

/* returns the content_length header as a string.          */
/* INPUT : content_length_t *content_length | content_length header.  */
/* returns null on error. */
int
content_length_2char (content_length_t * cl, char **dest)
{
  if (cl == NULL)
    return -1;
  *dest = sgetcopy (cl->value);
  return 0;
}

/* deallocates a content_length_t strcture.  */
/* INPUT : content_length_t *content_length | content_length header. */
void
content_length_free (content_length_t * content_length)
{
  if (content_length == NULL)
    return;
  sfree (content_length->value);
}

int
content_length_clone (content_length_t * ctl, content_length_t ** dest)
{
  int i;
  content_length_t *cl;

  *dest = NULL;
  if (ctl == NULL)
    return -1;
  /*
     empty headers are allowed:
     if (ctl->value==NULL) return -1;
   */
  i = content_length_init (&cl);
  if (i == -1)			/* allocation failed */
    return -1;
  if (ctl->value != NULL)
    cl->value = sgetcopy (ctl->value);

  *dest = cl;
  return 0;
}
