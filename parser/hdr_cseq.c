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
cseq_init (cseq_t ** cseq)
{
  *cseq = (cseq_t *) smalloc (sizeof (cseq_t));
  if (*cseq == NULL)
    return -1;
  (*cseq)->method = NULL;
  (*cseq)->number = NULL;
  return 0;
}

/* fills the cseq header of message.               */
/* INPUT :  char *hvalue | value of header.   */
/* OUTPUT: sip_t *sip | structure to save results. */
/* returns -1 on error. */
int
msg_setcseq (sip_t * sip, char *hvalue)
{
  int i;

  if (sip->cseq != NULL)
    return -1;
  i = cseq_init (&(sip->cseq));
  if (i != 0)
    return -1;
#ifdef USE_TMP_BUFFER
  sip->message_property = 2;
#endif
  i = cseq_parse (sip->cseq, hvalue);
  if (i != 0)
    {
      cseq_free (sip->cseq);
      sfree (sip->cseq);
      sip->cseq = NULL;
      return -1;
    }
  return 0;
}

/* fills the cseq strucuture.                      */
/* INPUT : char *hvalue | value of header.         */
/* OUTPUT: sip_t *sip | structure to save results. */
/* returns -1 on error. */
int
cseq_parse (cseq_t * cseq, char *hvalue)
{
  char *method = NULL;
  char *end = NULL;

  cseq->number = NULL;
  cseq->method = NULL;

  method = strchr (hvalue, ' ');	/* SEARCH FOR SPACE */
  end = hvalue + strlen (hvalue);

  if (method == NULL)
    return -1;

  if (method - hvalue + 1 < 2)
    return -1;
  cseq->number = (char *) smalloc (method - hvalue + 1);
  if (cseq->number == NULL)
    return -1;
  sstrncpy (cseq->number, hvalue, method - hvalue);
  sclrspace (cseq->number);

  if (end - method + 1 < 2)
    return -1;
  cseq->method = (char *) smalloc (end - method + 1);
  if (cseq->method == NULL)
    return -1;
  sstrncpy (cseq->method, method + 1, end - method);
  sclrspace (cseq->method);

  return 0;			/* ok */
}

/* returns the cseq header.            */
/* INPUT : sip_t *sip | sip message.   */
/* returns null on error. */
cseq_t *
msg_getcseq (sip_t * sip)
{
  return sip->cseq;
}

char *
cseq_getnumber (cseq_t * cseq)
{
  return cseq->number;
}

char *
cseq_getmethod (cseq_t * cseq)
{
  return cseq->method;
}

void
cseq_setnumber (cseq_t * cseq, char *number)
{
  cseq->number = (char *) number;
}

void
cseq_setmethod (cseq_t * cseq, char *method)
{
  cseq->method = (char *) method;
}

/* returns the cseq header as a string.          */
/* INPUT : cseq_t *cseq | cseq header.  */
/* returns null on error. */
int
cseq_2char (cseq_t * cseq, char **dest)
{
  int len;

  *dest = NULL;
  if ((cseq == NULL) || (cseq->number == NULL) || (cseq->method == NULL))
    return -1;
  len = strlen (cseq->method) + strlen (cseq->number) + 2;
  *dest = (char *) smalloc (len);
  if (*dest == NULL)
    return -1;
  sprintf (*dest, "%s %s", cseq->number, cseq->method);
  return 0;
}

/* deallocates a cseq_t structure.  */
/* INPUT : cseq_t *cseq | cseq. */
void
cseq_free (cseq_t * cseq)
{
  if (cseq == NULL)
    return;
  sfree (cseq->method);
  sfree (cseq->number);
}

int
cseq_clone (cseq_t * cseq, cseq_t ** dest)
{
  int i;
  cseq_t *cs;

  *dest = NULL;
  if (cseq == NULL)
    return -1;
  if (cseq->method == NULL)
    return -1;
  if (cseq->number == NULL)
    return -1;

  i = cseq_init (&cs);
  if (i != 0)
    {
      cseq_free (cs);
      sfree (cs);
      return -1;
    }
  cs->method = sgetcopy (cseq->method);
  cs->number = sgetcopy (cseq->number);

  *dest = cs;
  return 0;
}
