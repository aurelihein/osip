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
authorization_init (authorization_t ** dest)
{
  *dest = (authorization_t *) smalloc (sizeof (authorization_t));
  if (*dest == NULL)
    return -1;
  (*dest)->auth_type = NULL;
  (*dest)->username = NULL;
  (*dest)->realm = NULL;
  (*dest)->nonce = NULL;
  (*dest)->uri = NULL;
  (*dest)->response = NULL;
  (*dest)->digest = NULL;	/* DO NOT USE IT IN AUTHORIZATION_T HEADER?? */
  (*dest)->algorithm = NULL;	/* optionnal, default is "md5" */
  (*dest)->cnonce = NULL;	/* optionnal */
  (*dest)->opaque = NULL;	/* optionnal */
  (*dest)->message_qop = NULL;	/* optionnal */
  (*dest)->nonce_count = NULL;	/* optionnal */
  (*dest)->auth_param = NULL;	/* for other headers --NOT IMPLEMENTED-- */
  return 0;
}

/* fills the www-authenticate header of message.               */
/* INPUT :  char *hvalue | value of header.   */
/* OUTPUT: sip_t *sip | structure to save results. */
/* returns -1 on error. */
int
msg_setauthorization (sip_t * sip, char *hvalue)
{
  authorization_t *authorization;
  int i;

  if (sip == NULL || sip->authorizations == NULL)
    return -1;
  i = authorization_init (&authorization);
  if (i != 0)
    return -1;
  i = authorization_parse (authorization, hvalue);
  if (i != 0)
    {
      authorization_free (authorization);
      sfree (authorization);
      return -1;
    }
#ifdef USE_TMP_BUFFER
  sip->message_property = 2;
#endif
  list_add (sip->authorizations, authorization, -1);
  return 0;
}

/* fills the www-authenticate structure.           */
/* INPUT : char *hvalue | value of header.         */
/* OUTPUT: sip_t *sip | structure to save results. */
/* returns -1 on error. */
/* TODO:
   digest-challenge tken has no order preference??
   verify many situations (extra SP....)
*/
int
authorization_parse (authorization_t * auth, char *hvalue)
{
  char *space = NULL;
  char *next = NULL;

  space = strchr (hvalue, ' ');	/* SEARCH FOR SPACE */
  if (space == NULL)
    return -1;

  if (space - hvalue < 1)
    return -1;
  auth->auth_type = (char *) smalloc (space - hvalue + 1);
  sstrncpy (auth->auth_type, hvalue, space - hvalue);

  for (;;)
    {
      int parse_ok = 0;

      if (quoted_string_set ("username", space, &(auth->username), &next))
	return -1;
      if (next == NULL)
	return 0;		/* end of header detected! */
      else if (next != space)
	{
	  space = next;
	  parse_ok++;
	}
      if (quoted_string_set ("realm", space, &(auth->realm), &next))
	return -1;
      if (next == NULL)
	return 0;
      else if (next != space)
	{
	  space = next;
	  parse_ok++;
	}
      if (quoted_string_set ("nonce", space, &(auth->nonce), &next))
	return -1;
      if (next == NULL)
	return 0;		/* end of header detected! */
      else if (next != space)
	{
	  space = next;
	  parse_ok++;
	}
      if (quoted_string_set ("uri", space, &(auth->uri), &next))
	return -1;
      if (next == NULL)
	return 0;		/* end of header detected! */
      else if (next != space)
	{
	  space = next;
	  parse_ok++;
	}
      if (quoted_string_set ("response", space, &(auth->response), &next))
	return -1;
      if (next == NULL)
	return 0;		/* end of header detected! */
      else if (next != space)
	{
	  space = next;
	  parse_ok++;
	}
      if (quoted_string_set ("digest", space, &(auth->digest), &next))
	return -1;
      if (next == NULL)
	return 0;		/* end of header detected! */
      else if (next != space)
	{
	  space = next;
	  parse_ok++;
	}
      if (token_set ("algorithm", space, &(auth->algorithm), &next))
	return -1;
      if (next == NULL)
	return 0;		/* end of header detected! */
      else if (next != space)
	{
	  space = next;
	  parse_ok++;
	}
      if (quoted_string_set ("cnonce", space, &(auth->cnonce), &next))
	return -1;
      if (next == NULL)
	return 0;		/* end of header detected! */
      else if (next != space)
	{
	  space = next;
	  parse_ok++;
	}
      if (quoted_string_set ("opaque", space, &(auth->opaque), &next))
	return -1;
      if (next == NULL)
	return 0;		/* end of header detected! */
      else if (next != space)
	{
	  space = next;
	  parse_ok++;
	}
      if (token_set ("qop", space, &(auth->message_qop), &next))
	return -1;
      if (next == NULL)
	return 0;		/* end of header detected! */
      else if (next != space)
	{
	  space = next;
	  parse_ok++;
	}
      if (token_set ("nc", space, &(auth->nonce_count), &next))
	return -1;
      if (next == NULL)
	return 0;		/* end of header detected! */
      else if (next != space)
	{
	  space = next;
	  parse_ok++;
	}
      /* nothing was recognized:
         here, we should handle a list of unknown tokens where:
         token1 = ( token2 | quoted_text ) */
      /* TODO */

      if (0 == parse_ok)
	{
	  char *quote1, *quote2, *tmp;

	  /* CAUTION */
	  /* parameter not understood!!! I'm too lazy to handle IT */
	  /* let's simply bypass it */
	  if (strlen (space) < 1)
	    return 0;
	  tmp = strchr (space + 1, ',');
	  if (tmp == NULL)	/* it was the last header */
	    return 0;
	  quote1 = quote_find (space);
	  if ((quote1 != NULL) && (quote1 < tmp))	/* this may be a quoted string! */
	    {
	      quote2 = quote_find (quote1 + 1);
	      if (quote2 == NULL)
		return -1;	/* bad header format... */
	      if (tmp < quote2)	/* the comma is inside the quotes! */
		space = strchr (quote2, ',');
	      else
		space = tmp;
	      if (space == NULL)	/* it was the last header */
		return 0;
	    }
	  else
	    space = tmp;
	  /* continue parsing... */
	}
    }
  return 0;			/* ok */
}

/* returns the authorization header.   */
/* INPUT : sip_t *sip | sip message.   */
/* returns null on error. */
int
msg_getauthorization (sip_t * sip, int pos, authorization_t ** dest)
{
  authorization_t *authorization;

  *dest = NULL;
  if (list_size (sip->authorizations) <= pos)
    return -1;			/* does not exist */
  authorization = (authorization_t *) list_get (sip->authorizations, pos);
  *dest = authorization;
  return pos;
}

char *
authorization_getauth_type (authorization_t * authorization)
{
  return authorization->auth_type;
}

void
authorization_setauth_type (authorization_t * authorization, char *auth_type)
{
  authorization->auth_type = (char *) auth_type;
}

char *
authorization_getusername (authorization_t * authorization)
{
  return authorization->username;
}

void
authorization_setusername (authorization_t * authorization, char *username)
{
  authorization->username = (char *) username;
}

char *
authorization_getrealm (authorization_t * authorization)
{
  return authorization->realm;
}

void
authorization_setrealm (authorization_t * authorization, char *realm)
{
  authorization->realm = (char *) realm;
}

char *
authorization_getnonce (authorization_t * authorization)
{
  return authorization->nonce;
}

void
authorization_setnonce (authorization_t * authorization, char *nonce)
{
  authorization->nonce = (char *) nonce;
}

char *
authorization_geturi (authorization_t * authorization)
{
  return authorization->uri;
}

void
authorization_seturi (authorization_t * authorization, char *uri)
{
  authorization->uri = (char *) uri;
}

char *
authorization_getresponse (authorization_t * authorization)
{
  return authorization->response;
}

void
authorization_setresponse (authorization_t * authorization, char *response)
{
  authorization->response = (char *) response;
}

char *
authorization_getdigest (authorization_t * authorization)
{
  return authorization->digest;
}

void
authorization_setdigest (authorization_t * authorization, char *digest)
{
  authorization->digest = (char *) digest;
}

char *
authorization_getalgorithm (authorization_t * authorization)
{
  return authorization->algorithm;
}

void
authorization_setalgorithm (authorization_t * authorization, char *algorithm)
{
  authorization->algorithm = (char *) algorithm;
}

char *
authorization_getcnonce (authorization_t * authorization)
{
  return authorization->cnonce;
}

void
authorization_setcnonce (authorization_t * authorization, char *cnonce)
{
  authorization->cnonce = (char *) cnonce;
}

char *
authorization_getopaque (authorization_t * authorization)
{
  return authorization->opaque;
}

void
authorization_setopaque (authorization_t * authorization, char *opaque)
{
  authorization->opaque = (char *) opaque;
}

char *
authorization_getmessage_qop (authorization_t * authorization)
{
  return authorization->message_qop;
}

void
authorization_setmessage_qop (authorization_t * authorization,
			      char *message_qop)
{
  authorization->message_qop = (char *) message_qop;
}

char *
authorization_getnonce_count (authorization_t * authorization)
{
  return authorization->nonce_count;
}

void
authorization_setnonce_count (authorization_t * authorization,
			      char *nonce_count)
{
  authorization->nonce_count = (char *) nonce_count;
}


/* returns the authorization header as a string.          */
/* INPUT : authorization_t *authorization | authorization header.  */
/* returns null on error. */
int
authorization_2char (authorization_t * auth, char **dest)
{
  int len;
  char *tmp;

  *dest = NULL;
  /* DO NOT REALLY KNOW THE LIST OF MANDATORY PARAMETER: Please HELP! */
  if ((auth == NULL) || (auth->auth_type == NULL) || (auth->realm == NULL)
      || (auth->nonce == NULL))
    return -1;

  len = strlen (auth->auth_type) + 1;
  if (auth->username != NULL)
    len = len + 10 + strlen (auth->username);
  if (auth->realm != NULL)
    len = len + 8 + strlen (auth->realm);
  if (auth->nonce != NULL)
    len = len + 8 + strlen (auth->nonce);
  if (auth->uri != NULL)
    len = len + 6 + strlen (auth->uri);
  if (auth->response != NULL)
    len = len + 11 + strlen (auth->response);
  len = len + 2;
  if (auth->digest != NULL)
    len = len + strlen (auth->digest) + 9;
  if (auth->algorithm != NULL)
    len = len + strlen (auth->algorithm) + 12;
  if (auth->cnonce != NULL)
    len = len + strlen (auth->cnonce) + 9;
  if (auth->opaque != NULL)
    len = len + 9 + strlen (auth->opaque);
  if (auth->nonce_count != NULL)
    len = len + strlen (auth->nonce_count) + 5;
  if (auth->message_qop != NULL)
    len = len + strlen (auth->message_qop) + 6;

  tmp = (char *) smalloc (len);
  if (tmp == NULL)
    return -1;
  *dest = tmp;

  sstrncpy (tmp, auth->auth_type, strlen (auth->auth_type));
  tmp = tmp + strlen (tmp);

  if (auth->username != NULL)
    {
      sstrncpy (tmp, " username=", 10);
      tmp = tmp + 10;
      /* !! username-value must be a quoted string !! */
      sstrncpy (tmp, auth->username, strlen (auth->username));
      tmp = tmp + strlen (tmp);
    }

  if (auth->realm != NULL)
    {
      sstrncpy (tmp, ", realm=", 8);
      tmp = tmp + 8;
      /* !! realm-value must be a quoted string !! */
      sstrncpy (tmp, auth->realm, strlen (auth->realm));
      tmp = tmp + strlen (tmp);
    }
  if (auth->nonce != NULL)
    {
      sstrncpy (tmp, ", nonce=", 8);
      tmp = tmp + 8;
      /* !! nonce-value must be a quoted string !! */
      sstrncpy (tmp, auth->nonce, strlen (auth->nonce));
      tmp = tmp + strlen (tmp);
    }

  if (auth->uri != NULL)
    {
      sstrncpy (tmp, ", uri=", 6);
      tmp = tmp + 6;
      /* !! domain-value must be a list of URI in a quoted string !! */
      sstrncpy (tmp, auth->uri, strlen (auth->uri));
      tmp = tmp + strlen (tmp);
    }
  if (auth->response != NULL)
    {
      sstrncpy (tmp, ", response=", 11);
      tmp = tmp + 11;
      /* !! domain-value must be a list of URI in a quoted string !! */
      sstrncpy (tmp, auth->response, strlen (auth->response));
      tmp = tmp + strlen (tmp);
    }

  if (auth->digest != NULL)
    {
      sstrncpy (tmp, ", digest=", 9);
      tmp = tmp + 9;
      /* !! domain-value must be a list of URI in a quoted string !! */
      sstrncpy (tmp, auth->digest, strlen (auth->digest));
      tmp = tmp + strlen (tmp);
    }
  if (auth->algorithm != NULL)
    {
      sstrncpy (tmp, ", algorithm=", 12);
      tmp = tmp + 12;
      sstrncpy (tmp, auth->algorithm, strlen (auth->algorithm));
      tmp = tmp + strlen (tmp);
    }
  if (auth->cnonce != NULL)
    {
      sstrncpy (tmp, ", cnonce=", 9);
      tmp = tmp + 9;
      sstrncpy (tmp, auth->cnonce, strlen (auth->cnonce));
      tmp = tmp + strlen (tmp);
    }
  if (auth->opaque != NULL)
    {
      sstrncpy (tmp, ", opaque=", 9);
      tmp = tmp + 9;
      sstrncpy (tmp, auth->opaque, strlen (auth->opaque));
      tmp = tmp + strlen (tmp);
    }
  if (auth->message_qop != NULL)
    {
      sstrncpy (tmp, ", qop=", 6);
      tmp = tmp + 6;
      sstrncpy (tmp, auth->message_qop, strlen (auth->message_qop));
      tmp = tmp + strlen (tmp);
    }
  if (auth->nonce_count != NULL)
    {
      sstrncpy (tmp, ", nc=", 5);
      tmp = tmp + 5;
      sstrncpy (tmp, auth->nonce_count, strlen (auth->nonce_count));
      tmp = tmp + strlen (tmp);
    }
  return 0;
}

/* deallocates a authorization_t structure.  */
/* INPUT : authorization_t *authorization | authorization. */
void
authorization_free (authorization_t * authorization)
{
  if (authorization == NULL)
    return;
  sfree (authorization->auth_type);
  sfree (authorization->username);
  sfree (authorization->realm);
  sfree (authorization->nonce);
  sfree (authorization->uri);
  sfree (authorization->response);
  sfree (authorization->digest);
  sfree (authorization->algorithm);
  sfree (authorization->cnonce);
  sfree (authorization->opaque);
  sfree (authorization->message_qop);
  sfree (authorization->nonce_count);
}

int
authorization_clone (authorization_t * auth, authorization_t ** dest)
{
  int i;
  authorization_t *au;

  *dest = NULL;
  if (auth == NULL)
    return -1;
  /* to be removed?
     if (auth->auth_type==NULL) return -1;
     if (auth->username==NULL) return -1;
     if (auth->realm==NULL) return -1;
     if (auth->nonce==NULL) return -1;
     if (auth->uri==NULL) return -1;
     if (auth->response==NULL) return -1;
     if (auth->opaque==NULL) return -1;
   */

  i = authorization_init (&au);
  if (i == -1)			/* allocation failed */
    return -1;
  if (auth->auth_type != NULL)
    {
      au->auth_type = sgetcopy (auth->auth_type);
      if (au->auth_type == NULL)
	goto ac_error;
    }
  if (auth->username != NULL)
    {
      au->username = sgetcopy (auth->username);
      if (au->username == NULL)
	goto ac_error;
    }
  if (auth->realm != NULL)
    {
      au->realm = sgetcopy (auth->realm);
      if (auth->realm == NULL)
	goto ac_error;
    }
  if (auth->nonce != NULL)
    {
      au->nonce = sgetcopy (auth->nonce);
      if (auth->nonce == NULL)
	goto ac_error;
    }
  if (auth->uri != NULL)
    {
      au->uri = sgetcopy (auth->uri);
      if (au->uri == NULL)
	goto ac_error;
    }
  if (auth->response != NULL)
    {
      au->response = sgetcopy (auth->response);
      if (auth->response == NULL)
	goto ac_error;
    }
  if (auth->digest != NULL)
    {
      au->digest = sgetcopy (auth->digest);
      if (au->digest == NULL)
	goto ac_error;
    }
  if (auth->algorithm != NULL)
    {
      au->algorithm = sgetcopy (auth->algorithm);
      if (auth->algorithm == NULL)
	goto ac_error;
    }
  if (auth->cnonce != NULL)
    {
      au->cnonce = sgetcopy (auth->cnonce);
      if (au->cnonce == NULL)
	goto ac_error;
    }
  if (auth->opaque != NULL)
    {
      au->opaque = sgetcopy (auth->opaque);
      if (auth->opaque == NULL)
	goto ac_error;
    }
  if (auth->message_qop != NULL)
    {
      au->message_qop = sgetcopy (auth->message_qop);
      if (auth->message_qop == NULL)
	goto ac_error;
    }
  if (auth->nonce_count != NULL)
    {
      au->nonce_count = sgetcopy (auth->nonce_count);
      if (auth->nonce_count == NULL)
	goto ac_error;
    }

  *dest = au;
  return 0;

ac_error:
  authorization_free (au);
  sfree (au);
  return -1;
}
