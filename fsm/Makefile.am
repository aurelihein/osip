
EXTRA_DIST = makefile.vxw fsm.h

lib_LTLIBRARIES = libosipfsm.la

libosipfsm_la_SOURCES =  \
fsm_misc.c     fsm_uac4inv.c  fsm_uac4req.c   fsm_uas4inv.c \
fsm_uas4req.c  trn_manager.c  trn_timers.c    port_thread.c \
port_sema.c    port_fifo.c    evt_server.c    osip.c        \
sdp_negoc.c

libosipfsm_la_LDFLAGS = -version-info $(LIBOSIP_SO_VERSION) \
-release $(OSIP_RELEASE)  @FSM_LIB@  @EXTRA_LIB@


INCLUDES = -I$(top_srcdir)
CFLAGS = -Wall @SIP_CFLAGS@ @SIP_FSM_FLAGS@ @SIP_EXTRA_FLAGS@ @SIP_FLAGS@


