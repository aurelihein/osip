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

#ifndef _SMSGTYPES_H_
#define _SMSGTYPES_H_

#include <osip/list.h>
#include <osip/urls.h>


/**
 * @file smsgtypes.h
 * @brief oSIP type definitions
 */

/**
 * @defgroup oSIP_TYPES oSIP type definitions
 * @ingroup oSIP
 * @{
 */


#ifdef __cplusplus
extern "C"
{
#endif

#ifndef SIP_MESSAGE_MAX_LENGTH
/**
 * You can re-define your own maximum length for SIP message.
 * <BR>The default value is 20000 characters. (which is much more that the MTU!)
 * <BR>BUG: If you try to build a message string (by calling msg_2char), with
 * a higher length, your application will crash. Anybody building a commercial
 * application should fix that behavior (and hopefully send the code back to me!)
 */
#define SIP_MESSAGE_MAX_LENGTH 4000
#endif

#ifndef BODY_MESSAGE_MAX_SIZE
/**
 * You can define the maximum authorised length for a body inside a SIP message.
 */
#define BODY_MESSAGE_MAX_SIZE  500
#endif

/**
 * Structure for startline (1st line of SIP message either REQUEST and RESPONSE).
 * @defvar startline_t
 */
  typedef struct startline_t startline_t;

  struct startline_t
  {
    /* msgevttype_t method; */
    char *sipmethod;
    char *sipversion;

    /* req */
    url_t *rquri;

    /* resp */
    char *statuscode;
    char *reasonphrase;

  };

/**
 * Structure for 'unknown' headers.
 * NOTE: 'unknown' header' are used in oSIP for all header that are not
 * defined by oSIP in the sip_t structure. This means that all 'unknown'
 * header has to be handled with the API related to this structure.
 * @defvar startline_t
 */
  typedef struct header_t header_t;

  struct header_t
  {
    char *hname;
    char *hvalue;
  };

/**
 * Structure for CSeq headers.
 * @defvar cseq_t
 */
  typedef struct cseq_t cseq_t;

  struct cseq_t
  {
    char *method;
    char *number;
  };

/**
 * Structure for Via headers.
 * @defvar via_t
 */
  typedef struct via_t via_t;

  struct via_t
  {
    char *version;
    char *protocol;
    char *host;
    char *port;
    char *comment;
    list_t *via_params;
  };

/**
 * Structure for generic parameter headers.
 * Generic parameter are used in a lot of headers. (To, From, Route,
 * Record-Route...) All those headers use a common API but this is
 * hidden by MACROs that you can be found in smsg.h.
 * @defvar cseq_t
 */
  typedef url_param_t generic_param_t;

/**
 * Structure for From headers.
 * @defvar from_t
 */
  typedef struct from_t from_t;

  struct from_t
  {
    char *displayname;
    url_t *url;			/* could contain various urischeme_t ? only in the future */
    list_t *gen_params;
  };

/**
 * Structure for To headers.
 * @defvar to_t
 */
  typedef from_t to_t;
/**
 * Structure for Contact headers.
 * @defvar contact_t
 */
  typedef from_t contact_t;
/**
 * Structure for Record-Route headers.
 * @defvar record_route_t
 */
  typedef from_t record_route_t;
/**
 * Structure for Route headers.
 * @defvar route_t
 */
  typedef from_t route_t;

/**
 * Structure for Call-Id headers.
 * @defvar call_id_t
 */
  typedef struct call_id_t call_id_t;

  struct call_id_t
  {
    char *number;
    char *host;
  };

/**
 * Structure for Content-Length headers.
 * @defvar content_length_t
 */
  typedef struct content_length_t content_length_t;

  struct content_length_t
  {
    char *value;
  };

/**
 * Structure for Language-Tag headers. - NOT IMPLEMENTED -
 * @defvar language_tag_t
 */
  typedef struct language_tag_t language_tag_t;

  struct language_tag_t
  {
    list_t *tags;
  };

/**
 * Structure for Allow headers.
 * @defvar allow_t
 */
  typedef content_length_t allow_t;
/**
 * Structure for Content-Encoding headers.
 * @defvar content_encoding_t
 */
  typedef content_length_t content_encoding_t;
/**
 * Structure for Mime-Version headers.
 * @defvar mime_version_t
 */
  typedef content_length_t mime_version_t;

/* typedef url_param_t content_type_param_t; */

/**
 * Structure for Content-Type headers.
 * @defvar content_type_t
 */
  typedef struct content_type_t content_type_t;

  struct content_type_t
  {
    char *type;
    char *subtype;
    list_t *gen_params;
  };

/**
 * Structure for accept headers.
 * @defvar accept_t
 */
  typedef content_type_t accept_t;

/**
 * Structure for Accept-Encoding headers.
 * @defvar accept_encoding_t
 */
  typedef struct accept_encoding_t accept_encoding_t;

  struct accept_encoding_t
  {
    char *element;
    list_t *gen_params;
  };

/**
 * Structure for Accept-Language headers.
 * @defvar accept_language_t
 */
  typedef accept_encoding_t accept_language_t;

/**
 * Structure for Call-Info headers.
 * @defvar call_info_t
 */
  typedef struct call_info_t call_info_t;

  struct call_info_t
  {
    char *element;
    list_t *gen_params;
  };

/**
 * Structure for Alert-Info headers.
 * @defvar alert_info_t
 */
  typedef call_info_t alert_info_t;
/**
 * Structure for Error-Info headers.
 * @defvar error_info_t
 */
  typedef call_info_t error_info_t;

/**
 * Structure for Content-Disposition headers.
 * @defvar content_disposition_t
 */
  typedef call_info_t content_disposition_t;
/**
 * Structure for encryption headers. - NOT IMPLEMENTED -
 * @defvar encryption_t
 */
  typedef call_info_t encryption_t;

/**
 * Structure for WWW-Authenticate headers.
 * @defvar www_authenticate_t
 */
  typedef struct www_authenticate_t www_authenticate_t;

  struct www_authenticate_t
  {
    char *auth_type;		/* ( "Basic" | "Digest" )      */
    char *realm;		/* mandatory ( quoted-string ) */
    char *domain;		/* optional  <"> 1#URI <">     */
    char *nonce;		/* mandatory */
    char *opaque;		/* optional  */
    char *stale;		/* optional  ( "true" | "false" ) */
    char *algorithm;		/* optional  ( "MD5"  | token   ) */
    char *qop_options;		/* optional  */
    char *auth_param;		/* optional  */
  };

/**
 * Structure for Proxy-Authenticate headers.
 * @defvar proxy_authenticate_t
 */
  typedef www_authenticate_t proxy_authenticate_t;

/**
 * Structure for Authorization headers.
 * @defvar authorization_t
 */
  typedef struct authorization_t authorization_t;

  struct authorization_t
  {
    char *auth_type;		/* ( "Basic" | "Digest" )      */
    char *username;
    char *realm;		/* mandatory ( quoted-string ) */
    char *nonce;
    char *uri;
    char *response;
    char *digest;		/* DO NOT USE IT IN AUTHORIZATION_T HEADER?? */
    char *algorithm;		/* optionnal, default is "md5" */
    char *cnonce;		/* optionnal */
    char *opaque;		/* optionnal */
    char *message_qop;		/* optionnal */
    char *nonce_count;		/* optionnal */
    char *auth_param;		/* optionnal */
  };

/**
 * Structure for Proxy-Authorization headers.
 * @defvar proxy_authorization_t
 */
  typedef authorization_t proxy_authorization_t;

/**
 * Structure for Body - LIGHT SUPPORT FOR MIME FORMAT: TO BE TESTED-.
 * @defvar body_t
 */
  typedef struct body_t body_t;

  struct body_t
  {
    char *body;
    list_t *headers;
    content_type_t *content_type;
    /*  content_length_t   *content_length; */
  };

/**
 * Structure for SIP Message (REQUEST and RESPONSE).
 * @defvar sip_t
 */
  typedef struct sip_t sip_t;

  struct sip_t
  {
    startline_t *strtline;

    list_t *accepts;
    list_t *accept_encodings;
    list_t *accept_languages;
    list_t *alert_infos;
    list_t *allows;
    list_t *authorizations;
    call_id_t *call_id;
    list_t *call_infos;
    list_t *contacts;
    list_t *content_dispositions;
    list_t *content_encodings;
    content_length_t *contentlength;
    content_type_t *content_type;
    cseq_t *cseq;
    list_t *error_infos;
    from_t *from;
    mime_version_t *mime_version;
    list_t *proxy_authenticates;
    list_t *proxy_authorizations;
    list_t *record_routes;
    list_t *routes;
    to_t *to;
    list_t *vias;
    list_t *www_authenticates;

    list_t *headers;

    list_t *bodies;

    /*
       1: structure and buffer "message" are identical.
       2: buffer "message" is not up to date with the structure info (call msg_2char to update it).
     */
    int message_property;
    char *message;
  };


#ifdef __cplusplus
}
#endif

/** @} */

#endif
