

noinst_PROGRAMS = ua 
CFLAGS = -Wall  @SIP_CFLAGS@ @SIP_EXAMPLE_FLAGS@ @SIP_EXTRA_FLAGS@ @SIP_FLAGS@

ua_SOURCES =  app.c    example_agentlogic.c  example_rcfile.c \
example_callbackuac.c  example_callbackuas.c \
msg_invite.c           msg_register.c \
msg_bye.c              msg_ack.c \
msg_cancel.c           port_net.c \
example_design.c       udp_send.c \
udp_listen.c           transaction_mt.c

ua_LDFLAGS = -O  -Wall 
ua_LDADD = $(top_builddir)/fsm/libosipfsm.la $(top_builddir)/parser/libosipparser.la  @EXTRA_LIB@ @EXAMPLE_LIB@

INCLUDES = -I$(top_srcdir)
EXTRA_DIST = app.h  example_rcfile.h makefile.vxw