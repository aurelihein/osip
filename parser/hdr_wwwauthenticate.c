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
www_authenticate_init (www_authenticate_t ** dest)
{
  *dest = (www_authenticate_t *) smalloc (sizeof (www_authenticate_t));
  if (*dest == NULL)
    return -1;
  (*dest)->auth_type = NULL;
  (*dest)->realm = NULL;
  (*dest)->domain = NULL;	/* optionnal */
  (*dest)->nonce = NULL;
  (*dest)->opaque = NULL;	/* optionnal */
  (*dest)->stale = NULL;	/* optionnal */
  (*dest)->algorithm = NULL;	/* optionnal */
  (*dest)->qop_options = NULL;	/* optionnal (contains a list of qop-value) */
  return 0;
}

/* fills the www-authenticate header of message.               */
/* INPUT :  char *hvalue | value of header.   */
/* OUTPUT: sip_t *sip | structure to save results. */
/* returns -1 on error. */
int
msg_setwww_authenticate (sip_t * sip, char *hvalue)
{
  www_authenticate_t *www_authenticate;
  int i;

  if (sip == NULL || sip->www_authenticates == NULL)
    return -1;
  i = www_authenticate_init (&www_authenticate);
  if (i != 0)
    return -1;
  i = www_authenticate_parse (www_authenticate, hvalue);
  if (i != 0)
    {
      www_authenticate_free (www_authenticate);
      sfree (www_authenticate);
      return -1;
    }
#ifdef USE_TMP_BUFFER
  sip->message_property = 2;
#endif
  list_add (sip->www_authenticates, www_authenticate, -1);
  return 0;
}

int
quoted_string_set (char *name, char *str, char **result, char **next)
{
  *next = str;
  if (*result != NULL)
    return 0;			/* already parsed */
  *next = NULL;
  while ((' ' == *str) || ('\t' == *str) || (',' == *str))
    if (*str)
      str++;
    else
      return -1;		/* bad header format */

  if (strlen (str) <= strlen (name))
    return -1;			/* bad header format... */
#if (!defined WIN32 && !defined _WIN32_WCE)
  if (strncasecmp (name, str, strlen (name)) == 0)
#else
  if (_strnicmp (name, str, strlen (name)) == 0)
#endif
    {
      char *quote1;
      char *quote2;
      char *tmp;
      char *hack = strchr (str, '=');

      while (' ' == *(hack - 1))	/* get rid of extra spaces */
	hack--;
      if ((size_t) (hack - str) != strlen (name))
	{
	  *next = str;
	  return 0;
	}

      quote1 = quote_find (str);
      if (quote1 == NULL)
	return -1;		/* bad header format... */
      quote2 = quote_find (quote1 + 1);
      if (quote2 == NULL)
	return -1;		/* bad header format... */
      if (quote2 - quote1 == 1)
	{
	  /* this is a special case! The quote contains nothing! */
	  /* example:   Digest opaque="",cnonce=""               */
	  /* in this case, we just forget the parameter... this  */
	  /* this should prevent from user manipulating empty    */
	  /* strings */
	  tmp = quote2 + 1;	/* next element start here */
	  for (; *tmp == ' ' || *tmp == '\t'; tmp++)
	    {
	    }
	  for (; *tmp == '\n' || *tmp == '\r'; tmp++)
	    {
	    }			/* skip LWS */
	  *next = NULL;
	  if (*tmp == '\0')	/* end of header detected */
	    return 0;
	  if (*tmp != '\t' && *tmp != ' ')
	    /* LWS here ? */
	    *next = tmp;
	  else
	    {			/* it is: skip it... */
	      for (; *tmp == ' ' || *tmp == '\t'; tmp++)
		{
		}
	      if (*tmp == '\0')	/* end of header detected */
		return 0;
	      *next = tmp;
	    }
	  return 0;
	}
      *result = (char *) smalloc (quote2 - quote1 + 3);
      if (*result == NULL)
	return -1;
      sstrncpy (*result, quote1, quote2 - quote1 + 1);
      tmp = quote2 + 1;		/* next element start here */
      for (; *tmp == ' ' || *tmp == '\t'; tmp++)
	{
	}
      for (; *tmp == '\n' || *tmp == '\r'; tmp++)
	{
	}			/* skip LWS */
      *next = NULL;
      if (*tmp == '\0')		/* end of header detected */
	return 0;
      if (*tmp != '\t' && *tmp != ' ')
	/* LWS here ? */
	*next = tmp;
      else
	{			/* it is: skip it... */
	  for (; *tmp == ' ' || *tmp == '\t'; tmp++)
	    {
	    }
	  if (*tmp == '\0')	/* end of header detected */
	    return 0;
	  *next = tmp;
	}
    }
  else
    *next = str;		/* wrong header asked! */
  return 0;
}

int
token_set (char *name, char *str, char **result, char **next)
{
  char *beg;
  char *tmp;

  *next = str;
  if (*result != NULL)
    return 0;			/* already parsed */
  *next = NULL;

  beg = strchr (str, '=');
  if (beg == NULL)
    return -1;			/* bad header format... */

  if (strlen (str) < 6)
    return 0;			/* end of header... */

  while ((' ' == *str) || ('\t' == *str) || (',' == *str))
    if (*str)
      str++;
    else
      return -1;		/* bad header format */

#if (!defined WIN32 && !defined _WIN32_WCE)
  if (strncasecmp (name, str, strlen (name)) == 0)
#else
  if (_strnicmp (name, str, strlen (name)) == 0)
#endif
    {
      char *end;

      end = strchr (str, ',');
      if (end == NULL)
	end = str + strlen (str);	/* This is the end of the header */

      if (end - beg < 2)
	return -1;
      *result = (char *) smalloc (end - beg);
      if (*result == NULL)
	return -1;
      sstrncpy (*result, beg + 1, end - beg - 1);
      sclrspace (*result);

      /* make sure the element does not contain more parameter */
      tmp = (*end) ? (end + 1) : end;    //end + 1;
      for (; *tmp == ' ' || *tmp == '\t'; tmp++)
	{
	}
      for (; *tmp == '\n' || *tmp == '\r'; tmp++)
	{
	}			/* skip LWS */
      *next = NULL;
      if (*tmp == '\0')		/* end of header detected */
	return 0;
      if (*tmp != '\t' && *tmp != ' ')
	/* LWS here ? */
	*next = tmp;
      else
	{			/* it is: skip it... */
	  for (; *tmp == ' ' || *tmp == '\t'; tmp++)
	    {
	    }
	  if (*tmp == '\0')	/* end of header detected */
	    return 0;
	  *next = tmp;
	}
    }
  else
    *next = str;		/* next element start here */
  return 0;
}

/* fills the www-authenticate strucuture.                      */
/* INPUT : char *hvalue | value of header.         */
/* OUTPUT: sip_t *sip | structure to save results. */
/* returns -1 on error. */
/* TODO:
   digest-challenge tken has no order preference??
   verify many situations (extra SP....)
*/
int
www_authenticate_parse (www_authenticate_t * wwwa, char *hvalue)
{
  char *space = NULL;
  char *next = NULL;

  space = strchr (hvalue, ' ');	/* SEARCH FOR SPACE */
  if (space == NULL)
    return -1;

  if (space - hvalue + 1 < 2)
    return -1;
  wwwa->auth_type = (char *) smalloc (space - hvalue + 1);
  if (wwwa->auth_type == NULL)
    return -1;
  sstrncpy (wwwa->auth_type, hvalue, space - hvalue);

  for (;;)
    {
      int parse_ok = 0;

      if (quoted_string_set ("realm", space, &(wwwa->realm), &next))
	return -1;
      if (next == NULL)
	return 0;		/* end of header detected! */
      else if (next != space)
	{
	  space = next;
	  parse_ok++;
	}
      if (quoted_string_set ("domain", space, &(wwwa->domain), &next))
	return -1;
      if (next == NULL)
	return 0;		/* end of header detected! */
      else if (next != space)
	{
	  space = next;
	  parse_ok++;
	}
      if (quoted_string_set ("nonce", space, &(wwwa->nonce), &next))
	return -1;
      if (next == NULL)
	return 0;		/* end of header detected! */
      else if (next != space)
	{
	  space = next;
	  parse_ok++;
	}
      if (quoted_string_set ("opaque", space, &(wwwa->opaque), &next))
	return -1;
      if (next == NULL)
	return 0;		/* end of header detected! */
      else if (next != space)
	{
	  space = next;
	  parse_ok++;
	}
      if (token_set ("stale", space, &(wwwa->stale), &next))
	return -1;
      if (next == NULL)
	return 0;		/* end of header detected! */
      else if (next != space)
	{
	  space = next;
	  parse_ok++;
	}
      if (token_set ("algorithm", space, &(wwwa->algorithm), &next))
	return -1;
      if (next == NULL)
	return 0;		/* end of header detected! */
      else if (next != space)
	{
	  space = next;
	  parse_ok++;
	}
      if (quoted_string_set ("qop", space, &(wwwa->qop_options), &next))
	return -1;
      if (next == NULL)
	return 0;		/* end of header detected! */
      else if (next != space)
	{
	  space = next;
	  parse_ok++;
	}
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

/* returns the www_authenticate header.            */
/* INPUT : sip_t *sip | sip message.   */
/* returns null on error. */
int
msg_getwww_authenticate (sip_t * sip, int pos, www_authenticate_t ** dest)
{
  www_authenticate_t *www_authenticate;

  *dest = NULL;
  if (list_size (sip->www_authenticates) <= pos)
    return -1;			/* does not exist */

  www_authenticate =
    (www_authenticate_t *) list_get (sip->www_authenticates, pos);

  *dest = www_authenticate;
  return pos;
}

char *
www_authenticate_getauth_type (www_authenticate_t * www_authenticate)
{
  return www_authenticate->auth_type;
}

void
www_authenticate_setauth_type (www_authenticate_t * www_authenticate,
			       char *auth_type)
{
  www_authenticate->auth_type = (char *) auth_type;
}

char *
www_authenticate_getrealm (www_authenticate_t * www_authenticate)
{
  return www_authenticate->realm;
}

void
www_authenticate_setrealm (www_authenticate_t * www_authenticate, char *realm)
{
  www_authenticate->realm = (char *) realm;
}

char *
www_authenticate_getdomain (www_authenticate_t * www_authenticate)
{
  return www_authenticate->domain;
}

void
www_authenticate_setdomain (www_authenticate_t * www_authenticate,
			    char *domain)
{
  www_authenticate->domain = (char *) domain;
}

char *
www_authenticate_getnonce (www_authenticate_t * www_authenticate)
{
  return www_authenticate->nonce;
}

void
www_authenticate_setnonce (www_authenticate_t * www_authenticate, char *nonce)
{
  www_authenticate->nonce = (char *) nonce;
}

char *
www_authenticate_getstale (www_authenticate_t * www_authenticate)
{
  return www_authenticate->stale;
}

void
www_authenticate_setstale (www_authenticate_t * www_authenticate, char *stale)
{
  www_authenticate->stale = (char *) stale;
}

char *
www_authenticate_getopaque (www_authenticate_t * www_authenticate)
{
  return www_authenticate->opaque;
}

void
www_authenticate_setopaque (www_authenticate_t * www_authenticate,
			    char *opaque)
{
  www_authenticate->opaque = (char *) opaque;
}

char *
www_authenticate_getalgorithm (www_authenticate_t * www_authenticate)
{
  return www_authenticate->algorithm;
}

void
www_authenticate_setalgorithm (www_authenticate_t * www_authenticate,
			       char *algorithm)
{
  www_authenticate->algorithm = (char *) algorithm;
}

char *
www_authenticate_getqop_options (www_authenticate_t * www_authenticate)
{
  return www_authenticate->qop_options;
}

void
www_authenticate_setqop_options (www_authenticate_t * www_authenticate,
				 char *qop_options)
{
  www_authenticate->qop_options = (char *) qop_options;
}



/* returns the www_authenticate header as a string.          */
/* INPUT : www_authenticate_t *www_authenticate | www_authenticate header.  */
/* returns null on error. */
int
www_authenticate_2char (www_authenticate_t * wwwa, char **dest)
{
  int len;
  char *tmp;

  *dest = NULL;
  if ((wwwa == NULL) || (wwwa->auth_type == NULL) || (wwwa->realm == NULL)
      || (wwwa->nonce == NULL))
    return -1;

  len = strlen (wwwa->auth_type) + 1;

  if (wwwa->realm != NULL)
    len = len + strlen (wwwa->realm) + 7;
  if (wwwa->nonce != NULL)
    len = len + strlen (wwwa->nonce) + 8;
  len = len + 2;
  if (wwwa->domain != NULL)
    len = len + strlen (wwwa->domain) + 9;
  if (wwwa->opaque != NULL)
    len = len + strlen (wwwa->opaque) + 9;
  if (wwwa->stale != NULL)
    len = len + strlen (wwwa->stale) + 8;
  if (wwwa->algorithm != NULL)
    len = len + strlen (wwwa->algorithm) + 12;
  if (wwwa->qop_options != NULL)
    len = len + strlen (wwwa->qop_options) + 6;

  tmp = (char *) smalloc (len);
  if (tmp == NULL)
    return -1;
  *dest = tmp;

  sstrncpy (tmp, wwwa->auth_type, strlen (wwwa->auth_type));
  tmp = tmp + strlen (tmp);

  if (wwwa->realm != NULL)
    {
      sstrncpy (tmp, " realm=", 7);
      tmp = tmp + 7;
      sstrncpy (tmp, wwwa->realm, strlen (wwwa->realm));
      tmp = tmp + strlen (tmp);
    }
  if (wwwa->domain != NULL)
    {
      sstrncpy (tmp, ", domain=", 9);
      tmp = tmp + 9;
      sstrncpy (tmp, wwwa->domain, strlen (wwwa->domain));
      tmp = tmp + strlen (tmp);
    }
  if (wwwa->nonce != NULL)
    {
      sstrncpy (tmp, ", nonce=", 8);
      tmp = tmp + 8;
      sstrncpy (tmp, wwwa->nonce, strlen (wwwa->nonce));
      tmp = tmp + strlen (tmp);
    }
  if (wwwa->opaque != NULL)
    {
      sstrncpy (tmp, ", opaque=", 9);
      tmp = tmp + 9;
      sstrncpy (tmp, wwwa->opaque, strlen (wwwa->opaque));
      tmp = tmp + strlen (tmp);
    }
  if (wwwa->stale != NULL)
    {
      sstrncpy (tmp, ", stale=", 8);
      tmp = tmp + 8;
      sstrncpy (tmp, wwwa->stale, strlen (wwwa->stale));
      tmp = tmp + strlen (tmp);
    }
  if (wwwa->algorithm != NULL)
    {
      sstrncpy (tmp, ", algorithm=", 12);
      tmp = tmp + 12;
      sstrncpy (tmp, wwwa->algorithm, strlen (wwwa->algorithm));
      tmp = tmp + strlen (tmp);
    }
  if (wwwa->qop_options != NULL)
    {
      sstrncpy (tmp, ", qop=", 6);
      tmp = tmp + 6;
      sstrncpy (tmp, wwwa->qop_options, strlen (wwwa->qop_options));
      tmp = tmp + strlen (tmp);
    }

  return 0;
}

/* deallocates a www_authenticate_t structure.  */
/* INPUT : www_authenticate_t *www_authenticate | www_authenticate. */
void
www_authenticate_free (www_authenticate_t * www_authenticate)
{
  if (www_authenticate == NULL)
    return;
  if (www_authenticate->auth_type != NULL)
    sfree (www_authenticate->auth_type);
  if (www_authenticate->realm != NULL)
    sfree (www_authenticate->realm);
  if (www_authenticate->domain != NULL)
    sfree (www_authenticate->domain);
  sfree (www_authenticate->nonce);
  if (www_authenticate->opaque != NULL)
    sfree (www_authenticate->opaque);
  if (www_authenticate->stale != NULL)
    sfree (www_authenticate->stale);
  if (www_authenticate->algorithm != NULL)
    sfree (www_authenticate->algorithm);
  if (www_authenticate->qop_options != NULL)
    sfree (www_authenticate->qop_options);
}

int
www_authenticate_clone (www_authenticate_t * wwwa, www_authenticate_t ** dest)
{
  int i;
  www_authenticate_t *wa;

  *dest = NULL;
  if (wwwa == NULL)
    return -1;
  if (wwwa->auth_type == NULL)
    return -1;
  if (wwwa->realm == NULL)
    return -1;
  if (wwwa->nonce == NULL)
    return -1;

  i = www_authenticate_init (&wa);
  if (i == -1)			/* allocation failed */
    return -1;
  wa->auth_type = sgetcopy (wwwa->auth_type);
  wa->realm = sgetcopy (wwwa->realm);
  if (wwwa->domain != NULL)
    wa->domain = sgetcopy (wwwa->domain);
  wa->nonce = sgetcopy (wwwa->nonce);
  if (wwwa->opaque != NULL)
    wa->opaque = sgetcopy (wwwa->opaque);
  if (wwwa->stale != NULL)
    wa->stale = sgetcopy (wwwa->stale);
  if (wwwa->algorithm != NULL)
    wa->algorithm = sgetcopy (wwwa->algorithm);
  if (wwwa->qop_options != NULL)
    wa->qop_options = sgetcopy (wwwa->qop_options);

  *dest = wa;
  return 0;
}
