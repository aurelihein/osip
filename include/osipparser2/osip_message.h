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


#ifndef _OSIP_MESSAGE_H_
#define _OSIP_MESSAGE_H_

#include <osipparser2/osip_const.h>
#include <osipparser2/osip_headers.h>
#include <osipparser2/osip_body.h>

/**
 * @file osip_message.h
 * @brief oSIP SIP Message Accessor Routines
 *
 * This is the SIP accessor and parser related API.
 * <BR>Understanding the parser implementation will prevent you from
 * using it improperly. Read this carefully.<BR>
 * <BR>This implementation could be seen as a partial implementation
 * of the whole SIP syntax. In other words, the parser is 'tolerant'
 * and will not detect a lot of error cases. As an example, no error
 * will be detected while trying to parse the following request-uri:
 * <BR><pre><code>INVITE sip:jack@atosc.org:abcd SIP/2.0</pre></code>
 * <BR>This code shows that even if your SIP message is parsed
 * correctly by oSIP, it may still be not compliant. This could be
 * used by attackers to make your application crash or whatever.
 * In this example, if you are trying to call the atoi() method with
 * the string 'abcd', your application will crash. Of course, there
 * exist solutions! You can check yourself for the validity of the
 * string or use the strtol() method (found on most unix) which is
 * capable of detecting such error cases.
 * <BR>Are you wondering why the parser has been built this way?
 * <BR>The initial answer is that each SIP application have
 * different requirement and some (the proxy!) needs SIP message
 * to be parsed as quickly as possible. Also, most applications
 * only need a few information from a SIP message. (the first Via
 * is the only one interesting!). If the parser was fully checking each
 * Via field validity, it would consume too much CPU on useless
 * operations. If you think this model does not fit your application,
 * then you should buy a slow stack :-).
 * <BR>Is there any plan to change that behaviour?
 * <BR>I do not need it, but if this interest you, it would be
 * possible to compile oSIP in 2 different ways: a full checker
 * model could be useful for SIP application with no performance
 * requirements. Any contributions is welcomed and will be merged
 * if it's made optional.
 */

/**
 * @defgroup oSIP_SMSG oSIP message API
 * @ingroup oSIP
 * @{
 */

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Structure for SIP Message (REQUEST and RESPONSE).
 * @defvar osip_message_t
 */
  typedef struct osip_message osip_message_t;

  struct osip_message
  {
    /* Start-Line definition */
    char *sip_version;
    /* req */
    osip_uri_t *req_uri;
    char *sip_method;
    /* resp */
    int status_code;
    char *reason_phrase;
    /* End of Start-Line definition */

    osip_list_t *accepts;
    osip_list_t *accept_encodings;
    osip_list_t *accept_languages;
    osip_list_t *alert_infos;
    osip_list_t *allows;
    osip_list_t *authorizations;
    osip_call_id_t *call_id;
    osip_list_t *call_infos;
    osip_list_t *contacts;
    osip_list_t *content_dispositions;
    osip_list_t *content_encodings;
    osip_content_length_t *content_length;
    osip_content_type_t *content_type;
    osip_cseq_t *cseq;
    osip_list_t *error_infos;
    osip_from_t *from;
    osip_mime_version_t *mime_version;
    osip_list_t *proxy_authenticates;
    osip_list_t *proxy_authorizations;
    osip_list_t *record_routes;
    osip_list_t *routes;
    osip_to_t *to;
    osip_list_t *vias;
    osip_list_t *www_authenticates;

    osip_list_t *headers;

    osip_list_t *bodies;

    /*
       1: structure and buffer "message" are identical.
       2: buffer "message" is not up to date with the structure info (call osip_message_to_str to update it).
     */
    int message_property;
    char *message;
  };

#ifndef SIP_MESSAGE_MAX_LENGTH
/**
 * You can re-define your own maximum length for SIP message.
 */
#define SIP_MESSAGE_MAX_LENGTH 4000
#endif

#ifndef BODY_MESSAGE_MAX_SIZE
/**
 * You can define the maximum length for a body inside a SIP message.
 */
#define BODY_MESSAGE_MAX_SIZE  1000
#endif

/**
 * Allocate a osip_message_t element.
 * @param sip The element to allocate.
 */
  int osip_message_init (osip_message_t ** sip);
/**
 * Free all resource in a osip_message_t element.
 * @param sip The element to free.
 */
  void osip_message_free (osip_message_t * sip);
/**
 * Parse a osip_message_t element.
 * @param sip The resulting element.
 * @param message The buffer to parse.
 */
  int osip_message_parse (osip_message_t * sip, const char *message);
/**
 * Parse a osip_message_t element.
 * @param sip The resulting element.
 * @param message The buffer to parse.
 * @param message The length of buffer to parse.
 */
  int osip_message_parse2 (osip_message_t * sip, const char *buf, size_t length);
/**
 * Get a string representation of a osip_message_t element.
 * @param sip The element to work on.
 * @param dest new allocated buffer returned.
 */
  int osip_message_to_str (osip_message_t * sip, char **dest);
/**
 * Clone a osip_message_t element.
 * @param sip The element to clone.
 * @param dest The new allocated element cloned.
 */
  int osip_message_clone (const osip_message_t * sip, osip_message_t ** dest);

/**
 * Set the reason phrase. This is entirely free in SIP.
 * @param sip The element to work on.
 * @param reason The reason phrase.
 */
  void osip_message_set_reason_phrase (osip_message_t * sip, char *reason);
/**
 * Get the reason phrase. This is entirely free in SIP.
 * @param sip The element to work on.
 */
  char *osip_message_get_reason_phrase (const osip_message_t * sip);
/**
 * Set the status code. This is entirely free in SIP.
 * @param sip The element to work on.
 * @param statuscode The status code.
 */
  void osip_message_set_status_code (osip_message_t * sip, int statuscode);
/**
 * Get the status code.
 * @param sip The element to work on.
 */
  int osip_message_get_status_code (const osip_message_t * sip);
/**
 * Set the method. You can set any string here.
 * @param sip The element to work on.
 * @param method The method name.
 */
  void osip_message_set_method (osip_message_t * sip, char *method);
/**
 * Get the method name.
 * @param sip The element to work on.
 */
  char *osip_message_get_method (const osip_message_t * sip);
/**
 * Set the SIP version used. (default is "SIP/2.0")
 * @param sip The element to work on.
 * @param version The version of SIP.
 */
  void osip_message_set_version (osip_message_t * sip, char *version);
/**
 * Get the SIP version.
 * @param sip The element to work on.
 */
  char *osip_message_get_version (const osip_message_t * sip);
/**
 * Set the Request-URI.
 * @param sip The element to work on.
 * @param uri The uri to set.
 */
  void osip_message_set_uri (osip_message_t * sip, osip_uri_t * uri);
/**
 * Get the Request-URI.
 * @param sip The element to work on.
 */
  osip_uri_t *osip_message_get_uri (const osip_message_t * sip);


/*
 *  These are helpfull MACROs to test messages type.
 */
/**
 * Test if the message is a SIP RESPONSE
 * @param msg the SIP message.
 */
#define MSG_IS_RESPONSE(msg) ((msg)->status_code!=0)
/**
 * Test if the message is a SIP REQUEST
 * @param msg the SIP message.
 */
#define MSG_IS_REQUEST(msg)  ((msg)->status_code==0)

/**
 * Test if the message is an INVITE REQUEST
 * @param msg the SIP message.
 */
#define MSG_IS_INVITE(msg)   (MSG_IS_REQUEST(msg) && \
			      0==strncmp((msg)->sip_method,"INVITE",6))
/**
 * Test if the message is an ACK REQUEST
 * @param msg the SIP message.
 */
#define MSG_IS_ACK(msg)      (MSG_IS_REQUEST(msg) && \
			      0==strncmp((msg)->sip_method,"ACK",3))
/**
 * Test if the message is a REGISTER REQUEST
 * @param msg the SIP message.
 */
#define MSG_IS_REGISTER(msg) (MSG_IS_REQUEST(msg) && \
			      0==strncmp((msg)->sip_method,"REGISTER",8))
/**
 * Test if the message is a BYE REQUEST
 * @param msg the SIP message.
 */
#define MSG_IS_BYE(msg)      (MSG_IS_REQUEST(msg) && \
			      0==strncmp((msg)->sip_method,"BYE",3))
/**
 * Test if the message is an OPTIONS REQUEST
 * @param msg the SIP message.
 */
#define MSG_IS_OPTIONS(msg)  (MSG_IS_REQUEST(msg) && \
			      0==strncmp((msg)->sip_method,"OPTIONS",7))
/**
 * Test if the message is an INFO REQUEST
 * @param msg the SIP message.
 */
#define MSG_IS_INFO(msg)     (MSG_IS_REQUEST(msg) && \
			      0==strncmp((msg)->sip_method,"INFO",4))
/**
 * Test if the message is a CANCEL REQUEST
 * @param msg the SIP message.
 */
#define MSG_IS_CANCEL(msg)   (MSG_IS_REQUEST(msg) && \
			      0==strncmp((msg)->sip_method,"CANCEL",6))
/**
 * Test if the message is a REFER REQUEST
 * @param msg the SIP message.
 */
#define MSG_IS_REFER(msg)   (MSG_IS_REQUEST(msg) && \
			      0==strncmp((msg)->sip_method,"REFER",5))
/**
 * Test if the message is a NOTIFY REQUEST
 * @param msg the SIP message.
 */
#define MSG_IS_NOTIFY(msg)   (MSG_IS_REQUEST(msg) && \
			      0==strncmp((msg)->sip_method,"NOTIFY",6))
/**
 * Test if the message is a SUBSCRIBE REQUEST
 * @param msg the SIP message.
 */
#define MSG_IS_SUBSCRIBE(msg)  (MSG_IS_REQUEST(msg) && \
			      0==strncmp((msg)->sip_method,"SUBSCRIBE",9))
/**
 * Test if the message is a MESSAGE REQUEST
 * @param msg the SIP message.
 */
#define MSG_IS_MESSAGE(msg)  (MSG_IS_REQUEST(msg) && \
			      0==strncmp((msg)->sip_method,"MESSAGE",7))
/**
 * Test if the message is a PRACK REQUEST  (!! PRACK IS NOT SUPPORTED by the fsm!!)
 * @param msg the SIP message.
 */
#define MSG_IS_PRACK(msg)    (MSG_IS_REQUEST(msg) && \
			      0==strncmp((msg)->sip_method,"PRACK",5))


#define MSG_IS_UPDATE(msg)    (MSG_IS_REQUEST(msg) && \
			      0==strncmp((msg)->sip_method,"UPDATE",6))


/**
 * Test if the message is a response with status between 100 and 199
 * @param msg the SIP message.
 */
#define MSG_IS_STATUS_1XX(msg) ((msg)->status_code >= 100 && \
                                (msg)->status_code < 200)
/**
 * Test if the message is a response with status between 200 and 299
 * @param msg the SIP message.
 */
#define MSG_IS_STATUS_2XX(msg) ((msg)->status_code >= 200 && \
                                (msg)->status_code < 300)
/**
 * Test if the message is a response with status between 300 and 399
 * @param msg the SIP message.
 */
#define MSG_IS_STATUS_3XX(msg) ((msg)->status_code >= 300 && \
                                (msg)->status_code < 400)
/**
 * Test if the message is a response with status between 400 and 499
 * @param msg the SIP message.
 */
#define MSG_IS_STATUS_4XX(msg) ((msg)->status_code >= 400 && \
                                (msg)->status_code < 500)
/**
 * Test if the message is a response with status between 500 and 599
 * @param msg the SIP message.
 */
#define MSG_IS_STATUS_5XX(msg) ((msg)->status_code >= 500 && \
                                (msg)->status_code < 600)
/**
 * Test if the message is a response with status between 600 and 699
 * @param msg the SIP message.
 */
#define MSG_IS_STATUS_6XX(msg) ((msg)->status_code >= 600 && \
                                (msg)->status_code < 700)
/**
 * Test if the message is a response with a status set to the code value.
 * @param msg the SIP message.
 * @param code the status code.
 */
#define MSG_TEST_CODE(msg,code) (MSG_IS_RESPONSE(msg) && \
				 (code)==(msg)->status_code)
/**
 * Test if the message is a response for a REQUEST of certain type
 * @param msg the SIP message.
 * @param requestname the method name to match.
 */
#define MSG_IS_RESPONSE_FOR(msg,requestname)  (MSG_IS_RESPONSE(msg) && \
				 0==strcmp((msg)->cseq->method,(requestname)))


/**
 * Allocate a generic parameter element.
 * @param GP The element to work on.
 */
#define osip_generic_param_init(GP) osip_uri_param_init(GP)
/**
 * Free a generic parameter element.
 * @param GP The element to work on.
 */
#define osip_generic_param_free(GP) osip_uri_param_free(GP)
/**
 * Set values of a generic parameter element.
 * @param GP The element to work on.
 * @param NAME The token name.
 * @param VALUE The token value.
 */
#define osip_generic_param_set(GP, NAME, VALUE)  osip_uri_param_set(GP, NAME, VALUE)
/**
 * Clone a generic parameter element.
 * @param GP The element to work on.
 * @param DEST The resulting new allocated buffer.
 */
#define osip_generic_param_clone(GP,DEST)     osip_uri_param_clone(GP,DEST)
#ifndef DOXYGEN
/*
 * Free a list of a generic parameter element.
 * @param LIST The list of generic parameter element to free.
 */
#define osip_generic_param_freelist(LIST)       osip_uri_param_freelist(LIST)
#endif
/**
 * Allocate and add a generic parameter element in a list.
 * @param LIST The list of generic parameter element to work on.
 * @param NAME The token name.
 * @param VALUE The token value.
 */
#define osip_generic_param_add(LIST,NAME,VALUE)        osip_uri_param_add(LIST,NAME,VALUE)
/**
 * Find in a generic parameter element in a list.
 * @param LIST The list of generic parameter element to work on.
 * @param NAME The name of the parameter element to find.
 * @param DEST A pointer on the element found.
 */
#define osip_generic_param_get_byname(LIST,NAME,DEST) osip_uri_param_get_byname(LIST,NAME,DEST)

/**
 * Set the name of a generic parameter element.
 * @param generic_param The element to work on.
 * @param name the token name to set.
 */
  void osip_generic_param_set_name (osip_generic_param_t * generic_param,
				    char *name);
/**
 * Get the name of a generic parameter element.
 * @param generic_param The element to work on.
 */
  char *osip_generic_param_get_name (const osip_generic_param_t *
				     generic_param);
/**
 * Set the value of a generic parameter element.
 * @param generic_param The element to work on.
 * @param value the token name to set.
 */
  void osip_generic_param_set_value (osip_generic_param_t * generic_param,
				     char *value);
/**
 * Get the value of a generic parameter element.
 * @param generic_param The element to work on.
 */
  char *osip_generic_param_get_value (const osip_generic_param_t *
				      generic_param);

/** @} */


#ifdef __cplusplus
}
#endif

#endif
