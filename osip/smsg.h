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


#ifndef _SMSG_H_
#define _SMSG_H_

#include <osip/const.h>		/* constant definitions */
#include <osip/smsgtypes.h>	/* type     definitions */

/**
 * @file smsg.h
 * @brief oSIP parser Routines
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
 * @defgroup oSIP_SMSG oSIP parser Handling
 * @ingroup oSIP
 * @{
 */

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Initialise the oSIP parser.
 */
  int parser_init ();

/**
 * Fix the via header for INCOMING requests only.
 * a copy of ip_addr is done.
 */
  int msg_fix_last_via_header (sip_t * request, char *ip_addr, int port);

/**
 * Allocate a sip_t element.
 * @param sip The element to allocate.
 */
  int msg_init (sip_t ** sip);
/**
 * Free all resource in a sip_t element.
 * @param sip The element to free.
 */
  void msg_free (sip_t * sip);
/**
 * Parse a sip_t element.
 * @param sip The resulting element.
 * @param message The buffer to parse.
 */
  int msg_parse (sip_t * sip, char *message);
/**
 * Get a string representation of a sip_t element.
 * @param sip The element to work on.
 * @param dest new allocated buffer returned.
 */
  int msg_2char (sip_t * sip, char **dest);
/**
 * Clone a sip_t element.
 * @param sip The element to clone.
 * @param dest The new allocated element cloned.
 */
  int msg_clone (sip_t * sip, sip_t ** dest);

/**
 * define this macro to avoid building several times
 * the message on retransmissions. If you have changed
 * the sip_t element since last call of msg_2char() you
 * can call msg_force_update() to force a rebuild.
*/
#ifdef USE_TMP_BUFFER
/**
 * Check if the element is already built. (so msg_2char won't build it again)
 * @param sip The element to check.
*/
  int msg_get_property (sip_t * sip);
#endif

/**
 * Force a sip_t element to be rebuild on next msg_2char() call.
 * @param sip The element to work on.
 */
  int msg_force_update (sip_t * sip);

/**
 * Get the usual reason phrase as defined in SIP for a specific status code.
 * @param status_code A status code.
 */
  char *msg_getreason (int status_code);
/**
 * Set the reason phrase. This is entirely free in SIP.
 * @param sip The element to work on.
 * @param reason The reason phrase.
 */
  void msg_setreasonphrase (sip_t * sip, char *reason);
/**
 * Get the reason phrase. This is entirely free in SIP.
 * @param sip The element to work on.
 */
  char *msg_getreasonphrase (sip_t * sip);
/**
 * Set the status code. This is entirely free in SIP.
 * @param sip The element to work on.
 * @param statuscode The status code.
 */
  void msg_setstatuscode (sip_t * sip, char *statuscode);
/**
 * Get the status code.
 * @param sip The element to work on.
 */
  char *msg_getstatuscode (sip_t * sip);
/**
 * Set the method. You can set any string here.
 * @param sip The element to work on.
 * @param method The method name.
 */
  void msg_setmethod (sip_t * sip, char *method);
/**
 * Get the method name.
 * @param sip The element to work on.
 */
  char *msg_getmethod (sip_t * sip);
/**
 * Set the SIP version used. (use "SIP/2.0")
 * @param sip The element to work on.
 * @param version The version of SIP.
 */
  void msg_setversion (sip_t * sip, char *version);
/**
 * Get the SIP version.
 * @param sip The element to work on.
 */
  char *msg_getversion (sip_t * sip);
/**
 * Set the Request-URI.
 * @param sip The element to work on.
 * @param uri The uri to set.
 */
  void msg_seturi (sip_t * sip, url_t * uri);
/**
 * Get the Request-URI.
 * @param sip The element to work on.
 */
  url_t *msg_geturi (sip_t * sip);

/**
 * Set the Accept header.
 * @param sip The element to work on.
 * @param hvalue The string describing the element.
 */
  int msg_setaccept (sip_t * sip, char *hvalue);
/**
 * Get one Accept header.
 * @param sip The element to work on.
 * @param pos The index of the element to get.
 * @param dest A pointer on the header found.
 */
  int msg_getaccept (sip_t * sip, int pos, accept_t ** dest);
/**
 * Set the Accept-encoding header.
 * @param sip The element to work on.
 * @param hvalue The string describing the element.
 */
  int msg_setaccept_encoding (sip_t * sip, char *hvalue);
/**
 * Get one Accept-encoding header.
 * @param sip The element to work on.
 * @param pos The index of the element to get.
 * @param dest A pointer on the header found.
 */
  int msg_getaccept_encoding (sip_t * sip, int pos,
			      accept_encoding_t ** dest);
/**
 * Set the Accept-language header.
 * @param sip The element to work on.
 * @param hvalue The string describing the element.
 */
  int msg_setaccept_language (sip_t * sip, char *hvalue);
/**
 * Get one Accept header.
 * @param sip The element to work on.
 * @param pos The index of the element to get.
 * @param dest A pointer on the header found.
 */
  int msg_getaccept_language (sip_t * sip, int pos,
			      accept_language_t ** dest);
/**
 * Set the Alert-info header.
 * @param sip The element to work on.
 * @param hvalue The string describing the element.
 */
  int msg_setalert_info (sip_t * sip, char *hvalue);
/**
 * Get one Alert-info header.
 * @param sip The element to work on.
 * @param pos The index of the element to get.
 * @param dest A pointer on the header found.
 */
  int msg_getalert_info (sip_t * sip, int pos, alert_info_t ** dest);
/**
 * Set the Allow header.
 * @param sip The element to work on.
 * @param hvalue The string describing the element.
 */
  int msg_setallow (sip_t * sip, char *hvalue);
/**
 * Get one Allow header.
 * @param sip The element to work on.
 * @param pos The index of the element to get.
 * @param dest A pointer on the header found.
 */
  int msg_getallow (sip_t * sip, int pos, allow_t ** dest);
/**
 * Set the Authorization header.
 * @param sip The element to work on.
 * @param hvalue The string describing the element.
 */
  int msg_setauthorization (sip_t * sip, char *hvalue);
/**
 * Get one Authorization header.
 * @param sip The element to work on.
 * @param pos The index of the element to get.
 * @param dest A pointer on the header found.
 */
  int msg_getauthorization (sip_t * sip, int pos, authorization_t ** dest);
/**
 * Set the Call-id header.
 * @param sip The element to work on.
 * @param hvalue The string describing the element.
 */
  int msg_setcall_id (sip_t * sip, char *hvalue);
/**
 * Get one Call-id header.
 * @param sip The element to work on.
 */
  call_id_t *msg_getcall_id (sip_t * sip);
/**
 * Set the Call-info header.
 * @param sip The element to work on.
 * @param hvalue The string describing the element.
 */
  int msg_setcall_info (sip_t * sip, char *hvalue);
/**
 * Get one Call-info header.
 * @param sip The element to work on.
 * @param pos The index of the element to get.
 * @param dest A pointer on the header found.
 */
  int msg_getcall_info (sip_t * sip, int pos, call_info_t ** dest);
/**
 * Set the Contact header.
 * @param sip The element to work on.
 * @param hvalue The string describing the element.
 */
  int msg_setcontact (sip_t * sip, char *hvalue);
/**
 * Get one Contact header.
 * @param sip The element to work on.
 * @param pos The index of the element to get.
 * @param dest A pointer on the header found.
 */
  int msg_getcontact (sip_t * sip, int pos, contact_t ** dest);
/**
 * Set the Content-disposition header.
 * @param sip The element to work on.
 * @param hvalue The string describing the element.
 */
  int msg_setcontent_disposition (sip_t * sip, char *hvalue);
/**
 * Get one Content-disposition header.
 * @param sip The element to work on.
 * @param pos The index of the element to get.
 * @param dest A pointer on the header found.
 */
  int msg_getcontent_disposition (sip_t * sip, int pos,
				  content_disposition_t ** dest);
/**
 * Set the Content-encoding header.
 * @param sip The element to work on.
 * @param hvalue The string describing the element.
 */
  int msg_setcontent_encoding (sip_t * sip, char *hvalue);
/**
 * Get one Content-encoding header.
 * @param sip The element to work on.
 * @param pos The index of the element to get.
 * @param dest A pointer on the header found.
 */
  int msg_getcontent_encoding (sip_t * sip, int pos,
			       content_encoding_t ** dest);
/**
 * Set the Content-length header.
 * @param sip The element to work on.
 * @param hvalue The string describing the element.
 */
  int msg_setcontent_length (sip_t * sip, char *hvalue);
/**
 * Get one Content-length header.
 * @param sip The element to work on.
 */
  content_length_t *msg_getcontent_length (sip_t * sip);
/**
 * Set the Content-type header.
 * @param sip The element to work on.
 * @param hvalue The string describing the element.
 */
  int msg_setcontent_type (sip_t * sip, char *hvalue);
/**
 * Get one Content-type header.
 * @param sip The element to work on.
 */
  content_type_t *msg_getcontent_type (sip_t * sip);
/**
 * Set the Cseq header.
 * @param sip The element to work on.
 * @param hvalue The string describing the element.
 */
  int msg_setcseq (sip_t * sip, char *hvalue);
/**
 * Get one Cseq header.
 * @param sip The element to work on.
 */
  cseq_t *msg_getcseq (sip_t * sip);
/**
 * Set the Error-info header.
 * @param sip The element to work on.
 * @param hvalue The string describing the element.
 */
  int msg_seterror_info (sip_t * sip, char *hvalue);
/**
 * Get one Error-info header.
 * @param sip The element to work on.
 * @param pos The index of the element to get.
 * @param dest A pointer on the header found.
 */
  int msg_geterror_info (sip_t * sip, int pos, error_info_t ** dest);
/**
 * Set the From header.
 * @param sip The element to work on.
 * @param hvalue The string describing the element.
 */
  int msg_setfrom (sip_t * sip, char *hvalue);
/**
 * Get the From header.
 * @param sip The element to work on.
 */
  from_t *msg_getfrom (sip_t * sip);
/**
 * Set the mime-version header.
 * @param sip The element to work on.
 * @param hvalue The string describing the element.
 */
  int msg_setmime_version (sip_t * sip, char *hvalue);
/**
 * Get the Mime-version header.
 * @param sip The element to work on.
 */
  mime_version_t *msg_getmime_version (sip_t * sip);
/**
 * Set the Proxy-authenticate header.
 * @param sip The element to work on.
 * @param hvalue The string describing the element.
 */
  int msg_setproxy_authenticate (sip_t * sip, char *hvalue);
/**
 * Get the Proxy-authenticate header.
 * @param sip The element to work on.
 * @param pos The index of the element to get.
 * @param dest A pointer on the header found.
 */
  int msg_getproxy_authenticate (sip_t * sip, int pos,
				 proxy_authenticate_t ** dest);
/**
 * Set the Proxy-authorization header.
 * @param sip The element to work on.
 * @param hvalue The string describing the element.
 */
  int msg_setproxy_authorization (sip_t * sip, char *hvalue);
/**
 * Get one Proxy-authorization header.
 * @param sip The element to work on.
 * @param pos The index of the element to get.
 * @param dest A pointer on the header found.
 */
  int msg_getproxy_authorization (sip_t * sip, int pos,
				  proxy_authorization_t ** dest);
/**
 * Set the Record-Route header.
 * @param sip The element to work on.
 * @param hvalue The string describing the element.
 */
  int msg_setrecord_route (sip_t * sip, char *hvalue);
/**
 * Get one Record-route header.
 * @param sip The element to work on.
 * @param pos The index of the element to get.
 * @param dest A pointer on the header found.
 */
  int msg_getrecord_route (sip_t * sip, int pos, record_route_t ** dest);
/**
 * Set the Route header.
 * @param sip The element to work on.
 * @param hvalue The string describing the element.
 */
  int msg_setroute (sip_t * sip, char *hvalue);
/**
 * Get one Route header.
 * @param sip The element to work on.
 * @param pos The index of the element to get.
 * @param dest A pointer on the header found.
 */
  int msg_getroute (sip_t * sip, int pos, route_t ** dest);
/**
 * Set the To header.
 * @param sip The element to work on.
 * @param hvalue The string describing the element.
 */
  int msg_setto (sip_t * sip, char *hvalue);
/**
 * Get the To header.
 * @param sip The element to work on.
 */
  to_t *msg_getto (sip_t * sip);
/**
 * Set the Via header.
 * @param sip The element to work on.
 * @param hvalue The string describing the element.
 */
  int msg_setvia (sip_t * sip, char *hvalue);
/**
 * Get one Via header.
 * @param sip The element to work on.
 * @param pos The index of the element to get.
 * @param dest A pointer on the header found.
 */
  int msg_getvia (sip_t * sip, int pos, via_t ** dest);
/**
 * Set the Www-authenticate header.
 * @param sip The element to work on.
 * @param hvalue The string describing the element.
 */
  int msg_setwww_authenticate (sip_t * sip, char *hvalue);
/**
 * Get one Www-authenticate header.
 * @param sip The element to work on.
 * @param pos The index of the element to get.
 * @param dest A pointer on the header found.
 */
  int msg_getwww_authenticate (sip_t * sip, int pos,
			       www_authenticate_t ** dest);
/**
 * Allocate and Add an "unknown" header (not defined in oSIP).
 * @param sip The element to work on.
 * @param hname The token name.
 * @param hvalue The token value.
 */
  int msg_setheader (sip_t * sip, char *hname, char *hvalue);
/**
 * Allocate and Add an "unknown" header (not defined in oSIP).
 * The element is add on the top of the unknown header list. 
 * @param sip The element to work on.
 * @param hname The token name.
 * @param hvalue The token value.
 */
  int msg_settopheader (sip_t * sip, char *hname, char *hvalue);
/**
 * Find an "unknown" header. (not defined in oSIP)
 * @param sip The element to work on.
 * @param hname The name of the header to find.
 * @param pos The index where to start searching for the header.
 * @param dest A pointer to the header found.
 */
  int msg_header_getbyname (sip_t * sip, char *hname, int pos,
			    header_t ** dest);
/**
 * Get one "unknown" header.
 * @param sip The element to work on.
 * @param pos The index of the element to get.
 * @param dest A pointer on the header found.
 */
  int msg_getheader (sip_t * sip, int pos, header_t ** dest);

/**
 * Set the Body of the SIP message.
 * @param sip The element to work on.
 * @param buf The string containing the body.
 */
  int msg_setbody (sip_t * sip, char *buf);
/**
 * Set a type for a body. (NOT TESTED! use with care)
 * @param sip The element to work on.
 * @param buf the mime type of body.
 */
  int msg_setbody_mime (sip_t * sip, char *buf);
/**
 * Get one body header.
 * @param sip The element to work on.
 * @param pos The index of the element to get.
 * @param dest A pointer on the body found.
 */
  int msg_getbody (sip_t * sip, int pos, body_t ** dest);


/*
 *  These are helpfull MACROs to test messages type.
 */
/**
 * Test if the message is a SIP RESPONSE
 * @param msg the SIP message.
 */
#define MSG_IS_RESPONSE(msg) ((msg)->strtline->statuscode!=NULL)
/**
 * Test if the message is a SIP REQUEST
 * @param msg the SIP message.
 */
#define MSG_IS_REQUEST(msg)  ((msg)->strtline->statuscode==NULL)

/**
 * Test if the message is an INVITE REQUEST
 * @param msg the SIP message.
 */
#define MSG_IS_INVITE(msg)   (MSG_IS_REQUEST(msg) && \
			      0==strncmp((msg)->strtline->sipmethod,"INVITE",6))
/**
 * Test if the message is an ACK REQUEST
 * @param msg the SIP message.
 */
#define MSG_IS_ACK(msg)      (MSG_IS_REQUEST(msg) && \
			      0==strncmp((msg)->strtline->sipmethod,"ACK",3))
/**
 * Test if the message is a REGISTER REQUEST
 * @param msg the SIP message.
 */
#define MSG_IS_REGISTER(msg) (MSG_IS_REQUEST(msg) && \
			      0==strncmp((msg)->strtline->sipmethod,"REGISTER",8))
/**
 * Test if the message is a BYE REQUEST
 * @param msg the SIP message.
 */
#define MSG_IS_BYE(msg)      (MSG_IS_REQUEST(msg) && \
			      0==strncmp((msg)->strtline->sipmethod,"BYE",3))
/**
 * Test if the message is an OPTIONS REQUEST
 * @param msg the SIP message.
 */
#define MSG_IS_OPTIONS(msg)  (MSG_IS_REQUEST(msg) && \
			      0==strncmp((msg)->strtline->sipmethod,"OPTIONS",7))
/**
 * Test if the message is an INFO REQUEST
 * @param msg the SIP message.
 */
#define MSG_IS_INFO(msg)     (MSG_IS_REQUEST(msg) && \
			      0==strncmp((msg)->strtline->sipmethod,"INFO",4))
/**
 * Test if the message is a CANCEL REQUEST
 * @param msg the SIP message.
 */
#define MSG_IS_CANCEL(msg)   (MSG_IS_REQUEST(msg) && \
			      0==strncmp((msg)->strtline->sipmethod,"CANCEL",6))
/**
 * Test if the message is a REFER REQUEST
 * @param msg the SIP message.
 */
#define MSG_IS_REFER(msg)   (MSG_IS_REQUEST(msg) && \
			      0==strncmp((msg)->strtline->sipmethod,"REFER",5))
/**
 * Test if the message is a NOTIFY REQUEST
 * @param msg the SIP message.
 */
#define MSG_IS_NOTIFY(msg)   (MSG_IS_REQUEST(msg) && \
			      0==strncmp((msg)->strtline->sipmethod,"NOTIFY",6))
/**
 * Test if the message is a SUBSCRIBE REQUEST
 * @param msg the SIP message.
 */
#define MSG_IS_SUBSCRIBE(msg)  (MSG_IS_REQUEST(msg) && \
			      0==strncmp((msg)->strtline->sipmethod,"SUBSCRIBE",9))
/**
 * Test if the message is a MESSAGE REQUEST
 * @param msg the SIP message.
 */
#define MSG_IS_MESSAGE(msg)  (MSG_IS_REQUEST(msg) && \
			      0==strncmp((msg)->strtline->sipmethod,"MESSAGE",7))
/**
 * Test if the message is a PRACK REQUEST  (!! PRACK IS NOT SUPPORTED by the fsm!!)
 * @param msg the SIP message.
 */
#define MSG_IS_PRACK(msg)    (MSG_IS_REQUEST(msg) && \
			      0==strncmp((msg)->strtline->sipmethod,"PRACK",5))

/**
 * Test if the message is a response with status between 100 and 199
 * @param msg the SIP message.
 */
#define MSG_IS_STATUS_1XX(msg) (MSG_IS_RESPONSE(msg) && \
				0==strncmp((msg)->strtline->statuscode,"1",1))
/**
 * Test if the message is a response with status between 200 and 299
 * @param msg the SIP message.
 */
#define MSG_IS_STATUS_2XX(msg) (MSG_IS_RESPONSE(msg) && \
				0==strncmp((msg)->strtline->statuscode,"2",1))
/**
 * Test if the message is a response with status between 300 and 399
 * @param msg the SIP message.
 */
#define MSG_IS_STATUS_3XX(msg) (MSG_IS_RESPONSE(msg) && \
				0==strncmp((msg)->strtline->statuscode,"3",1))
/**
 * Test if the message is a response with status between 400 and 499
 * @param msg the SIP message.
 */
#define MSG_IS_STATUS_4XX(msg) (MSG_IS_RESPONSE(msg) && \
				0==strncmp((msg)->strtline->statuscode,"4",1))
/**
 * Test if the message is a response with status between 500 and 599
 * @param msg the SIP message.
 */
#define MSG_IS_STATUS_5XX(msg) (MSG_IS_RESPONSE(msg) && \
				0==strncmp((msg)->strtline->statuscode,"5",1))
/**
 * Test if the message is a response with status between 600 and 699
 * @param msg the SIP message.
 */
#define MSG_IS_STATUS_6XX(msg) (MSG_IS_RESPONSE(msg) && \
				0==strncmp((msg)->strtline->statuscode,"6",1))
/**
 * Test if the message is a response with a status set to the code value.
 * @param msg the SIP message.
 * @param code the status code.
 */
#define MSG_TEST_CODE(msg,code) (MSG_IS_RESPONSE(msg) && \
				 code==(int)satoi((msg)->strtline->statuscode))
/**
 * Test if the message is a response for a REQUEST of certain type
 * @param msg the SIP message.
 * @param requestname the method name to match.
 */
#define MSG_IS_RESPONSEFOR(msg,requestname)  (MSG_IS_RESPONSE(msg) && \
				 0==strcmp((msg)->cseq->method,requestname))

/**
 * Allocate a body_t element.
 * @param body The element to work on.
 */
  int body_init (body_t ** body);
/**
 * Free a body_t element.
 * @param body The element to work on.
 */
  void body_free (body_t * body);
/**
 * Parse a body_t element.
 * @param body The element to work on.
 * @param buf The buffer to parse.
 */
  int body_parse (body_t * body, char *buf);
/**
 * Parse a body_t element. (for mime message format) (NOT TESTED, use with care)
 * @param body The element to work on.
 * @param buf The buffer to parse.
 */
  int body_parse_mime (body_t * body, char *buf);
/**
 * Get a string representation of a body_t element.
 * @param body The element to work on.
 * @param dest The resulting buffer.
 */
  int body_2char (body_t * body, char **dest);

/**
 * Allocate a generic parameter element.
 * @param GP The element to work on.
 */
#define generic_param_init(GP) url_param_init(GP)
/**
 * Free a generic parameter element.
 * @param GP The element to work on.
 */
#define generic_param_free(GP) url_param_free(GP)
/**
 * Set values of a generic parameter element.
 * @param GP The element to work on.
 * @param NAME The token name.
 * @param VALUE The token value.
 */
#define generic_param_set(GP, NAME, VALUE)  url_param_set(GP, NAME, VALUE)
/**
 * Clone a generic parameter element.
 * @param GP The element to work on.
 * @param DEST The resulting new allocated buffer.
 */
#define generic_param_clone(GP,DEST)     url_param_clone(GP,DEST)
#ifndef DOXYGEN
/*
 * Free a list of a generic parameter element.
 * @param LIST The list of generic parameter element to free.
 */
#define generic_param_freelist(LIST)       url_param_freelist(LIST)
#endif
/**
 * Allocate and add a generic parameter element in a list.
 * @param LIST The list of generic parameter element to work on.
 * @param NAME The token name.
 * @param VALUE The token value.
 */
#define generic_param_add(LIST,NAME,VALUE)        url_param_add(LIST,NAME,VALUE)
/**
 * Find in a generic parameter element in a list.
 * @param LIST The list of generic parameter element to work on.
 * @param NAME The name of the parameter element to find.
 * @param DEST A pointer on the element found.
 */
#define generic_param_getbyname(LIST,NAME,DEST) url_param_getbyname(LIST,NAME,DEST)

/**
 * Set the name of a generic parameter element.
 * @param generic_param The element to work on.
 * @param name the token name to set.
 */
  void generic_param_setname (generic_param_t * generic_param, char *name);
/**
 * Get the name of a generic parameter element.
 * @param generic_param The element to work on.
 */
  char *generic_param_getname (generic_param_t * generic_param);
/**
 * Set the value of a generic parameter element.
 * @param generic_param The element to work on.
 * @param value the token name to set.
 */
  void generic_param_setvalue (generic_param_t * generic_param, char *value);
/**
 * Get the value of a generic parameter element.
 * @param generic_param The element to work on.
 */
  char *generic_param_getvalue (generic_param_t * generic_param);


/**
 * Allocate a header element.
 * @param header The element to work on.
 */
  int header_init (header_t ** header);
/**
 * Free a header element.
 * @param header The element to work on.
 */
  void header_free (header_t * header);
/**
 * Get a string representation of a header element.
 * @param header The element to work on.
 * @param dest A pointer on the new allocated buffer.
 */
  int header_2char (header_t * header, char **dest);
/**
 * Get the token name a header element.
 * @param header The element to work on.
 */
  char *header_getname (header_t * header);
/**
 * Set the token name a header element.
 * @param header The element to work on.
 * @param pname The token name to set.
 */
  void header_setname (header_t * header, char *pname);
/**
 * Get the token value a header element.
 * @param header The element to work on.
 */
  char *header_getvalue (header_t * header);
/**
 * Set the token value a header element.
 * @param header The element to work on.
 * @param pvalue The token value to set.
 */
  void header_setvalue (header_t * header, char *pvalue);
/**
 * Clone a header element.
 * @param header The element to work on.
 * @param dest A pointer on the copy of the element.
 */
  int header_clone (header_t * header, header_t ** dest);

/**
 * Allocate an Accept element.
 * @param header The element to work on.
 */
#define accept_init(header)     content_type_init(header)
/**
 * Free an Accept element.
 * @param header The element to work on.
 */
#define accept_free(header)     content_type_free(header)
/**
 * Parse an Accept element.
 * @param header The element to work on.
 * @param hvalue The string to parse.
 */
#define accept_parse(header, hvalue) content_type_parse(header, hvalue)
/**
 * Get a string representation of an Accept element.
 * @param header The element to work on.
 * @param dest A pointer on the new allocated string.
 */
#define accept_2char(header, dest) content_type_2char(header, dest)
/**
 * Clone an Accept element.
 * @param header The element to work on.
 * @param dest A pointer on the copy of the element.
 */
#define accept_clone(header, dest) content_type_clone(header, dest)

/**
 * Allocate and add a header parameter in an Accept element.
 * @param header The element to work on.
 * @param name The token name.
 * @param value The token value.
 */
#define accept_param_add(header,name,value)  generic_param_add((header)->gen_params,name,value)
/**
 * Find a header parameter in an Accept element.
 * @param header The element to work on.
 * @param name The token name to search.
 * @param dest A pointer on the element found.
 */
#define accept_param_getbyname(header,name,dest) generic_param_getbyname((header)->gen_params,name,dest)

/**
 * Allocate a Accept-Encoding element.
 * @param header The element to work on.
 */
  int accept_encoding_init (accept_encoding_t ** header);
/**
 * Parse a Accept-Encoding element.
 * @param header The element to work on.
 * @param hvalue The string to parse.
 */
  int accept_encoding_parse (accept_encoding_t * header, char *hvalue);
/**
 * Get a string representation of a Accept-Encoding element.
 * @param header The element to work on.
 * @param dest A pointer on the new allocated string.
 */
  int accept_encoding_2char (accept_encoding_t * header, char **dest);
/**
 * Free a Accept-Encoding element.
 * @param header The element to work on.
 */
  void accept_encoding_free (accept_encoding_t * header);
/**
 * Clone a Accept-Encoding element.
 * @param header The element to work on.
 * @param dest A pointer on the copy of the element.
 */
  int accept_encoding_clone (accept_encoding_t * header,
			     accept_encoding_t ** dest);

/**
 * Set the value of an Accept-Encoding element.
 * @param header The element to work on.
 * @param value The token value to set.
 */
  void accept_encoding_setelement (accept_encoding_t * header, char *value);
/**
 * Get the value of an Accept-Encoding element.
 * @param header The element to work on.
 */
  char *accept_encoding_getelement (accept_encoding_t * header);
/**
 * Allocate and Add a header parameter in an Accept-Encoding element.
 * @param header The element to work on.
 * @param name The token name for the new parameter.
 * @param value The token value for the new parameter.
 */
#define accept_encoding_param_add(header,name,value)  generic_param_add((header)->gen_params,name,value)
/**
 * Find a header parameter in an Accept-Encoding element.
 * @param header The element to work on.
 * @param name The token name to search.
 * @param dest A pointer on the element found.
 */
#define accept_encoding_param_getbyname(header,name,dest) generic_param_getbyname((header)->gen_params,name,dest)

/**
 * Allocate an Accept-Language element.
 * @param header The element to work on.
 */
#define accept_language_init(header)      accept_encoding_init(header)
/**
 * Parse an Accept-Language element.
 * @param header The element to work on.
 * @param hvalue The string to parse.
 */
#define accept_language_parse(header, hvalue)  accept_encoding_parse(header, hvalue)
/**
 * Get a string representation of an Accept-Language element.
 * @param header The element to work on.
 * @param dest A pointer on the new allocated string.
 */
#define accept_language_2char(header, dest)  accept_encoding_2char(header, dest)
/**
 * Free an Accept-Language element.
 * @param header The element to work on.
 */
#define accept_language_free(header)      accept_encoding_free(header)
/**
 * Clone an Accept-Language element.
 * @param header The element to work on.
 * @param dest A pointer on the copy of the element.
 */
#define accept_language_clone(header, dest) accept_encoding_clone(header, dest)

/**
 * Get the value of an Accept-Language element.
 * @param header The element to work on.
 */
#define accept_language_getelement(header)     accept_encoding_getelement(header)
/**
 * Set the value of an Accept-Language element.
 * @param header The element to work on.
 * @param value The value to set.
 */
#define accept_language_setelement(header, value)  accept_encoding_setelement(header, value)
/**
 * Allocate and add a generic parameter element in an Accept-Language element.
 * @param header The element to work on.
 * @param name The token name.
 * @param value The token value.
 */
#define accept_language_param_add(header,name,value)  generic_param_add((header)->gen_params,name,value)
/**
 * Find a header parameter in a Accept-Language element.
 * @param header The element to work on.
 * @param name The token name to search.
 * @param dest A pointer on the element found.
 */
#define accept_language_param_getbyname(header,name,dest) generic_param_getbyname((header)->gen_params,name,dest)

/**
 * Allocate a Alert-Info element.
 * @param header The element to work on.
 */
#define  alert_info_init(header)      call_info_init(header)
/**
 * Free a Alert-Info element.
 * @param header The element to work on.
 */
#define  alert_info_free(header)      call_info_free(header)
/**
 * Parse a Alert-Info element.
 * @param header The element to work on.
 * @param hvalue The string to parse.
 */
#define  alert_info_parse(header, hvalue)  call_info_parse(header, hvalue)
/**
 * Get a string representation of a Alert-Info element.
 * @param header The element to work on.
 * @param dest A pointer on the new allocated string.
 */
#define  alert_info_2char(header,dest)   call_info_2char(header,dest)
/**
 * Clone a Alert-Info element.
 * @param header The element to work on.
 * @param dest A pointer on the copy of the element.
 */
#define  alert_info_clone(header, dest)  call_info_clone(header, dest)
/**
 * Get uri from an Alert-Info element.
 * @param header The element to work on.
 */
#define  alert_info_geturi(header)    call_info_geturi(header)
/**
 * Set the uri of an Alert-Info element.
 * @param header The element to work on.
 * @param uri The value of the new parameter.
 */
#define  alert_info_seturi(header, uri) call_info_seturi(header, uri)

/**
 * Allocate a Allow element.
 * @param header The element to work on.
 */
#define allow_init(header)      content_length_init(header)
/**
 * Parse a Allow element.
 * @param header The element to work on.
 * @param hvalue The string to parse.
 */
#define allow_parse(header, hvalue)  content_length_parse(header, hvalue)
/**
 * Get a string representation of a Allow element.
 * @param header The element to work on.
 * @param dest A pointer on the new allocated string.
 */
#define allow_2char(header, dest)  content_length_2char(header, dest)
/**
 * Free a Allow element.
 * @param header The element to work on.
 */
#define allow_free(header)      content_length_free(header)
/**
 * Clone a Allow element.
 * @param header The element to work on.
 * @param dest A pointer on the copy of the element.
 */
#define allow_clone(header, dest)  content_length_clone(header, dest)

/**
 * Allocate a Authorization element.
 * @param header The element to work on.
 */
  int authorization_init (authorization_t ** header);
/**
 * Parse a Authorization element.
 * @param header The element to work on.
 * @param hvalue The string to parse.
 */
  int authorization_parse (authorization_t * header, char *hvalue);
/**
 * Get a string representation of a Authorization element.
 * @param header The element to work on.
 * @param dest A pointer on the new allocated string.
 */
  int authorization_2char (authorization_t * header, char **dest);
/**
 * Free a Authorization element.
 * @param header The element to work on.
 */
  void authorization_free (authorization_t * header);
/**
 * Clone a Authorization element.
 * @param header The element to work on.
 * @param dest A pointer on the copy of the element.
 */
  int authorization_clone (authorization_t * header, authorization_t ** dest);

/**
 * Get value of the auth_type parameter from a Authorization element.
 * @param header The element to work on.
 */
  char *authorization_getauth_type (authorization_t * header);
/**
 * Add the auth_type parameter from a Authorization element.
 * @param header The element to work on.
 * @param value The value of the new parameter.
 */
  void authorization_setauth_type (authorization_t * header, char *value);
/**
 * Get value of the username parameter from a Authorization element.
 * @param header The element to work on.
 */
  char *authorization_getusername (authorization_t * header);
/**
 * Add the username parameter from a Authorization element.
 * @param header The element to work on.
 * @param value The value of the new parameter.
 */
  void authorization_setusername (authorization_t * header, char *value);
/**
 * Get value of the realm parameter from a Authorization element.
 * @param header The element to work on.
 */
  char *authorization_getrealm (authorization_t * header);
/**
 * Add the realm parameter from a Authorization element.
 * @param header The element to work on.
 * @param value The value of the new parameter.
 */
  void authorization_setrealm (authorization_t * header, char *value);
/**
 * Get value of the nonce parameter from a Authorization element.
 * @param header The element to work on.
 */
  char *authorization_getnonce (authorization_t * header);
/**
 * Add the nonce parameter from a Authorization element.
 * @param header The element to work on.
 * @param value The value of the new parameter.
 */
  void authorization_setnonce (authorization_t * header, char *value);
/**
 * Get value of the uri parameter from a Authorization element.
 * @param header The element to work on.
 */
  char *authorization_geturi (authorization_t * header);
/**
 * Add the uri parameter from a Authorization element.
 * @param header The element to work on.
 * @param value The value of the new parameter.
 */
  void authorization_seturi (authorization_t * header, char *value);
/**
 * Get value of the response parameter from a Authorization element.
 * @param header The element to work on.
 */
  char *authorization_getresponse (authorization_t * header);
/**
 * Add the response parameter from a Authorization element.
 * @param header The element to work on.
 * @param value The value of the new parameter.
 */
  void authorization_setresponse (authorization_t * header, char *value);
/**
 * Get value of the digest parameter from a Authorization element.
 * @param header The element to work on.
 */
  char *authorization_getdigest (authorization_t * header);
/**
 * Add the digest parameter from a Authorization element.
 * @param header The element to work on.
 * @param value The value of the new parameter.
 */
  void authorization_setdigest (authorization_t * header, char *value);
/**
 * Get value of the algorithm parameter from a Authorization element.
 * @param header The element to work on.
 */
  char *authorization_getalgorithm (authorization_t * header);
/**
 * Add the algorithm parameter from a Authorization element.
 * @param header The element to work on.
 * @param value The value of the new parameter.
 */
  void authorization_setalgorithm (authorization_t * header, char *value);
/**
 * Get value of the cnonce parameter from a Authorization element.
 * @param header The element to work on.
 */
  char *authorization_getcnonce (authorization_t * header);
/**
 * Add the cnonce parameter from a Authorization element.
 * @param header The element to work on.
 * @param value The value of the new parameter.
 */
  void authorization_setcnonce (authorization_t * header, char *value);
/**
 * Get value of the opaque parameter from a Authorization element.
 * @param header The element to work on.
 */
  char *authorization_getopaque (authorization_t * header);
/**
 * Add the opaque parameter from a Authorization element.
 * @param header The element to work on.
 * @param value The value of the new parameter.
 */
  void authorization_setopaque (authorization_t * header, char *value);
/**
 * Get value of the message_qop parameter from a Authorization element.
 * @param header The element to work on.
 */
  char *authorization_getmessage_qop (authorization_t * header);
/**
 * Add the message_qop parameter from a Authorization element.
 * @param header The element to work on.
 * @param value The value of the new parameter.
 */
  void authorization_setmessage_qop (authorization_t * header, char *value);
/**
 * Get value of the nonce_count parameter from a Authorization element.
 * @param header The element to work on.
 */
  char *authorization_getnonce_count (authorization_t * header);
/**
 * Add the nonce_count parameter from a Authorization element.
 * @param header The element to work on.
 * @param value The value of the new parameter.
 */
  void authorization_setnonce_count (authorization_t * header, char *value);


/**
 * Allocate a Call-id element.
 * @param header The element to work on.
 */
  int call_id_init (call_id_t ** header);
/**
 * Free a Call-id element.
 * @param header The element to work on.
 */
  void call_id_free (call_id_t * header);
/**
 * Parse a Call-id element.
 * @param header The element to work on.
 * @param hvalue The string to parse.
 */
  int call_id_parse (call_id_t * header, char *hvalue);
/**
 * Get a string representation of a Call-id element.
 * @param header The element to work on.
 * @param dest A pointer on the new allocated string.
 */
  int call_id_2char (call_id_t * header, char **dest);
/**
 * Clone a Call-id element.
 * @param header The element to work on.
 * @param dest A pointer on the copy of the element.
 */
  int call_id_clone (call_id_t * header, call_id_t ** dest);
/**
 * Set the number in the Call-Id element.
 * @param header The element to work on.
 * @param value The value of the element.
 */
  void call_id_setnumber (call_id_t * header, char *value);
/**
 * Get the number from a Call-Id header.
 * @param header The element to work on.
 */
  char *call_id_getnumber (call_id_t * header);
/**
 * Set the host in the Call-Id element.
 * @param header The element to work on.
 * @param value The value of the element.
 */
  void call_id_sethost (call_id_t * header, char *value);
/**
 * Get the host from a Call-Id header.
 * @param header The element to work on.
 */
  char *call_id_gethost (call_id_t * header);

/**
 * Allocate a Call-Info element.
 * @param header The element to work on.
 */
  int call_info_init (call_info_t ** header);
/**
 * Free a Call-Info element.
 * @param header The element to work on.
 */
  void call_info_free (call_info_t * header);
/**
 * Parse a Call-Info element.
 * @param header The element to work on.
 * @param hvalue The string to parse.
 */
  int call_info_parse (call_info_t * header, char *hvalue);
/**
 * Get a string representation of a Call-Info element.
 * @param header The element to work on.
 * @param dest A pointer on the new allocated string.
 */
  int call_info_2char (call_info_t * header, char **dest);
/**
 * Clone a Call-Info element.
 * @param header The element to work on.
 * @param dest A pointer on the copy of the element.
 */
  int call_info_clone (call_info_t * header, call_info_t ** dest);
/**
 * Get the uri from a Call_Info header.
 * @param header The element to work on.
 */
  char *call_info_geturi (call_info_t * header);
/**
 * Set the uri in the Call_Info element.
 * @param header The element to work on.
 * @param uri The value of the element.
 */
  void call_info_seturi (call_info_t * header, char *uri);

/**
 * Allocate a Contact element.
 * @param header The element to work on.
 */
  int contact_init (contact_t ** header);
/**
 * Free a Contact element.
 * @param header The element to work on.
 */
  void contact_free (contact_t * header);
/**
 * Parse a Contact element.
 * @param header The element to work on.
 * @param hvalue The string to parse.
 */
  int contact_parse (contact_t * header, char *hvalue);
/**
 * Get a string representation of a Contact element.
 * @param header The element to work on.
 * @param dest A pointer on the new allocated string.
 */
  int contact_2char (contact_t * header, char **dest);
/**
 * Clone a Contact element.
 * @param header The element to work on.
 * @param dest A pointer on the copy of the element.
 */
  int contact_clone (contact_t * header, contact_t ** dest);
/**
 * Get the displayname from a Contact header.
 * @param header The element to work on.
 */
#define contact_getdisplayname(header) from_getdisplayname((from_t*)header)
/**
 * Set the displayname in the Contact element.
 * @param header The element to work on.
 * @param value The value of the element.
 */
#define contact_setdisplayname(header,value) from_setdisplayname((from_t*)header, value)
/**
 * Get the url from a Contact header.
 * @param header The element to work on.
 */
#define contact_geturl(header)         from_geturl((from_t*)header)
/**
 * Set the url in the Contact element.
 * @param header The element to work on.
 * @param url The value of the element.
 */
#define contact_seturl(header,url)       from_seturl((from_t*)header,url)
/**
 * Get a header parameter from a Contact element.
 * @param header The element to work on.
 * @param pos The index of the element to get.
 * @param dest A pointer on the element found.
 */
#define contact_param_get(header,pos,dest) from_param_get((from_t*)header,pos,dest)
/**
 * Allocate and add a generic parameter element in a list.
 * @param header The element to work on.
 * @param name The token name.
 * @param value The token value.
 */
#define contact_param_add(header,name, value) generic_param_add((header)->gen_params, name,value)
/**
 * Find a header parameter in a Contact element.
 * @param header The element to work on.
 * @param name The token name to search.
 * @param dest A pointer on the element found.
 */
#define contact_param_getbyname(header,name,dest) generic_param_getbyname((header)->gen_params,name,dest)

/**
 * Allocate a Content-Disposition element.
 * @param header The element to work on.
 */
#define content_disposition_init(header)      call_info_init(header)
/**
 * Free a Content-Disposition element.
 * @param header The element to work on.
 */
#define content_disposition_free(header)      call_info_free(header)
/**
 * Parse a Content-Disposition element.
 * @param header The element to work on.
 * @param hvalue The string to parse.
 */
  int content_disposition_parse (content_disposition_t * header,
				 char *hvalue);
/**
 * Get a string representation of a Content-Disposition element.
 * @param header The element to work on.
 * @param dest A pointer on the new allocated string.
 */
#define content_disposition_2char(header,dest)   call_info_2char(header,dest)
/**
 * Clone a Content-Disposition element.
 * @param header The element to work on.
 * @param dest A pointer on the copy of the element.
 */
#define content_disposition_clone(header, dest)  call_info_clone(header, dest)

/* type is of: "render" | "session" | "icon" | "alert" */
/**
 * Set the type in the Content-Disposition element.
 * @param header The element to work on.
 * @param value The value of the element.
 */
#define content_disposition_settype(header, value) call_info_seturi(header, value)
/**
 * Get the type from a Content-Disposition header.
 * @param header The element to work on.
 */
#define content_disposition_gettype(header)    call_info_geturi(header)

/**
 * Allocate a Content-Encoding element.
 * @param header The element to work on.
 */
#define content_encoding_init(header)      content_length_init(header)
/**
 * Parse a Content-Encoding element.
 * @param header The element to work on.
 * @param hvalue The string to parse.
 */
#define content_encoding_parse(header, hvalue)  content_length_parse(header, hvalue)
/**
 * Get a string representation of a Content-Encoding element.
 * @param header The element to work on.
 * @param dest A pointer on the new allocated string.
 */
#define content_encoding_2char(header, dest)  content_length_2char(header, dest)
/**
 * Free a Content-Encoding element.
 * @param header The element to work on.
 */
#define content_encoding_free(header)      content_length_free(header)
/**
 * Clone a Content-Encoding element.
 * @param header The element to work on.
 * @param dest A pointer on the copy of the element.
 */
#define content_encoding_clone(header, dest)  content_length_clone(header, dest)

/**
 * Allocate a Content-Length element.
 * @param header The element to work on.
 */
  int content_length_init (content_length_t ** header);
/**
 * Free a Content-Length element.
 * @param header The element to work on.
 */
  void content_length_free (content_length_t * header);
/**
 * Parse a Content-Length element.
 * @param header The element to work on.
 * @param hvalue The string to parse.
 */
  int content_length_parse (content_length_t * header, char *hvalue);
/**
 * Get a string representation of a Content-Length element.
 * @param header The element to work on.
 * @param dest A pointer on the new allocated string.
 */
  int content_length_2char (content_length_t * header, char **dest);
/**
 * Clone a Content-Length element.
 * @param header The element to work on.
 * @param dest A pointer on the copy of the element.
 */
  int content_length_clone (content_length_t * header,
			    content_length_t ** dest);

/**
 * Allocate a Content-Type element.
 * @param header The element to work on.
 */
  int content_type_init (content_type_t ** header);
/**
 * Free a Content-Type element.
 * @param header The element to work on.
 */
  void content_type_free (content_type_t * header);
/**
 * Parse a Content-Type element.
 * @param header The element to work on.
 * @param hvalue The string to parse.
 */
  int content_type_parse (content_type_t * header, char *hvalue);
/**
 * Get a string representation of a Content-Type element.
 * @param header The element to work on.
 * @param dest A pointer on the new allocated string.
 */
  int content_type_2char (content_type_t * header, char **dest);
/**
 * Clone a Content-Type element.
 * @param header The element to work on.
 * @param dest A pointer on the copy of the element.
 */
  int content_type_clone (content_type_t * header, content_type_t ** dest);

/**
 * Allocate and add a generic parameter element in a list.
 * @param header The element to work on.
 * @param name The token name.
 * @param value The token value.
 */
#define content_type_param_add(header,name,value)  generic_param_add((header)->gen_params,name,value)
/**
 * Find a header parameter in a Content-Type element.
 * @param header The element to work on.
 * @param name The token name to search.
 * @param dest A pointer on the element found.
 */
#define content_type_param_getbyname(header,name,dest) generic_param_getbyname((header)->gen_params,name,dest)

/**
 * Allocate a CSeq element.
 * @param header The element to work on.
 */
  int cseq_init (cseq_t ** header);
/**
 * Free a CSeq element.
 * @param header The element to work on.
 */
  void cseq_free (cseq_t * header);
/**
 * Parse a CSeq element.
 * @param header The element to work on.
 * @param hvalue The string to parse.
 */
  int cseq_parse (cseq_t * header, char *hvalue);
/**
 * Get a string representation of a CSeq element.
 * @param header The element to work on.
 * @param dest A pointer on the new allocated string.
 */
  int cseq_2char (cseq_t * header, char **dest);
/**
 * Clone a CSeq element.
 * @param header The element to work on.
 * @param dest A pointer on the copy of the element.
 */
  int cseq_clone (cseq_t * header, cseq_t ** dest);
/**
 * Set the number in the CSeq element.
 * @param header The element to work on.
 * @param value The value of the element.
 */
  void cseq_setnumber (cseq_t * header, char *value);
/**
 * Get the number from a CSeq header.
 * @param header The element to work on.
 */
  char *cseq_getnumber (cseq_t * header);
/**
 * Set the method in the CSeq element.
 * @param header The element to work on.
 * @param value The value of the element.
 */
  void cseq_setmethod (cseq_t * header, char *value);
/**
 * Get the method from a CSeq header.
 * @param header The element to work on.
 */
  char *cseq_getmethod (cseq_t * header);

/**
 * Allocate a Error-Info element.
 * @param header The element to work on.
 */
#define  error_info_init(header)      call_info_init(header)
/**
 * Free a Error-Info element.
 * @param header The element to work on.
 */
#define  error_info_free(header)      call_info_free(header)
/**
 * Parse a Error-Info element.
 * @param header The element to work on.
 * @param hvalue The string to parse.
 */
#define  error_info_parse(header, hvalue)  call_info_parse(header, hvalue)
/**
 * Get a string representation of a Error-Info element.
 * @param header The element to work on.
 * @param dest A pointer on the new allocated string.
 */
#define  error_info_2char(header,dest)   call_info_2char(header,dest)
/**
 * Clone a Error-Info element.
 * @param header The element to work on.
 * @param dest A pointer on the copy of the element.
 */
#define  error_info_clone(header, dest)  call_info_clone(header, dest)

/**
 * Set the uri in the Error-Info element.
 * @param header The element to work on.
 * @param uri The uri of the element.
 */
#define  error_info_seturi(header, uri) call_info_seturi(header, uri)
/**
 * Get the uri from a Error-Info header.
 * @param header The element to work on.
 */
#define  error_info_geturi(header)    call_info_geturi(header)

/**
 * Allocate a From element.
 * @param header The element to work on.
 */
  int from_init (from_t ** header);
/**
 * Free a From element.
 * @param header The element to work on.
 */
  void from_free (from_t * header);
/**
 * Parse a From element.
 * @param header The element to work on.
 * @param hvalue The string to parse.
 */
  int from_parse (from_t * header, char *hvalue);
/**
 * Get a string representation of a From element.
 * @param header The element to work on.
 * @param dest A pointer on the new allocated string.
 */
  int from_2char (from_t * header, char **dest);
/**
 * Clone a From element.
 * @param header The element to work on.
 * @param dest A pointer on the copy of the element.
 */
  int from_clone (from_t * header, from_t ** dest);
/**
 * Set the displayname in the From element.
 * @param header The element to work on.
 * @param value The value of the element.
 */
  void from_setdisplayname (from_t * header, char *value);
/**
 * Get the displayname from a From header.
 * @param header The element to work on.
 */
  char *from_getdisplayname (from_t * header);
/**
 * Set the url in the From element.
 * @param header The element to work on.
 * @param url The value of the element.
 */
  void from_seturl (from_t * header, url_t * url);
/**
 * Get the url from a From header.
 * @param header The element to work on.
 */
  url_t *from_geturl (from_t * header);
/**
 * Get a header parameter from a From element.
 * @param header The element to work on.
 * @param pos The index of the element to get.
 * @param dest A pointer on the element found.
 */
  int from_param_get (from_t * header, int pos, generic_param_t ** dest);
/**
 * Allocate and add a generic parameter element in a list.
 * @param header The element to work on.
 * @param name The token name.
 * @param value The token value.
 */
#define from_param_add(header,name,value)      generic_param_add((header)->gen_params,name,value)
/**
 * Find a header parameter in a From element.
 * @param header The element to work on.
 * @param name The token name to search.
 * @param dest A pointer on the element found.
 */
#define from_param_getbyname(header,name,dest) generic_param_getbyname((header)->gen_params,name,dest)

/**
 * Find the tag parameter in a From element.
 * @param header The element to work on.
 * @param dest A pointer on the element found.
 */
#define from_get_tag(header,dest)    generic_param_getbyname((header)->gen_params, "tag",dest)
/**
 * Allocate and add a tag parameter element in a Contact element.
 * @param header The element to work on.
 * @param value The token value.
 */
#define from_set_tag(header,value)     generic_param_add((header)->gen_params, sgetcopy("tag"),value)

#ifndef DOXYGEN			/* avoid DOXYGEN warning */
/* Compare the username, host and tag part (if exist) of the two froms */
  int from_compare (from_t * header1, from_t * header2);
#endif

/**
 * Allocate a Mime-Version element.
 * @param header The element to work on.
 */
#define mime_version_init(header)      content_length_init(header)
/**
 * Parse a Mime-Version element.
 * @param header The element to work on.
 * @param hvalue The string to parse.
 */
#define mime_version_parse(header, hvalue)  content_length_parse(header, hvalue)
/**
 * Get a string representation of a Mime-Version element.
 * @param header The element to work on.
 * @param dest A pointer on the new allocated string.
 */
#define mime_version_2char(header, dest)  content_length_2char(header, dest)
/**
 * Free a Mime-Version element.
 * @param header The element to work on.
 */
#define mime_version_free(header)      content_length_free(header)
/**
 * Clone a Mime-Version element.
 * @param header The element to work on.
 * @param dest A pointer on the copy of the element.
 */
#define mime_version_clone(header, dest)  content_length_clone(header, dest)

/**
 * Allocate a Proxy-Authenticate element.
 * @param header The element to work on.
 */
#define proxy_authenticate_init(header)     www_authenticate_init(header)
/**
 * Parse a Proxy-Authenticate element.
 * @param header The element to work on.
 * @param hvalue The string to parse.
 */
#define proxy_authenticate_parse(header, hvalue) www_authenticate_parse(header, hvalue)
/**
 * Get a string representation of a Proxy-Authenticate element.
 * @param header The element to work on.
 * @param dest A pointer on the new allocated string.
 */
#define proxy_authenticate_2char(header, dest) www_authenticate_2char(header, dest)
/**
 * Free a Proxy-Authenticate element.
 * @param header The element to work on.
 */
#define proxy_authenticate_free(header)     www_authenticate_free(header)
/**
 * Clone a Proxy-Authenticate element.
 * @param header The element to work on.
 * @param dest A pointer on the copy of the element.
 */
#define proxy_authenticate_clone(header, dest) www_authenticate_clone(header, dest)

/**
 * Get value of the auth_type parameter from a Proxy-Authenticate element.
 * @param header The element to work on.
 */
#define proxy_authenticate_getauth_type(header)   www_authenticate_getauth_type(header)
/**
 * Add the auth_type parameter from a Proxy-Authenticate element.
 * @param header The element to work on.
 * @param value The value of the new parameter.
 */
#define proxy_authenticate_setauth_type(header,value) www_authenticate_setauth_type(header, value)
/**
 * Get value of the realm parameter from a Proxy-Authenticate element.
 * @param header The element to work on.
 */
#define proxy_authenticate_getrealm(header)     www_authenticate_getrealm(header)
/**
 * Add the realm parameter from a Proxy-Authenticate element.
 * @param header The element to work on.
 * @param value The value of the new parameter.
 */
#define proxy_authenticate_setrealm(header, value)  www_authenticate_setrealm(header, value)
/**
 * Get value of the domain parameter from a Proxy-Authenticate element.
 * @param header The element to work on.
 */
#define proxy_authenticate_getdomain(header)    www_authenticate_getdomain(header)
/**
 * Add the domain parameter from a Proxy-Authenticate element.
 * @param header The element to work on.
 * @param value The value of the new parameter.
 */
#define proxy_authenticate_setdomain(header, value) www_authenticate_setdomain(header, value)
/**
 * Get value of the nonce parameter from a Proxy-Authenticate element.
 * @param header The element to work on.
 */
#define proxy_authenticate_getnonce(header)     www_authenticate_getnonce(header)
/**
 * Add the nonce parameter from a Proxy-Authenticate element.
 * @param header The element to work on.
 * @param value The value of the new parameter.
 */
#define proxy_authenticate_setnonce(header, value)  www_authenticate_setnonce(header, value)
/**
 * Get value of the opaque parameter from a Proxy-Authenticate element.
 * @param header The element to work on.
 */
#define proxy_authenticate_getopaque(header)    www_authenticate_getopaque(header)
/**
 * Add the opaque parameter from a Proxy-Authenticate element.
 * @param header The element to work on.
 * @param value The value of the new parameter.
 */
#define proxy_authenticate_setopaque(header, value) www_authenticate_setopaque(header, value)
/**
 * Get value of the stale parameter from a Proxy-Authenticate element.
 * @param header The element to work on.
 */
#define proxy_authenticate_getstale(header)     www_authenticate_getstale(header)
/**
 * Add the stale parameter from a Proxy-Authenticate element.
 * @param header The element to work on.
 * @param value The value of the new parameter.
 */
#define proxy_authenticate_setstale(header, value)  www_authenticate_setstale(header, value)
/**
 * Add a stale parameter set to "true" in a proxy-Authenticate element.
 * @param header The element to work on.
 */
#define proxy_authenticate_setstale_true(header) www_authenticate_setstale(header,sgetcopy("true"))
/**
 * Add a stale parameter set to "false" in a Proxy-Authenticate element.
 * @param header The element to work on.
 */
#define proxy_authenticate_setstale_false(header) www_authenticate_setstale(header,sgetcopy("false"))
/**
 * Get value of the algorithm parameter from a Proxy-Authenticate element.
 * @param header The element to work on.
 */
#define proxy_authenticate_getalgorithm(header) www_authenticate_getalgorithm(header)
/**
 * Add the algorithm parameter from a Proxy-Authenticate element.
 * @param header The element to work on.
 * @param value The value of the new parameter.
 */
#define proxy_authenticate_setalgorithm(header, value) www_authenticate_setalgorithm(header, value)
/**
 * Add the algorithm parameter set to "MD5" in a Proxy-Authenticate element.
 * @param header The element to work on.
 */
#define proxy_authenticate_setalgorithm_MD5(header) www_authenticate_setalgorithm(header,sgetcopy("MD5"))
/**
 * Get value of the qop_options parameter from a Proxy-Authenticate element.
 * @param header The element to work on.
 */
#define proxy_authenticate_getqop_options(header) www_authenticate_getqop_options(header)
/**
 * Add the qop_options parameter from a Proxy-Authenticate element.
 * @param header The element to work on.
 * @param value The value of the new parameter.
 */
#define proxy_authenticate_setqop_options(header,value) www_authenticate_setqop_options(header,value)


/**
 * Allocate a Proxy-Authorization element.
 * @param header The element to work on.
 */
#define proxy_authorization_init(header)     authorization_init(header)
/**
 * Parse a Proxy-Authorization element.
 * @param header The element to work on.
 * @param hvalue The string to parse.
 */
#define proxy_authorization_parse(header, hvalue) authorization_parse(header, hvalue)
/**
 * Get a string representation of a Proxy-Authorization element.
 * @param header The element to work on.
 * @param dest A pointer on the new allocated string.
 */
#define proxy_authorization_2char(header, dest) authorization_2char(header, dest)
/**
 * Free a Proxy-Authorization element.
 * @param header The element to work on.
 */
#define proxy_authorization_free(header)     authorization_free(header)
/**
 * Clone a Proxy-Authorization element.
 * @param header The element to work on.
 * @param dest A pointer on the copy of the element.
 */
#define proxy_authorization_clone(header, dest) authorization_clone(header, dest)

/**
 * Get value of the auth_type parameter from a Proxy-Authorization element.
 * @param header The element to work on.
 */
#define proxy_authorization_getauth_type(header)    authorization_getauth_type(header)
/**
 * Add the auth_type parameter from a Proxy-Authorization element.
 * @param header The element to work on.
 * @param value The value of the new parameter.
 */
#define proxy_authorization_setauth_type(header, value) authorization_setauth_type(header, value)
/**
 * Get value of the username parameter from a Proxy-Authorization element.
 * @param header The element to work on.
 */
#define proxy_authorization_getusername(header)    authorization_getusername(header)
/**
 * Add the username parameter from a Proxy-Authorization element.
 * @param header The element to work on.
 * @param value The value of the new parameter.
 */
#define proxy_authorization_setusername(header, value) authorization_setusername(header, value)
/**
 * Get value of the realm parameter from a Proxy-Authorization element.
 * @param header The element to work on.
 */
#define proxy_authorization_getrealm(header)       authorization_getrealm(header)
/**
 * Add the realm parameter from a Proxy-Authorization element.
 * @param header The element to work on.
 * @param value The value of the new parameter.
 */
#define proxy_authorization_setrealm(header, value)    authorization_setrealm(header, value)
/**
 * Get value of the nonce parameter from a Proxy-Authorization element.
 * @param header The element to work on.
 */
#define proxy_authorization_getnonce(header)       authorization_getnonce(header)
/**
 * Add the nonce parameter from a Proxy-Authorization element.
 * @param header The element to work on.
 * @param value The value of the new parameter.
 */
#define proxy_authorization_setnonce(header, value)    authorization_setnonce(header, value)
/**
 * Get value of the uri parameter from a Proxy-Authorization element.
 * @param header The element to work on.
 */
#define proxy_authorization_geturi(header)         authorization_geturi(header)
/**
 * Add the uri parameter from a Proxy-Authorization element.
 * @param header The element to work on.
 * @param value The value of the new parameter.
 */
#define proxy_authorization_seturi(header, value)      authorization_seturi(header, value)
/**
 * Get value of the response parameter from a Proxy-Authorization element.
 * @param header The element to work on.
 */
#define proxy_authorization_getresponse(header)    authorization_getresponse(header)
/**
 * Add the response parameter from a Proxy-Authorization element.
 * @param header The element to work on.
 * @param value The value of the new parameter.
 */
#define proxy_authorization_setresponse(header, value) authorization_setresponse(header, value)
/**
 * Get value of the digest parameter from a Proxy-Authorization element.
 * @param header The element to work on.
 */
#define proxy_authorization_getdigest(header)      authorization_getdigest(header)
/**
 * Add the digest parameter from a Proxy-Authorization element.
 * @param header The element to work on.
 * @param value The value of the new parameter.
 */
#define proxy_authorization_setdigest(header, value)   authorization_setdigest(header, value)
/**
 * Get value of the algorithm parameter from a Proxy-Authorization element.
 * @param header The element to work on.
 */
#define proxy_authorization_getalgorithm(header)   authorization_getalgorithm(header)
/**
 * Add the algorithm parameter from a Proxy-Authorization element.
 * @param header The element to work on.
 * @param value The value of the new parameter.
 */
#define proxy_authorization_setalgorithm(header,value) authorization_setalgorithm(header,value)
/**
 * Get value of the cnonce parameter from a Proxy-Authorization element.
 * @param header The element to work on.
 */
#define proxy_authorization_getcnonce(header)      authorization_getcnonce(header)
/**
 * Add the cnonce parameter from a Proxy-Authorization element.
 * @param header The element to work on.
 * @param value The value of the new parameter.
 */
#define proxy_authorization_setcnonce(header, value)   authorization_setcnonce(header, value)
/**
 * Get value of the opaque parameter from a Proxy-Authorization element.
 * @param header The element to work on.
 */
#define proxy_authorization_getopaque(header)      authorization_getopaque(header)
/**
 * Add the opaque parameter from a Proxy-Authorization element.
 * @param header The element to work on.
 * @param value The value of the new parameter.
 */
#define proxy_authorization_setopaque(header, value)   authorization_setopaque(header, value)
/**
 * Get value of the message_qop parameter from a Proxy-Authorization element.
 * @param header The element to work on.
 */
#define proxy_authorization_getmessage_qop(header) authorization_getmessage_qop(header)
/**
 * Add the message_qop parameter from a Proxy-Authorization element.
 * @param header The element to work on.
 * @param value The value of the new parameter.
 */
#define proxy_authorization_setmessage_qop(header, value) authorization_setmessage_qop(header, value)
/**
 * Get value of the nonce_count parameter from a Proxy-Authorization element.
 * @param header The element to work on.
 */
#define proxy_authorization_getnonce_count(header) authorization_getnonce_count(header)
/**
 * Add the nonce_count parameter from a Proxy-Authorization element.
 * @param header The element to work on.
 * @param value The value of the new parameter.
 */
#define proxy_authorization_setnonce_count(header, value) authorization_setnonce_count(header, value)


/**
 * Allocate a Record-Route element.
 * @param header The element to work on.
 */
  int record_route_init (record_route_t ** header);
/**
 * Free a Record-Route element.
 * @param header The element to work on.
 */
  void record_route_free (record_route_t * header);
/**
 * Parse a Record-Route element.
 * @param header The element to work on.
 * @param hvalue The string to parse.
 */
  int record_route_parse (record_route_t * header, char *hvalue);
/**
 * Get a string representation of a Record-Route element.
 * @param header The element to work on.
 * @param dest A pointer on the new allocated string.
 */
  int record_route_2char (record_route_t * header, char **dest);
/**
 * Clone a Record-Route element.
 * @param header The element to work on.
 * @param dest A pointer on the copy of the element.
 */
#define record_route_clone(header,dest)      from_clone(header,dest)
/**
 * Set the url in the Record-Route element.
 * @param header The element to work on.
 * @param url The value of the element.
 */
#define record_route_seturl(header,url)      from_seturl((from_t*)header,url)
/**
 * Get the url from a Record-Route header.
 * @param header The element to work on.
 */
#define record_route_geturl(header)          from_geturl((from_t*)header)
/**
 * Get a header parameter from a Record-Route element.
 * @param header The element to work on.
 * @param pos The index of the element to get.
 * @param dest A pointer on the element found.
 */
#define record_route_param_get(header,pos,dest)  from_param_get((from_t*)header,pos,dest)
/**
 * Allocate and add a generic parameter element in a list.
 * @param header The element to work on.
 * @param name The token name.
 * @param value The token value.
 */
#define record_route_param_add(header,name,value) generic_param_add((header)->gen_params,name,value)
/**
 * Find a header parameter in a Record-Route element.
 * @param header The element to work on.
 * @param name The token name to search.
 * @param dest A pointer on the element found.
 */
#define record_route_param_getbyname(header,name,dest) generic_param_getbyname((header)->gen_params,name,dest)

#ifdef __VXWORKS_OS__
/* route_init is already defined somewhere on VXWORKS.. */
  int route_init2 (route_t ** header);
#else
/**
 * Allocate a Route element.
 * @param header The element to work on.
 */
  int route_init (route_t ** header);
#endif
/**
 * Free a Route element.
 * @param header The element to work on.
 */
  void route_free (route_t * header);
/**
 * Parse a Route element.
 * @param header The element to work on.
 * @param hvalue The string to parse.
 */
  int route_parse (route_t * header, char *hvalue);
/**
 * Get a string representation of a Route element.
 * @param header The element to work on.
 * @param dest A pointer on the new allocated string.
 */
  int route_2char (route_t * header, char **dest);
/**
 * Clone a Route element.
 * @param header The element to work on.
 * @param dest A pointer on the copy of the element.
 */
#define route_clone(header,dest)       from_clone(header,dest)
/**
 * Set the url in the Route element.
 * @param header The element to work on.
 * @param url The value of the element.
 */
#define route_seturl(header,url)    from_seturl((from_t*)header,url)
/**
 * Get the url from a Route header.
 * @param header The element to work on.
 */
#define route_geturl(header)        from_geturl((from_t*)header)
/**
 * Get a header parameter from a Route element.
 * @param header The element to work on.
 * @param pos The index of the element to get.
 * @param dest A pointer on the element found.
 */
#define route_param_get(header,pos,dest) from_param_get((from_t*)header,pos,dest)
/**
 * Allocate and add a generic parameter element in a Route element.
 * @param header The element to work on.
 * @param name The token name.
 * @param value The token value.
 */
#define route_param_add(header,name,value)   generic_param_add((header)->gen_params,name,value)
/**
 * Find a header parameter in a Route element.
 * @param header The element to work on.
 * @param name The token name to search.
 * @param dest A pointer on the element found.
 */
#define route_param_getbyname(header,name,dest) generic_param_getbyname((header)->gen_params,name,dest)

/**
 * Allocate a To element.
 * @param header The element to work on.
 */
  int to_init (to_t ** header);
/**
 * Free a To element.
 * @param header The element to work on.
 */
  void to_free (to_t * header);
/**
 * Parse a To element.
 * @param header The element to work on.
 * @param hvalue The string to parse.
 */
  int to_parse (to_t * header, char *hvalue);
/**
 * Get a string representation of a To element.
 * @param header The element to work on.
 * @param dest A pointer on the new allocated string.
 */
  int to_2char (to_t * header, char **dest);
/**
 * Clone a To element.
 * @param header The element to work on.
 * @param dest A pointer on the copy of the element.
 */
  int to_clone (to_t * header, to_t ** dest);
/**
 * Set the displayname in the To element.
 * @param header The element to work on.
 * @param value The value of the element.
 */
#define to_setdisplayname(header,value) from_setdisplayname((from_t*)header,value)
/**
 * Get the displayname from a To header.
 * @param header The element to work on.
 */
#define to_getdisplayname(header)       from_getdisplayname((from_t*)header)
/**
 * Set the url in the To element.
 * @param header The element to work on.
 * @param url The value of the element.
 */
#define to_seturl(header,url)         from_seturl((from_t*)header,url)
/**
 * Get the url from a To header.
 * @param header The element to work on.
 */
#define to_geturl(header)               from_geturl((from_t*)header)
/**
 * Get a header parameter from a To element.
 * @param header The element to work on.
 * @param pos The index of the element to get.
 * @param dest A pointer on the element found.
 */
#define to_param_get(header,pos,dest) from_param_get((from_t*)header,pos,dest)
/**
 * Find a header parameter in a To element.
 * @param header The element to work on.
 * @param name The token name to search.
 * @param dest A pointer on the element found.
 */
#define to_param_getbyname(header,name,dest) generic_param_getbyname((header)->gen_params,name,dest)
/**
 * Allocate and add a generic parameter element in a list.
 * @param header The element to work on.
 * @param name The token name.
 * @param value The token value.
 */
#define to_param_add(header,name,value) generic_param_add((header)->gen_params,name,value)

/**
 * Allocate and add a tag parameter element in a list.
 * @param header The element to work on.
 * @param value The token value.
 */
#define to_set_tag(header,value) generic_param_add((header)->gen_params, sgetcopy("tag"),value)
/**
 * Find a tag parameter in a To element.
 * @param header The element to work on.
 * @param dest A pointer on the element found.
 */
#define to_get_tag(header,dest) generic_param_getbyname((header)->gen_params, "tag",dest)

#ifndef DOXYGEN			/* avoid DOXYGEN warning */
/* Compare the username, host and tag part of the two froms */
#define to_compare(header1, header2) from_compare((from_t *)header1, (from_t *)header2)
#endif


/**
 * Allocate a Via element.
 * @param header The element to work on.
 */
  int via_init (via_t ** header);
/**
 * Free a Via element.
 * @param header The element to work on.
 */
  void via_free (via_t * header);
/**
 * Parse a Via element.
 * @param header The element to work on.
 * @param hvalue The string to parse.
 */
  int via_parse (via_t * header, char *hvalue);
/**
 * Get a string representation of a Via element.
 * @param header The element to work on.
 * @param dest A pointer on the new allocated string.
 */
  int via_2char (via_t * header, char **dest);
/**
 * Clone a Via element.
 * @param header The element to work on.
 * @param dest A pointer on the copy of the element.
 */
  int via_clone (via_t * header, via_t ** dest);
/**
 * Set the SIP version in the Via element.
 * @param header The element to work on.
 * @param value The value of the element.
 */
  void via_setversion (via_t * header, char *value);
/**
 * Get the SIP version from a Via header.
 * @param header The element to work on.
 */
  char *via_getversion (via_t * header);
/**
 * Set the protocol in the Via element.
 * @param header The element to work on.
 * @param value The value of the element.
 */
  void via_setprotocol (via_t * header, char *value);
/**
 * Get the protocol from a Via header.
 * @param header The element to work on.
 */
  char *via_getprotocol (via_t * header);
/**
 * Set the host in the Via element.
 * @param header The element to work on.
 * @param value The value of the element.
 */
  void via_sethost (via_t * header, char *value);
/**
 * Get the host from a Via header.
 * @param header The element to work on.
 */
  char *via_gethost (via_t * header);
/**
 * Set the port in the Via element.
 * @param header The element to work on.
 * @param value The value of the element.
 */
  void via_setport (via_t * header, char *value);
/**
 * Get the port from a Via header.
 * @param header The element to work on.
 */
  char *via_getport (via_t * header);
/**
 * Set the comment in the Via element.
 * @param header The element to work on.
 * @param value The value of the element.
 */
  void via_setcomment (via_t * header, char *value);
/**
 * Get the comment from a Via header.
 * @param header The element to work on.
 */
  char *via_getcomment (via_t * header);

/**
 * Allocate and add a hidden parameter element in a list.
 * @param header The element to work on.
 */
#define via_set_hidden(header)    generic_param_add((header)->via_params,sgetcopy("hidden"),NULL)
/**
 * Allocate and add a ttl parameter element in a list.
 * @param header The element to work on.
 * @param value The token value.
 */
#define via_set_ttl(header,value)   generic_param_add((header)->via_params,sgetcopy("ttl"),value)
/**
 * Allocate and add a maddr parameter element in a list.
 * @param header The element to work on.
 * @param value The token value.
 */
#define via_set_maddr(header,value)   generic_param_add((header)->via_params,sgetcopy("maddr"),value)
/**
 * Allocate and add a received parameter element in a list.
 * @param header The element to work on.
 * @param value The token value.
 */
#define via_set_received(header,value) generic_param_add((header)->via_params,sgetcopy("received"),value)
/**
 * Allocate and add a branch parameter element in a list.
 * @param header The element to work on.
 * @param value The token value.
 */
#define via_set_branch(header,value)  generic_param_add((header)->via_params,sgetcopy("branch"),value)

/**
 * Allocate and add a generic parameter element in a list.
 * @param header The element to work on.
 * @param name The token name.
 * @param value The token value.
 */
#define via_param_add(header,name,value)      generic_param_add((header)->via_params,name,value)
/**
 * Find a header parameter in a Via element.
 * @param header The element to work on.
 * @param name The token name to search.
 * @param dest A pointer on the element found.
 */
#define via_param_getbyname(header,name,dest) generic_param_getbyname((header)->via_params,name,dest)


/**
 * Allocate a Www-Authenticate element.
 * @param header The element to work on.
 */
  int www_authenticate_init (www_authenticate_t ** header);
/**
 * Parse a Www-Authenticate element.
 * @param header The element to work on.
 * @param hvalue The string to parse.
 */
  int www_authenticate_parse (www_authenticate_t * header, char *hvalue);
/**
 * Get a string representation of a Www-Authenticate element.
 * @param header The element to work on.
 * @param dest A pointer on the new allocated string.
 */
  int www_authenticate_2char (www_authenticate_t * header, char **dest);
/**
 * Free a Www-Authenticate element.
 * @param header The element to work on.
 */
  void www_authenticate_free (www_authenticate_t * header);
/**
 * Clone a Www-Authenticate element.
 * @param header The element to work on.
 * @param dest A pointer on the copy of the element.
 */
  int www_authenticate_clone (www_authenticate_t * header,
			      www_authenticate_t ** dest);

/**
 * Get value of the auth_type parameter from a Www-Authenticate element.
 * @param header The element to work on.
 */
  char *www_authenticate_getauth_type (www_authenticate_t * header);
/**
 * Add the auth_type parameter from a Www-Authenticate element.
 * @param header The element to work on.
 * @param value The value of the new parameter.
 */
  void www_authenticate_setauth_type (www_authenticate_t * header,
				      char *value);
/**
 * Get value of the realm parameter from a Www-Authenticate element.
 * @param header The element to work on.
 */
  char *www_authenticate_getrealm (www_authenticate_t * header);
/**
 * Add the realm parameter from a Www-Authenticate element.
 * @param header The element to work on.
 * @param value The value of the new parameter.
 */
  void www_authenticate_setrealm (www_authenticate_t * header, char *value);
/**
 * Get value of the domain parameter from a Www-Authenticate element.
 * @param header The element to work on.
 */
  char *www_authenticate_getdomain (www_authenticate_t * header);
/**
 * Add the domain parameter from a Www-Authenticate element.
 * @param header The element to work on.
 * @param value The value of the new parameter.
 */
  void www_authenticate_setdomain (www_authenticate_t * header, char *value);
/**
 * Get value of the nonce parameter from a Www-Authenticate element.
 * @param header The element to work on.
 */
  char *www_authenticate_getnonce (www_authenticate_t * header);
/**
 * Add the nonce parameter from a Www-Authenticate element.
 * @param header The element to work on.
 * @param value The value of the new parameter.
 */
  void www_authenticate_setnonce (www_authenticate_t * header, char *value);
/**
 * Get value of the opaque parameter from a Www-Authenticate element.
 * @param header The element to work on.
 */
  char *www_authenticate_getopaque (www_authenticate_t * header);
/**
 * Add the opaque parameter from a Www-Authenticate element.
 * @param header The element to work on.
 * @param value The value of the new parameter.
 */
  void www_authenticate_setopaque (www_authenticate_t * header, char *value);
/**
 * Get value of the stale parameter from a Www-Authenticate element.
 * @param header The element to work on.
 */
  char *www_authenticate_getstale (www_authenticate_t * header);
/**
 * Add the stale parameter in a Www-Authenticate element.
 * @param header The element to work on.
 * @param value The value of the new parameter.
 */
  void www_authenticate_setstale (www_authenticate_t * header, char *value);
/**
 * Add a stale parameter set to "true" in a Www-Authenticate element.
 * @param header The element to work on.
 */
#define www_authenticate_setstale_true(header) www_authenticate_setstale(header,sgetcopy("true"))
/**
 * Add a stale parameter set to "false" in a Www-Authenticate element.
 * @param header The element to work on.
 */
#define www_authenticate_setstale_false(header) www_authenticate_setstale(header,sgetcopy("false"))
/**
 * Get value of the algorithm parameter from a Www-Authenticate element.
 * @param header The element to work on.
 */
  char *www_authenticate_getalgorithm (www_authenticate_t * header);
/**
 * Add the algorithm parameter in a Www-Authenticate element.
 * @param header The element to work on.
 * @param value The value of the new parameter.
 */
  void www_authenticate_setalgorithm (www_authenticate_t * header,
				      char *value);
/**
 * Add the algorithm parameter set to "MD5" in a Www-Authenticate element.
 * @param header The element to work on.
 */
#define www_authenticate_setalgorithm_MD5(header) www_authenticate_setalgorithm(header,sgetcopy("MD5"))
/**
 * Get value of the qop_options parameter from a Www-Authenticate element.
 * @param header The element to work on.
 */
  char *www_authenticate_getqop_options (www_authenticate_t * header);
/**
 * Add the qop_options parameter from a Www-Authenticate element.
 * @param header The element to work on.
 * @param value The value of the new parameter.
 */
  void www_authenticate_setqop_options (www_authenticate_t * header,
					char *value);


/* trace facilities */
#ifndef DOXYGEN			/* avoid DOXYGEN warning */
#ifdef ENABLE_TRACE
  void msg_logrequest (sip_t * sip, char *fmt);
  void msg_logresponse (sip_t * sip, char *fmt);
#else
  #define msg_logrequest(P,Q) ;
  #define msg_logresponse(P,Q) ;
#endif
#endif


/**
 * Allocate and Add a new Date header.
 * @param sipmsg The element to work on.
 * @param value the value of the new header.
 */
#define msg_setdate(sipmsg,value)            msg_setheader((sip_t *)sipmsg,(char *)"date",value)
/**
 * Find a Date header.
 * @param sipmsg The element to work on.
 * @param pos The index of the header in the list of unknown header.
 * @param dest A pointer on the element found.
 */
#define msg_getdate(sipmsg,pos,dest)          msg_header_getbyname(( sip_t *)sipmsg,"date",pos,(header_t **)dest)
/**
 * Allocate and Add a new Encryption header.
 * @param sipmsg The element to work on.
 * @param value the value of the new header.
 */
#define msg_setencryption(sipmsg,value)      msg_setheader((sip_t *)sipmsg,(char *)"encryption",value)
/**
 * Find an Encryption header.
 * @param sipmsg The element to work on.
 * @param pos The index of the header in the list of unknown header.
 * @param dest A pointer on the element found.
 */
#define msg_getencryption(sipmsg,pos,dest)    msg_header_getbyname(( sip_t *)sipmsg,"encryption",pos,(header_t **)dest)
/**
 * Allocate and Add a new Organization header.
 * @param sipmsg The element to work on.
 * @param value the value of the new header.
 */
#define msg_setorganization(sipmsg,value)    msg_setheader((sip_t *)sipmsg,(char *)"organization",value)
/**
 * Find an Organization header.
 * @param sipmsg The element to work on.
 * @param pos The index of the header in the list of unknown header.
 * @param dest A pointer on the element found.
 */
#define msg_getorganization(sipmsg,pos,dest)  msg_header_getbyname(( sip_t *)sipmsg,"organization",pos,(header_t **)dest)
/**
 * Allocate and Add a new Require header.
 * @param sipmsg The element to work on.
 * @param value the value of the new header.
 */
#define msg_setrequire(sipmsg,value)         msg_setheader((sip_t *)sipmsg,(char *)"require",value)
/**
 * Find a Require header.
 * @param sipmsg The element to work on.
 * @param pos The index of the header in the list of unknown header.
 * @param dest A pointer on the element found.
 */
#define msg_getrequire(sipmsg,pos,dest)       msg_header_getbyname(( sip_t *)sipmsg,"require",pos,(header_t **)dest)
/**
 * Allocate and Add a new Supported header.
 * @param sipmsg The element to work on.
 * @param value the value of the new header.
 */
#define msg_setsupported(sipmsg,value)       msg_setheader((sip_t *)sipmsg,(char *)"supported",value)
/**
 * Find a Supported header.
 * @param sipmsg The element to work on.
 * @param pos The index of the header in the list of unknown header.
 * @param dest A pointer on the element found.
 */
#define msg_getsupported(sipmsg,pos,dest)     msg_header_getbyname(( sip_t *)sipmsg,"supported",pos,(header_t **)dest)
/**
 * Allocate and Add a new Timestamp header.
 * @param sipmsg The element to work on.
 * @param value the value of the new header.
 */
#define msg_settimestamp(sipmsg,value)       msg_setheader((sip_t *)sipmsg,(char *)"timestamp",value)
/**
 * Find a Timestamp header.
 * @param sipmsg The element to work on.
 * @param pos The index of the header in the list of unknown header.
 * @param dest A pointer on the element found.
 */
#define msg_gettimestamp(sipmsg,pos,dest)     msg_header_getbyname(( sip_t *)sipmsg,"timestamp",pos,(header_t **)dest)
/**
 * Allocate and Add a new User-Agent header.
 * @param sipmsg The element to work on.
 * @param value the value of the new header.
 */
#define msg_setuser_agent(sipmsg,value)      msg_setheader((sip_t *)sipmsg,(char *)"user-agent",value)
/**
 * Find a User-Agent header.
 * @param sipmsg The element to work on.
 * @param pos The index of the header in the list of unknown header.
 * @param dest A pointer on the element found.
 */
#define msg_getuser_agent(sipmsg,pos,dest)    msg_header_getbyname(( sip_t *)sipmsg,"user-agent",pos,(header_t **)dest)
/**
 * Allocate and Add a new Content-Language header.
 * @param sipmsg The element to work on.
 * @param value the value of the new header.
 */
#define msg_setcontent_language(sipmsg,value) msg_setheader((sip_t *)sipmsg,(char *)"content-language",value)
/**
 * Find a Content-Language header.
 * @param sipmsg The element to work on.
 * @param pos The index of the header in the list of unknown header.
 * @param dest A pointer on the element found.
 */
#define msg_getcontent_language(sipmsg,pos,dest) msg_header_getbyname(( sip_t *)sipmsg,"content-language",pos,(header_t **)dest)
/**
 * Allocate and Add a new Expires header.
 * @param sipmsg The element to work on.
 * @param value the value of the new header.
 */
#define msg_setexpires(sipmsg,value)         msg_setheader((sip_t *)sipmsg,(char *)"expires",value)
/**
 * Find a Expires header.
 * @param sipmsg The element to work on.
 * @param pos The index of the header in the list of unknown header.
 * @param dest A pointer on the element found.
 */
#define msg_getexpires(sipmsg,pos,dest)       msg_header_getbyname(( sip_t *)sipmsg,"expires",pos,(header_t **)dest)
/**
 * Allocate and Add a new In-Reply-To header.
 * @param sipmsg The element to work on.
 * @param value the value of the new header.
 */
#define msg_setin_reply_to(sipmsg,value)     msg_setheader((sip_t *)sipmsg,(char *)"in-reply-to",value)
/**
 * Find a In-Reply-To header.
 * @param sipmsg The element to work on.
 * @param pos The index of the header in the list of unknown header.
 * @param dest A pointer on the element found.
 */
#define msg_getin_reply_to(sipmsg,pos,dest)   msg_header_getbyname(( sip_t *)sipmsg,"in-reply-to",pos,(header_t **)dest)
/**
 * Allocate and Add a new Max-Forward header.
 * @param sipmsg The element to work on.
 * @param value the value of the new header.
 */
#define msg_setmax_forward(sipmsg,value)     msg_setheader((sip_t *)sipmsg,(char *)"max-forwards",value)
#define msg_setmax_forwards(sipmsg,value)     msg_setheader((sip_t *)sipmsg,(char *)"max-forwards",value)
/**
 * Find a Max-Forward header.
 * @param sipmsg The element to work on.
 * @param pos The index of the header in the list of unknown header.
 * @param dest A pointer on the element found.
 */
#define msg_getmax_forward(sipmsg,pos,dest)   msg_header_getbyname(( sip_t *)sipmsg,"max-forwards",pos,(header_t **)dest)
#define msg_getmax_forwards(sipmsg,pos,dest)   msg_header_getbyname(( sip_t *)sipmsg,"max-forwards",pos,(header_t **)dest)
/**
 * Allocate and Add a new Priority header.
 * @param sipmsg The element to work on.
 * @param value the value of the new header.
 */
#define msg_setpriority(sipmsg,value)        msg_setheader((sip_t *)sipmsg,(char *)"priority",value)
/**
 * Find a Priority header.
 * @param sipmsg The element to work on.
 * @param pos The index of the header in the list of unknown header.
 * @param dest A pointer on the element found.
 */
#define msg_getpriority(sipmsg,pos,dest)      msg_header_getbyname(( sip_t *)sipmsg,"priority",pos,(header_t **)dest)
/**
 * Allocate and Add a new Proxy-Require header.
 * @param sipmsg The element to work on.
 * @param value the value of the new header.
 */
#define msg_setproxy_require(sipmsg,value)   msg_setheader((sip_t *)sipmsg,(char *)"proxy-require",value)
/**
 * Find a Proxy-Require header.
 * @param sipmsg The element to work on.
 * @param pos The index of the header in the list of unknown header.
 * @param dest A pointer on the element found.
 */
#define msg_getproxy_require(sipmsg,pos,dest) msg_header_getbyname(( sip_t *)sipmsg,"proxy-require",pos,(header_t **)dest)
/**
 * Allocate and Add a new Response-Key header.
 * @param sipmsg The element to work on.
 * @param value the value of the new header.
 */
#define msg_setresponse_key(sipmsg,value)    msg_setheader((sip_t *)sipmsg,(char *)"response-key",value)
/**
 * Find a Response-Key header.
 * @param sipmsg The element to work on.
 * @param pos The index of the header in the list of unknown header.
 * @param dest A pointer on the element found.
 */
#define msg_getresponse_key(sipmsg,pos,dest)  msg_header_getbyname(( sip_t *)sipmsg,"response-key",pos,(header_t **)dest)
/**
 * Allocate and Add a new Subject header.
 * @param sipmsg The element to work on.
 * @param value the value of the new header.
 */
#define msg_setsubject(sipmsg,value)         msg_setheader((sip_t *)sipmsg,(char *)"subject",value)
/**
 * Find a Subject header.
 * @param sipmsg The element to work on.
 * @param pos The index of the header in the list of unknown header.
 * @param dest A pointer on the element found.
 */
#define msg_getsubject(sipmsg,pos,dest)       msg_header_getbyname(( sip_t *)sipmsg,"subject",pos,(header_t **)dest)
/**
 * Allocate and Add a new Retry-After header.
 * @param sipmsg The element to work on.
 * @param value the value of the new header.
 */
#define msg_setretry_after(sipmsg,value)     msg_setheader((sip_t *)sipmsg,(char *)"retry-after",value)
/**
 * Find a Retry-After header.
 * @param sipmsg The element to work on.
 * @param pos The index of the header in the list of unknown header.
 * @param dest A pointer on the element found.
 */
#define msg_getretry_after(sipmsg,pos,dest)   msg_header_getbyname(( sip_t *)sipmsg,"retry-after",pos,(header_t **)dest)
/**
 * Allocate and Add a new Server header.
 * @param sipmsg The element to work on.
 * @param value the value of the new header.
 */
#define msg_setserver(sipmsg,value)          msg_setheader((sip_t *)sipmsg,(char *)"server",value)
/**
 * Find a Server header.
 * @param sipmsg The element to work on.
 * @param pos The index of the header in the list of unknown header.
 * @param dest A pointer on the element found.
 */
#define msg_getserver(sipmsg,pos,dest)        msg_header_getbyname(( sip_t *)sipmsg,"server",pos,(header_t **)dest)
/**
 * Allocate and Add a new Unsupported header.
 * @param sipmsg The element to work on.
 * @param value the value of the new header.
 */
#define msg_setunsupported(sipmsg,value)     msg_setheader((sip_t *)sipmsg,(char *)"unsupported",value)
/**
 * Find a Unsupported header.
 * @param sipmsg The element to work on.
 * @param pos The index of the header in the list of unknown header.
 * @param dest A pointer on the element found.
 */
#define msg_getunsupported(sipmsg,pos,dest)   msg_header_getbyname(( sip_t *)sipmsg,"unsupported",pos,(header_t **)dest)
/**
 * Allocate and Add a new Warning header.
 * @param sipmsg The element to work on.
 * @param value the value of the new header.
 */
#define msg_setwarning(sipmsg,value)         msg_setheader((sip_t *)sipmsg,(char *)"warning",value)
/**
 * Find a Warning header.
 * @param sipmsg The element to work on.
 * @param pos The index of the header in the list of unknown header.
 * @param dest A pointer on the element found.
 */
#define msg_getwarning(sipmsg,pos,dest)       msg_header_getbyname(( sip_t *)sipmsg,"warning",pos,(header_t **)dest)

/** @} */


#ifdef __cplusplus
}
#endif

#endif
