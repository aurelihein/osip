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

#ifndef _URLS_H_
#define _URLS_H_

#include <osip/const.h>

typedef enum {
  SIP,
  TEL           /* NOT ACTUALLY SUPPORTED */
}urischeme_t;

typedef struct _url_param_t {
  char *gname;
  char *gvalue;
} url_param_t;

typedef url_param_t url_header_t;

int   url_param_init(url_param_t **url_param);
void  url_param_free(url_param_t *url_param);
int   url_param_set (url_param_t *url_param, char *pname, char *pvalue);
void  url_param_freelist (list_t *params);
int   url_param_add      (list_t *params, char *pname, char *pvalue);
int   url_param_getbyname(list_t *params, char *pname, url_param_t **uparam);
int   url_param_clone(url_param_t *uparam, url_param_t **dest);

#define url_header_init(UH) url_param_init(UH)
#define url_header_free(UH) url_param_free(UH)
#define url_header_set(UH)  url_param_set(UH)
#define url_header_freelist(L)       url_param_freelist(L)
#define url_header_add(L,N,V)        url_param_add(L,N,V)
#define url_header_getbyname(L,N,UH) url_param_getbyname(L,N,UH)
#define url_header_clone(UH,DEST)    url_param_clone(UH,DEST)
/*
int   url_header_init(url_header_t **url_header);
void  url_header_free(url_header_t *url_header);
int   url_header_set (url_header_t *url_header, char *pname, char *pvalue);
void  url_header_freelist (list_t *params);
int   url_header_add(list_t *url_headers, char *pname, char *pvalue);
int   url_header_getbyname(list_t *params, char *pname, url_header_t **uparam);
*/

typedef struct _sipurl_t {
  char *scheme;
  char *username;
  char *password;
  char *host;
  char *port;
  list_t *url_params;
  list_t *url_headers;

  char *string; /* other schemes */
} url_t ;

int     url_init(url_t **url);
void    url_free(url_t *url);
int     url_parse(url_t *url, char *buf);
int     url_parse_headers(url_t *url, char *headers);
int     url_parse_params (url_t *url, char *params );
int     url_2char(url_t *url,char **dest);
int     url_clone(url_t *url,url_t **dest);

void    url_setscheme(url_t *url, char *scheme);
char*   url_getscheme(url_t *url);
void    url_sethost(url_t *url, char *host);
char*   url_gethost(url_t *url);
void    url_setusername(url_t *url, char *username);
char*   url_getusername(url_t *url);
void    url_setpassword(url_t *url, char *password);
char*   url_getpassword(url_t *url);
void    url_sethost(url_t *url, char *host);
char*   url_gethost(url_t *url);
void    url_setport(url_t *url, char *port);
char*   url_getport(url_t *url);



#define url_set_transport_udp(U)   url_param_add(U->url_params, "transport", "udp")
#define url_set_transport_tcp(U)   url_param_add(U->url_params, "transport", "tcp")
#define url_set_transport_sctp(U)  url_param_add(U->url_params, "transport", "sctp")
#define url_set_transport_tls(U)   url_param_add(U->url_params, "transport", "tls")
#define url_set_transport(U,T)     url_param_add(U->url_params, "transport", T)

#define url_set_user_phone(U)     url_param_add(U->url_params, "user", "phone")
#define url_set_user_ip(U)        url_param_add(U->url_params, "user", "ip")
#define url_set_user(U, USER)     url_param_add(U->url_params, "user", USER)

#define url_set_method_invite(U)  url_param_add(U->url_params, "method", "INVITE")
#define url_set_method_ack(U)     url_param_add(U->url_params, "method", "ACK")
#define url_set_method_options(U) url_param_add(U->url_params, "method", "OPTIONS")
#define url_set_method_bye(U)     url_param_add(U->url_params, "method", "BYE")
#define url_set_method_cancel(U)  url_param_add(U->url_params, "method", "CANCEL")
#define url_set_method_register(U) url_param_add(U->url_params,"method", "REGISTER")
#define url_set_method(U, M)      url_param_add(U->url_params, "method", M)
#define url_set_ttl(U, T)         url_param_add(U->url_params, "ttl", T)
#define url_set_maddr(U, M)       url_param_add(U->url_params, "maddr", M)

#define url_uparam_get(U,I,GP)    url_param_get(U->url_params,I,GP)
#define url_uparam_add(F,N,V)     url_param_add(F->url_params,N,V)
#define url_uparam_getbyname(F,N,UH)  url_param_getbyname(F->url_params,N,UH)

#define url_uheader_get(U,I,GP)   url_header_get(U->url_headers,I,GP)
#define url_uheader_add(F,N,V)    url_header_add(F->url_headers,N,V)
#define url_uheader_getbyname(F,N,UH) url_header_getbyname(F->url_headers,N,UH)


#endif /*  _URLS_H_ */
