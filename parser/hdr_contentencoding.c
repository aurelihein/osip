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

/* content-Encoding = token
   token possible values are gzip,compress,deflate
*/
int
msg_setcontent_encoding (sip_t * sip, char *hvalue)
{
  content_encoding_t *content_encoding;
  int i;

  i = content_encoding_init (&content_encoding);
  if (i != 0)
    return -1;
  i = content_encoding_parse (content_encoding, hvalue);
  if (i != 0)
    {
      content_encoding_free (content_encoding);
      sfree (content_encoding);
      return -1;
    }
#ifdef USE_TMP_BUFFER
  sip->message_property = 2;
#endif
  list_add (sip->content_encodings, content_encoding, -1);
  return 0;
}

int
msg_getcontent_encoding (sip_t * sip, int pos, content_encoding_t ** dest)
{
  content_encoding_t *ce;

  *dest = NULL;
  if (list_size (sip->content_encodings) <= pos)
    return -1;			/* does not exist */
  ce = (content_encoding_t *) list_get (sip->content_encodings, pos);
  *dest = ce;
  return pos;
}
