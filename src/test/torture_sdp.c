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


#ifdef ENABLE_MPATROL
#include <mpatrol.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#include <osipparser2/osip_port.h>
#include <osipparser2/sdp_message.h>
#include <osip2/osip_negotiation.h>

int test_sdp_message (char *msg, int verbose);
int test_accessor_get_api (sdp_message_t * sdp);
int test_accessor_set_api (sdp_message_t * sdp);


typedef struct _ua_context_t
{

  /* only one audio port is allowed at this time.. In the case, we
     receive more than one m audio line, this may fail... */
  char *m_audio_port;		/* audio port to be used for this session */
  char *m_video_port;		/* audio port to be used for this session */

}
ua_context_t;

ua_context_t *ua_context = NULL;

osip_negotiation_t *osip_negotiation = NULL;

int
ua_sdp_accept_audio_codec (osip_negotiation_ctx_t * context,
			   char *port, char *number_of_port,
			   int audio_qty, char *payload)
{
  /* this may come from buggy implementation who                 */
  /* propose several sdp lines while they only want 1 connection */
  if (0 != audio_qty)
    return -1;

  if (0 == strcmp (payload, "0") || 0 == strcmp (payload, "3") ||
      0 == strcmp (payload, "7") || 0 == strcmp (payload, "8"))
    return 0;
  return -1;
}

int
ua_sdp_accept_video_codec (osip_negotiation_ctx_t * context,
			   char *port, char *number_of_port,
			   int video_qty, char *payload)
{
  /* this may come from buggy implementation who                 */
  /* propose several sdp lines while they only want 1 connection */
  if (0 != video_qty)
    return -1;
  /* ... */
  if (0 == strcmp (payload, "31"))
    return 0;
  return -1;
}

int
ua_sdp_accept_other_codec (osip_negotiation_ctx_t * context,
			   char *type, char *port,
			   char *number_of_port, char *payload)
{
  /* ... */
  return -1;
}

char *
ua_sdp_get_video_port (osip_negotiation_ctx_t * context, int pos_media)
{
  ua_context_t *ua_con;

  ua_con = (ua_context_t *) context->mycontext;
  return osip_strdup (ua_con->m_video_port);	/* this port should not be static ... */
  /* also, this method should be called more than once... */
  /* If there is more than one audio line, this may fail :( */
}

char *
ua_sdp_get_audio_port (osip_negotiation_ctx_t * context, int pos_media)
{
  ua_context_t *ua_con;

  ua_con = (ua_context_t *) context->mycontext;
  return osip_strdup (ua_con->m_audio_port);	/* this port should not be static ... */
  /* also, this method should be called more than once... */
  /* If there is more than one audio line, this may fail :( */
}

int
main (int argc, char **argv)
{
  int i;
  int verbose = 0;		/* 0: verbose, 1 (or nothing: not verbose) */
  char *marker;
  FILE *torture_file;
  char *tmp;
  char *msg;
  char *tmpmsg;
  static int num_test = 0;

  if (argc > 3)
    {
      if (0 == strncmp (argv[3], "-v", 2))
	verbose = 1;
    }

  torture_file = fopen (argv[1], "r");
  if (torture_file == NULL)
    {
      fprintf (stderr,
	       "Failed to open \"torture_sdps\" file.\nUsage: %s torture_file [-v]\n",
	       argv[0]);
      exit (1);
    }

  ua_context = (ua_context_t *) osip_malloc (sizeof (ua_context_t));

  ua_context->m_audio_port = osip_strdup ("20030");
  ua_context->m_video_port = osip_strdup ("20040");

  i = osip_negotiation_init (&osip_negotiation);
  if (i != 0)
    {
      fprintf (stderr, "Failed to initialize the SDP negociator\n");
      fclose(torture_file);
      exit (1);
    }
  osip_negotiation_set_o_username (osip_negotiation, osip_strdup ("userX"));
  osip_negotiation_set_o_session_id (osip_negotiation, osip_strdup ("20000001"));
  osip_negotiation_set_o_session_version (osip_negotiation, osip_strdup ("20000001"));
  osip_negotiation_set_o_nettype (osip_negotiation, osip_strdup ("IN"));
  osip_negotiation_set_o_addrtype (osip_negotiation, osip_strdup ("IP4"));
  osip_negotiation_set_o_addr (osip_negotiation, osip_strdup ("192.168.1.114"));

  osip_negotiation_set_c_nettype (osip_negotiation, osip_strdup ("IN"));
  osip_negotiation_set_c_addrtype (osip_negotiation, osip_strdup ("IP4"));
  osip_negotiation_set_c_addr (osip_negotiation, osip_strdup ("192.168.1.114"));

  /* ALL CODEC MUST SHARE THE SAME "C=" line and proto as the media 
     will appear on the same "m" line... */
  osip_negotiation_add_support_for_audio_codec (osip_negotiation, 
						osip_strdup ("0"),
						osip_strdup ("2"),
						/* NULL, */
					  osip_strdup ("RTP/AVP"),
					  NULL, NULL, NULL,
					  NULL, NULL,
					  osip_strdup ("0 PCMU/8000"));
  osip_negotiation_add_support_for_audio_codec (osip_negotiation, 
					       osip_strdup ("3"), NULL,
					  osip_strdup ("RTP/AVP"), NULL, NULL,
					  NULL, NULL, NULL,
					  osip_strdup ("3 GSM/8000"));
  osip_negotiation_add_support_for_audio_codec (osip_negotiation, 
					       osip_strdup ("7"), NULL,
					  osip_strdup ("RTP/AVP"), NULL, NULL,
					  NULL, NULL, NULL,
					  osip_strdup ("7 LPC/8000"));
  osip_negotiation_add_support_for_audio_codec (osip_negotiation, 
					       osip_strdup ("8"), NULL,
					  osip_strdup ("RTP/AVP"), NULL, NULL,
					  NULL, NULL, NULL,
					  osip_strdup ("8 PCMA/8000"));
  osip_negotiation_add_support_for_video_codec (osip_negotiation, 
					       osip_strdup ("31"),
					  NULL,
					  osip_strdup ("RTP/AVP"),
					  NULL, NULL, NULL,
					  NULL, NULL,
					  osip_strdup ("31 H261/90000"));

  osip_negotiation_set_fcn_accept_audio_codec (osip_negotiation, &ua_sdp_accept_audio_codec);
  osip_negotiation_set_fcn_accept_video_codec (osip_negotiation, &ua_sdp_accept_video_codec);
  osip_negotiation_set_fcn_accept_other_codec (osip_negotiation, &ua_sdp_accept_other_codec);
  osip_negotiation_set_fcn_get_audio_port (osip_negotiation, &ua_sdp_get_audio_port);
  osip_negotiation_set_fcn_get_video_port (osip_negotiation, &ua_sdp_get_video_port);

  i = 0;
  tmp = (char *) osip_malloc (500);
  marker = fgets (tmp, 500, torture_file);	/* lines are under 500 */
  while (marker != NULL && i < atoi (argv[2]))
    {
      if (0 == strncmp (tmp, "|", 1))
	i++;
      marker = fgets (tmp, 500, torture_file);
    }

  num_test++;

  msg = (char *) osip_malloc (10000);	/* msg are under 10000 */
  tmpmsg = msg;

  if (marker == NULL)
    {
      fprintf (stderr,
	       "Error! The message's number you specified does not exist\n");
      fclose(torture_file);
      osip_free(msg);
      exit (1);			/* end of file detected! */
    }
  /* this part reads an entire message, separator is "|" */
  /* (it is unlinkely that it will appear in messages!) */
  while (marker != NULL && strncmp (tmp, "|", 1))
    {
      osip_strncpy (tmpmsg, tmp, strlen (tmp));
      tmpmsg = tmpmsg + strlen (tmp);
      marker = fgets (tmp, 500, torture_file);
    }

  if (verbose)
    {
      fprintf (stdout, "test %s : ============================ \n", argv[2]);
      fprintf (stdout, "%s", msg);

      if (0 == test_sdp_message (msg, verbose))
	fprintf (stdout, "test %s : ============================ OK\n",
		 argv[2]);
      else
	fprintf (stdout, "test %s : ============================ FAILED\n",
		 argv[2]);
    }
  else
    {
      if (0 == test_sdp_message (msg, verbose))
	fprintf (stdout, "test %s : OK\n", argv[2]);
      else
	fprintf (stdout, "test %s : FAILED\n", argv[2]);
    }

  osip_free (msg);
  osip_free (tmp);
  osip_negotiation_free (osip_negotiation);
  osip_free (ua_context->m_audio_port);
  osip_free (ua_context->m_video_port);
  osip_free (ua_context);
  fclose(torture_file);
  return 0;
}

int
test_sdp_message (char *msg, int verbose)
{
  sdp_message_t *sdp;

  {
    char *result;

    sdp_message_init (&sdp);
    if (sdp_message_parse (sdp, msg) != 0)
      {
	fprintf (stdout, "ERROR: failed while parsing!\n");
	sdp_message_free (sdp);		/* try to free msg, even if it failed! */
	/* this seems dangerous..... */
	return -1;
      }
    else
      {
	int i;

	i = sdp_message_to_str (sdp, &result);
	test_accessor_get_api (sdp);
	if (i == -1)
	  {
	    fprintf (stdout, "ERROR: failed while printing message!\n");
	    sdp_message_free (sdp);
	    return -1;
	  }
	else
	  {
	    if (verbose)
	      fprintf (stdout, "%s", result);
	    if (strlen (result) != strlen (msg))
	      fprintf (stdout, "length differ from original message!\n");
	    if (0 == strncmp (result, msg, strlen (result)))
	      fprintf (stdout, "result equals msg!!\n");
	    osip_free (result);
	    {
	      osip_negotiation_ctx_t *context;

	      sdp_message_t *dest;

	      i = osip_negotiation_ctx_init (&context);
	      i = osip_negotiation_ctx_set_mycontext (context, (void *) ua_context);

	      {
		sdp_message_t *sdp;

		osip_negotiation_sdp_build_offer (osip_negotiation,
						  context, &sdp,
						  ua_context->m_audio_port,
						  ua_context->m_video_port);
		sdp_message_to_str (sdp, &result);
		fprintf (stdout, "Here is the offer:\n%s\n", result);
		osip_free (result);
		osip_negotiation_sdp_message_put_on_hold (sdp);
		sdp_message_to_str (sdp, &result);
		fprintf (stdout, "Here is the offer on hold:\n%s\n", result);
		osip_free (result);
		sdp_message_free (sdp);
	      }



	      i = osip_negotiation_ctx_set_remote_sdp (context, sdp);
	      if (i != 0)
		{
		  fprintf (stdout,
			   "Initialisation of context failed. Could not negociate\n");
		}
	      else
		{
		  fprintf (stdout, "Trying to execute a SIP negotiation:\n");
		  i = osip_negotiation_ctx_execute_negotiation (osip_negotiation,
							       context);
		  fprintf (stdout, "return code: %i\n", i);
		  if (i == 200)
		    {
		      dest = osip_negotiation_ctx_get_local_sdp (context);
		      fprintf (stdout, "SDP answer:\n");
		      i = sdp_message_to_str (dest, &result);
		      if (i != 0)
			fprintf (stdout,
				 "Error found in SDP answer while printing\n");
		      else
			fprintf (stdout, "%s\n", result);
		      osip_free (result);
		    }
		  osip_negotiation_ctx_free (context);
		  return 0;
		}
	    }
	  }
	sdp_message_free (sdp);
      }
  }
  return 0;
}

int
test_accessor_set_api (sdp_message_t * sdp)
{
  return 0;
}

int
test_accessor_get_api (sdp_message_t * sdp)
{
  char *tmp;
  char *tmp2;
  char *tmp3;
  char *tmp4;
  char *tmp5;
  int i;
  int k;

  printf ("v_version:      |%s|\n", sdp_message_v_version_get (sdp));
  printf ("o_originator:   |%s|", sdp_message_o_username_get (sdp));
  printf (" |%s|", sdp_message_o_sess_id_get (sdp));
  printf (" |%s|", sdp_message_o_sess_version_get (sdp));
  printf (" |%s|", sdp_message_o_nettype_get (sdp));
  printf (" |%s|", sdp_message_o_addrtype_get (sdp));
  printf (" |%s|\n", sdp_message_o_addr_get (sdp));
  if (sdp_message_s_name_get (sdp))
    printf ("s_name:         |%s|\n", sdp_message_s_name_get (sdp));
  if (sdp_message_i_info_get (sdp, -1))
    printf ("i_info:         |%s|\n", sdp_message_i_info_get (sdp, -1));
  if (sdp_message_u_uri_get (sdp))
    printf ("u_uri:          |%s|\n", sdp_message_u_uri_get (sdp));

  i = 0;
  do
    {
      tmp = sdp_e_email_get (sdp, i);
      if (tmp != NULL)
	printf ("e_email:        |%s|\n", tmp);
      i++;
    }
  while (tmp != NULL);
  i = 0;
  do
    {
      tmp = sdp_message_p_phone_get (sdp, i);
      if (tmp != NULL)
	printf ("p_phone:        |%s|\n", tmp);
      i++;
    }
  while (tmp != NULL);

  k = 0;
  tmp = sdp_message_c_nettype_get (sdp, -1, k);
  tmp2 = sdp_message_c_addrtype_get (sdp, -1, k);
  tmp3 = sdp_message_c_addr_get (sdp, -1, k);
  tmp4 = sdp_message_c_addr_multicast_ttl_get (sdp, -1, k);
  tmp5 = sdp_message_c_addr_multicast_int_get (sdp, -1, k);
  if (tmp != NULL)
    printf ("c_connection:   |%s| |%s| |%s| |%s| |%s|\n",
	    tmp, tmp2, tmp3, tmp4, tmp5);

  k = 0;
  do
    {
      tmp = sdp_message_b_bwtype_get (sdp, -1, k);
      tmp2 = sdp_message_b_bandwidth_get (sdp, -1, k);
      if (tmp != NULL)
	printf ("b_bandwidth:    |%s|:|%s|\n", tmp, tmp2);
      k++;
    }
  while (tmp != NULL);

  k = 0;
  do
    {
      tmp = sdp_message_t_start_time_get (sdp, k);
      tmp2 = sdp_message_t_stop_time_get (sdp, k);
      if (tmp != NULL)
	printf ("t_descr_time:   |%s| |%s|\n", tmp, tmp2);
      i = 0;
      do
	{
	  tmp2 = sdp_message_r_repeat_get (sdp, k, i);
	  i++;
	  if (tmp2 != NULL)
	    printf ("r_repeat:    |%s|\n", tmp2);
	}
      while (tmp2 != NULL);
      k++;
    }
  while (tmp != NULL);

  /* TODO r */

  if (sdp_message_z_adjustments_get (sdp) != NULL)
    printf ("z_adjustments:  |%s|\n", sdp_message_z_adjustments_get (sdp));

  tmp = sdp_message_k_keytype_get (sdp, -1);
  tmp2 = sdp_message_k_keydata_get (sdp, -1);
  if (tmp != NULL)
    printf ("k_key:          |%s|:|%s|\n", tmp, tmp2);

  k = 0;
  do
    {
      tmp = sdp_message_a_att_field_get (sdp, -1, k);
      tmp2 = sdp_message_a_att_value_get (sdp, -1, k);
      if (tmp != NULL)
	printf ("a_attribute:    |%s|:|%s|\n", tmp, tmp2);
      k++;
    }
  while (tmp != NULL);

  i = 0;
  while (!sdp_message_endof_media (sdp, i))
    {

      tmp = sdp_message_m_media_get (sdp, i);
      tmp2 = sdp_message_m_port_get (sdp, i);
      tmp3 = sdp_message_m_number_of_port_get (sdp, i);
      tmp4 = sdp_message_m_proto_get (sdp, i);
      if (tmp != NULL)
	printf ("m_media:        |%s| |%s| |%s| |%s|", tmp, tmp2, tmp3, tmp4);
      k = 0;
      do
	{
	  tmp = sdp_message_m_payload_get (sdp, i, k);
	  if (tmp != NULL)
	    printf (" |%s|", tmp);
	  k++;
	}
      while (tmp != NULL);
      printf ("\n");
      k = 0;
      do
	{
	  tmp = sdp_message_c_nettype_get (sdp, i, k);
	  tmp2 = sdp_message_c_addrtype_get (sdp, i, k);
	  tmp3 = sdp_message_c_addr_get (sdp, i, k);
	  tmp4 = sdp_message_c_addr_multicast_ttl_get (sdp, i, k);
	  tmp5 = sdp_message_c_addr_multicast_int_get (sdp, i, k);
	  if (tmp != NULL)
	    printf ("c_connection:   |%s| |%s| |%s| |%s| |%s|\n",
		    tmp, tmp2, tmp3, tmp4, tmp5);
	  k++;
	}
      while (tmp != NULL);

      k = 0;
      do
	{
	  tmp = sdp_message_b_bwtype_get (sdp, i, k);
	  tmp2 = sdp_message_b_bandwidth_get (sdp, i, k);
	  if (tmp != NULL)
	    printf ("b_bandwidth:    |%s|:|%s|\n", tmp, tmp2);
	  k++;
	}
      while (tmp != NULL);


      tmp = sdp_message_k_keytype_get (sdp, i);
      tmp2 = sdp_message_k_keydata_get (sdp, i);
      if (tmp != NULL)
	printf ("k_key:          |%s|:|%s|\n", tmp, tmp2);

      k = 0;
      do
	{
	  tmp = sdp_message_a_att_field_get (sdp, i, k);
	  tmp2 = sdp_message_a_att_value_get (sdp, i, k);
	  if (tmp != NULL)
	    printf ("a_attribute:    |%s|:|%s|\n", tmp, tmp2);
	  k++;
	}
      while (tmp != NULL);

      i++;
    }

  return 0;
}

/*
int
ua_sdp_set_info(osip_negotiation_ctx_t *context, sdp_message_t *dest) {
  return 0;

}
int
ua_sdp_set_uri(osip_negotiation_ctx_t *context, sdp_message_t *dest) {
  return 0;
}

int
ua_sdp_add_email(osip_negotiation_ctx_t *context, sdp_message_t *dest) {
  return 0;
}

int
ua_sdp_add_phone(osip_negotiation_ctx_t *context, sdp_message_t *dest) {
  return 0;
}

int
ua_sdp_add_attributes(osip_negotiation_ctx_t *context, sdp_message_t *dest, int pos_media) {
  return 0;
}
*/
