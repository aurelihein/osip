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


#ifndef _CONST_H_
#define _CONST_H_

#ifdef ENABLE_MPATROL
#include <mpatrol.h>
#endif

#define CRLF "\r\n\0"
#define CR   "\r\0"
#define LF   "\n\0"
#define SP   " \0"


#define ACCEPT   "accept"
#define ACCEPT_ENCODING "accept-encoding"
#define ACCEPT_LANGUAGE "accept-language"
#define CALL_ID_SHORT "i"
#define CALL_ID "call-id"
#define CALL_INFO "call-info"
#define CONTACT_SHORT "m"
#define CONTACT "contact"
#define CSEQ "cseq"
#define SIPDATE "date"
#define ENCRYPTION "encryption"
#define FROM_SHORT "f"
#define FROM "from"
#define MIME_VERSION "mime-version"
#define ORGANIZATION "organization"
#define RECORD_ROUTE "record-route"
#define REQUIRE "require"
#define SUPPORTED "supported"
#define TIMESTAMP "timestamp"
#define TO_SHORT "t"
#define TO "to"
#define USER_AGENT "user-agent"
#define VIA_SHORT "v"
#define VIA "via"
#define ALLOW "allow"
#define CONTENT_DISPOSITION "content-disposition"
#define CONTENT_ENCODING_SHORT "e"
#define CONTENT_ENCODING "content-encoding"
#define CONTENT_LANGUAGE "content-language"
#define CONTENT_LENGTH_SHORT "l"
#define CONTENT_LENGTH "content-length"
#define CONTENT_TYPE_SHORT "c"
#define CONTENT_TYPE "content-type"
#define EXPIRES "expires"
#define ALERT_INFO "alert-info"
#define AUTHORIZATION "authorization"
#define IN_REPLY_TO "in-reply-to"
#define MAX_FORWARDS "max-forwards"
#define PRIORITY "priority"
#define PROXY_AUTHORIZATION "proxy-authorization"
#define PROXY_REQUIRE "proxy-require"
#define ROUTE "route"
#define RESPONSE_KEY "response-key"
#define SUBJECT_SHORT "s"
#define SUBJECT "subject"
#define ERROR_INFO "error-info"
#define PROXY_AUTHENTICATE "proxy-authenticate"
#define RETRY_AFTER "retry-after"
#define SERVE "serve"
#define UNSUPPORTED "unsupported"
#define WARNING "warning"
#define WWW_AUTHENTICATE "www-authenticate"


#endif /*  _CONST_H_ */
