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


static int
startline_2charreq (startline_t * strtline, char **dest)
{
  char *tmp;
  char *rquri;
  int i;

  *dest = NULL;
  if ((strtline == NULL) || (strtline->rquri == NULL)
      || (strtline->sipmethod == NULL) || (strtline->sipversion == NULL))
    return -1;

  i = url_2char (strtline->rquri, &rquri);
  if (i == -1)
    return -1;
  *dest = (char *) smalloc (strlen (strtline->sipmethod)
			    + strlen (rquri) + strlen (strtline->sipversion) +
			    3);
  tmp = *dest;
  sstrncpy (tmp, strtline->sipmethod, strlen (strtline->sipmethod));
  tmp = tmp + strlen (strtline->sipmethod);
  sstrncpy (tmp, " ", 1);
  tmp = tmp + 1;
  sstrncpy (tmp, rquri, strlen (rquri));
  tmp = tmp + strlen (rquri);
  sstrncpy (tmp, " ", 1);
  tmp = tmp + 1;
  sstrncpy (tmp, strtline->sipversion, strlen (strtline->sipversion));

  sfree (rquri);
  return 0;
}

static int
startline_2charresp (startline_t * strtline, char **dest)
{
  char *tmp;

  *dest = NULL;
  if ((strtline == NULL) || (strtline->reasonphrase == NULL)
      || (strtline->statuscode == NULL) || (strtline->sipversion == NULL))
    return -1;
  *dest = (char *) smalloc (strlen (strtline->sipversion)
			    + strlen (strtline->statuscode)
			    + strlen (strtline->reasonphrase) + 4);
  tmp = *dest;

  sstrncpy (tmp, strtline->sipversion, strlen (strtline->sipversion));
  tmp = tmp + strlen (strtline->sipversion);
  sstrncpy (tmp, " ", 1);
  tmp = tmp + 1;
  sstrncpy (tmp, strtline->statuscode, strlen (strtline->statuscode));
  tmp = tmp + strlen (strtline->statuscode);
  sstrncpy (tmp, " ", 1);
  tmp = tmp + 1;
  sstrncpy (tmp, strtline->reasonphrase, strlen (strtline->reasonphrase));

  return 0;
}

static int
msg_startline_2char (startline_t * strtline, char **dest)
{

  if (strtline->sipmethod != NULL)
    return startline_2charreq (strtline, dest);
  if (strtline->statuscode != NULL)
    return startline_2charresp (strtline, dest);

  OSIP_TRACE (osip_trace
	      (__FILE__, __LINE__, TRACE_LEVEL1, NULL,
	       "ERROR strtline->method has no value!\n"));
  return -1;			/* should never come here */
}

char *
msg_getreasonphrase (sip_t * sip)
{
  return sip->strtline->reasonphrase;
}

char *
msg_getstatuscode (sip_t * sip)
{
  return sip->strtline->statuscode;
}

char *
msg_getmethod (sip_t * sip)
{
  return sip->strtline->sipmethod;
}

char *
msg_getversion (sip_t * sip)
{
  return sip->strtline->sipversion;
}

url_t *
msg_geturi (sip_t * sip)
{
  return sip->strtline->rquri;
}

int
strcat_simple_header (char **_string, int *malloc_size,
		      char **_message, void *ptr_header, char *header_name,
		      int size_of_header, int (*xxx_2char) (void *, char **),
		      char **next)
{
  char *string;
  char *message;
  char *tmp;
  int i;

  string = *_string;
  message = *_message;

  if (ptr_header != NULL)
    {
      if (*malloc_size < message - string + 100 + size_of_header)
	/* take some memory avoid to realloc too much often */
	{			/* should not happen often */
	  int size = message - string;
	  *malloc_size = message - string + size_of_header + 100;
	  string = realloc (string, *malloc_size);
	  if (string == NULL)
	    {
	      *_string = NULL;
	      *_message = NULL;
	      return -1;
	    }
	  message = string + size;
	}
      sstrncpy (message, header_name, size_of_header);
      message = message + strlen (message);
      i = xxx_2char (ptr_header, &tmp);
      if (i == -1)
	{
	  *_string = string;
	  *_message = message;
	  *next = NULL;
	  return -1;
	}
      if (*malloc_size < message - string + (int) strlen (tmp) + 100)
	{
	  int size = message - string;
	  *malloc_size = message - string + strlen (tmp) + 100;
	  string = realloc (string, *malloc_size);
	  if (string == NULL)
	    {
	      *_string = NULL;
	      *_message = NULL;
	      return -1;
	    }
	  message = string + size;
	}
      sstrncpy (message, tmp, strlen (tmp));
      sfree (tmp);
      message = message + strlen (message);
      sstrncpy (message, CRLF, 2);
      message = message + 2;
    }
  *_string = string;
  *_message = message;
  *next = message;
  return 0;
}

int
strcat_headers_one_per_line (char **_string, int *malloc_size,
			     char **_message, list_t * headers,
			     char *header, int size_of_header,
			     int (*xxx_2char) (void *, char **), char **next)
{
  char *string;
  char *message;
  char *tmp;
  int pos = 0;
  int i;

  string = *_string;
  message = *_message;

  while (!list_eol (headers, pos))
    {
      void *elt;

      elt = (void *) list_get (headers, pos);

      if (*malloc_size < message - string + 100 + size_of_header)
	/* take some memory avoid to realloc too much often */
	{			/* should not happen often */
	  int size = message - string;
	  *malloc_size = message - string + size_of_header + 100;
	  string = realloc (string, *malloc_size);
	  if (string == NULL)
	    {
	      *_string = NULL;
	      *_message = NULL;
	      return -1;
	    }
	  message = string + size;
	}
      sstrncpy (message, header, size_of_header);
      i = xxx_2char (elt, &tmp);
      if (i == -1)
	{
	  *_string = string;
	  *_message = message;
	  *next = NULL;
	  return -1;
	}
      message = message + strlen (message);

      if (*malloc_size < message - string + (int) strlen (tmp) + 100)
	{
	  int size = message - string;
	  *malloc_size = message - string + strlen (tmp) + 100;
	  string = realloc (string, *malloc_size);
	  if (string == NULL)
	    {
	      *_string = NULL;
	      *_message = NULL;
	      return -1;
	    }
	  message = string + size;
	}
      sstrncpy (message, tmp, strlen (tmp));
      sfree (tmp);
      message = message + strlen (message);
      sstrncpy (message, CRLF, 2);
      message = message + 2;

      pos++;
    }
  *_string = string;
  *_message = message;
  *next = message;
  return 0;
}

int
strcat_headers_all_on_one_line (char **_string, int *malloc_size,
				char **_message, list_t * headers,
				char *header, int size_of_header,
				int (*xxx_2char) (void *, char **),
				char **next)
{
  char *string;
  char *message;
  char *tmp;
  int pos = 0;
  int i;

  string = *_string;
  message = *_message;

  pos = 0;
  while (!list_eol (headers, pos))
    {
      if (*malloc_size < message - string + 100 + size_of_header)
	/* take some memory avoid to realloc too much often */
	{			/* should not happen often */
	  int size = message - string;
	  *malloc_size = message - string + size_of_header + 100;
	  string = realloc (string, *malloc_size);
	  if (string == NULL)
	    {
	      *_string = NULL;
	      *_message = NULL;
	      return -1;
	    }
	  message = string + size;
	}
      sstrncpy (message, header, size_of_header);
      message = message + strlen (message);

      while (!list_eol (headers, pos))
	{
	  void *elt;

	  elt = (void *) list_get (headers, pos);
	  i = xxx_2char (elt, &tmp);
	  if (i == -1)
	    {
	      *_string = string;
	      *_message = message;
	      *next = NULL;
	      return -1;
	    }
	  if (*malloc_size < message - string + (int) strlen (tmp) + 100)
	    {
	      int size = message - string;
	      *malloc_size = message - string + (int) strlen (tmp) + 100;
	      string = realloc (string, *malloc_size);
	      if (string == NULL)
		{
		  *_string = NULL;
		  *_message = NULL;
		  return -1;
		}
	      message = string + size;
	    }
	  sstrncpy (message, tmp, strlen (tmp));
	  sfree (tmp);
	  message = message + strlen (message);
	  pos++;
	  if (!list_eol (headers, pos))
	    {
	      strncpy (message, ", ", 2);
	      message = message + 2;
	    }
	}
      sstrncpy (message, CRLF, 2);
      message = message + 2;
    }
  *_string = string;
  *_message = message;
  *next = message;
  return 0;
}

#ifdef USE_TMP_BUFFER
 /* return values:
    1: structure and buffer "message" are identical.
    2: buffer "message" is not up to date with the structure info (call msg_2char to update it).
    -1 on error.
  */
int
msg_get_property (sip_t * sip)
{
  if (sip == NULL)
    return -1;
  return sip->message_property;
}
#endif

int
msg_force_update (sip_t * sip)
{
#ifdef USE_TMP_BUFFER
  if (sip == NULL)
    return -1;
  sip->message_property = 2;
#endif
  return 0;
}

int
msg_2char (sip_t * sip, char **dest)
{
  int malloc_size;
  /* Added at SIPit day1 */
  char *start_of_bodies;
  char *content_length_to_modify = NULL;

  char *message;
  char *next;
  char *tmp;
  int pos;
  int i;
  malloc_size = SIP_MESSAGE_MAX_LENGTH;

  *dest = NULL;
  if ((sip == NULL) || (sip->strtline == NULL) || (sip->to == NULL)
      || (sip->from == NULL))
    return -1;

#ifdef USE_TMP_BUFFER
  {
#ifdef ENABLE_DEBUG
    static int number_of_call;
    static int number_of_call_avoided;

    number_of_call++;
#endif
    if (1 == msg_get_property (sip))
      {				/* message is already available in "message" */
#ifdef ENABLE_DEBUG
	number_of_call_avoided++;
#endif

	*dest = sgetcopy (sip->message);	/* is we just return the pointer, people will
						   get issues while upgrading the library. This
						   is because they are used to call free(*dest)
						   right after using the buffer. This may be
						   changed while moving to another branch
						   developpement. replacement line will be:
						   *dest = sip->message; */
#ifdef ENABLE_DEBUG
	/* if (number_of_call_avoided % 1000 == 0)
	  printf ("number of call msg_2char avoided: %i\n",
	     number_of_call_avoided); */
#endif
	return 0;

      }
    else
      {
	/* message should be rebuilt: delete the old one if exists. */
	sfree (sip->message);
	sip->message = NULL;
      }
  }
#endif

  message = (char *) smalloc (SIP_MESSAGE_MAX_LENGTH);	/* ???? message could be > 4000  */
  *dest = message;

  /* add the first line of message */
  i = msg_startline_2char (sip->strtline, &tmp);
  if (i == -1)
    {
      sfree (*dest);
      *dest = NULL;
      return -1;
    }
  sstrncpy (message, tmp, strlen (tmp));
  sfree (tmp);
  message = message + strlen (message);
  sstrncpy (message, CRLF, 2);
  message = message + 2;

  i =
    strcat_headers_one_per_line (dest, &malloc_size, &message, sip->vias,
				 "Via: ", 5,
				 ((int (*)(void *, char **)) &via_2char),
				 &next);
  if (i != 0)
    {
      sfree (*dest);
      *dest = NULL;
      return -1;
    }
  message = next;

  i =
    strcat_headers_one_per_line (dest, &malloc_size, &message,
				 sip->record_routes, "Record-Route: ", 14,
				 ((int (*)(void *, char **))
				  &record_route_2char), &next);
  if (i != 0)
    {
      sfree (*dest);
      *dest = NULL;
      return -1;
    }
  message = next;

  i =
    strcat_headers_one_per_line (dest, &malloc_size, &message, sip->routes,
				 "Route: ", 7,
				 ((int (*)(void *, char **)) &route_2char),
				 &next);
  if (i != 0)
    {
      sfree (*dest);
      *dest = NULL;
      return -1;
    }
  message = next;

  i = strcat_simple_header (dest, &malloc_size, &message,
			    sip->from, "From: ", 6,
			    ((int (*)(void *, char **)) &from_2char), &next);
  if (i != 0)
    {
      sfree (*dest);
      *dest = NULL;
      return -1;
    }
  message = next;

  i = strcat_simple_header (dest, &malloc_size, &message,
			    sip->to, "To: ", 4,
			    ((int (*)(void *, char **)) &to_2char), &next);
  if (i != 0)
    {
      sfree (*dest);
      *dest = NULL;
      return -1;
    }
  message = next;

  i = strcat_simple_header (dest, &malloc_size, &message,
			    sip->call_id, "Call-ID: ", 9,
			    ((int (*)(void *, char **)) &call_id_2char),
			    &next);
  if (i != 0)
    {
      sfree (*dest);
      *dest = NULL;
      return -1;
    }
  message = next;

  i = strcat_simple_header (dest, &malloc_size, &message,
			    sip->cseq, "CSeq: ", 6,
			    ((int (*)(void *, char **)) &cseq_2char), &next);
  if (i != 0)
    {
      sfree (*dest);
      *dest = NULL;
      return -1;
    }
  message = next;

  i =
    strcat_headers_one_per_line (dest, &malloc_size, &message, sip->contacts,
				 "Contact: ", 9,
				 ((int (*)(void *, char **)) &contact_2char),
				 &next);
  if (i != 0)
    {
      sfree (*dest);
      *dest = NULL;
      return -1;
    }
  message = next;

  i = strcat_headers_one_per_line (dest, &malloc_size, &message,
				   sip->authorizations, "Authorization: ", 15,
				   ((int (*)(void *, char **))
				    &authorization_2char), &next);
  if (i != 0)
    {
      sfree (*dest);
      *dest = NULL;
      return -1;
    }
  message = next;

  i =
    strcat_headers_one_per_line (dest, &malloc_size, &message,
				 sip->www_authenticates, "WWW-Authenticate: ",
				 18,
				 ((int (*)(void *, char **))
				  &www_authenticate_2char), &next);
  if (i != 0)
    {
      sfree (*dest);
      *dest = NULL;
      return -1;
    }
  message = next;

  i =
    strcat_headers_one_per_line (dest, &malloc_size, &message,
				 sip->proxy_authenticates,
				 "Proxy-Authenticate: ", 20,
				 ((int (*)(void *, char **))
				  &www_authenticate_2char), &next);
  if (i != 0)
    {
      sfree (*dest);
      *dest = NULL;
      return -1;
    }
  message = next;

  i =
    strcat_headers_one_per_line (dest, &malloc_size, &message,
				 sip->proxy_authorizations,
				 "Proxy-Authorization: ", 21,
				 ((int (*)(void *, char **))
				  &authorization_2char), &next);
  if (i != 0)
    {
      sfree (*dest);
      *dest = NULL;
      return -1;
    }
  message = next;


  pos = 0;
  while (!list_eol (sip->headers, pos))
    {
      header_t *header;

      header = (header_t *) list_get (sip->headers, pos);
      i = header_2char (header, &tmp);
      if (i == -1)
	{
	  sfree (*dest);
	  *dest = NULL;
	  return -1;
	}
      sstrncpy (message, tmp, strlen (tmp));
      sfree (tmp);
      message = message + strlen (message);
      sstrncpy (message, CRLF, 2);
      message = message + 2;

      pos++;
    }

  i =
    strcat_headers_all_on_one_line (dest, &malloc_size, &message, sip->allows,
				    "Allow: ", 7,
				    ((int (*)(void *, char **))
				     &content_length_2char), &next);
  if (i != 0)
    {
      sfree (*dest);
      *dest = NULL;
      return -1;
    }
  message = next;

  i = strcat_simple_header (dest, &malloc_size, &message,
			    sip->content_type, "Content-Type: ", 14,
			    ((int (*)(void *, char **)) &content_type_2char),
			    &next);
  if (i != 0)
    {
      sfree (*dest);
      *dest = NULL;
      return -1;
    }
  message = next;

  i =
    strcat_headers_all_on_one_line (dest, &malloc_size, &message,
				    sip->content_encodings,
				    "Content-Encoding: ", 18,
				    ((int (*)(void *, char **))
				     &content_length_2char), &next);
  if (i != 0)
    {
      sfree (*dest);
      *dest = NULL;
      return -1;
    }
  message = next;

  i = strcat_simple_header (dest, &malloc_size, &message,
			    sip->mime_version, "Mime-Version: ", 14,
			    ((int (*)(void *, char **))
			     &content_length_2char), &next);
  if (i != 0)
    {
      sfree (*dest);
      *dest = NULL;
      return -1;
    }
  message = next;


  i =
    strcat_headers_one_per_line (dest, &malloc_size, &message,
				 sip->call_infos, "Call-Info: ", 11,
				 ((int (*)(void *, char **))
				  &call_info_2char), &next);
  if (i != 0)
    {
      sfree (*dest);
      *dest = NULL;
      return -1;
    }
  message = next;

  i =
    strcat_headers_one_per_line (dest, &malloc_size, &message,
				 sip->alert_infos, "Alert-Info: ", 12,
				 ((int (*)(void *, char **))
				  &call_info_2char), &next);
  if (i != 0)
    {
      sfree (*dest);
      *dest = NULL;
      return -1;
    }
  message = next;

  i =
    strcat_headers_one_per_line (dest, &malloc_size, &message,
				 sip->error_infos, "Error-Info: ", 12,
				 ((int (*)(void *, char **))
				  &call_info_2char), &next);
  if (i != 0)
    {
      sfree (*dest);
      *dest = NULL;
      return -1;
    }
  message = next;

  i =
    strcat_headers_all_on_one_line (dest, &malloc_size, &message,
				    sip->accepts, "Accept: ", 8,
				    ((int (*)(void *, char **))
				     &content_type_2char), &next);
  if (i != 0)
    {
      sfree (*dest);
      *dest = NULL;
      return -1;
    }
  message = next;

  i =
    strcat_headers_all_on_one_line (dest, &malloc_size, &message,
				    sip->accept_encodings,
				    "Accept-Encoding: ", 17,
				    ((int (*)(void *, char **))
				     &accept_encoding_2char), &next);
  if (i != 0)
    {
      sfree (*dest);
      *dest = NULL;
      return -1;
    }
  message = next;

  i =
    strcat_headers_all_on_one_line (dest, &malloc_size, &message,
				    sip->accept_languages,
				    "Accept-Language: ", 17,
				    ((int (*)(void *, char **))
				     &accept_encoding_2char), &next);
  if (i != 0)
    {
      sfree (*dest);
      *dest = NULL;
      return -1;
    }
  message = next;



  /* we have to create the body before adding the contentlength */

  sstrncpy (message, "Content-Length: ", 16);
  message = message + strlen (message);

  /* SIPit Day1
     ALWAYS RECALCULATE?
     if (sip->contentlength!=NULL)
     {
     i = content_length_2char(sip->contentlength, &tmp);
     if (i==-1) {
     sfree(*dest);
     *dest = NULL;
     return -1;
     }
     sstrncpy(message,tmp,strlen(tmp));
     sfree(tmp);
     }
     else
     { */
  if (list_eol (sip->bodies, 0))	/* no body */
    sstrncpy (message, "0", 1);
  else
    {
      /* BUG: p130 (rfc2543bis-04)
         "No SP after last token or quoted string"

         In fact, if extra spaces exist: the stack can't be used
         to make user-agent that wants to make authentication...
         This should be changed...
       */

      sstrncpy (message, "     ", 5);
      content_length_to_modify = message;
    }
  /*  } */
  message = message + strlen (message);
  sstrncpy (message, CRLF, 2);
  message = message + 2;


  /* end of headers */
  sstrncpy (message, CRLF, 2);
  message = message + 2;

  if (list_eol (sip->bodies, 0))
    {
#ifdef USE_TMP_BUFFER
      /* same remark as at the beginning of the method */
      sip->message_property = 1;
      sip->message = sgetcopy (*dest);
#endif
      return 0;			/* it's all done */
    }
  start_of_bodies = message;

  pos = 0;
  while (!list_eol (sip->bodies, pos))
    {
      body_t *body;

      body = (body_t *) list_get (sip->bodies, pos);

      if (sip->mime_version != NULL)
	{
	  sstrncpy (message, "--++", strlen ("--++"));
	  message = message + strlen (message);
	  sstrncpy (message, CRLF, 2);
	  message = message + 2;
	}

      i = body_2char (body, &tmp);
      if (i != 0)
	{
	  sfree (*dest);
	  *dest = NULL;
	  return -1;
	}
      sstrncpy (message, tmp, strlen (tmp));
      sfree (tmp);
      message = message + strlen (message);

      pos++;
    }

  if (sip->mime_version != NULL)
    {
      sstrncpy (message, "--++--", strlen ("--++--"));
      message = message + strlen (message);
      sstrncpy (message, CRLF, 2);
      message = message + 2;
      /* ADDED at SIPit day1:  Is this needed for MIME type?
         sstrncpy(message,CRLF,2);
         message = message + 2;
         strncpy(message,"\0",1); */
    }

  if (content_length_to_modify == NULL)
    {
      sfree (*dest);
      *dest = NULL;
      return -1;
    }

  /* we NOW have the length of bodies: */
  {
    int size = strlen (start_of_bodies);

    /* replace xxxxx by size */
    tmp = (char *) smalloc (15);
    /* to be improved? !!!could be an issue for authentication!!! */
#ifdef OLD_CODE
    sprintf (tmp, "%i     ", size);
    /* do not use sstrncpy here! */
    strncpy (content_length_to_modify, tmp, 5);
#else
    sprintf (tmp, "%i", size);
    strncpy (content_length_to_modify+5-strlen(tmp), tmp, strlen(tmp));
#endif
    sfree (tmp);
  }

#ifdef USE_TMP_BUFFER
  /* same remark as at the beginning of the method */
  sip->message_property = 1;
  sip->message = sgetcopy (*dest);
#endif
  return 0;
}

#ifdef ENABLE_TRACE

#ifdef _WIN32_WCE

void msg_logrequest (sip_t * sip, char *fmt) {}
void msg_logresponse (sip_t * sip, char *fmt) {}

#else

void
msg_logresponse (sip_t * sip, char *fmt)
{
  char *tmp1;
  char *tmp2;
  int i;

  if (1 == is_trace_level_activate (TRACE_LEVEL4))
    {
      i = msg_2char (sip, &tmp1);
      if (i != -1)
	{
	  osip_trace (__FILE__, __LINE__, TRACE_LEVEL4, stdout,
		      "MESSAGE :\n%s\n", tmp1);
	  sfree (tmp1);
	}
      else
	osip_trace (__FILE__, __LINE__, TRACE_LEVEL4, stdout,
		    "MESSAGE : Could not make a string of message!!!!\n");
    }

  if (1 == is_trace_level_activate (TRACE_LEVEL0))
    {
      i = from_2char (sip->from, &tmp1);
      if (i == -1)
	return;
      i = to_2char (sip->to, &tmp2);
      if (i == -1)
	return;

      osip_trace (__FILE__, __LINE__, TRACE_LEVEL0, NULL, fmt,
		  sip->strtline->statuscode, sip->strtline->reasonphrase,
		  sip->cseq->method, tmp1, tmp2, sip->cseq->number,
		  sip->call_id->number);

      sfree (tmp1);
      sfree (tmp2);
    }
}

void
msg_logrequest (sip_t * sip, char *fmt)
{
  char *tmp1;
  char *tmp2;
  int i;

  if (1 == is_trace_level_activate (TRACE_LEVEL4))
    {
      i = msg_2char (sip, &tmp1);
      if (i != -1)
	{
	  osip_trace (__FILE__, __LINE__, TRACE_LEVEL4, stdout,
		      "<app.c> MESSAGE :\n%s\n", tmp1);
	  sfree (tmp1);
	}
      else
	osip_trace (__FILE__, __LINE__, TRACE_LEVEL4, stdout,
		    "<app.c> MESSAGE :\n Could not make a string of message\n");
    }

  if (1 == is_trace_level_activate (TRACE_LEVEL0))
    {
      i = from_2char (sip->from, &tmp1);
      if (i == -1)
	return;
      i = to_2char (sip->to, &tmp2);
      if (i == -1)
	return;

      osip_trace (__FILE__, __LINE__, TRACE_LEVEL0, NULL, fmt,
		  sip->cseq->method, tmp1, tmp2, sip->cseq->number,
		  sip->call_id->number);
      sfree (tmp1);
      sfree (tmp2);
    }
}
#endif

#endif

