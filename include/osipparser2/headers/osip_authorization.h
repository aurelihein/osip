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


#ifndef _OSIP_AUTHORIZATION_H_
#define _OSIP_AUTHORIZATION_H_


/**
 * @file osip_authorization.h
 * @brief oSIP osip_authorization header definition.
 */

/**
 * @defgroup oSIP_AUTHORIZATION oSIP authorization header definition.
 * @ingroup oSIP_HEADERS
 * @{
 */

/**
 * Structure for Authorization headers.
 * @defvar osip_authorization_t
 */
  typedef struct osip_authorization osip_authorization_t;

  struct osip_authorization
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


#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Allocate a Authorization element.
 * @param header The element to work on.
 */
  int osip_authorization_init (osip_authorization_t ** header);
/**
 * Parse a Authorization element.
 * @param header The element to work on.
 * @param hvalue The string to parse.
 */
  int osip_authorization_parse (osip_authorization_t * header, const char *hvalue);
/**
 * Get a string representation of a Authorization element.
 * @param header The element to work on.
 * @param dest A pointer on the new allocated string.
 */
  int osip_authorization_to_str (const osip_authorization_t * header, char **dest);
/**
 * Free a Authorization element.
 * @param header The element to work on.
 */
  void osip_authorization_free (osip_authorization_t * header);
/**
 * Clone a Authorization element.
 * @param header The element to work on.
 * @param dest A pointer on the copy of the element.
 */
  int osip_authorization_clone (const osip_authorization_t * header,
  			   osip_authorization_t ** dest);

/**
 * Get value of the auth_type parameter from a Authorization element.
 * @param header The element to work on.
 */
  char *osip_authorization_get_auth_type (const osip_authorization_t * header);
/**
 * Add the auth_type parameter from a Authorization element.
 * @param header The element to work on.
 * @param value The value of the new parameter.
 */
  void osip_authorization_set_auth_type (osip_authorization_t * header, char *value);
/**
 * Get value of the username parameter from a Authorization element.
 * @param header The element to work on.
 */
  char *osip_authorization_get_username (osip_authorization_t * header);
/**
 * Add the username parameter from a Authorization element.
 * @param header The element to work on.
 * @param value The value of the new parameter.
 */
  void osip_authorization_set_username (osip_authorization_t * header, char *value);
/**
 * Get value of the realm parameter from a Authorization element.
 * @param header The element to work on.
 */
  char *osip_authorization_get_realm (osip_authorization_t * header);
/**
 * Add the realm parameter from a Authorization element.
 * @param header The element to work on.
 * @param value The value of the new parameter.
 */
  void osip_authorization_set_realm (osip_authorization_t * header, char *value);
/**
 * Get value of the nonce parameter from a Authorization element.
 * @param header The element to work on.
 */
  char *osip_authorization_get_nonce (osip_authorization_t * header);
/**
 * Add the nonce parameter from a Authorization element.
 * @param header The element to work on.
 * @param value The value of the new parameter.
 */
  void osip_authorization_set_nonce (osip_authorization_t * header, char *value);
/**
 * Get value of the uri parameter from a Authorization element.
 * @param header The element to work on.
 */
  char *osip_authorization_get_uri (osip_authorization_t * header);
/**
 * Add the uri parameter from a Authorization element.
 * @param header The element to work on.
 * @param value The value of the new parameter.
 */
  void osip_authorization_set_uri (osip_authorization_t * header, char *value);
/**
 * Get value of the response parameter from a Authorization element.
 * @param header The element to work on.
 */
  char *osip_authorization_get_response (osip_authorization_t * header);
/**
 * Add the response parameter from a Authorization element.
 * @param header The element to work on.
 * @param value The value of the new parameter.
 */
  void osip_authorization_set_response (osip_authorization_t * header, char *value);
/**
 * Get value of the digest parameter from a Authorization element.
 * @param header The element to work on.
 */
  char *osip_authorization_get_digest (osip_authorization_t * header);
/**
 * Add the digest parameter from a Authorization element.
 * @param header The element to work on.
 * @param value The value of the new parameter.
 */
  void osip_authorization_set_digest (osip_authorization_t * header, char *value);
/**
 * Get value of the algorithm parameter from a Authorization element.
 * @param header The element to work on.
 */
  char *osip_authorization_get_algorithm (osip_authorization_t * header);
/**
 * Add the algorithm parameter from a Authorization element.
 * @param header The element to work on.
 * @param value The value of the new parameter.
 */
  void osip_authorization_set_algorithm (osip_authorization_t * header, char *value);
/**
 * Get value of the cnonce parameter from a Authorization element.
 * @param header The element to work on.
 */
  char *osip_authorization_get_cnonce (osip_authorization_t * header);
/**
 * Add the cnonce parameter from a Authorization element.
 * @param header The element to work on.
 * @param value The value of the new parameter.
 */
  void osip_authorization_set_cnonce (osip_authorization_t * header, char *value);
/**
 * Get value of the opaque parameter from a Authorization element.
 * @param header The element to work on.
 */
  char *osip_authorization_get_opaque (osip_authorization_t * header);
/**
 * Add the opaque parameter from a Authorization element.
 * @param header The element to work on.
 * @param value The value of the new parameter.
 */
  void osip_authorization_set_opaque (osip_authorization_t * header, char *value);
/**
 * Get value of the message_qop parameter from a Authorization element.
 * @param header The element to work on.
 */
  char *osip_authorization_get_message_qop (osip_authorization_t * header);
/**
 * Add the message_qop parameter from a Authorization element.
 * @param header The element to work on.
 * @param value The value of the new parameter.
 */
  void osip_authorization_set_message_qop (osip_authorization_t * header, char *value);
/**
 * Get value of the nonce_count parameter from a Authorization element.
 * @param header The element to work on.
 */
  char *osip_authorization_get_nonce_count (osip_authorization_t * header);
/**
 * Add the nonce_count parameter from a Authorization element.
 * @param header The element to work on.
 * @param value The value of the new parameter.
 */
  void osip_authorization_set_nonce_count (osip_authorization_t * header, char *value);

#ifdef __cplusplus
}
#endif

/** @} */

#endif
