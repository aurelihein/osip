
SUBDIRS = res

EXTRA_DIST = tst tst_sdp README

noinst_PROGRAMS = torture_test torture_sdp_test turl tfrom tto tcontact tvia tcallid tcontentt trecordr troute twwwa
INCLUDES = -I$(top_srcdir)/include
AM_CFLAGS = $(SIP_CFLAGS) $(SIP_PARSER_FLAGS) $(SIP_EXTRA_FLAGS)

twwwa_SOURCES =  twwwa.c
twwwa_LDADD = $(PARSER_LIB) $(EXTRA_LIB)      \
$(top_builddir)/src/osipparser2/osip_list.o   \
$(top_builddir)/src/osipparser2/osip_uri.o        \
$(top_builddir)/src/osipparser2/osip_port.o   \
$(top_builddir)/src/osipparser2/osip_www_authenticate.o

tcontentt_SOURCES =  tcontentt.c
tcontentt_LDADD = $(PARSER_LIB) $(EXTRA_LIB)        \
$(top_builddir)/src/osipparser2/osip_list.o         \
$(top_builddir)/src/osipparser2/osip_uri.o          \
$(top_builddir)/src/osipparser2/osip_port.o         \
$(top_builddir)/src/osipparser2/osip_content_type.o \
$(top_builddir)/src/osipparser2/osip_from.o

turl_SOURCES =  turls.c
turl_LDADD = $(PARSER_LIB) $(EXTRA_LIB) \
$(top_builddir)/src/osipparser2/osip_list.o     \
$(top_builddir)/src/osipparser2/osip_uri.o      \
$(top_builddir)/src/osipparser2/osip_port.o     \
$(top_builddir)/src/osipparser2/osip_from.o

tfrom_SOURCES =  tfrom.c
tfrom_LDADD =  $(PARSER_LIB) $(EXTRA_LIB) \
$(top_builddir)/src/osipparser2/osip_list.o     \
$(top_builddir)/src/osipparser2/osip_uri.o      \
$(top_builddir)/src/osipparser2/osip_port.o     \
$(top_builddir)/src/osipparser2/osip_header.o  \
$(top_builddir)/src/osipparser2/osip_from.o

tto_SOURCES =  tto.c
tto_LDADD = $(PARSER_LIB) $(EXTRA_LIB)  \
$(top_builddir)/src/osipparser2/osip_list.o     \
$(top_builddir)/src/osipparser2/osip_uri.o          \
$(top_builddir)/src/osipparser2/osip_port.o     \
$(top_builddir)/src/osipparser2/osip_header.o   \
$(top_builddir)/src/osipparser2/osip_to.o        \
$(top_builddir)/src/osipparser2/osip_from.o


tcontact_SOURCES =  tcontact.c
tcontact_LDADD =  $(PARSER_LIB) $(EXTRA_LIB)  \
$(top_builddir)/src/osipparser2/osip_list.o      \
$(top_builddir)/src/osipparser2/osip_uri.o       \
$(top_builddir)/src/osipparser2/osip_port.o      \
$(top_builddir)/src/osipparser2/osip_header.o   \
$(top_builddir)/src/osipparser2/osip_from.o      \
$(top_builddir)/src/osipparser2/osip_to.o        \
$(top_builddir)/src/osipparser2/osip_contact.o

trecordr_SOURCES =  trecordr.c
trecordr_LDADD =  $(PARSER_LIB) $(EXTRA_LIB)  \
$(top_builddir)/src/osipparser2/osip_list.o      \
$(top_builddir)/src/osipparser2/osip_uri.o       \
$(top_builddir)/src/osipparser2/osip_port.o      \
$(top_builddir)/src/osipparser2/osip_header.o   \
$(top_builddir)/src/osipparser2/osip_from.o      \
$(top_builddir)/src/osipparser2/osip_to.o        \
$(top_builddir)/src/osipparser2/osip_record_route.o

troute_SOURCES =  troute.c
troute_LDADD =  $(PARSER_LIB) $(EXTRA_LIB) \
$(top_builddir)/src/osipparser2/osip_list.o     \
$(top_builddir)/src/osipparser2/osip_uri.o          \
$(top_builddir)/src/osipparser2/osip_port.o     \
$(top_builddir)/src/osipparser2/osip_header.o   \
$(top_builddir)/src/osipparser2/osip_from.o      \
$(top_builddir)/src/osipparser2/osip_to.o        \
$(top_builddir)/src/osipparser2/osip_route.o     \
$(top_builddir)/src/osipparser2/osip_record_route.o

tvia_SOURCES =  tvia.c
tvia_LDADD = $(PARSER_LIB) $(EXTRA_LIB) \
$(top_builddir)/src/osipparser2/osip_list.o     \
$(top_builddir)/src/osipparser2/osip_uri.o      \
$(top_builddir)/src/osipparser2/osip_port.o     \
$(top_builddir)/src/osipparser2/osip_header.o   \
$(top_builddir)/src/osipparser2/osip_via.o       \
$(top_builddir)/src/osipparser2/osip_from.o

tcallid_SOURCES =  tcallid.c
tcallid_LDADD = $(PARSER_LIB) $(EXTRA_LIB) \
$(top_builddir)/src/osipparser2/osip_list.o     \
$(top_builddir)/src/osipparser2/osip_uri.o      \
$(top_builddir)/src/osipparser2/osip_port.o     \
$(top_builddir)/src/osipparser2/osip_call_id.o  \
$(top_builddir)/src/osipparser2/osip_from.o


torture_sdp_test_SOURCES =  torture_sdp.c
torture_sdp_test_LDADD = $(PARSER_LIB) $(EXTRA_LIB) \
$(top_builddir)/src/osipparser2/osip_list.o     \
$(top_builddir)/src/osipparser2/osip_port.o     \
$(top_builddir)/src/osipparser2/sdp_message.o   \
$(top_builddir)/src/osipparser2/sdp_accessor.o  \
$(top_builddir)/src/osip2/osip_negotiation.o

torture_test_SOURCES =  torture.c
torture_test_LDADD = $(PARSER_LIB) $(EXTRA_LIB) \
$(top_builddir)/src/osipparser2/osip_list.o     \
$(top_builddir)/src/osipparser2/osip_port.o     \
$(top_builddir)/src/osipparser2/osip_body.o      \
$(top_builddir)/src/osipparser2/osip_call_id.o    \
$(top_builddir)/src/osipparser2/osip_contact.o   \
$(top_builddir)/src/osipparser2/osip_content_length.o   \
$(top_builddir)/src/osipparser2/osip_from.o      \
$(top_builddir)/src/osipparser2/osip_header.o   \
$(top_builddir)/src/osipparser2/osip_to.o        \
$(top_builddir)/src/osipparser2/osip_via.o       \
$(top_builddir)/src/osipparser2/osip_cseq.o      \
$(top_builddir)/src/osipparser2/osip_message_parse.o   \
$(top_builddir)/src/osipparser2/osip_message_to_str.o   \
$(top_builddir)/src/osipparser2/osip_parser_cfg.o       \
$(top_builddir)/src/osipparser2/osip_message.o   \
$(top_builddir)/src/osipparser2/osip_uri.o           \
$(top_builddir)/src/osipparser2/sdp_message.o    \
$(top_builddir)/src/osipparser2/osip_content_type.o \
$(top_builddir)/src/osipparser2/osip_route.o     \
$(top_builddir)/src/osipparser2/osip_mime_version.o \
$(top_builddir)/src/osipparser2/osip_record_route.o \
$(top_builddir)/src/osipparser2/osip_www_authenticate.o \
$(top_builddir)/src/osipparser2/osip_authorization.o \
$(top_builddir)/src/osipparser2/osip_proxy_authorization.o  \
$(top_builddir)/src/osipparser2/osip_proxy_authenticate.o \
$(top_builddir)/src/osipparser2/osip_accept.o    \
$(top_builddir)/src/osipparser2/osip_accept_encoding.o \
$(top_builddir)/src/osipparser2/osip_accept_language.o \
$(top_builddir)/src/osipparser2/osip_call_info.o  \
$(top_builddir)/src/osipparser2/osip_alert_info.o \
$(top_builddir)/src/osipparser2/osip_error_info.o \
$(top_builddir)/src/osipparser2/osip_allow.o     \
$(top_builddir)/src/osipparser2/osip_content_encoding.o \
$(top_builddir)/src/osipparser2/osip_content_disposition.o \
$(top_builddir)/src/osipparser2/sdp_accessor.o



check:
	@echo " ****** starting tests! ********"
	./$(top_srcdir)/src/test/tst ./$(top_srcdir)/src/test/res/torture_msgs -v -c > ./osip-chk.log 2>&1
	./$(top_srcdir)/src/test/tst_sdp ./$(top_srcdir)/src/test/res/torture_sdps -v -c > ./osipsdp-chk.log 2>&1

	@echo " ******              RESULTS                 ********"

	@echo " **SIP* NUMBER OF SUCCESS (87 are expected) ********"
	@grep "==========" osip-chk.log | grep OK | wc -l
	@echo " **SIP* NUMBER OF FAILURE (6 are expected)  ********"
	@grep "==========" osip-chk.log | grep FA | wc -l

	@echo " **SDP* NUMBER OF SUCCESS (15 are expected)  ********"
	@grep "==========" osipsdp-chk.log | grep OK | wc -l
	@echo " **SDP* NUMBER OF FAILURE (0 are expected)   ********"
	@grep "==========" osipsdp-chk.log | grep FA | wc -l
	@echo ""
	@echo "In case you have a doubt, send the generated"
	@echo "log file with your comment to <jack@atosc.org>."
