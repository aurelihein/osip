
EXTRA_DIST = fsm.h

lib_LTLIBRARIES = libfsmtl.la

libfsmtl_la_SOURCES =  \
ict_fsm.c      ist_fsm.c      nict_fsm.c     nist_fsm.c \
ict.c          ist.c          nict.c         nist.c     \
fsm_misc.c     osip.c         transaction.c  sipevent.c \
dialog.c       sdp_negoc.c    port_sema.c    port_fifo.c \
port_thread.c


libfsmtl_la_LDFLAGS = -version-info $(LIBOSIP_SO_VERSION) \
 $(FSM_LIB) $(EXTRA_LIB)


INCLUDES = -I$(top_srcdir)
AM_CFLAGS = $(SIP_CFLAGS) $(SIP_FSM_FLAGS) $(SIP_EXTRA_FLAGS)


