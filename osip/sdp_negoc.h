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

/* SDP and SIP automatic negociation services */

typedef struct _sdp_context_t {
  void *mycontext; /* add the pointer to your personnal context */
  sdp_t *remote;
  sdp_t *local;
} sdp_context_t;

int sdp_context_init(sdp_context_t **con);
void sdp_context_free(sdp_context_t *con);

/* this method is used by end-user application so any pointer can
   be associated with this context (usefull to link with your own context */
int sdp_context_set_mycontext(sdp_context_t *con, void *my_instance);
void *sdp_context_get_mycontext(sdp_context_t *con);

sdp_t *sdp_context_get_local_sdp(sdp_context_t *con);
int sdp_context_set_local_sdp(sdp_context_t *con, sdp_t *sdp);
sdp_t *sdp_context_get_remote_sdp(sdp_context_t *con);
int sdp_context_set_remote_sdp(sdp_context_t *con, sdp_t *sdp);

typedef struct _payload_t {
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
} payload_t;

int  payload_init(payload_t **payload);
void payload_free(payload_t *payload);

typedef struct _sdp_config_t {

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

} sdp_config_t;


int sdp_config_init();
void sdp_config_free();

int sdp_config_set_o_username(char *tmp);
int sdp_config_set_o_session_id(char *tmp);
int sdp_config_set_o_session_version(char *tmp);
int sdp_config_set_o_nettype(char *tmp);
int sdp_config_set_o_addrtype(char *tmp);
int sdp_config_set_o_addr(char *tmp);

int sdp_config_set_c_nettype(char *tmp);
int sdp_config_set_c_addrtype(char *tmp);
int sdp_config_set_c_addr(char *tmp);
int sdp_config_set_c_addr_multicast_ttl(char *tmp);
int sdp_config_set_c_addr_multicast_int(char *tmp);

int sdp_config_add_support_for_audio_codec(char *payload, char *number_of_port,
					   char *proto, char *c_nettype,
					   char *c_addrtype,char *c_addr,
					   char *c_addr_multicast_ttl,
					   char *c_addr_multicast_int,
					   char *a_rtpmap);
int sdp_config_add_support_for_video_codec(char *payload, char *number_of_port,
					   char *proto, char *c_nettype,
					   char *c_addrtype,char *c_addr,
					   char *c_addr_multicast_ttl,
					   char *c_addr_multicast_int,
					   char *a_rtpmap);
int sdp_config_add_support_for_other_codec(char *payload, char *number_of_port,
					   char *proto, char *c_nettype,
					   char *c_addrtype,char *c_addr,
					   char *c_addr_multicast_ttl,
					   char *c_addr_multicast_int,
					   char *a_rtpmap);

int sdp_config_remove_audio_payloads();
int sdp_config_remove_video_payloads();
int sdp_config_remove_other_payloads();

int sdp_config_set_fcn_set_info(int  (*fcn)(sdp_context_t *, sdp_t *));
int sdp_config_set_fcn_set_uri(int  (*fcn)(sdp_context_t *, sdp_t *));
int sdp_config_set_fcn_set_emails(int  (*fcn)(sdp_context_t *, sdp_t *));
int sdp_config_set_fcn_set_phones(int  (*fcn)(sdp_context_t *, sdp_t *));
int sdp_config_set_fcn_set_attributes(int  (*fcn)(sdp_context_t *, sdp_t *, int));
int sdp_config_set_fcn_accept_audio_codec(int  (*fcn)(sdp_context_t *, char *,
						      char *, int, char *));
int sdp_config_set_fcn_accept_video_codec(int  (*fcn)(sdp_context_t *, char *,
						      char *, int, char *));
int sdp_config_set_fcn_accept_other_codec(int  (*fcn)(sdp_context_t *, char *,
						      char *, char *, char *));
int sdp_config_set_fcn_get_audio_port(char*  (*fcn)(sdp_context_t *, int));
int sdp_config_set_fcn_get_video_port(char*  (*fcn)(sdp_context_t *, int));
int sdp_config_set_fcn_get_other_port(char*  (*fcn)(sdp_context_t *, int));

/* automatic negociation for UA
   sdp_context_t *context MUST be previously initialized */
int sdp_context_execute_negociation(sdp_context_t *context);


#endif /*_SDP_NEGOC_H_ */
