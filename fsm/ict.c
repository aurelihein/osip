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
ict_init (ict_t ** ict, osip_t * osip, sip_t * invite)
{
  route_t *route;
  int i;
  time_t now;

  OSIP_TRACE (osip_trace
	      (__FILE__, __LINE__, OSIP_INFO2, NULL,
	       "allocating ICT context\n"));

  *ict = (ict_t *) smalloc (sizeof (ict_t));
  if (*ict == NULL)
    return -1;

  now = time (NULL);
#ifndef DISABLE_MEMSET
  memset (*ict, 0, sizeof (ict_t));
#else
  (*ict)->destination = NULL;
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
	(*ict)->timer_a_length = DEFAULT_T1;
	(*ict)->timer_a_start = now;
	if (64 * DEFAULT_T1 < 32000)
	  (*ict)->timer_d_length = 32000;
	else
	  (*ict)->timer_d_length = 64 * DEFAULT_T1;
	(*ict)->timer_d_start = -1;	/* not started */
      }
    else
      {				/* TCP is used: */
	(*ict)->timer_a_length = -1;	/* A is not ACTIVE */
	(*ict)->timer_a_start = -1;	/* not started */

	(*ict)->timer_d_length = 0;	/* MUST do the transition immediatly */
	(*ict)->timer_d_start = -1;	/* not started */
      }
  }

  /* for PROXY, the destination MUST be set by the application layer,
     this one may not be correct. */
  msg_getroute (invite, 0, &route);
  if (route != NULL)
    {
      int port = 5060;

      if (route->url->port != NULL)
	port = satoi (route->url->port);
      ict_set_destination ((*ict), sgetcopy (route->url->host), port);
    }
  else
    (*ict)->port = 5060;

  (*ict)->timer_b_length = 64 * DEFAULT_T1;
  (*ict)->timer_b_start = now;	/* started */

  /* Oups! A bug! */
  /*  (*ict)->port  = 5060; */

  return 0;

ii_error_1:
  sfree (*ict);
  return -1;
}

int
ict_free (ict_t * ict)
{
  if (ict == NULL)
    return -1;
  OSIP_TRACE (osip_trace
	      (__FILE__, __LINE__, OSIP_INFO2, NULL, "free ict ressource\n"));

  sfree (ict->destination);
  return 0;
}

int
ict_set_destination (ict_t * ict, char *destination, int port)
{
  if (ict == NULL)
    return -1;
  if (ict->destination != NULL)
    sfree (ict->destination);
  ict->destination = destination;
  ict->port = port;
  return 0;
}

sipevent_t *
ict_need_timer_a_event (ict_t * ict, state_t state, int transactionid)
{
  time_t now = time (NULL);

  if (ict == NULL)
    return NULL;
  if (state == ICT_CALLING)
    {
      /* may need timer A */
      if (ict->timer_a_start == -1)
	return NULL;
      if ((now - ict->timer_a_start - 1) * 1000 > ict->timer_a_length)
	{
	  return osip_new_event (TIMEOUT_A, transactionid);
	}
    }
  return NULL;
}

sipevent_t *
ict_need_timer_b_event (ict_t * ict, state_t state, int transactionid)
{
  time_t now = time (NULL);

  if (ict == NULL)
    return NULL;
  if (state == ICT_CALLING)
    {
      /* may need timer B */
      if (ict->timer_b_start == -1)
	return NULL;
      if ((now - ict->timer_b_start) * 1000 > ict->timer_b_length)
	return osip_new_event (TIMEOUT_B, transactionid);
    }
  return NULL;
}

sipevent_t *
ict_need_timer_d_event (ict_t * ict, state_t state, int transactionid)
{
  time_t now = time (NULL);

  if (ict == NULL)
    return NULL;
  if (state == ICT_COMPLETED)
    {
      /* may need timer D */
      if (ict->timer_d_start == -1)
	return NULL;
      if ((now - ict->timer_d_start) * 1000 > ict->timer_d_length)
	return osip_new_event (TIMEOUT_D, transactionid);
    }
  return NULL;
}
