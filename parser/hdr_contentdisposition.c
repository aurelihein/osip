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
msg_setcontent_disposition (sip_t * sip, char *hvalue)
{
  content_disposition_t *content_disposition;
  int i;

  i = content_disposition_init (&content_disposition);
  if (i != 0)
    return -1;
  i = content_disposition_parse (content_disposition, hvalue);
  if (i != 0)
    {
      content_disposition_free (content_disposition);
      sfree (content_disposition);
      return -1;
    }
#ifdef USE_TMP_BUFFER
  sip->message_property = 2;
#endif
  list_add (sip->content_dispositions, content_disposition, -1);
  return 0;
}

int
msg_getcontent_disposition (sip_t * sip, int pos,
			    content_disposition_t ** dest)
{
  content_disposition_t *content_disposition;

  *dest = NULL;
  if (list_size (sip->content_dispositions) <= pos)
    return -1;			/* does not exist */
  content_disposition =
    (content_disposition_t *) list_get (sip->content_dispositions, pos);
  *dest = content_disposition;
  return pos;
}



int
content_disposition_parse (content_disposition_t * cd, char *hvalue)
{
  char *cd_params;

  cd_params = strchr (hvalue, ';');

  if (cd_params != NULL)
    {
      if (generic_param_parseall (cd->gen_params, cd_params) == -1)
	return -1;
    }
  else
    cd_params = hvalue + strlen (hvalue);

  if (cd_params - hvalue + 1 < 2)
    return -1;
  cd->element = (char *) smalloc (cd_params - hvalue + 1);
  if (cd->element == NULL)
    return -1;
  sstrncpy (cd->element, hvalue, cd_params - hvalue);
  sclrspace (cd->element);

  return 0;
}
