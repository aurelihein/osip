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

#include <stdio.h>
#include <stdlib.h>

#include <osip/smsg.h>
#include <osip/port.h>
#include "msg.h"



static int
startline_parsereq (startline_t * dest, char *buf, char **headers)
{
  char *p1;
  char *p2;
  char *requesturi;
  int i;

  dest->sipmethod = NULL;
  dest->statuscode = NULL;
  dest->reasonphrase = NULL;

  /* The first token is the method name: */
  p2 = strchr (buf, ' ');
  if (p2 == NULL)
    return -1;
  if (p2-buf==0)
    {
      OSIP_TRACE (osip_trace
		  (__FILE__, __LINE__, OSIP_ERROR, NULL,
		   "No space allowed here\n"));
      return -1;
    }
  dest->sipmethod = (char *) smalloc (p2 - buf + 1);
  sstrncpy (dest->sipmethod, buf, p2 - buf);

  /* The second token is a sip-url or a uri: */
  p1 = strchr (p2 + 2, ' ');	/* no space allowed inside sip-url */
  if (p1 - p2 < 2)
    return -1;
  requesturi = (char *) smalloc (p1 - p2);
  sstrncpy (requesturi, p2 + 1, (p1 - p2 - 1));
  sclrspace (requesturi);

  url_init (&(dest->rquri));
  i = url_parse (dest->rquri, requesturi);
  sfree (requesturi);
  if (i == -1)
    return -1;

  /* find the the version and the beginning of headers */
  {
    char *hp = p1;

    while ((*hp != '\r') && (*hp != '\n'))
      {
	if (*hp)
	  hp++;
	else
	  {
	    OSIP_TRACE (osip_trace
			(__FILE__, __LINE__, OSIP_ERROR, NULL,
			 "No crlf found\n"));
	    return -1;
	  }
      }
    if (hp - p1 < 2)
      return -1;
    dest->sipversion = (char *) smalloc (hp - p1);
    sstrncpy (dest->sipversion, p1 + 1, (hp - p1 - 1));

    hp++;
    if ((*hp) && ('\r' == hp[-1]) && ('\n' == hp[0]))
      hp++;
    (*headers) = hp;
  }
  return 0;
}

static int
startline_parseresp (startline_t * dest, char *buf, char **headers)
{
  char *statuscode;
  char *reasonphrase;

  dest->rquri = NULL;
  dest->sipmethod = NULL;

  *headers = buf;

  statuscode = strchr (buf, ' ');	/* search for first SPACE */
  if (statuscode == NULL)
    return -1;
  dest->sipversion = (char *) smalloc (statuscode - (*headers) + 1);
  sstrncpy (dest->sipversion, *headers, statuscode - (*headers));

  reasonphrase = strchr (statuscode + 1, ' ');
  dest->statuscode = (char *) smalloc (reasonphrase - statuscode);
  sstrncpy (dest->statuscode, statuscode + 1, reasonphrase - statuscode - 1);

  {
    char *hp = reasonphrase;

    while ((*hp != '\r') && (*hp != '\n'))
      {
	if (*hp)
	  hp++;
	else
	  {
	    OSIP_TRACE (osip_trace
			(__FILE__, __LINE__, OSIP_ERROR, NULL,
			 "No crlf found\n"));
	    return -1;
	  }
      }
    dest->reasonphrase = (char *) smalloc (hp - reasonphrase);
    sstrncpy (dest->reasonphrase, reasonphrase + 1, hp - reasonphrase - 1);

    hp++;
    if ((*hp) && ('\r' == hp[-1]) && ('\n' == hp[0]))
      hp++;
    (*headers) = hp;
  }
  return 0;
}

/* return size of request URI */
static int
msg_startline_parse (startline_t * dest, char *buf, char **headers)
{

  if (0 == strncmp ((const char *) buf, (const char *) "SIP/", 4))
    return startline_parseresp (dest, buf, headers);
  else
    return startline_parsereq (dest, buf, headers);
}

int
find_next_occurence (char *str, char *buf, char **index_of_str)
{
  *index_of_str = NULL;		/* AMD fix */
  if ((NULL == str) || (NULL == buf))
    return -1;
  /* TODO? we may prefer strcasestr instead of strstr? */
  *index_of_str = strstr ((const char *) buf, (const char *) str);
  if (NULL == (*index_of_str))
    return -1;
  return 0;
}

int
find_next_crlf (char *start_of_header, char **end_of_header)
{
  char *tmp;

  *end_of_header = NULL;	/* AMD fix */
  for (;;)
    {
      int leol = 1;		/* number of line-end symbols: */

      /* if CR or LF =1, if CRLF =2  */
      char *soh = start_of_header;

      while (('\r' != *soh) && ('\n' != *soh))
	{
	  if (*soh)
	    soh++;
	  else
	    {
	      OSIP_TRACE (osip_trace
			  (__FILE__, __LINE__, OSIP_ERROR, NULL,
			   "Final CRLF is missing\n"));
	      return -1;
	    }
	}


      if (('\r' == soh[0]) && ('\n' == soh[1]))
	{			/* case 1: CRLF is the separator */
	  tmp = soh + 1;
	  leol = 2;
	}
      else
	tmp = soh;		/* case 2 or 3: CR or LF is the separator */


      /* VERIFY if TMP is the end of header or LWS.            */
      /* LWS are extra SP, HT, CR and LF contained in headers. */
      if ((' ' == tmp[1]) || ('\t' == tmp[1]))
	{
	  /* AMD fix: SPECIAL case! for people who add extra spaces  */
	  /* with no meaning at the end of the body of SIP messages? */
	  /* if this occur before the body, this is an error case    */
	  /* because the last header of a message MUST end with a    */
	  /* CRLFCRLF (not only CRLF). I prefer to assume that buggy */
	  /* implementation exists... :( */
	  char *extraspaces = tmp + 2;

	  /* replace line end and TAB symbols by SP */
	  tmp[1] = ' ';
	  tmp[0] = ' ';
	  if (2 == leol)
	    tmp[-1] = ' ';

	  for (;;)
	    {			/* this is for line ending with '\cr\lf\t    ' (i.e more than one space after tab) */
	      if (' ' == *extraspaces)
		extraspaces++;
	      else if ('\t' == *extraspaces)
		{
		  *extraspaces = ' ';
		  extraspaces++;
		}
	      else if ('\0' == *extraspaces)
		{
		  tmp[1] = '\0';	/* this is the real limit of SIP message */
		  *end_of_header = tmp + 1;
		  return 0;
		}
	      else
		break;
	    }
	  /* YES, this is LWS extra characters    */
	  /* restart search of the end of headers */
	  start_of_header = extraspaces;
	}
      else
	{
	  *end_of_header = tmp + 1;	/* beggining of next header */
	  return 0;
	}
    }
}

int
find_next_crlfcrlf (char *start_of_part, char **end_of_part)
{
  char *start_of_line;
  char *end_of_line;
  int i;

  start_of_line = start_of_part;

  for (;;)
    {
      i = find_next_crlf (start_of_line, &end_of_line);
      if (i == -1)
	{			/* error case??? no end of mesage found */
	  OSIP_TRACE (osip_trace
		      (__FILE__, __LINE__, OSIP_ERROR, NULL,
		       "Final CRLF is missing\n"));
	  return -1;
	}
      if ('\0' == end_of_line[0])
	{			/* error case??? no end of message found */
	  OSIP_TRACE (osip_trace
		      (__FILE__, __LINE__, OSIP_ERROR, NULL,
		       "Final CRLF is missing\n"));
	  return -1;
	}
      else if ('\r' == end_of_line[0])
	{
	  if ('\n' == end_of_line[1])
	    end_of_line++;
	  *end_of_part = end_of_line + 1;
	  return 0;
	}
      else if ('\n' == end_of_line[0])
	{
	  *end_of_part = end_of_line + 1;
	  return 0;
	}
      start_of_line = end_of_line;
    }
}

int
msg_set_header (sip_t * sip, char *hname, char *hvalue)
{
  int my_index;

  if (hname == NULL)
    return -1;
  stolowercase (hname);

  /* some headers are analysed completely      */
  /* this method is used for selective parsing */
  my_index = parser_isknownheader (hname);
  if (my_index >= 0)		/* ok */
    {
      int ret;

      ret = parser_callmethod (my_index, sip, hvalue);
      if (ret == -1)
	{
	  OSIP_TRACE (osip_trace
		      (__FILE__, __LINE__, OSIP_ERROR, NULL,
		       "Could not set header: \"%s\" %s\n", hname, hvalue));
	  return -1;
	}
      return 0;
    }
  /* unknownheader */
  if (msg_setheader (sip, hname, hvalue) == -1)
    {
      OSIP_TRACE (osip_trace
		  (__FILE__, __LINE__, OSIP_ERROR, NULL,
		   "Could not set unknown header\n"));
      return -1;
    }

  return 0;
}

int
msg_handle_multiple_values (sip_t * sip, char *hname, char *hvalue)
{
  int i;
  char *ptr;			/* current location of the search */
  char *comma;			/* This is the separator we are elooking for */
  char *beg;			/* beg of a header */
  char *end;			/* end of a header */
  char *quote1;			/* first quote of a pair of quotes   */
  char *quote2;			/* second quuote of a pair of quotes */


  beg = hvalue;
  end = NULL;
  ptr = hvalue;
  if (hvalue == NULL)
    {
      i = msg_set_header (sip, hname, hvalue);
      if (i == -1)
	return -1;
      return 0;
    }

  comma = strchr (ptr, ',');


  stolowercase (hname);

  if (comma == NULL || (strncmp (hname, "date", 4) == 0 && strlen (hname) == 4 )
      || strncmp (hname, "organization", 12) == 0 || (strncmp (hname, "to", 2) == 0 && strlen (hname) == 2) || (strncmp (hname, "from", 4) == 0 && strlen (hname) == 4)	/* AMD: BUG fix */
      || strncmp (hname, "call-id", 7) == 0 || (strncmp (hname, "cseq", 4) == 0 && strlen (hname) == 4)	/* AMD: BUG fix */
      || strncmp (hname, "subject", 7) == 0 || strncmp (hname, "user-agent", 10) == 0 || strncmp (hname, "server", 6) == 0 || strncmp (hname, "www-authenticate", 16) == 0	/* AMD: BUG fix */
      || strncmp (hname, "authentication-info", 19) == 0 || strncmp (hname, "proxy-authenticate", 20) == 0 || strncmp (hname, "proxy-authorization", 19) == 0 || strncmp (hname, "proxy-authentication-info", 25) == 0	/* AMD: BUG fix */
      || strncmp (hname, "authorization", 13) == 0)
    /* there is no multiple header! likely      */
    /* to happen most of the time...            */
    /* or hname is a TEXT-UTF8-TRIM and may     */
    /* contain a comma. this is not a separator */
    /* THIS DOES NOT WORK FOR UNKNOWN HEADER!!!! */
    {
      i = msg_set_header (sip, hname, hvalue);
      if (i == -1)
	return -1;
      return 0;
    }

  quote2 = NULL;
  while (comma != NULL)
    {
      quote1 = quote_find (ptr);
      if (quote1 != NULL)
	{
	  quote2 = quote_find (quote1 + 1);
	  if (quote2 == NULL)
	    return -1;		/* quotes comes by pair */
	  ptr = quote2 + 1;
	}

      if ((quote1 == NULL) || (quote1 > comma))
	{
	  end = comma;
	  comma = strchr (comma + 1, ',');
	  ptr = comma + 1;
	}
      else if ((quote1 < comma) && (quote2 < comma))
	{			/* quotes are located before the comma, */
	  /* continue the search for next quotes  */
	  ptr = quote2 + 1;
	}
      else if ((quote1 < comma) && (comma < quote2))
	{			/* if comma is inside the quotes... */
	  /* continue with the next comma.    */
	  ptr = quote2 + 1;
	  comma = strchr (ptr, ',');
	  if (comma == NULL)
	    /* this header last at the end of the line! */
	    {			/* this one does not need an allocation... */
	      if (strlen (beg) < 2)
		return 0;	/* empty header */
	      sclrspace (beg);
	      i = msg_set_header (sip, hname, beg);
	      if (i == -1)
		return -1;
	      return 0;
	    }
	}

      if (end != NULL)
	{
	  char *avalue;

	  if (end - beg + 1 < 2)
	    return -1;
	  avalue = (char *) smalloc (end - beg + 1);
	  sstrncpy (avalue, beg, end - beg);
	  sclrspace (avalue);
	  /* really store the header in the sip structure */
	  i = msg_set_header (sip, hname, avalue);
	  sfree (avalue);
	  if (i == -1)
	    return -1;
	  beg = end + 1;
	  end = NULL;
	  if (comma == NULL)
	    /* this header last at the end of the line! */
	    {			/* this one does not need an allocation... */
	      if (strlen (beg) < 2)
		return 0;	/* empty header */
	      sclrspace (beg);
	      i = msg_set_header (sip, hname, beg);
	      if (i == -1)
		return -1;
	      return 0;
	    }
	}
    }
  return -1;			/* if comma is NULL, we should have already return 0 */
}

/* set all headers */
int
msg_headers_parse (sip_t * sip, char *start_of_header, char **body)
{
  char *colon_index;		/* index of ':' */
  char *hname;
  char *hvalue;
  char *end_of_header;
  int i;

  for (;;)
    {
      i = find_next_crlf (start_of_header, &end_of_header);
      if (i == -1)
	{
	  OSIP_TRACE (osip_trace
		      (__FILE__, __LINE__, OSIP_ERROR, NULL,
		       "End of header Not found\n"));
	  return -1;		/* this is an error case!     */
	}
      if (end_of_header[0] == '\0')
	{			/* final CRLF is missing */
	  OSIP_TRACE (osip_trace
		      (__FILE__, __LINE__, OSIP_ERROR, NULL,
		       "SIP message does not end with CRLFCRLF\n"));
	  return -1;
	}
      /* find the header name */
      colon_index = strchr (start_of_header, ':');
      if (colon_index == NULL)
	{
	  OSIP_TRACE (osip_trace
		      (__FILE__, __LINE__, OSIP_ERROR, NULL,
		       "End of header Not found\n"));
	  return -1;		/* this is also an error case */
	}
      if (colon_index - start_of_header + 1 < 2)
	return -1;
      if (end_of_header <= colon_index)
	{
	  OSIP_TRACE (osip_trace
		      (__FILE__, __LINE__, OSIP_ERROR, NULL,
		       "Malformed message\n"));
	  return -1;
	}
      hname = (char *) smalloc (colon_index - start_of_header + 1);
      sstrncpy (hname, start_of_header, colon_index - start_of_header);
      sclrspace (hname);

      {
	char *end;

	/* END of header is (end_of_header-2) if header separation is CRLF */
	/* END of header is (end_of_header-1) if header separation is CR or LF */
	if ((end_of_header[-2] == '\r') || (end_of_header[-2] == '\n'))
	  end = end_of_header - 2;
	else
	  end = end_of_header - 1;
	if ((end) - colon_index < 2)
	  hvalue = NULL;	/* some headers (subject) can be empty */
	else
	  {
	    hvalue = (char *) smalloc ((end) - colon_index);
	    sstrncpy (hvalue, colon_index + 1, (end) - colon_index - 1);
	    sclrspace (hvalue);
	  }
      }

      /* hvalue MAY contains multiple value. In this case, they   */
      /* are separated by commas. But, a comma may be part of a   */
      /* quoted-string ("here, and there" is an example where the */
      /* comma is not a separator!) */
      i = msg_handle_multiple_values (sip, hname, hvalue);

      sfree (hname);
      sfree (hvalue);
      if (i == -1)
	{
	  OSIP_TRACE (osip_trace
		      (__FILE__, __LINE__, OSIP_ERROR, NULL,
		       "End of header Not found\n"));
	  return -1;
	}

      /* the list of headers MUST always end with  */
      /* CRLFCRLF (also CRCR and LFLF are allowed) */
      if ((end_of_header[0] == '\r') || (end_of_header[0] == '\n'))
	{
	  *body = end_of_header;
	  return 0;		/* end of header found        */
	}
      /* continue on the next header */
      start_of_header = end_of_header;
    }

/* Unreachable code
 OSIP_TRACE (osip_trace
	      (__FILE__, __LINE__, OSIP_BUG, NULL,
	       "This code cannot be reached\n")); */
  return -1;
}


/* internal method to parse the body */
int
msg_body_parse (sip_t * sip, char *start_of_buf, char **next_body)
{
  char *start_of_body;
  char *end_of_body;
  char *tmp;
  int i;

  char *sep_boundary;
  generic_param_t *ct_param;

  /* if MIME-Version: does not exist we just have */
  /* to deal with one body and no header... */
  if (sip->mime_version == NULL)
    {				/* Mime-Version header does NOT exist */
      if (sip->content_type == NULL)
	return 0;		/* no body is attached */
      else
	{
	  size_t body_len;

	  if (start_of_buf[0] == '\0')
	    return -1;		/* final CRLF is missing */
	  /* get rid of the first CRLF */
	  if ('\r' == start_of_buf[0])
	    {
	      if ('\n' == start_of_buf[1])
		start_of_body = start_of_buf + 2;
	      else
		start_of_body = start_of_buf + 1;
	    }
	  else if ('\n' == start_of_buf[0])
	    start_of_body = start_of_buf + 1;
	  else
	    return -1;		/* message does not end with CRLFCRLF, CRCR or LFLF */

	  if (sip->contentlength != NULL)
	    body_len = satoi (sip->contentlength->value);
	  else
	    {			/* if content_length does not exist, set it. */
	      char *tmp = smalloc (8);

	      if (tmp == NULL)
		return -1;
	      body_len = strlen (start_of_body);
	      sprintf (tmp, "%i", body_len);
	      i = msg_setcontent_length (sip, tmp);
	      sfree (tmp);
	      if (i != 0)
		return -1;
	    }

	  if (body_len > strlen (start_of_body))	/* we do not receive the */
	    return -1;		/* complete message      */
	  /* end_of_body = start_of_body + strlen(start_of_body); */
	  end_of_body = start_of_body + body_len;
	  tmp = smalloc (end_of_body - start_of_body + 2);
	  if (tmp == NULL)
	    return -1;
	  sstrncpy (tmp, start_of_body, end_of_body - start_of_body);

	  i = msg_setbody (sip, tmp);
	  sfree (tmp);
	  if (i != 0)
	    return -1;
	  return 0;
	}
    }

  /* find the boundary */
  i = generic_param_getbyname (sip->content_type->gen_params,
			       "boundary", &ct_param);
  if (i != 0)
    return -1;

  if (ct_param == NULL)
    return -1;
  if (ct_param->gvalue == NULL)
    return -1;			/* No boundary but multiple headers??? */

  sep_boundary = (char *) smalloc (strlen (ct_param->gvalue) + 3);
  sprintf (sep_boundary, "--%s", ct_param->gvalue);

  *next_body = NULL;
  start_of_body = start_of_buf;
  for (;;)
    {
      i = find_next_occurence (sep_boundary, start_of_body, &start_of_body);
      if (i == -1)
	{
	  sfree (sep_boundary);
	  return -1;
	}
      i =
	find_next_occurence (sep_boundary,
			     start_of_body + strlen (sep_boundary),
			     &end_of_body);
      if (i == -1)
	{
	  sfree (sep_boundary);
	  return -1;
	}

      /* this is the real beginning of body */
      start_of_body = start_of_body + strlen (sep_boundary) + 2;

      tmp = smalloc (end_of_body - start_of_body + 1);
      sstrncpy (tmp, start_of_body, end_of_body - start_of_body);

      i = msg_setbody_mime (sip, tmp);
      sfree (tmp);
      if (i == -1)
	{
	  sfree (sep_boundary);
	  return -1;
	}

      if (strncmp (end_of_body + strlen (sep_boundary), "--", 2) == 0)
	{			/* end of all bodies */
	  *next_body = end_of_body;
	  sfree (sep_boundary);
	  return 0;
	}
      /* continue on the next body */
      start_of_body = end_of_body;
    }
  /* Unreachable code */
  /* sfree (sep_boundary); */
  return -1;
}


/* sip_t *sip is filled while analysing buf */
int
msg_parse (sip_t * sip, char *buf)
{
  int i;
  char *next_header_index;

  /* parse request or status line */
  i = msg_startline_parse (sip->strtline, buf, &next_header_index);
  if (i == -1)
    {
      OSIP_TRACE (osip_trace
		  (__FILE__, __LINE__, OSIP_ERROR, NULL,
		   "error in msg_startline_init()\n"));
      return -1;
    }
  buf = next_header_index;

  /* parse headers */
  i = msg_headers_parse (sip, buf, &next_header_index);
  if (i == -1)
    {
      OSIP_TRACE (osip_trace
		  (__FILE__, __LINE__, OSIP_ERROR, NULL,
		   "error in msg_headers_parse()\n"));
      return -1;
    }
  buf = next_header_index;

  /* this is a *very* simple test... (which handle most cases...) */
  if (strlen (buf) < 3)
    {
      /* this is mantory in the oSIP stack */
      if (sip->contentlength == NULL)
	msg_setcontent_length (sip, "0");
      return 0;			/* no body found */
    }

  i = msg_body_parse (sip, buf, &next_header_index);
  if (i == -1)
    {
      OSIP_TRACE (osip_trace
		  (__FILE__, __LINE__, OSIP_ERROR, NULL,
		   "error in msg_body_parse()\n"));
      return -1;
    }
  buf = next_header_index;

  /* this is mandatory in the oSIP stack */
  if (sip->contentlength == NULL)
    msg_setcontent_length (sip, "0");

  return 0;
}

/* This method just add a received parameter in the Via
   as requested by rfc3261 */
int
msg_fix_last_via_header (sip_t * request, char *ip_addr, int port)
{
  generic_param_t *rport;
  via_t *via;
  /* get Top most Via header: */
  if (request == NULL || request->strtline == NULL)
    return -1;
  if (MSG_IS_RESPONSE (request))
    return 0;			/* Don't fix Via header */

  via = list_get (request->vias, 0);
  if (via == NULL || via->host == NULL)
    /* Hey, we could build it? */
    return -1;

  via_param_getbyname (via, "rport", &rport);
  if (rport != NULL)
    {
      if (rport->gvalue == NULL)
	{
	  rport->gvalue = (char *) smalloc (9);
#if (defined WIN32 || defined _WIN32_WCE)
	  _snprintf (rport->gvalue, 8, "%i", port);
#else
	  snprintf (rport->gvalue, 8, "%i", port);
#endif
	}			/* else bug? */
    }

  /* only add the received parameter if the 'sent-by' value does not contains
     this ip address */
  if (0 == strcmp (via->host, ip_addr))	/* don't need the received parameter */
    return 0;
  via_set_received (via, sgetcopy (ip_addr));
  return 0;
}

char *
msg_getreason (int replycode)
{
  int i;

  i = replycode / 100;
  if (i == 1)
    {				/* 1xx  */
      if (replycode == 100)
	return sgetcopy ("Trying");
      if (replycode == 180)
	return sgetcopy ("Ringing");
      if (replycode == 181)
	return sgetcopy ("Call Is Being Forwarded");
      if (replycode == 182)
	return sgetcopy ("Queued");
      if (replycode == 183)
	return sgetcopy ("Session Progress");
    }
  if (i == 2)
    {				/* 2xx */
      return sgetcopy ("OK");
    }
  if (i == 3)
    {				/* 3xx */
      if (replycode == 300)
	return sgetcopy ("Multiple Choices");
      if (replycode == 301)
	return sgetcopy ("Moved Permanently");
      if (replycode == 302)
	return sgetcopy ("Moved Temporarily");
      if (replycode == 305)
	return sgetcopy ("Use Proxy");
      if (replycode == 380)
	return sgetcopy ("Alternative Service");
    }
  if (i == 4)
    {				/* 4xx */
      if (replycode == 400)
	return sgetcopy ("Bad Request");
      if (replycode == 401)
	return sgetcopy ("Unauthorized");
      if (replycode == 402)
	return sgetcopy ("Payment Required");
      if (replycode == 403)
	return sgetcopy ("Forbidden");
      if (replycode == 404)
	return sgetcopy ("Not Found");
      if (replycode == 405)
	return sgetcopy ("Method Not Allowed");
      if (replycode == 406)
	return sgetcopy ("Not Acceptable");
      if (replycode == 407)
	return sgetcopy ("Proxy Authentication Required");
      if (replycode == 408)
	return sgetcopy ("Request Timeout");
      if (replycode == 409)
	return sgetcopy ("Conflict");
      if (replycode == 410)
	return sgetcopy ("Gone");
      if (replycode == 411)
	return sgetcopy ("Length Required");
      if (replycode == 413)
	return sgetcopy ("Request Entity Too Large");
      if (replycode == 414)
	return sgetcopy ("Request-URI Too Large");
      if (replycode == 415)
	return sgetcopy ("Unsupported Media Type");
      if (replycode == 416)
	return sgetcopy ("Unsupported Uri Scheme");
      if (replycode == 420)
	return sgetcopy ("Bad Extension");
      /* 
         Robin Nayathodan <roooot@softhome.net> 
         N.K Electronics INDIA

         RFC 3261 10.3.7 
       */
      if (replycode == 423)
	return sgetcopy ("Interval Too Short");
      if (replycode == 480)
	return sgetcopy ("Temporarily not available");
      if (replycode == 481)
	return sgetcopy ("Call Leg/Transaction Does Not Exist");
      if (replycode == 482)
	return sgetcopy ("Loop Detected");
      if (replycode == 483)
	return sgetcopy ("Too Many Hops");
      if (replycode == 484)
	return sgetcopy ("Address Incomplete");
      if (replycode == 485)
	return sgetcopy ("Ambiguous");
      if (replycode == 486)
	return sgetcopy ("Busy Here");
      if (replycode == 487)
	return sgetcopy ("Request Cancelled");
      if (replycode == 488)
	return sgetcopy ("Not Acceptable Here");
      if (replycode == 489)
	return sgetcopy ("Bad Event");
    }
  if (i == 5)
    {				/* 5xx */
      if (replycode == 500)
	return sgetcopy ("Internal Server Error");
      if (replycode == 501)
	return sgetcopy ("Not Implemented");
      if (replycode == 502)
	return sgetcopy ("Bad Gateway");
      if (replycode == 503)
	return sgetcopy ("Service Unavailable");
      if (replycode == 504)
	return sgetcopy ("Gateway Time-out");
      if (replycode == 505)
	return sgetcopy ("SIP Version not supported");
    }
  if (i == 6)
    {				/* 6xx */
      if (replycode == 600)
	return sgetcopy ("Busy Everywhere");
      if (replycode == 603)
	return sgetcopy ("Decline");
      if (replycode == 604)
	return sgetcopy ("Does not exist anywhere");
      if (replycode == 606)
	return sgetcopy ("Not Acceptable");
    }

  return NULL;
}
