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

/* enable logging of memory accesses */
#ifdef MEMORY_LEAKS
static int freesipcptr = 0;
#endif

void
msg_startline_init (startline_t ** strtline)
{
  *strtline = (startline_t *) smalloc (sizeof (startline_t));
  (*strtline)->sipmethod = NULL;
  (*strtline)->sipversion = NULL;
  (*strtline)->statuscode = NULL;
  (*strtline)->reasonphrase = NULL;
  (*strtline)->rquri = NULL;
}

static void
msg_startline_free (startline_t * strtline)
{
  if (strtline == NULL)
    return;
  sfree (strtline->sipmethod);
  sfree (strtline->sipversion);
  if (strtline->rquri != NULL)
    {
      url_free (strtline->rquri);
      sfree (strtline->rquri);
    }
  sfree (strtline->statuscode);
  sfree (strtline->reasonphrase);
}


int
msg_init (sip_t ** sip)
{
#ifdef MEMORY_LEAKS
  static int comptr = 0;

  comptr++;
  freesipcptr++;
#endif
  *sip = (sip_t *) smalloc (sizeof (sip_t));
#ifdef MEMORY_LEAKS
  osip_trace (__FILE__, __LINE__, TRACE_LEVEL0, stdout,
	      "<msg_write.c> msg_init() = %i, malloc-free = %i, address = %x\n",
	      comptr, freesipcptr, *sip);
  fflush (stdout);
#endif

  msg_startline_init (&((*sip)->strtline));
  (*sip)->accepts = (list_t *) smalloc (sizeof (list_t));
  list_init ((*sip)->accepts);
  (*sip)->accept_encodings = (list_t *) smalloc (sizeof (list_t));
  list_init ((*sip)->accept_encodings);
  (*sip)->accept_languages = (list_t *) smalloc (sizeof (list_t));
  list_init ((*sip)->accept_languages);
  (*sip)->alert_infos = (list_t *) smalloc (sizeof (list_t));
  list_init ((*sip)->alert_infos);
  (*sip)->allows = (list_t *) smalloc (sizeof (list_t));
  list_init ((*sip)->allows);
  (*sip)->authorizations = (list_t *) smalloc (sizeof (list_t));
  list_init ((*sip)->authorizations);
  (*sip)->call_id = NULL;
  (*sip)->call_infos = (list_t *) smalloc (sizeof (list_t));
  list_init ((*sip)->call_infos);
  (*sip)->contacts = (list_t *) smalloc (sizeof (list_t));
  list_init ((*sip)->contacts);

  /* forget it: this feild is not suported */
  (*sip)->content_dispositions = NULL;

  (*sip)->content_encodings = (list_t *) smalloc (sizeof (list_t));
  list_init ((*sip)->content_encodings);
  (*sip)->contentlength = NULL;
  (*sip)->content_type = NULL;
  (*sip)->cseq = NULL;
  (*sip)->error_infos = (list_t *) smalloc (sizeof (list_t));
  list_init ((*sip)->error_infos);
  (*sip)->from = NULL;
  (*sip)->mime_version = NULL;
  (*sip)->proxy_authenticates = (list_t *) smalloc (sizeof (list_t));
  list_init ((*sip)->proxy_authenticates);
  (*sip)->proxy_authorizations = (list_t *) smalloc (sizeof (list_t));
  list_init ((*sip)->proxy_authorizations);
  (*sip)->record_routes = (list_t *) smalloc (sizeof (list_t));
  list_init ((*sip)->record_routes);
  (*sip)->routes = (list_t *) smalloc (sizeof (list_t));
  list_init ((*sip)->routes);
  (*sip)->to = NULL;
  (*sip)->vias = (list_t *) smalloc (sizeof (list_t));
  list_init ((*sip)->vias);
  (*sip)->www_authenticates = (list_t *) smalloc (sizeof (list_t));
  list_init ((*sip)->www_authenticates);

  (*sip)->bodies = (list_t *) smalloc (sizeof (list_t));
  list_init ((*sip)->bodies);

  (*sip)->headers = (list_t *) smalloc (sizeof (list_t));
  list_init ((*sip)->headers);

#ifdef USE_TMP_BUFFER
  (*sip)->message_property = 3;
  (*sip)->message = NULL;	/* buffer to avoid calling msg_2char many times (for retransmission) */
#endif
  return 0;			/* ok */
}


void
msg_setreasonphrase (sip_t * sip, char *reason)
{
  sip->strtline->reasonphrase = reason;
}

void
msg_setstatuscode (sip_t * sip, char *statuscode)
{
  sip->strtline->statuscode = statuscode;
}

void
msg_setmethod (sip_t * sip, char *sipmethod)
{
  sip->strtline->sipmethod = sipmethod;
}

void
msg_setversion (sip_t * sip, char *version)
{
  sip->strtline->sipversion = version;
}

void
msg_seturi (sip_t * sip, url_t * url)
{
  sip->strtline->rquri = url;
}

void
msg_free (sip_t * sip)
{
  int pos = 0;

#ifdef MEMORY_LEAKS
  static int comptr = 0;

  if (sip == NULL)
    return;
  comptr--;
  freesipcptr--;
  osip_trace (__FILE__, __LINE__, TRACE_LEVEL0, stdout,
	      "<msg_write.c> msg_free() = %i, malloc-free = %i, address = %x\n",
	      comptr, freesipcptr, sip);
#endif
  if (sip == NULL)
    return;
  msg_startline_free (sip->strtline);
  sfree (sip->strtline);

  {
    accept_t *accept;

    while (!list_eol (sip->accepts, pos))
      {
	accept = (accept_t *) list_get (sip->accepts, pos);
	list_remove (sip->accepts, pos);
	accept_free (accept);
	sfree (accept);
      }
    sfree (sip->accepts);
  }
  {
    accept_encoding_t *accept_encoding;

    while (!list_eol (sip->accept_encodings, pos))
      {
	accept_encoding =
	  (accept_encoding_t *) list_get (sip->accept_encodings, pos);
	list_remove (sip->accept_encodings, pos);
	accept_encoding_free (accept_encoding);
	sfree (accept_encoding);
      }
    sfree (sip->accept_encodings);
  }
  {
    accept_language_t *accept_language;

    while (!list_eol (sip->accept_languages, pos))
      {
	accept_language =
	  (accept_language_t *) list_get (sip->accept_languages, pos);
	list_remove (sip->accept_languages, pos);
	accept_language_free (accept_language);
	sfree (accept_language);
      }
    sfree (sip->accept_languages);
  }
  {
    alert_info_t *alert_info;

    while (!list_eol (sip->alert_infos, pos))
      {
	alert_info = (alert_info_t *) list_get (sip->alert_infos, pos);
	list_remove (sip->alert_infos, pos);
	alert_info_free (alert_info);
	sfree (alert_info);
      }
    sfree (sip->alert_infos);
  }
  {
    allow_t *al;

    while (!list_eol (sip->allows, pos))
      {
	al = (allow_t *) list_get (sip->allows, pos);
	list_remove (sip->allows, pos);
	allow_free (al);
	sfree (al);
      }
    sfree (sip->allows);
  }
  {
    authorization_t *al;

    while (!list_eol (sip->authorizations, pos))
      {
	al = (authorization_t *) list_get (sip->authorizations, pos);
	list_remove (sip->authorizations, pos);
	authorization_free (al);
	sfree (al);
      }
    sfree (sip->authorizations);
  }
  if (sip->call_id != NULL)
    {
      call_id_free (sip->call_id);
      sfree (sip->call_id);
    }
  {
    call_info_t *call_info;

    while (!list_eol (sip->call_infos, pos))
      {
	call_info = (call_info_t *) list_get (sip->call_infos, pos);
	list_remove (sip->call_infos, pos);
	call_info_free (call_info);
	sfree (call_info);
      }
    sfree (sip->call_infos);
  }
  {
    contact_t *contact;

    while (!list_eol (sip->contacts, pos))
      {
	contact = (contact_t *) list_get (sip->contacts, pos);
	list_remove (sip->contacts, pos);
	contact_free (contact);
	sfree (contact);
      }
    sfree (sip->contacts);
  }
  {
    content_encoding_t *ce;

    while (!list_eol (sip->content_encodings, pos))
      {
	ce = (content_encoding_t *) list_get (sip->content_encodings, pos);
	list_remove (sip->content_encodings, pos);
	content_encoding_free (ce);
	sfree (ce);
      }
    sfree (sip->content_encodings);
  }
  if (sip->contentlength != NULL)
    {
      content_length_free (sip->contentlength);
      sfree (sip->contentlength);
    }
  if (sip->content_type != NULL)
    {
      content_type_free (sip->content_type);
      sfree (sip->content_type);
    }
  if (sip->cseq != NULL)
    {
      cseq_free (sip->cseq);
      sfree (sip->cseq);
    }
  {
    error_info_t *error_info;

    while (!list_eol (sip->error_infos, pos))
      {
	error_info = (error_info_t *) list_get (sip->error_infos, pos);
	list_remove (sip->error_infos, pos);
	error_info_free (error_info);
	sfree (error_info);
      }
    sfree (sip->error_infos);
  }
  if (sip->from != NULL)
    {
      from_free (sip->from);
      sfree (sip->from);
    }
  if (sip->mime_version != NULL)
    {
      mime_version_free (sip->mime_version);
      sfree (sip->mime_version);
    }
  {
    proxy_authenticate_t *al;

    while (!list_eol (sip->proxy_authenticates, pos))
      {
	al =
	  (proxy_authenticate_t *) list_get (sip->proxy_authenticates, pos);
	list_remove (sip->proxy_authenticates, pos);
	proxy_authenticate_free (al);
	sfree (al);
      }
    sfree (sip->proxy_authenticates);
  }
  {
    proxy_authorization_t *proxy_authorization;

    while (!list_eol (sip->proxy_authorizations, pos))
      {
	proxy_authorization =
	  (proxy_authorization_t *) list_get (sip->proxy_authorizations, pos);
	list_remove (sip->proxy_authorizations, pos);
	proxy_authorization_free (proxy_authorization);
	sfree (proxy_authorization);
      }
    sfree (sip->proxy_authorizations);
  }
  {
    record_route_t *record_route;

    while (!list_eol (sip->record_routes, pos))
      {
	record_route = (record_route_t *) list_get (sip->record_routes, pos);
	list_remove (sip->record_routes, pos);
	record_route_free (record_route);
	sfree (record_route);
      }
    sfree (sip->record_routes);
  }
  {
    route_t *route;

    while (!list_eol (sip->routes, pos))
      {
	route = (route_t *) list_get (sip->routes, pos);
	list_remove (sip->routes, pos);
	route_free (route);
	sfree (route);
      }
    sfree (sip->routes);
  }
  if (sip->to != NULL)
    {
      to_free (sip->to);
      sfree (sip->to);
    }
  {
    via_t *via;

    while (!list_eol (sip->vias, pos))
      {
	via = (via_t *) list_get (sip->vias, pos);
	list_remove (sip->vias, pos);
	via_free (via);
	sfree (via);
      }
    sfree (sip->vias);
  }
  {
    www_authenticate_t *al;

    while (!list_eol (sip->www_authenticates, pos))
      {
	al = (www_authenticate_t *) list_get (sip->www_authenticates, pos);
	list_remove (sip->www_authenticates, pos);
	www_authenticate_free (al);
	sfree (al);
      }
    sfree (sip->www_authenticates);
  }

  {
    header_t *header;

    while (!list_eol (sip->headers, pos))
      {
	header = (header_t *) list_get (sip->headers, pos);
	list_remove (sip->headers, pos);
	header_free (header);
	sfree (header);
      }
    sfree (sip->headers);
  }

  {
    body_t *body;

    while (!list_eol (sip->bodies, pos))
      {
	body = (body_t *) list_get (sip->bodies, pos);
	list_remove (sip->bodies, pos);
	body_free (body);
	sfree (body);
      }
    sfree (sip->bodies);
  }
#ifdef USE_TMP_BUFFER
  sfree (sip->message);
#endif
}

int
body_clone (body_t * body, body_t ** dest)
{
  int pos;
  int i;
  body_t *copy;

  if (body == NULL)
    return -1;

  i = body_init (&copy);
  if (i != 0)
    return -1;

  copy->body = sgetcopy (body->body);

  if (body->content_type != NULL)
    {
      i = content_type_clone (body->content_type, &(copy->content_type));
      if (i != 0)
	goto bc_error1;
    }

  {
    header_t *header;
    header_t *header2;

    pos = 0;
    while (!list_eol (body->headers, pos))
      {
	header = (header_t *) list_get (body->headers, pos);
	i = header_clone (header, &header2);
	if (i != 0)
	  goto bc_error1;
	list_add (copy->headers, header2, -1);	/* insert as last element */
	pos++;
      }
  }

  *dest = copy;
  return 0;
bc_error1:
  body_free (copy);
  sfree (copy);
  return -1;
}

int
msg_clone (sip_t * sip, sip_t ** dest)
{
  sip_t *copy;
  int pos = 0;
  int i;

  if (sip == NULL)
    return -1;
  *dest = NULL;

  i = msg_init (&copy);
  if (i != 0)
    return -1;

  copy->strtline->sipmethod = sgetcopy (sip->strtline->sipmethod);
  copy->strtline->sipversion = sgetcopy (sip->strtline->sipversion);
  copy->strtline->statuscode = sgetcopy (sip->strtline->statuscode);
  copy->strtline->reasonphrase = sgetcopy (sip->strtline->reasonphrase);
  if (sip->strtline->rquri != NULL)
    {
      i = url_clone (sip->strtline->rquri, &(copy->strtline->rquri));
      if (i != 0)
	goto mc_error1;
    }

  {
    accept_t *accept;
    accept_t *accept2;

    pos = 0;
    while (!list_eol (sip->accepts, pos))
      {
	accept = (accept_t *) list_get (sip->accepts, pos);
	i = accept_clone (accept, &accept2);
	if (i != 0)
	  goto mc_error1;
	list_add (copy->accepts, accept2, -1);	/* insert as last element */
	pos++;
      }
  }
  {
    accept_encoding_t *accept_encoding;
    accept_encoding_t *accept_encoding2;

    pos = 0;
    while (!list_eol (sip->accept_encodings, pos))
      {
	accept_encoding =
	  (accept_encoding_t *) list_get (sip->accept_encodings, pos);
	i = accept_encoding_clone (accept_encoding, &accept_encoding2);
	if (i != 0)
	  goto mc_error1;
	list_add (copy->accept_encodings, accept_encoding2, -1);
	pos++;
      }
  }
  {
    accept_language_t *accept_language;
    accept_language_t *accept_language2;

    pos = 0;
    while (!list_eol (sip->accept_languages, pos))
      {
	accept_language =
	  (accept_language_t *) list_get (sip->accept_languages, pos);
	i = accept_language_clone (accept_language, &accept_language2);
	if (i != 0)
	  goto mc_error1;
	list_add (copy->accept_languages, accept_language2, -1);
	pos++;
      }
  }
  {
    alert_info_t *alert_info;
    alert_info_t *alert_info2;

    pos = 0;
    while (!list_eol (sip->alert_infos, pos))
      {
	alert_info = (alert_info_t *) list_get (sip->alert_infos, pos);
	i = alert_info_clone (alert_info, &alert_info2);
	if (i != 0)
	  goto mc_error1;
	list_add (copy->alert_infos, alert_info2, -1);
	pos++;
      }
  }
  {
    allow_t *allow;
    allow_t *allow2;

    pos = 0;
    while (!list_eol (sip->allows, pos))
      {
	allow = (allow_t *) list_get (sip->allows, pos);
	i = allow_clone (allow, &allow2);
	if (i != 0)
	  goto mc_error1;
	list_add (copy->allows, allow2, -1);
	pos++;
      }
  }
  {
    authorization_t *authorization;
    authorization_t *authorization2;

    pos = 0;
    while (!list_eol (sip->authorizations, pos))
      {
	authorization =
	  (authorization_t *) list_get (sip->authorizations, pos);
	i = authorization_clone (authorization, &authorization2);
	if (i != 0)
	  goto mc_error1;
	list_add (copy->authorizations, authorization2, -1);
	pos++;
      }
  }
  if (sip->call_id != NULL)
    {
      i = call_id_clone (sip->call_id, &(copy->call_id));
      if (i != 0)
	goto mc_error1;
    }
  {
    call_info_t *call_info;
    call_info_t *call_info2;

    pos = 0;
    while (!list_eol (sip->call_infos, pos))
      {
	call_info = (call_info_t *) list_get (sip->call_infos, pos);
	i = call_info_clone (call_info, &call_info2);
	if (i != 0)
	  goto mc_error1;
	list_add (copy->call_infos, call_info2, -1);
	pos++;
      }
  }
  {
    contact_t *contact;
    contact_t *contact2;

    pos = 0;
    while (!list_eol (sip->contacts, pos))
      {
	contact = (contact_t *) list_get (sip->contacts, pos);
	i = contact_clone (contact, &contact2);
	if (i != 0)
	  goto mc_error1;
	list_add (copy->contacts, contact2, -1);
	pos++;
      }
  }
  {
    content_encoding_t *content_encoding;
    content_encoding_t *content_encoding2;

    pos = 0;
    while (!list_eol (sip->content_encodings, pos))
      {
	content_encoding =
	  (content_encoding_t *) list_get (sip->content_encodings, pos);
	i = content_encoding_clone (content_encoding, &content_encoding2);
	if (i != 0)
	  goto mc_error1;
	list_add (copy->content_encodings, content_encoding2, -1);
	pos++;
      }
  }
  if (sip->contentlength != NULL)
    {
      i = content_length_clone (sip->contentlength, &(copy->contentlength));
      if (i != 0)
	goto mc_error1;
    }
  if (sip->content_type != NULL)
    {
      i = content_type_clone (sip->content_type, &(copy->content_type));
      if (i != 0)
	goto mc_error1;
    }
  if (sip->cseq != NULL)
    {
      i = cseq_clone (sip->cseq, &(copy->cseq));
      if (i != 0)
	goto mc_error1;
    }
  {
    error_info_t *error_info;
    error_info_t *error_info2;

    pos = 0;
    while (!list_eol (sip->error_infos, pos))
      {
	error_info = (error_info_t *) list_get (sip->error_infos, pos);
	i = error_info_clone (error_info, &error_info2);
	if (i != 0)
	  goto mc_error1;
	list_add (copy->error_infos, error_info2, -1);
	pos++;
      }
  }
  if (sip->from != NULL)
    {
      i = from_clone (sip->from, &(copy->from));
      if (i != 0)
	goto mc_error1;
    }
  if (sip->mime_version != NULL)
    {
      i = mime_version_clone (sip->mime_version, &(copy->mime_version));
      if (i != 0)
	goto mc_error1;
    }
  {
    proxy_authenticate_t *proxy_authenticate;
    proxy_authenticate_t *proxy_authenticate2;

    pos = 0;
    while (!list_eol (sip->proxy_authenticates, pos))
      {
	proxy_authenticate =
	  (proxy_authenticate_t *) list_get (sip->proxy_authenticates, pos);
	i =
	  proxy_authenticate_clone (proxy_authenticate, &proxy_authenticate2);
	if (i != 0)
	  goto mc_error1;
	list_add (copy->proxy_authenticates, proxy_authenticate2, -1);
	pos++;
      }
  }
  {
    proxy_authorization_t *proxy_authorization;
    proxy_authorization_t *proxy_authorization2;

    pos = 0;
    while (!list_eol (sip->proxy_authorizations, pos))
      {
	proxy_authorization =
	  (proxy_authorization_t *) list_get (sip->proxy_authorizations, pos);
	i =
	  proxy_authorization_clone (proxy_authorization,
				     &proxy_authorization2);
	if (i != 0)
	  goto mc_error1;
	list_add (copy->proxy_authorizations, proxy_authorization2, -1);
	pos++;
      }
  }
  {
    record_route_t *record_route;
    record_route_t *record_route2;

    pos = 0;
    while (!list_eol (sip->record_routes, pos))
      {
	record_route = (record_route_t *) list_get (sip->record_routes, pos);
	i = record_route_clone (record_route, &record_route2);
	if (i != 0)
	  goto mc_error1;
	list_add (copy->record_routes, record_route2, -1);
	pos++;
      }
  }
  {
    route_t *route;
    route_t *route2;

    pos = 0;
    while (!list_eol (sip->routes, pos))
      {
	route = (route_t *) list_get (sip->routes, pos);
	i = route_clone (route, &route2);
	if (i != 0)
	  goto mc_error1;
	list_add (copy->routes, route2, -1);
	pos++;
      }
  }
  if (sip->to != NULL)
    {
      i = to_clone (sip->to, &(copy->to));
      if (i != 0)
	goto mc_error1;
    }
  {
    via_t *via;
    via_t *via2;

    pos = 0;
    while (!list_eol (sip->vias, pos))
      {
	via = (via_t *) list_get (sip->vias, pos);
	i = via_clone (via, &via2);
	if (i != 0)
	  goto mc_error1;
	list_add (copy->vias, via2, -1);
	pos++;
      }
  }
  {
    www_authenticate_t *www_authenticate;
    www_authenticate_t *www_authenticate2;

    pos = 0;
    while (!list_eol (sip->www_authenticates, pos))
      {
	www_authenticate =
	  (www_authenticate_t *) list_get (sip->www_authenticates, pos);
	i = www_authenticate_clone (www_authenticate, &www_authenticate2);
	if (i != 0)
	  goto mc_error1;
	list_add (copy->www_authenticates, www_authenticate2, -1);
	pos++;
      }
  }

  {
    header_t *header;
    header_t *header2;

    pos = 0;
    while (!list_eol (sip->headers, pos))
      {
	header = (header_t *) list_get (sip->headers, pos);
	i = header_clone (header, &header2);
	if (i != 0)
	  goto mc_error1;
	list_add (copy->headers, header2, -1);
	pos++;
      }
  }

  {
    body_t *body;
    body_t *body2;

    pos = 0;
    while (!list_eol (sip->bodies, pos))
      {
	body = (body_t *) list_get (sip->bodies, pos);
	i = body_clone (body, &body2);
	if (i != 0)
	  goto mc_error1;
	list_add (copy->bodies, body2, -1);
	pos++;
      }
  }

#ifdef USE_TMP_BUFFER
  copy->message = sgetcopy (sip->message);
  copy->message_property = sip->message_property;
#endif

  *dest = copy;
  return 0;
mc_error1:
  msg_free (copy);
  sfree (copy);
  return -1;

}
