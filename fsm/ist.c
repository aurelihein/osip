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

#include <osip/port.h>
#include <osip/osip.h>

#include "fsm.h"

int
ist_init (ist_t ** ist, osip_t * osip, sip_t * invite)
{
  int i;

  OSIP_TRACE (osip_trace
	      (__FILE__, __LINE__, OSIP_INFO2, NULL,
	       "allocating IST context\n"));

  *ist = (ist_t *) smalloc (sizeof (ist_t));
  if (*ist == NULL)
    return -1;

#ifndef DISABLE_MEMSET
  memset (*ist, 0, sizeof (ist_t));
#endif
  /* for INVITE retransmissions */
  {
    via_t *via;
    char *proto;

    i = msg_getvia (invite, 0, &via);	/* get top via */
    if (i != 0)
      goto ii_error_1;
    proto = via_getprotocol (via);
    if (proto == NULL)
      goto ii_error_1;

#ifndef WIN32
    i = strncasecmp (proto, "TCP", 3);
#else
    i = _stricmp (proto, "TCP");
#endif
    if (i != 0)
      {				/* for other reliable protocol than TCP, the timer
				   must be desactived by the external application */
	(*ist)->timer_g_length = DEFAULT_T1;
	(*ist)->timer_g_start = -1;	/* not started */

	(*ist)->timer_i_length = DEFAULT_T4;
	(*ist)->timer_i_start = -1;	/* not started */
      }
    else
      {				/* TCP is used: */
	(*ist)->timer_g_length = -1;	/* A is not ACTIVE */
	(*ist)->timer_g_start = -1;

	(*ist)->timer_i_length = 0;	/* MUST do the transition immediatly */
	(*ist)->timer_i_start = -1;	/* not started */
      }
  }

  (*ist)->timer_h_length = 64 * DEFAULT_T1;
  (*ist)->timer_h_start = -1;	/* not started */

  (*ist)->auto_send_100 = 0;

  return 0;

ii_error_1:
  sfree (*ist);
  return -1;
}

int
ist_free (ist_t * ist)
{
  if (ist == NULL)
    return -1;
  OSIP_TRACE (osip_trace
	      (__FILE__, __LINE__, OSIP_INFO2, NULL, "free ist ressource\n"));

  return 0;
}

int
ist_set_auto_send_100 (ist_t * ist, int flag)
{
  if (ist == NULL)
    return -1;
  if (flag != 0)
    ist->auto_send_100 = -1;
  else
    ist->auto_send_100 = 0;
  return 0;
}

sipevent_t *
ist_need_timer_g_event (ist_t * ist, state_t state, int transactionid)
{
  time_t now = time (NULL);

  if (ist == NULL)
    return NULL;
  if (state == IST_COMPLETED)
    {
      if (ist->timer_g_start == -1)
	return NULL;
      if ((now - ist->timer_g_start - 1) * 1000 > ist->timer_g_length)
	{
	  return osip_new_event (TIMEOUT_G, transactionid);
	}
    }
  return NULL;
}

sipevent_t *
ist_need_timer_h_event (ist_t * ist, state_t state, int transactionid)
{
  time_t now = time (NULL);

  if (ist == NULL)
    return NULL;
  if (state == IST_COMPLETED)
    {
      /* may need timer H */
      if (ist->timer_h_start == -1)
	return NULL;
      if ((now - ist->timer_h_start - 1) * 1000 > ist->timer_h_length)
	{
	  return osip_new_event (TIMEOUT_H, transactionid);
	}
    }
  return NULL;
}

sipevent_t *
ist_need_timer_i_event (ist_t * ist, state_t state, int transactionid)
{
  time_t now = time (NULL);

  if (ist == NULL)
    return NULL;
  if (state == IST_CONFIRMED)
    {
      /* may need timer I */
      if (ist->timer_i_start == -1)
	return NULL;
      if ((now - ist->timer_i_start) * 1000 > ist->timer_i_length)
	return osip_new_event (TIMEOUT_I, transactionid);
    }
  return NULL;
}
