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


#ifndef _SMSG_H_
#define _SMSG_H_

#include <osip/const.h>       /* constant definitions */
#include <osip/smsgtypes.h>   /* type     definitions */

/* must be called before using the parser */
/* If you are using the fsm library, this */
/* method is automaticly called by        */
/* osip_global_init() */
int      parser_init();

/* API used to parse SIP messages */
int      msg_init(sip_t **sip);
void     msg_free(sip_t *sip);
int      msg_parse(sip_t *dest , char *message);
int      msg_2char(sip_t *sip, char **dest);
#ifdef USE_TMP_BUFFER
/* in the case where USE_TMP_BUFFER is defined, the parser
has the ability to only call msg_2char() once and keep a
copy of the message re-usable for later retransmissions.
Don't change your code inside your application: the
behavior is compatible with the old msg_2char() method. 
Note that if you make some modification in your message after
calling msg_2char once, the message may need to be rebuild but
may not know that it need so. Inform that you want to rebuild it
with msg_force_update().
*/
int      msg_get_property(sip_t *sip);
#endif
/* next call to msg_2char will replace the deprecated buffer()
 this is only used in USE_TMP_BUFFER mode */
int      msg_force_update(sip_t *sip);

void     msg_setreasonphrase(sip_t *sip, char *reason);
char*    msg_getreasonphrase(sip_t *sip);
void     msg_setstatuscode(sip_t *sip, char *statuscode);
char*    msg_getstatuscode(sip_t *sip);
void     msg_setmethod(sip_t *sip , char *method);
char*    msg_getmethod(sip_t *sip);
void     msg_setversion(sip_t *sip , char *version);
char*    msg_getversion(sip_t *sip);
void     msg_seturi(sip_t *sip , url_t *uri);
url_t*   msg_geturi(sip_t *sip);

int      msg_header_getbyname(sip_t *sip, char *hname, int pos, header_t **dest);
/* return the reason phrase.                */
char*    msg_getreason(int replycode);

/* API for adding/removing headers in/from SIP messages */
int      msg_setaccept(sip_t *sip, char *hvalue);
int      msg_getaccept(sip_t *sip,int pos,accept_t **dest);
int      msg_setaccept_encoding(sip_t *sip, char *hvalue);
int      msg_getaccept_encoding(sip_t *sip,int pos,accept_encoding_t **dest);
int      msg_setaccept_language(sip_t *sip, char *hvalue);
int      msg_getaccept_language(sip_t *sip,int pos,accept_language_t **dest);
int      msg_setalert_info       (sip_t *sip, char *hvalue);
int      msg_getalert_info       (sip_t *sip, int pos, alert_info_t **ci);
int      msg_setallow (sip_t *sip, char *hvalue);
int      msg_getallow (sip_t *sip, int pos,allow_t **ci);
int      msg_setheader      (sip_t *sip, char *hname, char *hvalue);
int      msg_getheader      (sip_t *sip, int pos, header_t **dest);
int      msg_setauthorization(sip_t *sip, char *hvalue);
authorization_t *msg_getauthorization(sip_t *sip);
int      msg_setcall_id     (sip_t *sip, char *hvalue);
call_id_t* msg_getcall_id   (sip_t *sip);
int      msg_setcall_info       (sip_t *sip, char *hvalue);
int      msg_getcall_info       (sip_t *sip, int pos, call_info_t **ci);
int      msg_setcontact     (sip_t *sip, char *hvalue);
int      msg_getcontact     (sip_t *sip, int pos, contact_t **contact);
int      msg_setcontent_disposition (sip_t *sip, char *hvalue);
int      msg_getcontent_disposition (sip_t *sip, int pos,content_disposition_t **ci);
int      msg_setcontent_encoding (sip_t *sip, char *hvalue);
int      msg_getcontent_encoding (sip_t *sip, int pos,content_encoding_t **ci);
int      msg_setcontent_length(sip_t *sip, char *hvalue);
content_length_t* msg_getcontent_length(sip_t *sip);
int      msg_setcontent_type(sip_t *sip, char *hvalue);
content_type_t* msg_getcontent_type(sip_t *sip);
int      msg_setcseq        (sip_t  *sip, char *hvalue);
cseq_t*  msg_getcseq        (sip_t  *sip);
int      msg_seterror_info       (sip_t *sip, char *hvalue);
int      msg_geterror_info       (sip_t *sip, int pos, error_info_t **ci);
int      msg_setfrom        (sip_t *sip, char *hvalue);
from_t*  msg_getfrom        (sip_t *sip);
int      msg_setmime_version(sip_t *sip, char *hvalue);
mime_version_t* msg_getmime_version(sip_t *sip);
int      msg_setproxy_authenticate(sip_t *sip, char *hvalue);
proxy_authenticate_t *msg_getproxy_authenticate(sip_t *sip);
int      msg_getproxy_authorization(sip_t *sip,int pos,
				    proxy_authorization_t **dest);
int      msg_setproxy_authorization(sip_t *sip, char *hvalue);
int      msg_setrecord_route(sip_t *sip, char *hvalue);
int      msg_getrecord_route(sip_t *sip, int pos, record_route_t **rr);
int      msg_setroute       (sip_t *sip, char *hvalue);
int      msg_getroute       (sip_t *sip, int pos, route_t **route);
int      msg_setto          (sip_t *sip, char *hvalue);
to_t*    msg_getto          (sip_t *sip);
int      msg_setvia         (sip_t *sip, char *hvalue);
int      msg_getvia         (sip_t *sip,int pos, via_t **dest);
int      msg_setwww_authenticate(sip_t *sip, char *hvalue);
www_authenticate_t *msg_getwww_authenticate(sip_t *sip);


int      msg_setbody        (sip_t *sip, char *buf);
int      msg_setbody_mime   (sip_t *sip, char *buf);
int      msg_getbody        (sip_t *sip, int pos, body_t **dest);


/* a few MACROS to ease readability of code */
#define MSG_IS_RESPONSE(resp)     ((resp)->strtline->statuscode!=NULL)
#define MSG_IS_REQUEST(req)       ((req)->strtline->statuscode==NULL)

#define MSG_IS_INVITE(msg)  (0==strncmp((msg)->strtline->sipmethod,"INVITE",6))
#define MSG_IS_ACK(msg)     (0==strncmp((msg)->strtline->sipmethod,"ACK",3))
#define MSG_IS_BYE(msg)     (0==strncmp((msg)->strtline->sipmethod,"BYE",3))
#define MSG_IS_REGISTER(msg) (0==strncmp((msg)->strtline->sipmethod,"REGISTER",8))
#define MSG_IS_CANCEL(msg)  (0==strncmp((msg)->strtline->sipmethod,"CANCEL",6))
#define MSG_IS_OPTIONS(msg) (0==strncmp((msg)->strtline->sipmethod,"OPTIONS",7))
#define MSG_IS_INFO(msg)    (0==strncmp((msg)->strtline->sipmethod,"INFO",4))
#define MSG_IS_PRACK(msg)   (0==strncmp((msg)->strtline->sipmethod,"PRACK",5))

#define MSG_IS_STATUS_1XX(msg)    (0==strncmp((msg)->strtline->statuscode,"1",1))
#define MSG_IS_STATUS_2XX(msg)    (0==strncmp((msg)->strtline->statuscode,"2",1))
#define MSG_IS_STATUS_3XX(msg)    (0==strncmp((msg)->strtline->statuscode,"3",1))
#define MSG_IS_STATUS_4XX(msg)    (0==strncmp((msg)->strtline->statuscode,"4",1))
#define MSG_IS_STATUS_5XX(msg)    (0==strncmp((msg)->strtline->statuscode,"5",1))
#define MSG_IS_STATUS_6XX(msg)    (0==strncmp((msg)->strtline->statuscode,"6",1))
#define MSG_TEST_CODE(resp, code) ((resp)->strtline->statuscode!=NULL \
				   && code==(int)satoi((resp)->strtline->statuscode))
#define MSG_IS_RESPONSEFOR(resp,requestname) \
                                  (0==strcmp((resp)->cseq->method,requestname))

/* special API to manage body */
int     body_init(body_t **body);
void    body_free(body_t *body);
int     body_parse(body_t *body, char *buf);
int     body_parse_mime(body_t *body, char *start_of_body);
int     body_2char(body_t *body, char **dest);

/* Accessor API for generic params */
#define generic_param_init(GP) url_param_init(GP)
#define generic_param_free(GP) url_param_free(GP)
#define generic_param_set(GP)  url_param_set(GP)
#define generic_param_freelist(L)       url_param_freelist(L)
#define generic_param_add(L,N,V)        url_param_add(L,N,V)
#define generic_param_getbyname(L,N,GP) url_param_getbyname(L,N,GP)
#define generic_param_clone(GP,DEST)     url_param_clone(GP,DEST)

void  generic_param_setname (generic_param_t *fparam, char *name);
char *generic_param_getname (generic_param_t *fparam);
void  generic_param_setvalue(generic_param_t *fparam, char *value);
char *generic_param_getvalue(generic_param_t *fparam);



/* Accessor API for headers */
int     header_init    (header_t **header);
void    header_free    (header_t *header);
int     header_2char   (header_t *header, char **dest);
char   *header_getname (header_t *header);
void    header_setname (header_t *header, char *pname);
char   *header_getvalue(header_t *header);
void    header_setvalue(header_t *header, char *pvalue);
int     header_clone   (header_t *header, header_t **dest);

#define accept_init(A)     content_type_init(A)
#define accept_free(A)     content_type_free(A)
#define accept_parse(A, H) content_type_parse(A, H)
#define accept_2char(A, D) content_type_2char(A, D)
#define accept_clone(A, D) content_type_clone(A, D)

#define accept_param_get(C,I,GP) generic_param_get((C)->gen_params, I, GP)
#define accept_param_add(C,N,V)  generic_param_add((C)->gen_params,N,V)
#define accept_param_getbyname(C,N,GP) generic_param_getbyname((C)->gen_params,N,GP)

int     accept_encoding_init(accept_encoding_t **dest);
int     accept_encoding_parse(accept_encoding_t *wwwa, char *hvalue);
int     accept_encoding_2char(accept_encoding_t *wwwa, char **dest);
void    accept_encoding_free(accept_encoding_t *accept_encoding);
int     accept_encoding_clone(accept_encoding_t *w,accept_encoding_t **dest);

char   *accept_encoding_getelement(accept_encoding_t *ae);
void    accept_encoding_setelement(accept_encoding_t *ae, char *element);
#define accept_encoding_param_get(A,I,GP) generic_param_get((A)->gen_params, I, GP)
#define accept_encoding_param_add(A,N,V)  generic_param_add((A)->gen_params,N,V)
#define accept_encoding_param_getbyname(A,N,GP) generic_param_getbyname((A)->gen_params,N,GP)

#define accept_language_init(A)      accept_encoding_init(A)
#define accept_language_parse(A, H)  accept_encoding_parse(A, H) 
#define accept_language_2char(A, D)  accept_encoding_2char(A, D) 
#define accept_language_free(A)      accept_encoding_free(A) 
#define accept_language_clone(A, DE) accept_encoding_clone(A, DE) 

#define accept_language_getelement(A)     accept_encoding_getelement(A) 
#define accept_language_setelement(A, E)  accept_encoding_setelement(A, E)
#define accept_language_param_get(A,I,GP) generic_param_get((A)->gen_params, I, GP)
#define accept_language_param_add(A,N,V)  generic_param_add((A)->gen_params,N,V)
#define accept_language_param_getbyname(A,N,GP) generic_param_getbyname((A)->gen_params,N,GP)

#define  alert_info_init(A)      call_info_init(A)
#define  alert_info_free(A)      call_info_free(A)
#define  alert_info_parse(A, H)  call_info_parse(A, H)
#define  alert_info_2char(A,D)   call_info_2char(A,D)
#define  alert_info_clone(A, D)  call_info_clone(A, D)

#define  alert_info_geturi(A)    call_info_geturi(A)
#define  alert_info_seturi(A, U) call_info_seturi(A, U)

#define allow_init(C)      content_length_init(C)
#define allow_parse(C, S)  content_length_parse(C, S)
#define allow_2char(C, D)  content_length_2char(C, D)
#define allow_free(C)      content_length_free(C)
#define allow_clone(C)     content_length_clone(C)

int     authorization_init(authorization_t **dest);
int     authorization_parse(authorization_t *wwwa, char *hvalue);
int     authorization_2char(authorization_t *wwwa, char **dest);
void    authorization_free(authorization_t *authorization);
int     authorization_clone(authorization_t *w,authorization_t **dest);

char *authorization_getauth_type(authorization_t *auth);
void authorization_setauth_type(authorization_t *auth, char *auth_type);
char *authorization_getusername(authorization_t *auth);
void authorization_setusername(authorization_t *auth, char *username);
char *authorization_getrealm(authorization_t *auth);
void authorization_setrealm(authorization_t *auth, char *realm);
char *authorization_getnonce(authorization_t *auth);
void authorization_setnonce(authorization_t *auth, char *nonce);
char *authorization_geturi(authorization_t *auth);
void authorization_seturi(authorization_t *auth, char *uri);
char *authorization_getresponse(authorization_t *auth);
void authorization_setresponse(authorization_t *auth, char *response);
char *authorization_getdigest(authorization_t *auth);
void authorization_setdigest(authorization_t *auth, char *digest);
char *authorization_getalgorithm(authorization_t *auth);
void authorization_setalgorithm(authorization_t *auth, char *algorithm);
char *authorization_getcnonce(authorization_t *auth);
void authorization_setcnonce(authorization_t *auth, char *cnonce);
char *authorization_getopaque(authorization_t *auth);
void authorization_setopaque(authorization_t *auth, char *opaque);
char *authorization_getmessage_qop(authorization_t *auth);
void authorization_setmessage_qop(authorization_t *auth, char *message_qop);
char *authorization_getnonce_count(authorization_t *auth);
void authorization_setnonce_count(authorization_t *auth, char *nonce_count);


int     call_id_init(call_id_t **callid);
void    call_id_free(call_id_t *callid);
int     call_id_parse(call_id_t *callid, char *hvalue);
int     call_id_2char(call_id_t *callid, char **dest);
char*   call_id_getnumber(call_id_t *callid);
void    call_id_setnumber(call_id_t *callid, char *number);
char*   call_id_gethost(call_id_t *callid);
void    call_id_sethost(call_id_t *callid, char *host);
int     call_id_clone(call_id_t *callid, call_id_t **dest);

int     call_info_init(call_info_t **call_info);
void    call_info_free(call_info_t *call_info);
int     call_info_parse(call_info_t *call_info, char *hvalue);
int     call_info_2char(call_info_t *call_info, char **dest);
int     call_info_clone(call_info_t *call_info, call_info_t **dest);

char*   call_info_geturi(call_info_t *call_info);
void    call_info_seturi(call_info_t *call_info, char *uri);

int     contact_init(contact_t **contact);
void    contact_free(contact_t *contact);
int     contact_parse(contact_t *contact, char *hvalue);
int     contact_2char(contact_t *contact, char **dest);
int     contact_clone(contact_t *contact, contact_t **dest);
#define contact_getdisplayname(C) from_getdisplayname((from_t*)C)
#define contact_setdisplayname(C,S) from_setdisplayname((from_t*)C, S)
#define contact_geturl(C)         from_geturl((from_t*)C)
#define contact_seturl(C,U)       from_seturl((from_t*)C,U)
#define contact_param_get(C,I,GP) from_param_get((from_t*)C,I,GP)
#define contact_param_add(F,GP)   generic_param_add((F)->gen_params, N,V)
#define contact_param_getbyname(C,N,GP) generic_param_getbyname((C)->gen_params,N,GP)

#define content_disposition_init(A)      call_info_init(A)
#define content_disposition_free(A)      call_info_free(A)
int     content_disposition_parse(content_disposition_t *cd, char *hvalue);
#define content_disposition_2char(A,D)   call_info_2char(A,D)
#define content_disposition_clone(A, D)  call_info_clone(A, D)

/* type is of: "render" | "session" | "icon" | "alert" */
#define content_disposition_gettype(A)    call_info_geturi(A)
#define content_disposition_settype(A, U) call_info_seturi(A, U)

#define content_encoding_init(C)      content_length_init(C)
#define content_encoding_parse(C, S)  content_length_parse(C, S)
#define content_encoding_2char(C, D)  content_length_2char(C, D)
#define content_encoding_free(C)      content_length_free(C)
#define content_encoding_clone(C)     content_length_clone(C)

int     content_length_init (content_length_t **cl);
void    content_length_free (content_length_t *content_length);
int     content_length_parse(content_length_t *contentlength, char *hvalue);
int     content_length_2char(content_length_t *content_length, char **dest);
int     content_length_clone(content_length_t *cl, content_length_t **dest);

int     content_type_init (content_type_t **content_type);
void    content_type_free (content_type_t *content_type);
int     content_type_parse(content_type_t *content_type, char *hvalue);
int     content_type_2char(content_type_t *content_type, char **dest);
int     content_type_clone(content_type_t *ct, content_type_t **dest);

#define content_type_param_get(C,I,GP) generic_param_get((C)->gen_params, I, GP)
#define content_type_param_add(C,N,V)  generic_param_add((C)->gen_params,N,V)
#define content_type_param_getbyname(C,N,GP) generic_param_getbyname((C)->gen_params,N,GP)

int     cseq_init(cseq_t **cseq);
void    cseq_free     (cseq_t *cseq );
int     cseq_parse    (cseq_t *cseq, char *hvalue);
int     cseq_2char    (cseq_t *header, char **dest);
char*   cseq_getnumber(cseq_t *cseq);
char*   cseq_getmethod(cseq_t *cseq);
void    cseq_setnumber(cseq_t *cseq, char *number);
void    cseq_setmethod(cseq_t *cseq, char *method);
int     cseq_clone(cseq_t *cseq, cseq_t **dest);

#define  error_info_init(A)      call_info_init(A)
#define  error_info_free(A)      call_info_free(A)
#define  error_info_parse(A, H)  call_info_parse(A, H)
#define  error_info_2char(A,D)   call_info_2char(A,D)
#define  error_info_clone(A, D)  call_info_clone(A, D)

#define  error_info_geturi(A)    call_info_geturi(A)
#define  error_info_seturi(A, U) call_info_seturi(A, U)

int     from_init(from_t **from);
void    from_free(from_t *from);
int     from_parse(from_t *from, char *string);
int     from_2char(from_t *from, char **dest);
char   *from_getdisplayname(from_t *from);
void    from_setdisplayname(from_t *from, char *displayname);
url_t  *from_geturl(from_t *from);
void    from_seturl(from_t *from, url_t *url);
int     from_param_get(from_t *from, int pos, generic_param_t **fparam);
#define from_param_add(F,N,V)      generic_param_add((F)->gen_params,N,V)
#define from_param_getbyname(F,N,GP) generic_param_getbyname((F)->gen_params,N,GP)
int     from_clone(from_t *from, from_t **dest);

#define from_get_tag(F,GP)    generic_param_getbyname((F)->gen_params, "tag",GP)
#define from_set_tag(T,V)     generic_param_add((T)->gen_params, sgetcopy("tag"),V)
/* Compare the username, host and tag part (if exist) of the two froms */
int     from_compare(from_t *from1, from_t *from2);

#define mime_version_init(C)      content_length_init(C)
#define mime_version_parse(C, S)  content_length_parse(C, S)
#define mime_version_2char(C, D)  content_length_2char(C, D)
#define mime_version_free(C)      content_length_free(C)
#define mime_version_clone(C)     content_length_clone(C)

#define proxy_authenticate_init(W)     www_authenticate_init(W) 
#define proxy_authenticate_parse(W, H) www_authenticate_parse(W, H)
#define proxy_authenticate_2char(W, D) www_authenticate_2char(W, D)
#define proxy_authenticate_free(W)     www_authenticate_free(W)
#define proxy_authenticate_clone(W, D) www_authenticate_clone(W, D)

#define proxy_authenticate_getauth_type(W)   www_authenticate_getauth_type(W)
#define proxy_authenticate_setauth_type(W,A) www_authenticate_setauth_type(W, A)
#define proxy_authenticate_getrealm(W)     www_authenticate_getrealm(W)
#define proxy_authenticate_setrealm(W, R)  www_authenticate_setrealm(W, R)
#define proxy_authenticate_getdomain(W)    www_authenticate_getdomain(W)
#define proxy_authenticate_setdomain(W, D) www_authenticate_setdomain(W, D)
#define proxy_authenticate_getnonce(W)     www_authenticate_getnonce(W)
#define proxy_authenticate_setnonce(W, N)  www_authenticate_setnonce(W, N)
#define proxy_authenticate_getopaque(W)    www_authenticate_getopaque(W)
#define proxy_authenticate_setopaque(W, O) www_authenticate_setopaque(W, O)
#define proxy_authenticate_getstale(W)     www_authenticate_getstale(W)
#define proxy_authenticate_setstale(W, S)  www_authenticate_setstale(W, S)
#define proxy_authenticate_setstale_true(W) www_authenticate_setstale(W,sgetcopy("true"))
#define proxy_authenticate_setstale_false(W) www_authenticate_setstale(W,sgetcopy("false"))
#define proxy_authenticate_getalgorithm(W) www_authenticate_getalgorithm(W)
#define proxy_authenticate_setalgorithm(W, A) www_authenticate_setalgorithm(W, A)
#define proxy_authenticate_setalgorithm_MD5(W) www_authenticate_setalgorithm(W,sgetcopy("MD5"))
#define proxy_authenticate_getqop_options(A) www_authenticate_getqop_options(A)
#define proxy_authenticate_setqop_options(A,Q) www_authenticate_setqop_options(A,Q)


#define proxy_authorization_init(A)     authorization_init(A) 
#define proxy_authorization_parse(A, H) authorization_parse(A, H)
#define proxy_authorization_2char(A, D) authorization_2char(A, D)
#define proxy_authorization_free(A)     authorization_free(A)
#define proxy_authorization_clone(A, D) authorization_clone(A, D)

#define proxy_authorization_getauth_type(A)    authorization_getauth_type(A)
#define proxy_authorization_setauth_type(A, U) authorization_setauth_type(A, U)
#define proxy_authorization_getusername(A)    authorization_getusername(A)
#define proxy_authorization_setusername(A, U) authorization_setusername(A, U)
#define proxy_authorization_getrealm(A)       authorization_getrealm(A)
#define proxy_authorization_setrealm(A, R)    authorization_setrealm(A, R)
#define proxy_authorization_getnonce(A)       authorization_getnonce(A)
#define proxy_authorization_setnonce(A, N)    authorization_setnonce(A, N)
#define proxy_authorization_geturi(A)         authorization_geturi(A)
#define proxy_authorization_seturi(A, U)      authorization_seturi(A, U)
#define proxy_authorization_getresponse(A)    authorization_getresponse(A)
#define proxy_authorization_setresponse(A, R) authorization_setresponse(A, R)
#define proxy_authorization_getdigest(A)      authorization_getdigest(A)
#define proxy_authorization_setdigest(A, D)   authorization_setdigest(A, D)
#define proxy_authorization_getalgorithm(A)   authorization_getalgorithm(A)
#define proxy_authorization_setalgorithm(A,L) authorization_setalgorithm(A,L)
#define proxy_authorization_getcnonce(A)      authorization_getcnonce(A)
#define proxy_authorization_setcnonce(A, O)   authorization_setcnonce(A, O)
#define proxy_authorization_getopaque(A)      authorization_getopaque(A)
#define proxy_authorization_setopaque(A, O)   authorization_setopaque(A, O)
#define proxy_authorization_getmessage_qop(A) authorization_getmessage_qop(A)
#define proxy_authorization_setmessage_qop(A, O) authorization_setmessage_qop(A, O)
#define proxy_authorization_getnonce_count(A) authorization_getnonce_count(A)
#define proxy_authorization_setnonce_count(A, O) authorization_setnonce_count(A, O)


int     record_route_init(record_route_t **rr);
void    record_route_free(record_route_t *rr);
int     record_route_parse(record_route_t *rr, char *hvalue);
int     record_route_2char(record_route_t *rr, char **dest);
#define record_route_seturl(R,U)        from_seturl((from_t*)R,U)
#define record_route_geturl(R)          from_geturl((from_t*)R)
#define record_route_param_get(R,I,GP)  from_param_get((from_t*)R, I, GP)
#define record_route_param_add(F,N,V)    generic_param_add((F)->gen_params,N,V)
#define record_route_param_getbyname(R,N,GP) generic_param_getbyname((R)->gen_params,N,GP)
#define record_route_clone(R,D)         from_clone(R,D)

#ifdef __VXWORKS_OS__
int     route_init2(route_t **rr);
#else
int     route_init(route_t **rr);
#endif
void    route_free(route_t *rr);
int     route_parse(route_t *rr, char *hvalue);
int     route_2char(route_t *rr, char **dest);
#define route_seturl(R,U)      from_seturl((from_t*)R,U)
#define route_geturl(R)        from_geturl((from_t*)R)
#define route_param_get(R,I,GP) from_param_get((from_t*)R, I, GP)
#define route_param_add(F,N,V)   generic_param_add((F)->gen_params,N,V)
#define route_param_getbyname(R,N,GP) generic_param_getbyname((R)->gen_params,N,GP)
#define route_clone(R,D)       from_clone(R,D)

int     to_init(to_t **to);
void    to_free(to_t *to);
int     to_parse(to_t *to, char *hvalue);
int     to_2char(to_t *to, char **dest);
int     to_clone(to_t *to, to_t **dest);
#define to_setdisplayname(T,S) from_setdisplayname((from_t*)T,S)
#define to_getdisplayname(T) from_getdisplayname((from_t*)T)
#define to_seturl(T,U)       from_seturl((from_t*)T,U)
#define to_geturl(T)         from_geturl((from_t*)T)
#define to_param_get(T,I,GP) from_param_get((from_t*)T, I, GP)
#define to_param_getbyname(T,N,GP) generic_param_getbyname((T)->gen_params,N,GP)
#define to_param_add(T,N,V) generic_param_add((T)->gen_params,N,V)

#define to_set_tag(T,V)     generic_param_add((T)->gen_params, "tag",V)
#define to_get_tag(T,GP)    generic_param_getbyname((T)->gen_params, sgetcopy("tag"),GP)

/* Compare the username, host and tag part of the two froms */
#define to_compare(T1, T2) from_compare((from_t *)T1, (from_t *)T2)


int     via_init(via_t **via);
void    via_free(via_t *via);
int     via_parse (via_t *via, char *hvalue);
int     via_2char(via_t *head, char **dest);
void    via_setversion(via_t *via, char *version);
char   *via_getversion(via_t *via);
void    via_setprotocol(via_t *via, char *protocol);
char   *via_getprotocol(via_t *via);
void    via_sethost(via_t *via, char *host);
char   *via_gethost(via_t *via);
void    via_setport(via_t *via, char *port);
char   *via_getport(via_t *via);
void    via_setcomment(via_t *via, char *comment);
char   *via_getcomment(via_t *via);
int     via_clone(via_t *via, via_t **dest);

#define via_set_hidden(V)    generic_param_add((V)->via_params,sgetcopy("hidden"),NULL)
#define via_set_ttl(V,TTL)   generic_param_add((V)->via_params,sgetcopy("ttl"),TTL)
#define via_set_maddr(V,M)   generic_param_add((V)->via_params,sgetcopy("maddr"),M)
#define via_set_received(V,R) generic_param_add((V)->via_params,sgetcopy("received"),R)
#define via_set_branch(V,B)  generic_param_add((V)->via_params,sgetcopy("branch"),B)

#define via_param_get(V,I,GP)       generic_param_get(V, I, GP)
#define via_param_add(VIA,N,V)      generic_param_add((V)->via_params,N,V)
#define via_param_getbyname(V,N,GP) generic_param_getbyname((V)->via_params,N,GP)


int www_authenticate_init(www_authenticate_t **dest);
int www_authenticate_parse(www_authenticate_t *wwwa, char *hvalue);
int www_authenticate_2char(www_authenticate_t *wwwa, char **dest);
void www_authenticate_free(www_authenticate_t *www_authenticate);
int www_authenticate_clone(www_authenticate_t *wwwa,www_authenticate_t **dest);

char *www_authenticate_getauth_type(www_authenticate_t *wwwa);
void www_authenticate_setauth_type(www_authenticate_t *wwwa, char *auth_type);
char *www_authenticate_getrealm(www_authenticate_t *wwwa);
void www_authenticate_setrealm(www_authenticate_t *wwwa, char *realm);
char *www_authenticate_getdomain(www_authenticate_t *wwwa);
void www_authenticate_setdomain(www_authenticate_t *wwwa, char *domain);
char *www_authenticate_getnonce(www_authenticate_t *wwwa);
void www_authenticate_setnonce(www_authenticate_t *wwwa, char *nonce);
char *www_authenticate_getopaque(www_authenticate_t *wwwa);
void www_authenticate_setopaque(www_authenticate_t *wwwa, char *opaque);
char *www_authenticate_getstale(www_authenticate_t *wwwa);
void www_authenticate_setstale(www_authenticate_t *wwwa, char *stale);
#define www_authenticate_setstale_true(W) www_authenticate_setstale(W,sgetcopy("true"))
#define www_authenticate_setstale_false(W) www_authenticate_setstale(W,sgetcopy("false"))
char *www_authenticate_getalgorithm(www_authenticate_t *wwwa);
void www_authenticate_setalgorithm(www_authenticate_t *wwwa, char *algorithm);
#define www_authenticate_setalgorithm_MD5(W) www_authenticate_setalgorithm(W,sgetcopy("MD5"))
char *www_authenticate_getqop_options(www_authenticate_t *wwwa);
void www_authenticate_setqop_options(www_authenticate_t *wwwa, char *qop_options);


/* trace facilities */
#ifdef ENABLE_TRACE
void    msg_logrequest(sip_t *sip,char *fmt);
void    msg_logresponse(sip_t *sip,char *fmt);
#else
#define msg_logrequest(P,Q) ;
#define msg_logresponse(P,Q) ;
#endif



#define msg_setdate(P,R)            msg_setheader((sip_t *)P,(char *)"date",R)
#define msg_getdate(P,Q,R)            msg_getnheader(( sip_t *)P,"date",Q,(header_t *)R)
#define msg_setencryption(P,R)      msg_setheader((sip_t *)P,(char *)"encryption",R)
#define msg_getencryption(P,Q,R)      msg_getnheader(( sip_t *)P,"encryption",Q,(header_t *)R)
#define msg_setorganization(P,R)    msg_setheader((sip_t *)P,(char *)"organization",R)
#define msg_getorganization(P,Q,R)    msg_getnheader(( sip_t *)P,"organization",Q,(header_t *)R)
#define msg_setrequire(P,R)         msg_setheader((sip_t *)P,(char *)"require",R)
#define msg_getrequire(P,Q,R)         msg_getnheader(( sip_t *)P,"require",Q,(header_t *)R)
#define msg_setsupported(P,R)       msg_setheader((sip_t *)P,(char *)"supported",R)
#define msg_getsupported(P,Q,R)       msg_getnheader(( sip_t *)P,"supported",Q,(header_t *)R)
#define msg_settimestamp(P,R)       msg_setheader((sip_t *)P,(char *)"timestamp",R)
#define msg_gettimestamp(P,Q,R)       msg_getnheader(( sip_t *)P,"timestamp",Q,(header_t *)R)
#define msg_setuser_agent(P,R)      msg_setheader((sip_t *)P,(char *)"user-agent",R)
#define msg_getuser_agent(P,Q,R)      msg_getnheader(( sip_t *)P,"user-agent",Q,(header_t *)R)
#define msg_setcontent_language(P,R) msg_setheader((sip_t *)P,(char *)"content-language",R)
#define msg_getcontent_language(P,Q,R) msg_getnheader(( sip_t *)P,"content-language",Q,(header_t *)R)
#define msg_setexpires(P,R)         msg_setheader((sip_t *)P,(char *)"expires",R)
#define msg_getexpires(P,Q,R)         msg_getnheader(( sip_t *)P,"expires",Q,(header_t *)R)
#define msg_setin_reply_to(P,R)     msg_setheader((sip_t *)P,(char *)"in-reply-to",R)
#define msg_getin_reply_to(P,Q,R)     msg_getnheader(( sip_t *)P,"in-reply-to",Q,(header_t *)R)
#define msg_setmax_forward(P,R)     msg_setheader((sip_t *)P,(char *)"max-forward",R)
#define msg_getmax_forward(P,Q,R)     msg_getnheader(( sip_t *)P,"max-forward",Q,(header_t *)R)
#define msg_setpriority(P,R)        msg_setheader((sip_t *)P,(char *)"priority",R)
#define msg_getpriority(P,Q,R)        msg_getnheader(( sip_t *)P,"priority",Q,(header_t *)R)
#define msg_setproxy_require(P,R)   msg_setheader((sip_t *)P,(char *)"proxy-require",R)
#define msg_getproxy_require(P,Q,R)   msg_getnheader(( sip_t *)P,"proxy-require",Q,(header_t *)R)
#define msg_setresponse_key(P,R)    msg_setheader((sip_t *)P,(char *)"response-key",R)
#define msg_getresponse_key(P,Q,R)    msg_getnheader(( sip_t *)P,"response-key",Q,(header_t *)R)
#define msg_setsubject(P,R)         msg_setheader((sip_t *)P,(char *)"subject",R)
#define msg_getsubject(P,Q,R)         msg_getnheader(( sip_t *)P,"subject",Q,(header_t *)R)
#define msg_setretry_after(P,R)     msg_setheader((sip_t *)P,(char *)"retry-after",R)
#define msg_getretry_after(P,Q,R)     msg_getnheader(( sip_t *)P,"retry-after",Q,(header_t *)R)
#define msg_setserver(P,R)          msg_setheader((sip_t *)P,(char *)"server",R)
#define msg_getserver(P,Q,R)          msg_getnheader(( sip_t *)P,"server",Q,(header_t *)R)
#define msg_setunsupported(P,R)     msg_setheader((sip_t *)P,(char *)"unsupported",R)
#define msg_getunsupported(P,Q,R)     msg_getnheader(( sip_t *)P,"unsupported",Q,(header_t *)R)
#define msg_setwarning(P,R)         msg_setheader((sip_t *)P,(char *)"warning",R)
#define msg_getwarning(P,Q,R)         msg_getnheader(( sip_t *)P,"warning",Q,(header_t *)R)


#endif


