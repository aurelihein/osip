/*
  The oSIP library implements the Session Initiation Protocol (SIP -rfc2543-)
  Copyright (C) 2001  Aymeric MOIZARD jack@atosc.org
  
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
msg_startline_init(startline_t **strtline)
{
  *strtline = (startline_t *)smalloc(sizeof(startline_t));
  (*strtline)->sipmethod    = NULL;
  (*strtline)->sipversion   = NULL;
  (*strtline)->statuscode   = NULL;
  (*strtline)->reasonphrase = NULL;
  (*strtline)->rquri        = NULL;
}

static void
msg_startline_free(startline_t *strtline)
{
  if (strtline==NULL) return;
  sfree(strtline->sipmethod);
  sfree(strtline->sipversion);
  if (strtline->rquri!=NULL)
    {
      url_free(strtline->rquri);
      sfree(strtline->rquri);
    }
  sfree(strtline->statuscode);
  sfree(strtline->reasonphrase);
}


int
msg_init(sip_t **sip) {
#ifdef MEMORY_LEAKS
  static int comptr = 0;
  comptr++;
  freesipcptr++;
#endif
  *sip = (sip_t *) smalloc(sizeof(sip_t));
#ifdef MEMORY_LEAKS
  trace(__FILE__,__LINE__,TRACE_LEVEL0,stdout,"<msg_write.c> msg_init() = %i, malloc-free = %i, address = %x\n",comptr, freesipcptr, *sip);
  fflush(stdout); 
#endif

  msg_startline_init(&((*sip)->strtline));
  (*sip)->accepts = (list_t *)smalloc(sizeof(list_t));
  list_init((*sip)->accepts);
  (*sip)->accept_encodings = (list_t *)smalloc(sizeof(list_t));
  list_init((*sip)->accept_encodings);
  (*sip)->accept_languages = (list_t *)smalloc(sizeof(list_t));
  list_init((*sip)->accept_languages);
  (*sip)->alert_infos = (list_t *)smalloc(sizeof(list_t));
  list_init((*sip)->alert_infos);
  (*sip)->allows = (list_t *)smalloc(sizeof(list_t));
  list_init((*sip)->allows);
  (*sip)->authorization  = NULL;
  (*sip)->call_id       = NULL;
  (*sip)->call_infos = (list_t *)smalloc(sizeof(list_t));
  list_init((*sip)->call_infos);
  (*sip)->contacts = (list_t *)smalloc(sizeof(list_t));
  list_init((*sip)->contacts);
  (*sip)->content_encodings = (list_t *)smalloc(sizeof(list_t));
  list_init((*sip)->content_encodings);
  (*sip)->contentlength = NULL;
  (*sip)->content_type  = NULL;
  (*sip)->cseq          = NULL;
  (*sip)->error_infos = (list_t *)smalloc(sizeof(list_t));
  list_init((*sip)->error_infos);
  (*sip)->from          = NULL;
  (*sip)->mime_version  = NULL;
  (*sip)->proxy_authenticate  = NULL;
  (*sip)->proxy_authorizations = (list_t *)smalloc(sizeof(list_t));
  list_init((*sip)->proxy_authorizations);
  (*sip)->record_routes = (list_t *)smalloc(sizeof(list_t));
  list_init((*sip)->record_routes);
  (*sip)->routes = (list_t *)smalloc(sizeof(list_t));
  list_init((*sip)->routes);
  (*sip)->to            = NULL;
  (*sip)->vias = (list_t *)smalloc(sizeof(list_t));
  list_init((*sip)->vias);
  (*sip)->www_authenticate  = NULL;

  (*sip)->bodies = (list_t *)smalloc(sizeof(list_t));
  list_init((*sip)->bodies);

  (*sip)->headers = (list_t *)smalloc(sizeof(list_t));
  list_init((*sip)->headers);

#ifdef USE_TMP_BUFFER
  (*sip)->message_property = 3;
  (*sip)->message = NULL;   /* buffer to avoid calling msg_2char many times (for retransmission) */
#endif
  return 0; /* ok */
}


void
msg_setreasonphrase(sip_t *sip, char *reason)
{
  sip->strtline->reasonphrase = reason;
}
void
msg_setstatuscode(sip_t *sip, char *statuscode)
{
  sip->strtline->statuscode = statuscode;
}
void
msg_setmethod(sip_t *sip, char *sipmethod)
{
  sip->strtline->sipmethod = sipmethod;
}
void
msg_setversion(sip_t *sip, char *version)
{
  sip->strtline->sipversion = version;
}
void
msg_seturi(sip_t *sip, url_t *url)
{
  sip->strtline->rquri = url;
}

void
msg_free(sip_t *sip)
{
  int pos = 0;
#ifdef MEMORY_LEAKS
  static int comptr = 0;
  if (sip==NULL) return;
  comptr--;
  freesipcptr--;
  trace(__FILE__,__LINE__,TRACE_LEVEL0,stdout,"<msg_write.c> msg_free() = %i, malloc-free = %i, address = %x\n",comptr, freesipcptr, sip);
#endif
  if (sip==NULL) return;
  msg_startline_free(sip->strtline);
  sfree(sip->strtline);
  
  {
    accept_t *accept;
    while (!list_eol(sip->accepts,pos))
      {
	accept = (accept_t *)list_get(sip->accepts,pos);
	list_remove(sip->accepts,pos);
	accept_free(accept);
	sfree(accept);
      }
    sfree(sip->accepts);
  }
  {
    accept_encoding_t *accept_encoding;
    while (!list_eol(sip->accept_encodings,pos))
      {
	accept_encoding = (accept_encoding_t *)list_get(sip->accept_encodings,pos);
	list_remove(sip->accept_encodings,pos);
	accept_encoding_free(accept_encoding);
	sfree(accept_encoding);
      }
    sfree(sip->accept_encodings);
  }
  {
    accept_language_t *accept_language;
    while (!list_eol(sip->accept_languages,pos))
      {
	accept_language = (accept_language_t *)list_get(sip->accept_languages,pos);
	list_remove(sip->accept_languages,pos);
	accept_language_free(accept_language);
	sfree(accept_language);
      }
    sfree(sip->accept_languages);
  }
  {
    alert_info_t *alert_info;
    while (!list_eol(sip->alert_infos,pos))
      {
	alert_info = (alert_info_t *)list_get(sip->alert_infos,pos);
	list_remove(sip->alert_infos,pos);
	alert_info_free(alert_info);
	sfree(alert_info);
      }
    sfree(sip->alert_infos);
  }
  {
    allow_t *al;
    while (!list_eol(sip->allows,pos))
      {
	al = (allow_t *)list_get(sip->allows,pos);
	list_remove(sip->allows,pos);
	allow_free(al);
	sfree(al);
      }
    sfree(sip->allows);
  }
  if (sip->authorization!=NULL)
    {
      authorization_free (sip->authorization);
      sfree(sip->authorization);
    }
  if (sip->call_id!=NULL)
    {
      call_id_free(sip->call_id);
      sfree(sip->call_id);
    }
  {
    call_info_t *call_info;
    while (!list_eol(sip->call_infos,pos))
      {
	call_info = (call_info_t *)list_get(sip->call_infos,pos);
	list_remove(sip->call_infos,pos);
	call_info_free(call_info);
	sfree(call_info);
      }
    sfree(sip->call_infos);
  }
  {
    contact_t *contact;
    while (!list_eol(sip->contacts,pos))
      {
	contact = (contact_t *)list_get(sip->contacts,pos);
	list_remove(sip->contacts,pos);
	contact_free(contact);
	sfree(contact);
      }
    sfree(sip->contacts);
  }
  {
    content_encoding_t *ce;
    while (!list_eol(sip->content_encodings,pos))
      {
	ce = (content_encoding_t *)list_get(sip->content_encodings,pos);
	list_remove(sip->content_encodings,pos);
	content_encoding_free(ce);
	sfree(ce);
      }
    sfree(sip->content_encodings);
  }
  if (sip->contentlength!=NULL)
    {
      content_length_free (sip->contentlength);
      sfree(sip->contentlength);
    }
  if (sip->content_type!=NULL)
    {
      content_type_free (sip->content_type);
      sfree(sip->content_type);
    }
  if (sip->cseq!=NULL)
    {
      cseq_free (sip->cseq);
      sfree(sip->cseq);
    }
  {
    error_info_t *error_info;
    while (!list_eol(sip->error_infos,pos))
      {
	error_info = (error_info_t *)list_get(sip->error_infos,pos);
	list_remove(sip->error_infos,pos);
	error_info_free(error_info);
	sfree(error_info);
      }
    sfree(sip->error_infos);
  }
  if (sip->from!=NULL)
    {
      from_free(sip->from);
      sfree(sip->from);
    }
  if (sip->mime_version!=NULL)
    {
      mime_version_free (sip->mime_version);
      sfree(sip->mime_version);
    }
  if (sip->proxy_authenticate!=NULL)
    {
      proxy_authenticate_free (sip->proxy_authenticate);
      sfree(sip->proxy_authenticate);
    }
  {
    proxy_authorization_t *proxy_authorization;
    while (!list_eol(sip->proxy_authorizations,pos))
      {
	proxy_authorization = (proxy_authorization_t *)list_get(sip->proxy_authorizations,pos);
	list_remove(sip->proxy_authorizations,pos);
	proxy_authorization_free(proxy_authorization);
	sfree(proxy_authorization);
      }
    sfree(sip->proxy_authorizations);
  }
  {
    record_route_t *record_route;
    while (!list_eol(sip->record_routes,pos))
      {
	record_route = (record_route_t *)list_get(sip->record_routes,pos);
	list_remove(sip->record_routes,pos);
	record_route_free(record_route);
	sfree(record_route);
      }
    sfree(sip->record_routes);
  }
  {
    route_t *route;
    while (!list_eol(sip->routes,pos))
      {
	route = (route_t *)list_get(sip->routes,pos);
	list_remove(sip->routes,pos);
	route_free(route);
	sfree(route);
      }
    sfree(sip->routes);
  }
  if (sip->to!=NULL)
    {
      to_free(sip->to);
      sfree(sip->to);
    }
  {
    via_t *via;
    while (!list_eol(sip->vias,pos))
      {
	via = (via_t *)list_get(sip->vias,pos);
	list_remove(sip->vias,pos);
	via_free(via);
	sfree(via);
      }
    sfree(sip->vias);
  }
  if (sip->www_authenticate!=NULL)
    {
      www_authenticate_free (sip->www_authenticate);
      sfree(sip->www_authenticate);
    }


  {
  header_t *header;
  while (!list_eol(sip->headers,pos))
    {
     header = (header_t *)list_get(sip->headers,pos);
     list_remove(sip->headers,pos);
     header_free(header);
     sfree(header);
    }
  sfree(sip->headers);
  }

  {
  body_t *body;
  while (!list_eol(sip->bodies,pos))
    {
     body = (body_t *)list_get(sip->bodies,pos);
     list_remove(sip->bodies,pos);
     body_free(body);
     sfree(body);
    }
  sfree(sip->bodies);
  }
#ifdef USE_TMP_BUFFER
  sfree(sip->message);
#endif
}

