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

/* adds the contact header to message.              */
/* INPUT : const char *hvalue | value of header.    */
/* OUTPUT: sip_t *sip | structure to save results.  */
/* returns -1 on error. */
int
msg_setcontact (sip_t * sip, char *hvalue)
{
  int i;
  contact_t *contact;

  i = contact_init (&contact);
  if (i != 0)
    return -1;
  i = contact_parse (contact, hvalue);
  if (i != 0)
    {
      contact_free (contact);
      sfree (contact);
      return -1;
    }
#ifdef USE_TMP_BUFFER
  sip->message_property = 2;
#endif
  list_add (sip->contacts, contact, -1);
  return 0;			/* ok */
}

/* parses a contact header.                                 */
/* INPUT : const char *string | pointer to a contact string.*/
/* OUTPUT: contact_t *contact | structure to save results.  */
/* returns -1 on error. */
int
contact_parse (contact_t * contact, char *hvalue)
{
  if (contact == NULL)
    return -1;
  if (strncmp (hvalue, "*", 1) == 0)
    {
      contact->displayname = sgetcopy (hvalue);
      return 0;
    }
  return from_parse ((from_t *) contact, hvalue);
}

int
contact_init (contact_t ** contact)
{
  return from_init ((from_t **) contact);
}

/* returns the pos of contact header.                      */
/* INPUT : int pos | pos of contact header.                */
/* INPUT : sip_t *sip | sip message.                       */
/* OUTPUT: contact_t *contact | structure to save results. */
/* returns -1 on error. */
int
msg_getcontact (sip_t * sip, int pos, contact_t ** dest)
{
  *dest = NULL;
  if (sip == NULL)
    return -1;
  if (list_size (sip->contacts) <= pos)
    return -1;			/* does not exist */
  *dest = (contact_t *) list_get (sip->contacts, pos);
  return pos;
}

/* returns the contact header as a string.*/
/* INPUT : contact_t *contact | contact.  */
/* returns null on error. */
int
contact_2char (contact_t * contact, char **dest)
{
  if (contact == NULL)
    return -1;
  if (contact->displayname != NULL)
    {
      if (strncmp (contact->displayname, "*", 1) == 0)
	{
	  *dest = sgetcopy ("*");
	  return 0;
	}
    }
  return from_2char ((from_t *) contact, dest);
}

/* deallocates a contact_t structure.  */
/* INPUT : contact_t *| contact. */
void
contact_free (contact_t * contact)
{
  from_free ((from_t *) contact);
}

int
contact_clone (contact_t * contact, contact_t ** dest)
{
  return from_clone ((from_t *) contact, (from_t **) dest);
}
