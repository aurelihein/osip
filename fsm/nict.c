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
nict_init (nict_t ** nict, osip_t * osip, sip_t * request)
{
  route_t *route;
  int i;
  time_t now;

  OSIP_TRACE (osip_trace
	      (__FILE__, __LINE__, OSIP_INFO2, NULL,
	       "allocating NICT context\n"));

  *nict = (nict_t *) smalloc (sizeof (nict_t));
  if (*nict == NULL)
    return -1;
  now = time (NULL);
#ifndef DISABLE_MEMSET
  memset (*nict, 0, sizeof (nict_t));
#else
  (*ict)->destination = NULL;
#endif
  /* for REQUEST retransmissions */
  {
    via_t *via;
    char *proto;

    i = msg_getvia (request, 0, &via);	/* get top via */
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
	(*nict)->timer_e_length = DEFAULT_T1;
	(*nict)->timer_e_start = now;	/* started */

	(*nict)->timer_k_length = DEFAULT_T4;
	(*nict)->timer_k_start = -1;	/* not started */
      }
    else
      {				/* TCP is used: */
	(*nict)->timer_e_length = -1;	/* E is not ACTIVE */
	(*nict)->timer_e_start = -1;

	(*nict)->timer_k_length = 0;	/* MUST do the transition immediatly */
	(*nict)->timer_k_start = -1;	/* not started */
      }
  }

  /* for PROXY, the destination MUST be set by the application layer,
     this one may not be correct. */
  msg_getroute (request, 0, &route);
  if (route != NULL)
    {
      int port = 5060;

      if (route->url->port != NULL)
	port = satoi (route->url->port);
      nict_set_destination ((*nict), sgetcopy (route->url->host), port);
    }
  else
    (*nict)->port = 5060;

  (*nict)->timer_f_length = 64 * DEFAULT_T1;
  (*nict)->timer_f_start = now;	/* started */

  /* Oups! a Bug! */
  /*  (*nict)->port  = 5060; */

  return 0;

ii_error_1:
  sfree (*nict);
  return -1;
}

int
nict_free (nict_t * nict)
{
  if (nict == NULL)
    return -1;
  OSIP_TRACE (osip_trace
	      (__FILE__, __LINE__, OSIP_INFO2, NULL,
	       "free nict ressource\n"));

  sfree (nict->destination);
  return 0;
}

int
nict_set_destination (nict_t * nict, char *destination, int port)
{
  if (nict == NULL)
    return -1;
  if (nict->destination != NULL)
    sfree (nict->destination);
  nict->destination = destination;
  nict->port = port;
  return 0;
}

sipevent_t *
nict_need_timer_e_event (nict_t * nict, state_t state, int transactionid)
{
  time_t now = time (NULL);

  if (nict == NULL)
    return NULL;
  if (state == NICT_PROCEEDING || state == NICT_TRYING)
    {
      if (nict->timer_e_start == -1)
	return NULL;
      if ((now - nict->timer_e_start - 1) * 1000 > nict->timer_e_length)
	return osip_new_event (TIMEOUT_E, transactionid);
    }
  return NULL;
}

sipevent_t *
nict_need_timer_f_event (nict_t * nict, state_t state, int transactionid)
{
  time_t now = time (NULL);

  if (nict == NULL)
    return NULL;
  if (state == NICT_PROCEEDING || state == NICT_TRYING)
    {
      if (nict->timer_f_start == -1)
	return NULL;
      if ((now - nict->timer_f_start - 1) * 1000 > nict->timer_f_length)
	return osip_new_event (TIMEOUT_F, transactionid);
    }
  return NULL;
}

sipevent_t *
nict_need_timer_k_event (nict_t * nict, state_t state, int transactionid)
{
  time_t now = time (NULL);

  if (nict == NULL)
    return NULL;
  if (state == NICT_COMPLETED)
    {
      if (nict->timer_k_start == -1)
	return NULL;
      if ((now - nict->timer_k_start - 1) * 1000 > nict->timer_k_length)
	return osip_new_event (TIMEOUT_K, transactionid);
    }
  return NULL;
}
