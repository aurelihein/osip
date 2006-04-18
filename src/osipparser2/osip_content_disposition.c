/*
  The oSIP library implements the Session Initiation Protocol (SIP -rfc3261-)
  Copyright (C) 2001,2002,2003,2004,2005  Aymeric MOIZARD jack@atosc.org
  
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

#include <osipparser2/osip_port.h>
#include <osipparser2/osip_message.h>
#include <osipparser2/osip_parser.h>
#include "parser.h"


int
osip_content_disposition_parse (osip_content_disposition_t * cd,
                                const char *hvalue)
{
  const char *cd_params;

  cd_params = strchr (hvalue, ';');

  if (cd_params != NULL)
    {
      if (__osip_generic_param_parseall (&cd->gen_params, cd_params) == -1)
        return -1;
  } else
    cd_params = hvalue + strlen (hvalue);

  if (cd_params - hvalue + 1 < 2)
    return -1;
  cd->element = (char *) osip_malloc (cd_params - hvalue + 1);
  if (cd->element == NULL)
    return -1;
  osip_clrncpy (cd->element, hvalue, cd_params - hvalue);

  return 0;
}
