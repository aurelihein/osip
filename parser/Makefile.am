
EXTRA_DIST = msg.h

lib_LTLIBRARIES = libosip.la

libosip_la_SOURCES =  \
hdr_mimeversion.c    hdr_recordroute.c        hdr_route.c \
hdr_contentlength.c  hdr_contenttype.c        hdr_cseq.c \
hdr_body.c     hdr_via.c      hdr_callid.c    hdr_contact.c \
hdr_from.c     hdr_headers.c  hdr_to.c        hdr_wwwauthenticate.c \
hdr_authorization.c hdr_proxyauthorization.c  hdr_proxyauthenticate.c \
hdr_accept.c   hdr_acceptencoding.c hdr_acceptlanguage.c  msg_cparser.c \
hdr_callinfo.c hdr_alertinfo.c hdr_errorinfo.c hdr_allow.c \
hdr_contentencoding.c hdr_contentdisposition.c \
msg_parser.c   msg_read.c     msg_write.c     port_misc.c \
port_list.c    port_malloc.c  urls.c \
md5c.c         sdp_rfc2327.c  sdp_accessor.c


libosip_la_LDFLAGS = -version-info $(LIBOSIP_SO_VERSION) \
$(PARSER_LIB) $(EXTRA_LIB)


INCLUDES = -I$(top_srcdir)
AM_CFLAGS = $(SIP_CFLAGS) $(SIP_PARSER_FLAGS) $(SIP_EXTRA_FLAGS)


