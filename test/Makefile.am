
EXTRA_DIST = 

noinst_PROGRAMS = torture_test torture_sdp_test turl tfrom tto tcontact tvia tcallid tcontentt trecordr troute twwwa
INCLUDES = -I$(top_srcdir)
AM_CFLAGS = $(SIP_CFLAGS) $(SIP_PARSER_FLAGS) $(SIP_EXTRA_FLAGS)

twwwa_SOURCES =  twwwa.c
twwwa_LDADD = $(PARSER_LIB) $(EXTRA_LIB) \
$(top_builddir)/parser/port_list.o   $(top_builddir)/parser/urls.o \
$(top_builddir)/parser/port_malloc.o $(top_builddir)/parser/port_misc.o \
$(top_builddir)/parser/hdr_wwwauthenticate.o

tcontentt_SOURCES =  tcontentt.c
tcontentt_LDADD = $(PARSER_LIB) $(EXTRA_LIB) \
$(top_builddir)/parser/port_list.o   $(top_builddir)/parser/urls.o \
$(top_builddir)/parser/port_malloc.o $(top_builddir)/parser/port_misc.o \
$(top_builddir)/parser/hdr_contenttype.o $(top_builddir)/parser/hdr_from.o

turl_SOURCES =  turls.c
turl_LDADD = $(PARSER_LIB) $(EXTRA_LIB) \
$(top_builddir)/parser/port_list.o     $(top_builddir)/parser/urls.o    \
$(top_builddir)/parser/port_malloc.o   $(top_builddir)/parser/port_misc.o \
$(top_builddir)/parser/hdr_from.o

tfrom_SOURCES =  tfrom.c
tfrom_LDADD =  $(PARSER_LIB) $(EXTRA_LIB) \
$(top_builddir)/parser/port_list.o     $(top_builddir)/parser/urls.o    \
$(top_builddir)/parser/port_malloc.o   $(top_builddir)/parser/port_misc.o  \
$(top_builddir)/parser/hdr_headers.o   $(top_builddir)/parser/hdr_from.o

tto_SOURCES =  tto.c
tto_LDADD = $(PARSER_LIB) $(EXTRA_LIB)  \
$(top_builddir)/parser/port_list.o     $(top_builddir)/parser/urls.o    \
$(top_builddir)/parser/port_malloc.o   $(top_builddir)/parser/port_misc.o  \
$(top_builddir)/parser/hdr_headers.o   $(top_builddir)/parser/hdr_to.o   \
$(top_builddir)/parser/hdr_from.o


tcontact_SOURCES =  tcontact.c
tcontact_LDADD =  $(PARSER_LIB) $(EXTRA_LIB)  \
$(top_builddir)/parser/port_list.o     $(top_builddir)/parser/urls.o    \
$(top_builddir)/parser/port_malloc.o   $(top_builddir)/parser/port_misc.o  \
$(top_builddir)/parser/hdr_headers.o   $(top_builddir)/parser/hdr_from.o \
$(top_builddir)/parser/hdr_to.o        $(top_builddir)/parser/hdr_contact.o

trecordr_SOURCES =  trecordr.c
trecordr_LDADD =  $(PARSER_LIB) $(EXTRA_LIB)  \
$(top_builddir)/parser/port_list.o     $(top_builddir)/parser/urls.o    \
$(top_builddir)/parser/port_malloc.o   $(top_builddir)/parser/port_misc.o  \
$(top_builddir)/parser/hdr_headers.o   $(top_builddir)/parser/hdr_from.o \
$(top_builddir)/parser/hdr_to.o        $(top_builddir)/parser/hdr_recordroute.o

troute_SOURCES =  troute.c
troute_LDADD =  $(PARSER_LIB) $(EXTRA_LIB) \
$(top_builddir)/parser/port_list.o     $(top_builddir)/parser/urls.o    \
$(top_builddir)/parser/port_malloc.o   $(top_builddir)/parser/port_misc.o  \
$(top_builddir)/parser/hdr_headers.o   $(top_builddir)/parser/hdr_from.o \
$(top_builddir)/parser/hdr_to.o        $(top_builddir)/parser/hdr_route.o \
$(top_builddir)/parser/hdr_recordroute.o

tvia_SOURCES =  tvia.c
tvia_LDADD = $(PARSER_LIB) $(EXTRA_LIB) \
$(top_builddir)/parser/port_list.o     $(top_builddir)/parser/urls.o    \
$(top_builddir)/parser/port_malloc.o   $(top_builddir)/parser/port_misc.o  \
$(top_builddir)/parser/hdr_headers.o   $(top_builddir)/parser/hdr_via.o \
$(top_builddir)/parser/hdr_from.o

tcallid_SOURCES =  tcallid.c
tcallid_LDADD = $(PARSER_LIB) $(EXTRA_LIB) \
$(top_builddir)/parser/port_list.o     $(top_builddir)/parser/urls.o    \
$(top_builddir)/parser/port_malloc.o   $(top_builddir)/parser/port_misc.o  \
$(top_builddir)/parser/hdr_callid.o    $(top_builddir)/parser/hdr_from.o


torture_sdp_test_SOURCES =  torture_sdp.c
torture_sdp_test_LDADD = $(PARSER_LIB) $(EXTRA_LIB) \
$(top_builddir)/parser/port_list.o   \
$(top_builddir)/parser/port_malloc.o   $(top_builddir)/parser/port_misc.o  \
$(top_builddir)/parser/sdp_rfc2327.o   $(top_builddir)/parser/sdp_accessor.o \
$(top_builddir)/fsm/sdp_negoc.o

torture_test_SOURCES =  torture.c
torture_test_LDADD = $(PARSER_LIB) $(EXTRA_LIB) \
$(top_builddir)/parser/port_list.o   \
$(top_builddir)/parser/port_malloc.o   $(top_builddir)/parser/port_misc.o  \
$(top_builddir)/parser/hdr_body.o      $(top_builddir)/parser/hdr_callid.o   \
$(top_builddir)/parser/hdr_contact.o   $(top_builddir)/parser/hdr_contentlength.o   \
$(top_builddir)/parser/hdr_from.o      $(top_builddir)/parser/hdr_headers.o   \
$(top_builddir)/parser/hdr_to.o        $(top_builddir)/parser/hdr_via.o  \
$(top_builddir)/parser/hdr_cseq.o      $(top_builddir)/parser/msg_cparser.o   \
$(top_builddir)/parser/msg_parser.o    $(top_builddir)/parser/msg_read.o   \
$(top_builddir)/parser/msg_write.o     $(top_builddir)/parser/urls.o    \
$(top_builddir)/parser/sdp_rfc2327.o   \
$(top_builddir)/parser/hdr_contenttype.o $(top_builddir)/parser/hdr_route.o \
$(top_builddir)/parser/hdr_mimeversion.o $(top_builddir)/parser/hdr_recordroute.o \
$(top_builddir)/parser/hdr_wwwauthenticate.o $(top_builddir)/parser/hdr_authorization.o \
$(top_builddir)/parser/hdr_proxyauthorization.o  $(top_builddir)/parser/hdr_proxyauthenticate.o \
$(top_builddir)/parser/hdr_accept.o $(top_builddir)/parser/hdr_acceptencoding.o \
$(top_builddir)/parser/hdr_acceptlanguage.o $(top_builddir)/parser/hdr_callinfo.o \
$(top_builddir)/parser/hdr_alertinfo.o $(top_builddir)/parser/hdr_errorinfo.o \
$(top_builddir)/parser/hdr_allow.o $(top_builddir)/parser/hdr_contentencoding.o \
$(top_builddir)/parser/hdr_contentdisposition.o $(top_builddir)/parser/sdp_accessor.o

