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

#ifndef _SMSGTYPES_H_
#define _SMSGTYPES_H_

#include <osip/list.h>
#include <osip/urls.h>

/* for the body headers    */
#ifndef BODY_MESSAGE_MAX_SIZE
#define BODY_MESSAGE_MAX_SIZE  500
#endif
#ifndef SIP_MESSAGE_MAX_LENGTH
#define SIP_MESSAGE_MAX_LENGTH 20000
#endif

typedef struct _startline_t {
  /* msgevttype_t method; */
  char *sipmethod;
  char *sipversion;

  /* req */
  url_t *rquri;

  /* resp */
  char *statuscode;
  char *reasonphrase;

} startline_t ;

typedef struct _header_t {
  char *hname;
  char *hvalue;
} header_t;

typedef struct _cseq_t {
  char *method;
  char *number;
} cseq_t;

typedef struct _via_t {
  char *version;
  char *protocol;
  char *host;
  char *port;
  char *comment;
  list_t *via_params;
} via_t;

typedef url_param_t generic_param_t;

typedef struct _from_t {
  char *displayname;
  url_t *url;  /* could contain various urischeme_t ? only in the future */
  list_t *gen_params;
} from_t;

typedef from_t to_t;
typedef from_t contact_t;
typedef from_t record_route_t;
typedef from_t route_t;

typedef struct _call_id_t {
  char *number;
  char *host;
} call_id_t;

typedef struct _content_length_t {
  char *value;
} content_length_t;

typedef struct _language_tag_t {
  list_t *tags;
} language_tag_t;

typedef content_length_t allow_t;
typedef content_length_t content_encoding_t;
typedef content_length_t mime_version_t;

/* typedef url_param_t content_type_param_t; */

typedef struct _content_type_t {
  char   *type;
  char   *subtype;
  list_t *gen_params;
} content_type_t;

typedef content_type_t accept_t;

typedef struct _accept_encoding_t {
  char   *element;
  list_t *gen_params;
} accept_encoding_t;

typedef accept_encoding_t accept_language_t;

typedef struct _call_info_t {
  char *element;
  list_t *gen_params;
} call_info_t;

typedef call_info_t alert_info_t;
typedef call_info_t error_info_t;

typedef call_info_t content_disposition_t;
typedef call_info_t encryption_t;

typedef struct _www_authenticate_t {
  char *auth_type;   /* ( "Basic" | "Digest" )      */
  char *realm;       /* mandatory ( quoted-string ) */
  char *domain;      /* optional  <"> 1#URI <">     */
  char *nonce;       /* mandatory */
  char *opaque;      /* optional  */
  char *stale;       /* optional  ( "true" | "false" ) */
  char *algorithm;   /* optional  ( "MD5"  | token   ) */
  char *qop_options; /* optional  */
  char *auth_param; /* optional  */
} www_authenticate_t;

typedef www_authenticate_t proxy_authenticate_t;

typedef struct _authorization_t {
  char *auth_type;   /* ( "Basic" | "Digest" )      */
  char *username;
  char *realm;       /* mandatory ( quoted-string ) */
  char *nonce;
  char *uri;
  char *response; 
  char *digest; /* DO NOT USE IT IN AUTHORIZATION_T HEADER?? */
  char *algorithm;  /* optionnal, default is "md5" */
  char *cnonce; /* optionnal */
  char *opaque; /* optionnal */
  char *message_qop; /* optionnal */
  char *nonce_count; /* optionnal */
  char *auth_param; /* optionnal */
} authorization_t;

typedef authorization_t proxy_authorization_t;

typedef struct _body_t {
  char   *body;
  list_t *headers;
  content_type_t *content_type;
  /*  content_length_t   *content_length; */
} body_t;

typedef struct _sip_t {
  startline_t  *strtline;

  list_t    *accepts;
  list_t    *accept_encodings;
  list_t    *accept_languages;
  list_t    *alert_infos;
  list_t    *allows;
  authorization_t *authorization;
  call_id_t *call_id;
  list_t    *call_infos;
  list_t    *contacts;
  list_t    *content_dispositions;
  list_t    *content_encodings;
  content_length_t *contentlength;
  content_type_t   *content_type;
  cseq_t    *cseq;
  list_t    *error_infos;
  from_t    *from;
  mime_version_t   *mime_version;
  proxy_authenticate_t *proxy_authenticate;
  list_t    *proxy_authorizations;
  list_t    *record_routes;
  list_t    *routes;
  to_t      *to;
  list_t    *vias;
  www_authenticate_t *www_authenticate;
  
  list_t    *headers;

  list_t    *bodies;

  /*
    1: structure and buffer "message" are identical.
    2: buffer "message" is not up to date with the structure info (call msg_2char to update it).
  */
  int   message_property; 
  char *message;
} sip_t;


#endif
