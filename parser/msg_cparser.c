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


#include <osip/smsg.h>
#include <osip/port.h>
#include "msg.h"

#ifndef USE_GPREF

static parser_config_t pconfig[NUMBER_OF_HEADERS];

/*
  list of compact header:
  i: Call-ID   => ok
  m: Contact   => ok
  e: Content-Encoding   => ok
  l: Content-Length   => ok
  c: Content-Type   => ok
  f: From   => ok
  s: Subject   => NOT A KNOWN HEADER! will be
                 available in the list of unknown headers
  t: To   => ok
  v: Via   => ok
  k: Supported  -> NOT A KNOWN HEADER! will be
                 available in the list of unknown headers
*/
/* This method must be called before using the parser */
int
parser_init ()
{

  pconfig[0].hname = ACCEPT;
  pconfig[0].setheader = (&msg_setaccept);
  pconfig[1].hname = ACCEPT_ENCODING;
  pconfig[1].setheader = (&msg_setaccept_encoding);
  pconfig[2].hname = ACCEPT_LANGUAGE;
  pconfig[2].setheader = (&msg_setaccept_language);
  pconfig[3].hname = ALERT_INFO;
  pconfig[3].setheader = (&msg_setalert_info);
  pconfig[4].hname = ALLOW;
  pconfig[4].setheader = (&msg_setallow);
  pconfig[5].hname = AUTHORIZATION;
  pconfig[5].setheader = (&msg_setauthorization);
  pconfig[6].hname = CONTENT_TYPE_SHORT;	/* "l" */
  pconfig[6].setheader = (&msg_setcontent_type);
  pconfig[7].hname = CALL_ID;
  pconfig[7].setheader = (&msg_setcall_id);
  pconfig[8].hname = CALL_INFO;
  pconfig[8].setheader = (&msg_setcall_info);
  pconfig[9].hname = CONTACT;
  pconfig[9].setheader = (&msg_setcontact);
  pconfig[10].hname = CONTENT_ENCODING;
  pconfig[10].setheader = (&msg_setcontent_encoding);
  pconfig[11].hname = CONTENT_LENGTH;
  pconfig[11].setheader = (&msg_setcontent_length);
  pconfig[12].hname = CONTENT_TYPE;
  pconfig[12].setheader = (&msg_setcontent_type);
  pconfig[13].hname = CSEQ;
  pconfig[13].setheader = (&msg_setcseq);
  pconfig[14].hname = CONTENT_ENCODING_SHORT;	/* "e" */
  pconfig[14].setheader = (&msg_setcontent_encoding);
  pconfig[15].hname = ERROR_INFO;
  pconfig[15].setheader = (&msg_seterror_info);
  pconfig[16].hname = FROM_SHORT;	/* "f" */
  pconfig[16].setheader = (&msg_setfrom);
  pconfig[17].hname = FROM;
  pconfig[17].setheader = (&msg_setfrom);
  pconfig[18].hname = CALL_ID_SHORT;	/* "i" */
  pconfig[18].setheader = (&msg_setcall_id);
  pconfig[19].hname = CONTENT_LENGTH_SHORT;	/* "l" */
  pconfig[19].setheader = (&msg_setcontent_length);
  pconfig[20].hname = CONTACT_SHORT;	/* "m" */
  pconfig[20].setheader = (&msg_setcontact);
  pconfig[21].hname = MIME_VERSION;
  pconfig[21].setheader = (&msg_setmime_version);
  pconfig[22].hname = PROXY_AUTHENTICATE;
  pconfig[22].setheader = (&msg_setproxy_authenticate);
  pconfig[23].hname = PROXY_AUTHORIZATION;
  pconfig[23].setheader = (&msg_setproxy_authorization);
  pconfig[24].hname = RECORD_ROUTE;
  pconfig[24].setheader = (&msg_setrecord_route);
  pconfig[25].hname = ROUTE;
  pconfig[25].setheader = (&msg_setroute);
  pconfig[26].hname = TO_SHORT;
  pconfig[26].setheader = (&msg_setto);
  pconfig[27].hname = TO;
  pconfig[27].setheader = (&msg_setto);
  pconfig[28].hname = VIA_SHORT;
  pconfig[28].setheader = (&msg_setvia);
  pconfig[29].hname = VIA;
  pconfig[29].setheader = (&msg_setvia);
  pconfig[30].hname = WWW_AUTHENTICATE;
  pconfig[30].setheader = (&msg_setwww_authenticate);

  return 0;
}

/* search the header hname through pconfig[] tab. 
   A quicker algorithm should be used.
   It returns the index of the header in the parser_config_t tab.
*/
int
parser_isknownheader (char *hname)
{
  size_t length;
  int iinf = 0;
  int isup = NUMBER_OF_HEADERS;
  int i = NUMBER_OF_HEADERS / 2;

  length = strlen (hname);

  for (;;)
    {
      if (i < 0 || i > NUMBER_OF_HEADERS - 1)
	return -1;

      if ((length == strlen (pconfig[i].hname))
	  && strncmp ((const char *) hname,
		      (const char *) pconfig[i].hname, length) == 0)
	return i;

      if (iinf == isup)
	return -1;		/* not found */
      if (iinf == isup - 1)
	{
	  if ((i < NUMBER_OF_HEADERS - 1)
	      && (length == strlen (pconfig[i + 1].hname))
	      && strncmp ((const char *) hname,
			  (const char *) pconfig[i + 1].hname, length) == 0)
	    return i + 1;
	  else
	    return -1;
/* Unreachable code??
	  if ((i > 0) && (length == strlen (pconfig[i - 1].hname))
	      && strncmp ((const char *) hname,
			  (const char *) pconfig[i - 1].hname, length) == 0)
	    return i - 1;
	  else
	    return -1;
*/
	}
      if (0 < strncmp ((const char *) hname,
		       (const char *) pconfig[i].hname, length))
	{
	  /* if this is true, search further */
	  iinf = i;
	  if (i == i + (isup - i) / 2)
	    i++;
	  else
	    i = i + (isup - i) / 2;
	}
      else
	{
	  isup = i;
	  if (i == i - (i - iinf) / 2)
	    i--;
	  else
	    i = i - (i - iinf) / 2;
	}
    }				/* end of (while (1)) */
  return -1;
}

#else /* USE_GPREF */
/* C code produced by gperf version 2.7.2 */
/* Command-line: gperf sip.gperf  */

#define TOTAL_KEYWORDS 53
#define MIN_WORD_LENGTH 1
#define MAX_WORD_LENGTH 19
#define MIN_HASH_VALUE 1
#define MAX_HASH_VALUE 132
/* maximum key range = 132, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned char asso_values[] = {
  133, 133, 133, 133, 133, 133, 133, 133, 133, 133,
  133, 133, 133, 133, 133, 133, 133, 133, 133, 133,
  133, 133, 133, 133, 133, 133, 133, 133, 133, 133,
  133, 133, 133, 133, 133, 133, 133, 133, 133, 133,
  133, 133, 133, 133, 133, 133, 133, 133, 133, 133,
  133, 133, 133, 133, 133, 133, 133, 133, 133, 133,
  133, 133, 133, 133, 133, 133, 133, 133, 133, 133,
  133, 133, 133, 133, 133, 133, 133, 133, 133, 133,
  133, 133, 133, 133, 133, 133, 133, 133, 133, 133,
  133, 133, 133, 133, 133, 133, 133, 20, 133, 25,
  41, 0, 5, 20, 25, 1, 133, 133, 10, 60,
  60, 0, 0, 0, 45, 15, 45, 30, 40, 0,
  133, 15, 133, 133, 133, 133, 133, 133, 133, 133,
  133, 133, 133, 133, 133, 133, 133, 133, 133, 133,
  133, 133, 133, 133, 133, 133, 133, 133, 133, 133,
  133, 133, 133, 133, 133, 133, 133, 133, 133, 133,
  133, 133, 133, 133, 133, 133, 133, 133, 133, 133,
  133, 133, 133, 133, 133, 133, 133, 133, 133, 133,
  133, 133, 133, 133, 133, 133, 133, 133, 133, 133,
  133, 133, 133, 133, 133, 133, 133, 133, 133, 133,
  133, 133, 133, 133, 133, 133, 133, 133, 133, 133,
  133, 133, 133, 133, 133, 133, 133, 133, 133, 133,
  133, 133, 133, 133, 133, 133, 133, 133, 133, 133,
  133, 133, 133, 133, 133, 133, 133, 133, 133, 133,
  133, 133, 133, 133, 133, 133, 133, 133, 133, 133,
  133, 133, 133, 133, 133, 133
};
static const parser_config_t pconfig[133] = {
  {"", NULL},
  {CONTENT_ENCODING_SHORT, &msg_setcontent_encoding},
  {"", NULL},
  {CALL_ID_SHORT, &msg_setcall_id},
  {"", NULL}, {"", NULL}, {"", NULL}, {"", NULL}, {"", NULL}, {"", NULL},
  {ERROR_INFO, &msg_seterror_info},
  {FROM_SHORT, &msg_setfrom},
  {IN_REPLY_TO, NULL},
  {PROXY_REQUIRE, NULL},
  {"", NULL}, {"", NULL},
  {WWW_AUTHENTICATE, &msg_setwww_authenticate},
  {"", NULL},
  {PROXY_AUTHENTICATE, &msg_setproxy_authenticate},
  {"", NULL}, {"", NULL},
  {CONTENT_LENGTH_SHORT, &msg_setcontent_length},
  {EXPIRES, NULL},
  {PRIORITY, NULL},
  {"", NULL},
  {ALLOW, &msg_setallow},
  {"", NULL},
  {WARNING, NULL},
  {"", NULL},
  {CSEQ, &msg_setcseq},
  {ALERT_INFO, &msg_setalert_info},
  {SUBJECT_SHORT, NULL},
  {"", NULL}, {"", NULL},
  {CALL_INFO, &msg_setcall_info},
  {ACCEPT_LANGUAGE, &msg_setaccept_language},
  {"", NULL},
  {CONTENT_TYPE, &msg_setcontent_type},
  {"", NULL},
  {AUTHENTICATION_INFO, NULL},
  {"", NULL},
  {CONTENT_LANGUAGE, NULL},
  {"", NULL}, {"", NULL}, {"", NULL},
  {SIPDATE, NULL},
  {"", NULL},
  {"to", &msg_setto},
  {"", NULL}, {"", NULL},
  {ROUTE, &msg_setroute},
  {CONTENT_TYPE_SHORT, &msg_setcontent_type},
  {REQUIRE, NULL},
  {REPLY_TO, NULL},
  {TIMESTAMP, NULL},
  {ACCEPT_ENCODING, &msg_setaccept_encoding},
  {"", NULL},
  {RECORD_ROUTE, &msg_setrecord_route},
  {"", NULL}, {"", NULL}, {"", NULL},
  {CONTENT_ENCODING, &msg_setcontent_encoding},
  {"", NULL},
  {VIA, &msg_setvia},
  {CONTENT_LENGTH, &msg_setcontent_length},
  {SUPPORTED, NULL},
  {SERVER, NULL},
  {SUBJECT, NULL},
  {"", NULL},
  {FROM, &msg_setfrom},
  {"", NULL},
  {ACCEPT, &msg_setaccept},
  {ORGANIZATION, NULL},
  {CALL_ID, &msg_setcall_id},
  {"", NULL}, {"", NULL}, {"", NULL},
  {CONTACT, &msg_setcontact},
  {"", NULL},
  {PROXY_AUTHORIZATION, &msg_setproxy_authorization},
  {"", NULL},
  {VIA_SHORT, &msg_setvia},
  {UNSUPPORTED, NULL},
  {"", NULL}, {"", NULL},
  {USER_AGENT, NULL},
  {MIN_EXPIRES, NULL},
  {MAX_FORWARDS, NULL},
  {"", NULL}, {"", NULL}, {"", NULL},
  {TO_SHORT, &msg_setto},
  {"", NULL},
  {AUTHORIZATION, &msg_setauthorization},
  {"", NULL}, {"", NULL}, {"", NULL}, {"", NULL}, {"", NULL}, {"", NULL}, {"",
									   NULL},
  {RETRY_AFTER, NULL},
  {"", NULL}, {"", NULL},
  {CONTENT_DISPOSITION, NULL},
  {"", NULL}, {"", NULL}, {"", NULL}, {"", NULL}, {"", NULL}, {"", NULL}, {"",
									   NULL},
  {"", NULL}, {"", NULL},
  {"", NULL}, {"", NULL}, {"", NULL}, {"", NULL}, {"", NULL}, {"", NULL}, {"",
									   NULL},
  {CONTACT_SHORT, &msg_setcontact},
  {"", NULL}, {"", NULL}, {"", NULL}, {"", NULL}, {"", NULL}, {"", NULL}, {"",
									   NULL},
  {"", NULL}, {"", NULL},
  {"", NULL},
  {MIME_VERSION, &msg_setmime_version}
};
static unsigned int
hash (str, len)
     register const char *str;
     register unsigned int len;
{
  return len + asso_values[(unsigned char) str[len - 1]] +
    asso_values[(unsigned char) str[0]];
}

#ifdef __GNUC__
__inline
#endif
  int
in_word_set (str, len)
     register const char *str;
     register unsigned int len;
{
  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
	{
	  register const char *s = pconfig[key].hname;

	  if (*str == *s && !strcmp (str + 1, s + 1)
	      && (pconfig[key].setheader != NULL))
	    return key;
	}
    }
  return -1;
}

int
parser_init ()
{
  return 0;			/* do not need initialization when using gpref */
}

int
parser_isknownheader (char *hname)
{
  int iLength;

  iLength = strlen (hname);
  return in_word_set (hname, iLength);
}

#endif

/* This method calls the method that is able to parse the header */
int
parser_callmethod (int i, sip_t * dest, char *hvalue)
{
  return pconfig[i].setheader (dest, hvalue);
}
