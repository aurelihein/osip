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


#include <osip/sdp_negoc.h>
#include <osip/port.h>

/* this MUST be initialized through a call to sdp_config_init() */
sdp_config_t *config = NULL;

int
sdp_context_init (sdp_context_t ** con)
{
  (*con) = (sdp_context_t *) smalloc (sizeof (sdp_context_t));
  if (*con == NULL)
    return -1;
  (*con)->remote = NULL;
  (*con)->local = NULL;
  return 0;
}

void
sdp_context_free (sdp_context_t * con)
{
  if (con == NULL)
    return;
  sdp_free (con->remote);
  sfree (con->remote);
  sdp_free (con->local);
  sfree (con->local);
}

/* this method is used by end-user application so any pointer can
   be associated with this context (usefull to link with your own context */
int
sdp_context_set_mycontext (sdp_context_t * con, void *my_instance)
{
  if (con == NULL)
    return -1;
  con->mycontext = my_instance;
  return 0;
}

void *
sdp_context_get_mycontext (sdp_context_t * con)
{
  if (con == NULL)
    return NULL;
  return con->mycontext;
}

sdp_t *
sdp_context_get_local_sdp (sdp_context_t * con)
{
  if (con == NULL)
    return NULL;
  return con->local;
}

int
sdp_context_set_local_sdp (sdp_context_t * con, sdp_t * sdp)
{
  if (con == NULL)
    return -1;
  con->local = sdp;
  return 0;
}

sdp_t *
sdp_context_get_remote_sdp (sdp_context_t * con)
{
  if (con == NULL)
    return NULL;
  return con->remote;
}

int
sdp_context_set_remote_sdp (sdp_context_t * con, sdp_t * sdp)
{
  if (con == NULL)
    return -1;
  con->remote = sdp;
  return 0;
}

int
payload_init (payload_t ** payload)
{
  *payload = (payload_t *) smalloc (sizeof (payload_t));
  if (*payload == NULL)
    return -1;
  (*payload)->payload = NULL;
  (*payload)->number_of_port = NULL;
  (*payload)->proto = NULL;
  (*payload)->c_nettype = NULL;
  (*payload)->c_addrtype = NULL;
  (*payload)->c_addr = NULL;
  (*payload)->c_addr_multicast_ttl = NULL;
  (*payload)->c_addr_multicast_int = NULL;
  (*payload)->a_rtpmap = NULL;
  return 0;
}

void
payload_free (payload_t * payload)
{
  if (payload == NULL)
    return;
  sfree (payload->payload);
  sfree (payload->number_of_port);
  sfree (payload->proto);
  sfree (payload->c_nettype);
  sfree (payload->c_addrtype);
  sfree (payload->c_addr);
  sfree (payload->c_addr_multicast_ttl);
  sfree (payload->c_addr_multicast_int);
  sfree (payload->a_rtpmap);
}

int
sdp_config_init ()
{
  config = (sdp_config_t *) smalloc (sizeof (sdp_config_t));
  if (config == NULL)
    return -1;
  config->o_username = NULL;
  config->o_session_id = NULL;
  config->o_session_version = NULL;
  config->o_nettype = NULL;
  config->o_addrtype = NULL;
  config->o_addr = NULL;

  config->c_nettype = NULL;
  config->c_addrtype = NULL;
  config->c_addr = NULL;
  config->c_addr_multicast_ttl = NULL;
  config->c_addr_multicast_int = NULL;

  /* supported codec for the SIP User Agent */
  config->audio_codec = (list_t *) smalloc (sizeof (list_t));
  list_init (config->audio_codec);
  config->video_codec = (list_t *) smalloc (sizeof (list_t));
  list_init (config->video_codec);
  config->other_codec = (list_t *) smalloc (sizeof (list_t));
  list_init (config->other_codec);

  config->fcn_set_info = NULL;
  config->fcn_set_uri = NULL;
  config->fcn_set_emails = NULL;
  config->fcn_set_phones = NULL;
  config->fcn_set_attributes = NULL;
  config->fcn_accept_audio_codec = NULL;
  config->fcn_accept_video_codec = NULL;
  config->fcn_accept_other_codec = NULL;
  return 0;
}

void
sdp_config_free ()
{
  if (config == NULL)
    return;
  sfree (config->o_username);
  sfree (config->o_session_id);
  sfree (config->o_session_version);
  sfree (config->o_nettype);
  sfree (config->o_addrtype);
  sfree (config->o_addr);

  sfree (config->c_nettype);
  sfree (config->c_addrtype);
  sfree (config->c_addr);
  sfree (config->c_addr_multicast_ttl);
  sfree (config->c_addr_multicast_int);

  list_special_free (config->audio_codec, (void *(*)(void *)) &payload_free);
  sfree (config->audio_codec);
  list_special_free (config->video_codec, (void *(*)(void *)) &payload_free);
  sfree (config->video_codec);
  list_special_free (config->other_codec, (void *(*)(void *)) &payload_free);
  sfree (config->other_codec);

  /* other are pointer to func, they don't need free() calls */

  /* yes, this is done here... :) */
  sfree (config);
}

int
sdp_config_set_o_username (char *tmp)
{
  if (config == NULL)
    return -1;
  config->o_username = tmp;
  return 0;
}

int
sdp_config_set_o_session_id (char *tmp)
{
  if (config == NULL)
    return -1;
  config->o_session_id = tmp;
  return 0;
}

int
sdp_config_set_o_session_version (char *tmp)
{
  if (config == NULL)
    return -1;
  config->o_session_version = tmp;
  return 0;
}

int
sdp_config_set_o_nettype (char *tmp)
{
  if (config == NULL)
    return -1;
  config->o_nettype = tmp;
  return 0;
}

int
sdp_config_set_o_addrtype (char *tmp)
{
  if (config == NULL)
    return -1;
  config->o_addrtype = tmp;
  return 0;
}

int
sdp_config_set_o_addr (char *tmp)
{
  if (config == NULL)
    return -1;
  config->o_addr = tmp;
  return 0;
}

int
sdp_config_set_c_nettype (char *tmp)
{
  if (config == NULL)
    return -1;
  config->c_nettype = tmp;
  return 0;
}

int
sdp_config_set_c_addrtype (char *tmp)
{
  if (config == NULL)
    return -1;
  config->c_addrtype = tmp;
  return 0;
}

int
sdp_config_set_c_addr (char *tmp)
{
  if (config == NULL)
    return -1;
  config->c_addr = tmp;
  return 0;
}

int
sdp_config_set_c_addr_multicast_ttl (char *tmp)
{
  if (config == NULL)
    return -1;
  config->c_addr_multicast_ttl = tmp;
  return 0;
}

int
sdp_config_set_c_addr_multicast_int (char *tmp)
{
  if (config == NULL)
    return -1;
  config->c_addr_multicast_int = tmp;
  return 0;
}

int
sdp_config_add_support_for_audio_codec (char *payload, char *number_of_port,
					char *proto, char *c_nettype,
					char *c_addrtype, char *c_addr,
					char *c_addr_multicast_ttl,
					char *c_addr_multicast_int,
					char *a_rtpmap)
{
  int i;
  payload_t *my_payload;

  i = payload_init (&my_payload);
  if (i != 0)
    return -1;
  my_payload->payload = payload;
  my_payload->number_of_port = number_of_port;
  my_payload->proto = proto;
  my_payload->c_nettype = c_nettype;
  my_payload->c_addrtype = c_addrtype;
  my_payload->c_addr = c_addr;
  my_payload->c_addr_multicast_ttl = c_addr_multicast_ttl;
  my_payload->c_addr_multicast_int = c_addr_multicast_int;
  my_payload->a_rtpmap = a_rtpmap;
  list_add (config->audio_codec, my_payload, -1);
  return 0;
}

int
sdp_config_add_support_for_video_codec (char *payload, char *number_of_port,
					char *proto, char *c_nettype,
					char *c_addrtype, char *c_addr,
					char *c_addr_multicast_ttl,
					char *c_addr_multicast_int,
					char *a_rtpmap)
{
  int i;
  payload_t *my_payload;

  i = payload_init (&my_payload);
  if (i != 0)
    return -1;
  my_payload->payload = payload;
  my_payload->number_of_port = number_of_port;
  my_payload->proto = proto;
  my_payload->c_nettype = c_nettype;
  my_payload->c_addrtype = c_addrtype;
  my_payload->c_addr = c_addr;
  my_payload->c_addr_multicast_ttl = c_addr_multicast_ttl;
  my_payload->c_addr_multicast_int = c_addr_multicast_int;
  my_payload->a_rtpmap = a_rtpmap;
  list_add (config->video_codec, my_payload, -1);
  return 0;
}

int
sdp_config_add_support_for_other_codec (char *payload, char *number_of_port,
					char *proto, char *c_nettype,
					char *c_addrtype, char *c_addr,
					char *c_addr_multicast_ttl,
					char *c_addr_multicast_int,
					char *a_rtpmap)
{
  int i;
  payload_t *my_payload;

  i = payload_init (&my_payload);
  if (i != 0)
    return -1;
  my_payload->payload = payload;
  my_payload->number_of_port = number_of_port;
  my_payload->proto = proto;
  my_payload->c_nettype = c_nettype;
  my_payload->c_addrtype = c_addrtype;
  my_payload->c_addr = c_addr;
  my_payload->c_addr_multicast_ttl = c_addr_multicast_ttl;
  my_payload->c_addr_multicast_int = c_addr_multicast_int;
  my_payload->a_rtpmap = a_rtpmap;
  list_add (config->other_codec, my_payload, -1);
  return 0;
}

int
sdp_config_remove_audio_payloads ()
{
  list_special_free (config->audio_codec, (void *(*)(void *)) &payload_free);
  return 0;
}

int
sdp_config_remove_video_payloads ()
{
  list_special_free (config->video_codec, (void *(*)(void *)) &payload_free);
  return 0;
}

int
sdp_config_remove_other_payloads ()
{
  list_special_free (config->other_codec, (void *(*)(void *)) &payload_free);
  return 0;
}

payload_t *
sdp_config_find_audio_payload (char *payload)
{
  payload_t *my;
  size_t length = strlen (payload);
  int pos = 0;

  if (payload == NULL)
    return NULL;
  while (!list_eol (config->audio_codec, pos))
    {
      my = (payload_t *) list_get (config->audio_codec, pos);
      if (strlen (my->payload) == length)
	if (0 == strncmp (my->payload, payload, length))
	  return my;
      pos++;
    }
  return NULL;
}

payload_t *
sdp_config_find_video_payload (char *payload)
{
  payload_t *my;
  size_t length = strlen (payload);
  int pos = 0;

  if (payload == NULL)
    return NULL;
  while (!list_eol (config->video_codec, pos))
    {
      my = (payload_t *) list_get (config->video_codec, pos);
      if (strlen (my->payload) == length)
	if (0 == strncmp (my->payload, payload, length))
	  return my;
      pos++;
    }
  return NULL;
}

payload_t *
sdp_config_find_other_payload (char *payload)
{
  payload_t *my;
  size_t length = strlen (payload);
  int pos = 0;

  if (payload == NULL)
    return NULL;
  while (!list_eol (config->other_codec, pos))
    {
      my = (payload_t *) list_get (config->other_codec, pos);
      if (strlen (my->payload) == length)
	if (0 == strncmp (my->payload, payload, length))
	  return my;
      pos++;
    }
  return NULL;
}

int
sdp_config_set_fcn_set_info (int (*fcn) (sdp_context_t *, sdp_t *))
{
  if (config == NULL)
    return -1;
  config->fcn_set_info = (int (*)(void *, sdp_t *)) fcn;
  return 0;
}

int
sdp_config_set_fcn_set_uri (int (*fcn) (sdp_context_t *, sdp_t *))
{
  if (config == NULL)
    return -1;
  config->fcn_set_uri = (int (*)(void *, sdp_t *)) fcn;
  return 0;
}

int
sdp_config_set_fcn_set_emails (int (*fcn) (sdp_context_t *, sdp_t *))
{
  if (config == NULL)
    return -1;
  config->fcn_set_emails = (int (*)(void *, sdp_t *)) fcn;
  return 0;
}

int
sdp_config_set_fcn_set_phones (int (*fcn) (sdp_context_t *, sdp_t *))
{
  if (config == NULL)
    return -1;
  config->fcn_set_phones = (int (*)(void *, sdp_t *)) fcn;
  return 0;
}

int
sdp_config_set_fcn_set_attributes (int (*fcn) (sdp_context_t *, sdp_t *, int))
{
  if (config == NULL)
    return -1;
  config->fcn_set_attributes = (int (*)(void *, sdp_t *, int)) fcn;
  return 0;
}

int
sdp_config_set_fcn_accept_audio_codec (int (*fcn) (sdp_context_t *, char *,
						   char *, int, char *))
{
  if (config == NULL)
    return -1;
  config->fcn_accept_audio_codec = (int (*)(void *, char *,
					    char *, int, char *)) fcn;
  return 0;
}

int
sdp_config_set_fcn_accept_video_codec (int (*fcn) (sdp_context_t *, char *,
						   char *, int, char *))
{
  if (config == NULL)
    return -1;
  config->fcn_accept_video_codec = (int (*)(void *, char *,
					    char *, int, char *)) fcn;
  return 0;
}

int
sdp_config_set_fcn_accept_other_codec (int (*fcn) (sdp_context_t *, char *,
						   char *, char *, char *))
{
  if (config == NULL)
    return -1;
  config->fcn_accept_other_codec = (int (*)(void *, char *,
					    char *, char *, char *)) fcn;
  return 0;
}

int
sdp_config_set_fcn_get_audio_port (char *(*fcn) (sdp_context_t *, int))
{
  if (config == NULL)
    return -1;
  config->fcn_get_audio_port = (char *(*)(void *, int)) fcn;
  return 0;
}

int
sdp_config_set_fcn_get_video_port (char *(*fcn) (sdp_context_t *, int))
{
  if (config == NULL)
    return -1;
  config->fcn_get_video_port = (char *(*)(void *, int)) fcn;
  return 0;
}

int
sdp_config_set_fcn_get_other_port (char *(*fcn) (sdp_context_t *, int))
{
  if (config == NULL)
    return -1;
  config->fcn_get_other_port = (char *(*)(void *, int)) fcn;
  return 0;
}

int
sdp_partial_clone (sdp_context_t * con, sdp_t * remote, sdp_t ** dest)
{
  int i;

  sdp_v_version_set (*dest, sgetcopy ("0"));

  /* those fields MUST be set */
  sdp_o_origin_set (*dest,
		    sgetcopy (config->o_username),
		    sgetcopy (config->o_session_id),
		    sgetcopy (config->o_session_version),
		    sgetcopy (config->o_nettype),
		    sgetcopy (config->o_addrtype), sgetcopy (config->o_addr));
  sdp_s_name_set (*dest, sgetcopy (remote->s_name));
  if (config->fcn_set_info != NULL)
    config->fcn_set_info (con, *dest);
  if (config->fcn_set_uri != NULL)
    config->fcn_set_uri (con, *dest);
  if (config->fcn_set_emails != NULL)
    config->fcn_set_emails (con, *dest);
  if (config->fcn_set_phones != NULL)
    config->fcn_set_phones (con, *dest);
  if (config->c_nettype != NULL)
    sdp_c_connection_add (*dest, -1,
			  sgetcopy (config->c_nettype),
			  sgetcopy (config->c_addrtype),
			  sgetcopy (config->c_addr),
			  sgetcopy (config->c_addr_multicast_ttl),
			  sgetcopy (config->c_addr_multicast_int));

  {				/* offer-answer draft says we must copy the "t=" line */
    char *tmp = sdp_t_start_time_get (remote, 0);
    char *tmp2 = sdp_t_stop_time_get (remote, 0);

    if (tmp == NULL || tmp2 == NULL)
      return -1;		/* no t line?? */
    i = sdp_t_time_descr_add (*dest, sgetcopy (tmp), sgetcopy (tmp2));
    if (i != 0)
      return -1;
  }
  if (config->fcn_set_attributes != NULL)
    config->fcn_set_attributes (con, *dest, -1);
  return 0;
}

int
sdp_confirm_media (sdp_context_t * context, sdp_t * remote, sdp_t ** dest)
{
  char *payload;
  char *tmp, *tmp2, *tmp3, *tmp4;
  int ret;
  int i;
  int k;
  int audio_qty = 0;		/* accepted audio line: do not accept more than one */
  int video_qty = 0;

  i = 0;
  while (!sdp_endof_media (remote, i))
    {
      tmp = sdp_m_media_get (remote, i);
      tmp2 = sdp_m_port_get (remote, i);
      tmp3 = sdp_m_number_of_port_get (remote, i);
      tmp4 = sdp_m_proto_get (remote, i);

      if (tmp == NULL)
	return -1;
      sdp_m_media_add (*dest, sgetcopy (tmp), sgetcopy ("0"),
		       NULL, sgetcopy (tmp4));
      k = 0;
      if (0 == strncmp (tmp, "audio", 5))
	{
	  do
	    {
	      payload = sdp_m_payload_get (remote, i, k);
	      if (payload != NULL)
		{
		  payload_t *my_payload =
		    sdp_config_find_audio_payload (payload);

		  if (my_payload != NULL)	/* payload is supported */
		    {
		      ret = -1;	/* somtimes, codec can be refused even if supported */
		      if (config->fcn_accept_audio_codec != NULL)
			ret = config->fcn_accept_audio_codec (context, tmp2,
							      tmp3, audio_qty,
							      payload);
		      if (0 == ret)
			{
			  sdp_m_payload_add (*dest, i, sgetcopy (payload));
			  if (my_payload->a_rtpmap != NULL)
			    sdp_a_attribute_add (*dest, i,
						 sgetcopy ("rtpmap"),
						 sgetcopy (my_payload->
							   a_rtpmap));
			  if (my_payload->c_nettype != NULL)
			    {
			      sdp_media_t *med =
				list_get ((*dest)->m_medias, i);

			      if (list_eol (med->c_connections, 0))
				sdp_c_connection_add (*dest, i,
						      sgetcopy (my_payload->
								c_nettype),
						      sgetcopy (my_payload->
								c_addrtype),
						      sgetcopy (my_payload->
								c_addr),
						      sgetcopy (my_payload->
								c_addr_multicast_ttl),
						      sgetcopy (my_payload->
								c_addr_multicast_int));
			    }
			}
		    }
		}
	      k++;
	    }
	  while (payload != NULL);
	  if (NULL != sdp_m_payload_get (*dest, i, 0))
	    audio_qty = 1;
	}
      else if (0 == strncmp (tmp, "video", 5))
	{
	  do
	    {
	      payload = sdp_m_payload_get (remote, i, k);
	      if (payload != NULL)
		{
		  payload_t *my_payload =
		    sdp_config_find_video_payload (payload);

		  if (my_payload != NULL)	/* payload is supported */
		    {
		      ret = -1;
		      if (config->fcn_accept_video_codec != NULL)
			ret =
			  config->fcn_accept_video_codec (context, tmp2, tmp3,
							  video_qty, payload);
		      if (0 == ret)
			{
			  sdp_m_payload_add (*dest, i, sgetcopy (payload));
			  /* TODO  set the attribute list (rtpmap..) */
			  if (my_payload->a_rtpmap != NULL)
			    sdp_a_attribute_add (*dest, i,
						 sgetcopy ("rtpmap"),
						 sgetcopy (my_payload->
							   a_rtpmap));
			  if (my_payload->c_nettype != NULL)
			    {
			      sdp_media_t *med =
				list_get ((*dest)->m_medias, i);

			      if (list_eol (med->c_connections, 0))
				sdp_c_connection_add (*dest, i,
						      sgetcopy (my_payload->
								c_nettype),
						      sgetcopy (my_payload->
								c_addrtype),
						      sgetcopy (my_payload->
								c_addr),
						      sgetcopy (my_payload->
								c_addr_multicast_ttl),
						      sgetcopy (my_payload->
								c_addr_multicast_int));
			    }
			}
		    }
		}
	      k++;
	    }
	  while (payload != NULL);
	  if (NULL != sdp_m_payload_get (*dest, i, 0))
	    video_qty = 1;
	}
      else
	{
	  do
	    {
	      payload = sdp_m_payload_get (remote, i, k);
	      if (payload != NULL)
		{
		  payload_t *my_payload =
		    sdp_config_find_other_payload (payload);

		  if (my_payload != NULL)	/* payload is supported */
		    {
		      ret = -1;
		      if (config->fcn_accept_other_codec != NULL)
			ret =
			  config->fcn_accept_other_codec (context, tmp, tmp2,
							  tmp3, payload);
		      if (0 == ret)
			{
			  sdp_m_payload_add (*dest, i, sgetcopy (payload));
			  /* rtpmap has no meaning here! */
			  if (my_payload->c_nettype != NULL)
			    {
			      sdp_media_t *med =
				list_get ((*dest)->m_medias, i);

			      if (list_eol (med->c_connections, 0))
				sdp_c_connection_add (*dest, i,
						      sgetcopy (my_payload->
								c_nettype),
						      sgetcopy (my_payload->
								c_addrtype),
						      sgetcopy (my_payload->
								c_addr),
						      sgetcopy (my_payload->
								c_addr_multicast_ttl),
						      sgetcopy (my_payload->
								c_addr_multicast_int));
			    }
			}
		    }
		}
	      k++;
	    }
	  while (payload != NULL);
	}
      i++;
    }
  return 0;
}

int
sdp_context_execute_negociation (sdp_context_t * context)
{
  int m_lines_that_match = 0;
  sdp_t *remote;
  sdp_t *local;
  int i;

  if (context == NULL)
    return -1;
  remote = context->remote;
  if (remote == NULL)
    return -1;

  i = sdp_init (&local);
  if (i != 0)
    return -1;

  if (0 != strncmp (remote->v_version, "0", 1))
    {
      sdp_free (local);
      sfree (local);
      /*      sdp_context->fcn_wrong_version(context); */
      return 406;		/* Not Acceptable */
    }

  i = sdp_partial_clone (context, remote, &local);
  if (i != 0)
    {
      sdp_free (local);
      sfree (local);
      return -1;
    }
  i = sdp_confirm_media (context, remote, &local);
  if (i != 0)
    {
      sdp_free (local);
      sfree (local);
      return i;
    }

  i = 0;
  while (!sdp_endof_media (local, i))
    {
      /* this is to refuse each line with no codec that matches! */
      if (NULL == sdp_m_payload_get (local, i, 0))
	{
	  sdp_media_t *med = list_get ((local)->m_medias, i);
	  char *str = sdp_m_payload_get (remote, i, 0);

	  sdp_m_payload_add (local, i, sgetcopy (str));
	  sfree (med->m_port);
	  med->m_port = sgetcopy ("0");	/* refuse this line */
	}
      else
	{			/* number of "m" lines that match */
	  sdp_media_t *med = list_get (local->m_medias, i);

	  m_lines_that_match++;
	  sfree (med->m_port);
	  /* AMD: use the correct fcn_get_xxx_port method: */
	  if (0 == strcmp (med->m_media, "audio"))
	    {
	      if (config->fcn_get_audio_port != NULL)
		med->m_port = config->fcn_get_audio_port (context, i);
	      else
		med->m_port = sgetcopy ("0");	/* should never happen */
	    }
	  else if (0 == strcmp (med->m_media, "video"))
	    {
	      if (config->fcn_get_video_port != NULL)
		med->m_port = config->fcn_get_video_port (context, i);
	      else
		med->m_port = sgetcopy ("0");	/* should never happen */
	    }
	  else
	    {
	      if (config->fcn_get_other_port != NULL)
		med->m_port = config->fcn_get_other_port (context, i);
	      else
		med->m_port = sgetcopy ("0");	/* should never happen */
	    }
	}
      i++;
    }
  if (m_lines_that_match > 0)
    {
      context->local = local;
      return 200;
    }
  else
    {
      sdp_free (local);
      sfree (local);
      return 415;
    }

}

int
sdp_build_offer (sdp_context_t * con, sdp_t ** sdp, char *audio_port,
		 char *video_port)
{
  int i;
  int media_line = 0;

  i = sdp_init (sdp);
  if (i != 0)
    return -1;

  sdp_v_version_set (*sdp, sgetcopy ("0"));

  /* those fields MUST be set */
  sdp_o_origin_set (*sdp,
		    sgetcopy (config->o_username),
		    sgetcopy (config->o_session_id),
		    sgetcopy (config->o_session_version),
		    sgetcopy (config->o_nettype),
		    sgetcopy (config->o_addrtype), sgetcopy (config->o_addr));
  sdp_s_name_set (*sdp, sgetcopy ("A call"));
  if (config->fcn_set_info != NULL)
    config->fcn_set_info (con, *sdp);
  if (config->fcn_set_uri != NULL)
    config->fcn_set_uri (con, *sdp);
  if (config->fcn_set_emails != NULL)
    config->fcn_set_emails (con, *sdp);
  if (config->fcn_set_phones != NULL)
    config->fcn_set_phones (con, *sdp);
  if (config->c_nettype != NULL)
    sdp_c_connection_add (*sdp, -1,
			  sgetcopy (config->c_nettype),
			  sgetcopy (config->c_addrtype),
			  sgetcopy (config->c_addr),
			  sgetcopy (config->c_addr_multicast_ttl),
			  sgetcopy (config->c_addr_multicast_int));

  {				/* offer-answer draft says we must copy the "t=" line */
    int now = time (NULL);
    char *tmp = smalloc (15);
    char *tmp2 = smalloc (15);

    sprintf (tmp, "%i", now);
    sprintf (tmp2, "%i", now + 3600);

    i = sdp_t_time_descr_add (*sdp, tmp, tmp2);
    if (i != 0)
      return -1;
  }
  if (config->fcn_set_attributes != NULL)
    config->fcn_set_attributes (con, *sdp, -1);


  /* add all audio codec */
  if (!list_eol (config->audio_codec, 0))
    {
      int pos = 0;
      payload_t *my = (payload_t *) list_get (config->audio_codec, pos);

      /* all media MUST have the same PROTO, PORT. */
      sdp_m_media_add (*sdp, sgetcopy ("audio"), sgetcopy (audio_port),
		       my->number_of_port, sgetcopy (my->proto));

      while (!list_eol (config->audio_codec, pos))
	{
	  my = (payload_t *) list_get (config->audio_codec, pos);
	  sdp_m_payload_add (*sdp, media_line, sgetcopy (my->payload));
	  if (my->a_rtpmap != NULL)
	    sdp_a_attribute_add (*sdp, media_line, sgetcopy ("rtpmap"),
				 sgetcopy (my->a_rtpmap));
	  pos++;
	}
      media_line++;
    }

  /* add all video codec */
  if (!list_eol (config->video_codec, 0))
    {
      int pos = 0;
      payload_t *my = (payload_t *) list_get (config->video_codec, pos);

      /* all media MUST have the same PROTO, PORT. */
      sdp_m_media_add (*sdp, sgetcopy ("video"), sgetcopy (video_port),
		       my->number_of_port, sgetcopy (my->proto));

      while (!list_eol (config->video_codec, pos))
	{
	  my = (payload_t *) list_get (config->video_codec, pos);
	  sdp_m_payload_add (*sdp, media_line, sgetcopy (my->payload));
	  if (my->a_rtpmap != NULL)
	    sdp_a_attribute_add (*sdp, media_line, sgetcopy ("rtpmap"),
				 sgetcopy (my->a_rtpmap));
	  pos++;
	}
      media_line++;
    }
  return 0;
}

/* build the SDP packet with only one audio codec and one video codec.
 * - Usefull if you don't want to restrict proposal to one codec only -
 * - Limitation, only one codec will be proposed
 */
int
__sdp_build_offer (sdp_context_t * con, sdp_t ** sdp, char *audio_port,
		   char *video_port, char *audio_codec, char *video_codec)
{
  int i;
  int media_line = 0;

  i = sdp_init (sdp);
  if (i != 0)
    return -1;

  sdp_v_version_set (*sdp, sgetcopy ("0"));

  /* those fields MUST be set */
  sdp_o_origin_set (*sdp,
		    sgetcopy (config->o_username),
		    sgetcopy (config->o_session_id),
		    sgetcopy (config->o_session_version),
		    sgetcopy (config->o_nettype),
		    sgetcopy (config->o_addrtype), sgetcopy (config->o_addr));
  sdp_s_name_set (*sdp, sgetcopy ("A call"));
  if (config->fcn_set_info != NULL)
    config->fcn_set_info (con, *sdp);
  if (config->fcn_set_uri != NULL)
    config->fcn_set_uri (con, *sdp);
  if (config->fcn_set_emails != NULL)
    config->fcn_set_emails (con, *sdp);
  if (config->fcn_set_phones != NULL)
    config->fcn_set_phones (con, *sdp);
  if (config->c_nettype != NULL)
    sdp_c_connection_add (*sdp, -1,
			  sgetcopy (config->c_nettype),
			  sgetcopy (config->c_addrtype),
			  sgetcopy (config->c_addr),
			  sgetcopy (config->c_addr_multicast_ttl),
			  sgetcopy (config->c_addr_multicast_int));

  {				/* offer-answer draft says we must copy the "t=" line */
    int now = time (NULL);
    char *tmp = smalloc (15);
    char *tmp2 = smalloc (15);

    sprintf (tmp, "%i", now);
    sprintf (tmp2, "%i", now + 3600);

    i = sdp_t_time_descr_add (*sdp, tmp, tmp2);
    if (i != 0)
      return -1;
  }
  if (config->fcn_set_attributes != NULL)
    config->fcn_set_attributes (con, *sdp, -1);


  /* add all audio codec */
  if (audio_codec!=NULL)
    {
      if (!list_eol (config->audio_codec, 0))
	{
	  int pos = 0;
	  payload_t *my = (payload_t *) list_get (config->audio_codec, pos);
	      
	  while (!list_eol (config->audio_codec, pos))
	    {
	      my = (payload_t *) list_get (config->audio_codec, pos);
	      if (0==strcmp(audio_codec, my->payload))
		{	  
		  /* all media MUST have the same PROTO, PORT. */
		  sdp_m_media_add (*sdp, sgetcopy ("audio"), sgetcopy (audio_port),
				   my->number_of_port, sgetcopy (my->proto));
		  sdp_m_payload_add (*sdp, media_line, sgetcopy (my->payload));
		  if (my->a_rtpmap != NULL)
		    sdp_a_attribute_add (*sdp, media_line, sgetcopy ("rtpmap"),
					 sgetcopy (my->a_rtpmap));
		  media_line++;
		  break;
		}
	      pos++;
	    }
	}
    }

  /* add all video codec */
  if (video_codec!=NULL)
    {
      if (!list_eol (config->video_codec, 0))
	{
	  int pos = 0;
	  payload_t *my = (payload_t *) list_get (config->video_codec, pos);
	      
	  while (!list_eol (config->video_codec, pos))
	    {
	      my = (payload_t *) list_get (config->video_codec, pos);
	      if (0==strcmp(video_codec, my->payload))
		{
		  /* all media MUST have the same PROTO, PORT. */
		  sdp_m_media_add (*sdp, sgetcopy ("video"), sgetcopy (video_port),
				   my->number_of_port, sgetcopy (my->proto));
		  sdp_m_payload_add (*sdp, media_line, sgetcopy (my->payload));
		  if (my->a_rtpmap != NULL)
		    sdp_a_attribute_add (*sdp, media_line, sgetcopy ("rtpmap"),
					 sgetcopy (my->a_rtpmap));
		  media_line++;
		  break;
		}
	      pos++;
	    }
	}
    }
  return 0;
}


int
sdp_put_on_hold (sdp_t * sdp)
{
  int pos;
  int pos_media = -1;
  char *rcvsnd;
  int recv_send = -1;

  pos = 0;
  rcvsnd = sdp_a_att_field_get (sdp, pos_media, pos);
  while (rcvsnd != NULL)
    {
      if (rcvsnd != NULL && 0 == strcmp (rcvsnd, "sendonly")
	  && 0 == strcmp (rcvsnd, "sendrecv"))
	{
	  recv_send = 0;
	}
      else if (rcvsnd != NULL && 0 == strcmp (rcvsnd, "recvonly"))
	{
	  recv_send = 0;
	  sprintf (rcvsnd, "sendonly");
	}
      pos++;
      rcvsnd = sdp_a_att_field_get (sdp, pos_media, pos);
    }

  pos_media = 0;
  while (!sdp_endof_media (sdp, pos_media))
    {
      pos = 0;
      rcvsnd = sdp_a_att_field_get (sdp, pos_media, pos);
      while (rcvsnd != NULL)
	{
	  if (rcvsnd != NULL && 0 == strcmp (rcvsnd, "sendonly"))
	    {
	      recv_send = 0;
	    }
	  else if (rcvsnd != NULL && 0 == strcmp (rcvsnd, "recvonly"))
	    {
	      recv_send = 0;
	      sprintf (rcvsnd, "sendonly");
	    }
	  pos++;
	  rcvsnd = sdp_a_att_field_get (sdp, pos_media, pos);
	}
      pos_media++;
    }

  if (recv_send == -1)
    {
      /* we need to add a global attribute with a feild set to "sendonly" */
      sdp_a_attribute_add (sdp, -1, sgetcopy ("sendonly"), NULL);
    }

  return 0;
}

int
sdp_put_off_hold (sdp_t * sdp)
{
  int pos;
  int pos_media = -1;
  char *rcvsnd;

  pos = 0;
  rcvsnd = sdp_a_att_field_get (sdp, pos_media, pos);
  while (rcvsnd != NULL)
    {
      if (rcvsnd != NULL && (0 == strcmp (rcvsnd, "sendonly")
			     || 0 == strcmp (rcvsnd, "recvonly")))
	{
	  sprintf (rcvsnd, "sendrecv");
	}
      pos++;
      rcvsnd = sdp_a_att_field_get (sdp, pos_media, pos);
    }

  pos_media = 0;
  while (!sdp_endof_media (sdp, pos_media))
    {
      pos = 0;
      rcvsnd = sdp_a_att_field_get (sdp, pos_media, pos);
      while (rcvsnd != NULL)
	{
	  if (rcvsnd != NULL && (0 == strcmp (rcvsnd, "sendonly")
				 || 0 == strcmp (rcvsnd, "recvonly")))
	    {
	      sprintf (rcvsnd, "sendrecv");
	    }
	  pos++;
	  rcvsnd = sdp_a_att_field_get (sdp, pos_media, pos);
	}
      pos_media++;
    }

  return 0;
}
