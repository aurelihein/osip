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


#include <osip/smsg.h>
#include <osip/port.h>
#include "msg.h"

static parser_config_t pconfig[NUMBER_OF_HEADERS];

/*
  list of compact header:
  i: Call-ID   => ok
  m: Contact   => ok
  e: Content-Encoding   => ok
  l: Content-Length   => ok
  c: Content-Type   => ok
  f: From   => ok
  s: Subject   => NOT A SUPPORTED HEADER! will be
                 available in the list of unknown headers
  t: To   => ok
  v: Via   => ok
*/
/* This method must be called before using the parser */
int parser_init() {

  pconfig[0].hname=ACCEPT;
  pconfig[0].setheader=&msg_setaccept;
  pconfig[1].hname=ACCEPT_ENCODING;
  pconfig[1].setheader=&msg_setaccept_encoding;
  pconfig[2].hname=ACCEPT_LANGUAGE;
  pconfig[2].setheader=&msg_setaccept_language;
  pconfig[3].hname=ALERT_INFO;
  pconfig[3].setheader=&msg_setalert_info;
  pconfig[4].hname=ALLOW;
  pconfig[4].setheader=&msg_setallow;
  pconfig[5].hname=AUTHORIZATION;
  pconfig[5].setheader=&msg_setauthorization;
  pconfig[6].hname=CONTENT_TYPE_SHORT;   /* "l" */
  pconfig[6].setheader=&msg_setcontent_type;
  pconfig[7].hname=CALL_ID;
  pconfig[7].setheader=&msg_setcall_id;
  pconfig[8].hname=CALL_INFO;
  pconfig[8].setheader=&msg_setcall_info;
  pconfig[9].hname=CONTACT;
  pconfig[9].setheader=&msg_setcontact;
  pconfig[10].hname=CONTENT_ENCODING;
  pconfig[10].setheader=&msg_setcontent_encoding;
  pconfig[11].hname=CONTENT_LENGTH;
  pconfig[11].setheader=&msg_setcontent_length;
  pconfig[12].hname=CONTENT_TYPE;
  pconfig[12].setheader=&msg_setcontent_type;
  pconfig[13].hname=CSEQ;
  pconfig[13].setheader=&msg_setcseq;
  pconfig[14].hname=CONTENT_ENCODING_SHORT;   /* "e" */
  pconfig[14].setheader=&msg_setcontent_encoding;
  pconfig[15].hname=ERROR_INFO;
  pconfig[15].setheader=&msg_seterror_info;
  pconfig[16].hname=FROM_SHORT;   /* "f" */
  pconfig[16].setheader=&msg_setfrom;
  pconfig[17].hname=FROM;
  pconfig[17].setheader=&msg_setfrom;
  pconfig[18].hname=CALL_ID_SHORT;   /* "i" */
  pconfig[18].setheader=&msg_setcall_id;
  pconfig[19].hname=CONTENT_LENGTH_SHORT;   /* "l" */
  pconfig[19].setheader=&msg_setcontent_length;
  pconfig[20].hname=CONTACT_SHORT;   /* "m" */
  pconfig[20].setheader=&msg_setcontact;
  pconfig[21].hname=MIME_VERSION;
  pconfig[21].setheader=&msg_setmime_version;
  pconfig[22].hname=PROXY_AUTHENTICATE;
  pconfig[22].setheader=&msg_setproxy_authenticate;
  pconfig[23].hname=PROXY_AUTHORIZATION;
  pconfig[23].setheader=&msg_setproxy_authorization;
  pconfig[24].hname=RECORD_ROUTE;
  pconfig[24].setheader=&msg_setrecord_route;
  pconfig[25].hname=ROUTE;
  pconfig[25].setheader=&msg_setroute;
  pconfig[26].hname=TO_SHORT;
  pconfig[26].setheader=&msg_setto;
  pconfig[27].hname=TO;
  pconfig[27].setheader=&msg_setto;
  pconfig[28].hname=VIA_SHORT;
  pconfig[28].setheader=&msg_setvia;
  pconfig[29].hname=VIA;
  pconfig[29].setheader=&msg_setvia;
  pconfig[30].hname=WWW_AUTHENTICATE;
  pconfig[30].setheader=&msg_setwww_authenticate;

  return 0;
}

/* This method calls the method that is able to parse the header */
int
parser_callmethod(int i,sip_t *dest,char *hvalue)
{
  return pconfig[i].setheader(dest,hvalue);
}

/* search the header hname through pconfig[] tab. 
   A quicker algorithm should be used.
   It returns the index of the header in the parser_config_t tab.
*/
int
parser_isknownheader(char *hname) {
  size_t length ;
  int iinf   = 0;
  int isup   = NUMBER_OF_HEADERS;
  int i      = NUMBER_OF_HEADERS/2;
  
  length = strlen(hname);

    while (1)
    {
      if (i<0||i>NUMBER_OF_HEADERS-1)
	return -1;

      if ((length==strlen(pconfig[i].hname))
	  &&strncmp((const char *) hname,
		    (const char *) pconfig[i].hname,
		    length)==0)
	return i;
      
      if (iinf==isup)
	return -1; /* not found */
      if (iinf==isup-1)
	{
	  if ((i<NUMBER_OF_HEADERS-1)
	      &&(length==strlen(pconfig[i+1].hname))
	      &&strncmp((const char *) hname,
			(const char *) pconfig[i+1].hname,
			length)==0)
	      return i+1;
	  else
	    return -1;
	  if ((i>0)&&(length==strlen(pconfig[i-1].hname))
	      &&strncmp((const char *) hname,
			(const char *) pconfig[i-1].hname,
			length)==0)
	      return i-1;
	  else
	    return -1;
	}
      if (0<strncmp((const char *) hname,
		    (const char *) pconfig[i].hname , length)) {
	/* if this is true, search further */
	iinf = i;	      
	if (i == i + (isup-i)/2) i++;
	else  i = i + (isup-i)/2;
      } else {
	isup = i;
	if (i == i - (i-iinf)/2) i--;
	else i = i - (i-iinf)/2;
      }
    } /* end of (while (1)) */
  return -1;
} 

