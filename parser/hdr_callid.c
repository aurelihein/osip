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

/* fills the call_id of message.                    */
/* INPUT : const char *hvalue | value of header.    */
/* OUTPUT: sip_t *sip | structure to save results.  */
/* returns -1 on error. */
int
msg_setcall_id (sip_t * sip, char *hvalue)
{
  int i;

  if (sip->call_id != NULL)
    return -1;
  i = call_id_init (&(sip->call_id));
  if (i != 0)
    return -1;
#ifdef USE_TMP_BUFFER
  sip->message_property = 2;
#endif
  i = call_id_parse (sip->call_id, hvalue);
  if (i != 0)
    {
      call_id_free (sip->call_id);
      sfree (sip->call_id);
      sip->call_id = NULL;
      return -1;
    }
  return 0;
}

/* returns the call_id.               */
/* INPUT : sip_t *sip | sip message.  */
call_id_t *
msg_getcall_id (sip_t * sip)
{
  return sip->call_id;
}

int
call_id_init (call_id_t ** callid)
{
  *callid = (call_id_t *) smalloc (sizeof (call_id_t));
  if (*callid == NULL)
    return -1;
  (*callid)->number = NULL;
  (*callid)->host = NULL;
  return 0;
}


/* deallocates a call_id_t structure. */
/* INPUT : call_id_t *| callid.       */
void
call_id_free (call_id_t * callid)
{
  if (callid == NULL)
    return;
  sfree (callid->number);
  sfree (callid->host);

  callid->number = NULL;
  callid->host = NULL;
}

/* fills the call_id structure.                           */
/* INPUT : char *hvalue | value of header.                */
/* OUTPUT: call_id_t *callid | structure to save results.  */
/* returns -1 on error. */
int
call_id_parse (call_id_t * callid, char *hvalue)
{
  char *host = 0;
  char *end = 0;

  callid->number = NULL;
  callid->host = NULL;

  host = strchr (hvalue, '@');	/* SEARCH FOR '@' */
  end = hvalue + strlen (hvalue);

  if (host == NULL)
    host = end;
  else
    {
      if (end - host + 1 < 2)
	return -1;
      callid->host = (char *) smalloc (end - host);
      if (callid->host == NULL)
	return -1;
      sstrncpy (callid->host, host + 1, end - host - 1);
      sclrspace (callid->host);
    }
  if (host - hvalue + 1 < 2)
    return -1;
  callid->number = (char *) smalloc (host - hvalue + 1);
  if (callid->number == NULL)
    return -1;
  sstrncpy (callid->number, hvalue, host - hvalue);
  sclrspace (callid->number);

  return 0;			/* ok */
}

/* returns the call_id as a string.          */
/* INPUT : call_id_t *call_id | call_id.  */
/* returns null on error. */
int
call_id_2char (call_id_t * callid, char **dest)
{
  *dest = NULL;
  if ((callid == NULL) || (callid->number == NULL))
    return -1;

  if (callid->host == NULL)
    {
      *dest = (char *) smalloc (strlen (callid->number) + 1);
      if (*dest == NULL)
	return -1;
      sprintf (*dest, "%s", callid->number);
    }
  else
    {
      *dest =
	(char *) smalloc (strlen (callid->number) + strlen (callid->host) +
			  2);
      if (*dest == NULL)
	return -1;
      sprintf (*dest, "%s@%s", callid->number, callid->host);
    }
  return 0;
}

char *
call_id_getnumber (call_id_t * callid)
{
  if (callid == NULL)
    return NULL;
  return callid->number;
}

char *
call_id_gethost (call_id_t * callid)
{
  if (callid == NULL)
    return NULL;
  return callid->host;
}

void
call_id_setnumber (call_id_t * callid, char *number)
{
  callid->number = number;
}

void
call_id_sethost (call_id_t * callid, char *host)
{
  callid->host = host;
}

int
call_id_clone (call_id_t * callid, call_id_t ** dest)
{
  int i;
  call_id_t *ci;

  *dest = NULL;
  if (callid == NULL)
    return -1;
  if (callid->number == NULL)
    return -1;

  i = call_id_init (&ci);
  if (i == -1)			/* allocation failed */
    return -1;
  ci->number = sgetcopy (callid->number);
  if (callid->host != NULL)
    ci->host = sgetcopy (callid->host);

  *dest = ci;
  return 0;
}
