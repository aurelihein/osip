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


#ifndef _SDP_NEGOC_H_
#define _SDP_NEGOC_H_

#include <osip/sdp.h>


/**
 * @file sdp_negoc.h
 * @brief oSIP and SDP offer/answer model Routines
 */

/**
 * @defgroup oSIP_OAM oSIP and SDP offer/answer model Handling
 * @ingroup oSIP
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Structure for applying the SDP offer/answer negotiation.
 * The goal is simply to give:
 *      1.  A configuration (sdp_config_t)
 *      2.  A remote SDP packet (generally from the INVITE)
 * The result is the creation of a local answer to
 * the remote SDP packet.
 * @defvar sdp_context_t
 */
typedef struct sdp_context_t sdp_context_t;

struct sdp_context_t {
  void *mycontext; /* add the pointer to your personal context */
  sdp_t *remote;
  sdp_t *local;
};

/**
 * Allocate a bandwidth element.
 * @param ctx The element to work on.
 */
int sdp_context_init(sdp_context_t **ctx);
/**
 * Free a bandwidth element.
 * @param ctx The element to work on.
 */
void sdp_context_free(sdp_context_t *ctx);

/**
 * Set the context associated to this negotiation.
 * @param ctx The element to work on.
 * @param value A pointer to your personal context.
 */
int sdp_context_set_mycontext(sdp_context_t *ctx, void *value);
/**
 * Get the context associated to this negotiation.
 * @param ctx The element to work on.
 */
void *sdp_context_get_mycontext(sdp_context_t *ctx);

/**
 * Set the local SDP packet associated to this negotiation.
 * NOTE: This is done by the 'negotiator'. (You only need to give
 * the remote SDP packet)
 * @param ctx The element to work on.
 * @param sdp The local SDP packet.
 */
int sdp_context_set_local_sdp(sdp_context_t *ctx, sdp_t *sdp);
/**
 * Get the local SDP packet associated to this negotiation.
 * @param ctx The element to work on.
 */
sdp_t *sdp_context_get_local_sdp(sdp_context_t *ctx);
/**
 * Set the remote SDP packet associated to this negotiation.
 * @param ctx The element to work on.
 * @param sdp The remote SDP packet.
 */
int sdp_context_set_remote_sdp(sdp_context_t *ctx, sdp_t *sdp);
/**
 * Get the remote SDP packet associated to this negotiation.
 * @param ctx The element to work on.
 */
sdp_t *sdp_context_get_remote_sdp(sdp_context_t *ctx);

/**
 * Structure for payload management. Each payload element
 * represents one codec of a media line.
 * @defvar payload_t
 */
typedef struct payload_t payload_t;

struct payload_t {
  char *payload;
  /*  char *port; this must be assigned by the application dynamicly */
  char *number_of_port;
  char *proto;
  char *c_nettype;
  char *c_addrtype;
  char *c_addr;
  char *c_addr_multicast_ttl;
  char *c_addr_multicast_int;
  /* rtpmap (rcvonly and other attributes are added dynamicly) */
  char *a_rtpmap;
};


/**
 * Allocate a payload element.
 * @param payload The payload.
 */
int  payload_init(payload_t **payload);
/**
 * Free a payload element.
 * @param payload The payload.
 */
void payload_free(payload_t *payload);

/**
 * Structure for storing the global configuration management.
 * The information you store here is used when computing a
 * remote SDP packet to build a compliant answer.
 * The main objectives is to:
 *    * automaticly refuse unknown media.
 *    * accept some of the known media.
 *    * make sure the SDP answer match the SDP offer.
 *    * simplify the SDP offer/answer model, as all unknown media
 *      are refused without any indication to the application layer.
 *    * In any case, you can still modify the entire SDP packet after
 *      a negotiation if you are not satisfied by the negotiation result.
 * @defvar sdp_config_t
 */
typedef struct sdp_config_t sdp_config_t;

struct sdp_config_t {

  char *o_username;
  char *o_session_id;
  char *o_session_version;
  char *o_nettype;
  char *o_addrtype;
  char *o_addr;

  char *c_nettype;
  char *c_addrtype;
  char *c_addr;
  char *c_addr_multicast_ttl;
  char *c_addr_multicast_int;

  list_t *audio_codec;
  list_t *video_codec;
  list_t *other_codec;

  int  (*fcn_set_info)(void*, sdp_t *);
  int  (*fcn_set_uri)(void*, sdp_t *);

  int  (*fcn_set_emails)(void*, sdp_t *);
  int  (*fcn_set_phones)(void*, sdp_t *);
  int  (*fcn_set_attributes)(void*, sdp_t *, int);
  int  (*fcn_accept_audio_codec)(void*, 
				 char *, char *,
				 int , char *);
  int  (*fcn_accept_video_codec)(void*, 
				 char *, char *,
				 int , char *);
  int  (*fcn_accept_other_codec)(void*, 
				 char *, char *,
				 char *, char *);
  char *(*fcn_get_audio_port)(void *, int);
  char *(*fcn_get_video_port)(void *, int);
  char *(*fcn_get_other_port)(void *, int);

};


/**
 * Initialise (and Allocate) a sdp_config element (this element is global).
 * This method must be called when the application is started.
 */
int sdp_config_init();
/**
 * Free resource stored by a sdp_config element.
 * This method must be called once when the application is stopped.
 */
void sdp_config_free();

/**
 * Set the local username ('o' field) of all local SDP packet.
 * @param tmp The username.
 */
int sdp_config_set_o_username(char *tmp);
/**
 * Set the local session id ('o' field) of all local SDP packet.
 * WARNING: this field should be updated for each new SDP packet?
 * @param tmp The session id.
 */
int sdp_config_set_o_session_id(char *tmp);
/**
 * Set the local session version ('o' field) of all local SDP packet.
 * WARNING: this field should be updated for each new SDP packet?
 * @param tmp The session version.
 */
int sdp_config_set_o_session_version(char *tmp);
/**
 * Set the local network type ('o' field) of all local SDP packet.
 * @param tmp The network type.
 */
int sdp_config_set_o_nettype(char *tmp);
/**
 * Set the local address type ('o' field) of all local SDP packet.
 * @param tmp The address type.
 */
int sdp_config_set_o_addrtype(char *tmp);
/**
 * Set the local IP address ('o' field) of all local SDP packet.
 * @param tmp The IP address.
 */
int sdp_config_set_o_addr(char *tmp);

/**
 * Set the local network type ('c' field) of all local SDP packet.
 * @param tmp The network type.
 */
int sdp_config_set_c_nettype(char *tmp);
/**
 * Set the local address type ('c' field) of all local SDP packet.
 * @param tmp The address type.
 */
int sdp_config_set_c_addrtype(char *tmp);
/**
 * Set the local IP address ('c' field) of all local SDP packet.
 * @param tmp The IP address.
 */
int sdp_config_set_c_addr(char *tmp);
/**
 * Set the local ttl for multicast address ('c' field) of all local SDP packet.
 * @param tmp The ttl for multicast address.
 */
int sdp_config_set_c_addr_multicast_ttl(char *tmp);
/**
 * Set the local int for multicast address ('c' field) of all local SDP packet.
 * @param tmp The int for multicast address.
 */
int sdp_config_set_c_addr_multicast_int(char *tmp);

/**
 * Add a supported audio codec.
 * Those codecs will be accepted as long as you return 0 when
 * the callback 'fcn_accept_audio_codec' is called with the specific payload.
 * @param payload The payload.
 * @param number_of_port The number of port (channel) for this codec.
 * @param proto The protocol.
 * @param c_nettype The network type in the 'c' field.
 * @param c_addrtype The address type in the 'c' field.
 * @param c_addr The address in the 'c' field.
 * @param c_addr_multicast_ttl The ttl for multicast address in the 'c' field.
 * @param c_addr_multicast_int The int for multicast address in the 'c' field.
 * @param a_rtpmap The rtpmap attribute in the 'a' field.
 */
int sdp_config_add_support_for_audio_codec(char *payload, char *number_of_port,
					   char *proto, char *c_nettype,
					   char *c_addrtype,char *c_addr,
					   char *c_addr_multicast_ttl,
					   char *c_addr_multicast_int,
					   char *a_rtpmap);
/**
 * Add a supported video codec.
 * Those codecs will be accepted as long as you return 0 when
 * the callback 'fcn_accept_video_codec' is called with the specific payload.
 * @param payload The payload.
 * @param number_of_port The number of port (channel) for this codec.
 * @param proto The protocol.
 * @param c_nettype The network type in the 'c' field.
 * @param c_addrtype The address type in the 'c' field.
 * @param c_addr The address in the 'c' field.
 * @param c_addr_multicast_ttl The ttl for multicast address in the 'c' field.
 * @param c_addr_multicast_int The int for multicast address in the 'c' field.
 * @param a_rtpmap The rtpmap attribute in the 'a' field.
 */
int sdp_config_add_support_for_video_codec(char *payload, char *number_of_port,
					   char *proto, char *c_nettype,
					   char *c_addrtype,char *c_addr,
					   char *c_addr_multicast_ttl,
					   char *c_addr_multicast_int,
					   char *a_rtpmap);
/**
 * Add a supported (non-audio and non-video) codec.
 * Those codecs will be accepted as long as you return 0 when
 * the callback 'fcn_accept_other_codec' is called with the specific payload.
 * @param payload The payload.
 * @param number_of_port The number of port (channel) for this codec.
 * @param proto The protocol.
 * @param c_nettype The network type in the 'c' field.
 * @param c_addrtype The address type in the 'c' field.
 * @param c_addr The address in the 'c' field.
 * @param c_addr_multicast_ttl The ttl for multicast address in the 'c' field.
 * @param c_addr_multicast_int The int for multicast address in the 'c' field.
 * @param a_rtpmap The rtpmap attribute in the 'a' field.
 */
int sdp_config_add_support_for_other_codec(char *payload, char *number_of_port,
					   char *proto, char *c_nettype,
					   char *c_addrtype,char *c_addr,
					   char *c_addr_multicast_ttl,
					   char *c_addr_multicast_int,
					   char *a_rtpmap);

#ifndef DOXYGEN
/**
 * Free resource in the global sdp_config..
 */
int sdp_config_remove_audio_payloads();
/**
 * Free resource in the global sdp_config..
 */
int sdp_config_remove_video_payloads();
/**
 * Free resource in the global sdp_config..
 */
int sdp_config_remove_other_payloads();
#endif

/**
 * Set the callback for setting info ('i' field) in a local SDP packet.
 * This callback is called once each time we need an 'i' field.
 * @param fcn The callback.
 */
int sdp_config_set_fcn_set_info(int  (*fcn)(sdp_context_t *, sdp_t *));
/**
 * Set the callback for setting a URI ('u' field) in a local SDP packet.
 * This callback is called once each time we need an 'u' field.
 * @param fcn The callback.
 */
int sdp_config_set_fcn_set_uri(int  (*fcn)(sdp_context_t *, sdp_t *));
/**
 * Set the callback for setting an email ('e' field) in a local SDP packet.
 * This callback is called once each time we need an 'e' field.
 * @param fcn The callback.
 */
int sdp_config_set_fcn_set_emails(int  (*fcn)(sdp_context_t *, sdp_t *));
/**
 * Set the callback for setting a phone ('p' field) in a local SDP packet.
 * This callback is called once each time we need an 'p' field.
 * @param fcn The callback.
 */
int sdp_config_set_fcn_set_phones(int  (*fcn)(sdp_context_t *, sdp_t *));
/**
 * Set the callback for setting an attribute ('a' field) in a local SDP packet.
 * This callback is called once each time we need an 'a' field.
 * @param fcn The callback.
 */
int sdp_config_set_fcn_set_attributes(int  (*fcn)(sdp_context_t *, sdp_t *, int));
/**
 * Set the callback used to accept a codec during a negotiation.
 * This callback is called once each time we need to accept a codec.
 * @param fcn The callback.
 */
int sdp_config_set_fcn_accept_audio_codec(int  (*fcn)(sdp_context_t *, char *,
						      char *, int, char *));
/**
 * Set the callback used to accept a codec during a negotiation.
 * This callback is called once each time we need to accept a codec.
 * @param fcn The callback.
 */
int sdp_config_set_fcn_accept_video_codec(int  (*fcn)(sdp_context_t *, char *,
						      char *, int, char *));
/**
 * Set the callback used to accept a codec during a negotiation.
 * This callback is called once each time we need to accept a codec.
 * @param fcn The callback.
 */
int sdp_config_set_fcn_accept_other_codec(int  (*fcn)(sdp_context_t *, char *,
						      char *, char *, char *));
/**
 * Set the callback for setting the port number ('m' field) in a local SDP packet.
 * This callback is called once each time a 'm' line is accepted.
 * @param fcn The callback.
 */
int sdp_config_set_fcn_get_audio_port(char*  (*fcn)(sdp_context_t *, int));
/**
 * Set the callback for setting the port number ('m' field) in a local SDP packet.
 * This callback is called once each time a 'm' line is accepted.
 * @param fcn The callback.
 */
int sdp_config_set_fcn_get_video_port(char*  (*fcn)(sdp_context_t *, int));
/**
 * Set the callback for setting the port number ('m' field) in a local SDP packet.
 * This callback is called once each time a 'm' line is accepted.
 * @param fcn The callback.
 */
int sdp_config_set_fcn_get_other_port(char*  (*fcn)(sdp_context_t *, int));

/**
 * Start the automatic negotiation for a UA
 * NOTE: You can previously set context->mycontext to point to your
 * personal context. This way you'll get access to your personal context
 * in the callback and you can easily take the correct decisions.
 * After this method is called, the negotiation will happen and
 * callbacks will be called. You can modify, add, remove SDP fields,
 * and accept and refuse the codec from your preferred list by using
 * those callbacks.
 * Of course, after the negotiation happen, you can modify the
 * SDP packet if you wish to improve it or just refine some attributes.
 * @param ctx The context holding the remote SDP offer.
 */
int sdp_context_execute_negotiation(sdp_context_t *ctx);

#ifndef DOXYGEN
/* deprecated for spelling reasons... */
#define sdp_context_execute_negociation sdp_context_execute_negotiation
#endif

/** @} */


#ifdef __cplusplus
}
#endif

#endif /*_SDP_NEGOC_H_ */
