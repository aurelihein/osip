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


#ifndef _CONST_H_
#define _CONST_H_

#ifdef ENABLE_MPATROL
#include <mpatrol.h>
#endif

#define CRLF "\r\n\0"
#define CR   "\r\0"
#define LF   "\n\0"
#define SP   " \0"


#define ACCEPT			       	"accept"
#define ACCEPT_ENCODING			"accept-encoding"
#define ACCEPT_LANGUAGE			"accept-language"
#define ALERT_INFO		       	"alert-info"
#define ALLOW			       	"allow"
#define AUTHENTICATION_INFO		"authentication-info"
#define AUTHORIZATION			"authorization"
#define CALL_ID			       	"call-id"
#define CALL_ID_SHORT			"i"
#define CALL_INFO		       	"call-info"
#define CONTACT			       	"contact"
#define CONTACT_SHORT			"m"
#define CONTENT_DISPOSITION		"content-disposition"
#define CONTENT_ENCODING_SHORT	        "e"
#define CONTENT_ENCODING		"content-encoding"
#define CONTENT_LANGUAGE		"content-language"
#define CONTENT_LENGTH_SHORT	        "l"
#define CONTENT_LENGTH			"content-length"
#define CONTENT_TYPE_SHORT		"c"
#define CONTENT_TYPE			"content-type"
#define CSEQ				"cseq"
#define SIPDATE				"date"
#define ERROR_INFO			"error-info"
#define EXPIRES				"expires"
#define FROM				"from"
#define FROM_SHORT			"f"
#define IN_REPLY_TO			"in-reply-to"
#define MAX_FORWARDS			"max-forwards"
#define MIME_VERSION			"mime-version"
#define MIN_EXPIRES			"min-expires"
#define ORGANIZATION			"organization"
#define PRIORITY			"priority"
#define PROXY_AUTHENTICATE		"proxy-authenticate"
#define PROXY_AUTHORIZATION		"proxy-authorization"
#define PROXY_REQUIRE			"proxy-require"
#define RECORD_ROUTE			"record-route"
#define REPLY_TO			"reply-to"
#define REQUIRE				"require"
#define RETRY_AFTER			"retry-after"
#define ROUTE				"route"
#define SERVER				"server"
#define SUBJECT				"subject"
#define SUBJECT_SHORT			"s"
#define SUPPORTED			"supported"
#define SUPPORTED_SHORT			"k"
#define TIMESTAMP			"timestamp"
#define TO				"to"
#define TO_SHORT			"t"
#define UNSUPPORTED			"unsupported"
#define USER_AGENT			"user-agent"
#define VIA				"via"
#define VIA_SHORT			"v"
#define WARNING				"warning"
#define WWW_AUTHENTICATE		"www-authenticate"


#endif /*  _CONST_H_ */
