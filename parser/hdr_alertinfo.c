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
msg_setalert_info (sip_t * sip, char *hvalue)
{
  alert_info_t *alert_info;
  int i;

  i = alert_info_init (&alert_info);
  if (i != 0)
    return -1;
  i = alert_info_parse (alert_info, hvalue);
  if (i != 0)
    {
      alert_info_free (alert_info);
      sfree (alert_info);
      return -1;
    }
#ifdef USE_TMP_BUFFER
  sip->message_property = 2;
#endif
  list_add (sip->alert_infos, alert_info, -1);
  return 0;
}

int
msg_getalert_info (sip_t * sip, int pos, alert_info_t ** dest)
{
  alert_info_t *alert_info;

  *dest = NULL;
  if (list_size (sip->alert_infos) <= pos)
    return -1;			/* does not exist */
  alert_info = (alert_info_t *) list_get (sip->alert_infos, pos);
  *dest = alert_info;
  return pos;
}
