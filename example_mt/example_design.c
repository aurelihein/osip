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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <signal.h>
#include <time.h>

#ifndef __VXWORKS_OS__
#ifdef WIN32
#include <winsock.h>
#else
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#endif
#else
/* should be ok */
#include "sockLib.h" 
#include "inetLib.h" 
#endif

#include "app.h"
#include <osip/osip.h>
#include <osip/smsg.h>
#include <osip/timers.h>
#include <osip/port.h>

extern int control_fd;

void
set_all_callbacks(osip_t *cf)
{
  osip_setcb_rcvresp_retransmission(cf,&cb_rcvresp_retransmission);
  osip_setcb_sndreq_retransmission(cf,&cb_sndreq_retransmission);
  osip_setcb_sndresp_retransmission(cf,&cb_sndresp_retransmission);
  osip_setcb_rcvreq_retransmission(cf,&cb_rcvreq_retransmission);
  
  osip_setcb_killtransaction(cf,&cb_killtransaction);
  osip_setcb_endoftransaction(cf,&cb_endoftransaction);
  
  osip_setcb_connection_refused(cf,&cb_connection_refused);
  osip_setcb_network_error(cf,&cb_network_error);
  
  osip_setcb_sndinvite  (cf,&cb_sndinvite);
  osip_setcb_sndack     (cf,&cb_sndack);
  osip_setcb_sndbye     (cf,&cb_sndbye);
  osip_setcb_sndcancel  (cf,&cb_sndcancel);
  osip_setcb_sndinfo    (cf,&cb_sndinfo);
  osip_setcb_sndoptions (cf,&cb_sndoptions);
  osip_setcb_sndregister(cf,&cb_sndregister);
  osip_setcb_sndprack   (cf,&cb_sndprack);
  osip_setcb_sndunkrequest(cf,&cb_sndunkrequest);

#ifndef EXTENDED_CALLBACKS

  osip_setcb_rcv1xx(cf,&cb_rcv1xx);
  osip_setcb_rcv2xx(cf,&cb_rcv2xx);
  osip_setcb_rcv3xx(cf,&cb_rcv3xx);
  osip_setcb_rcv4xx(cf,&cb_rcv4xx);
  osip_setcb_rcv5xx(cf,&cb_rcv5xx);
  osip_setcb_rcv6xx(cf,&cb_rcv6xx);
  
  osip_setcb_snd1xx(cf,&cb_snd1xx);
  osip_setcb_snd2xx(cf,&cb_snd2xx);
  osip_setcb_snd3xx(cf,&cb_snd3xx);
  osip_setcb_snd4xx(cf,&cb_snd4xx);
  osip_setcb_snd5xx(cf,&cb_snd5xx);
  osip_setcb_snd6xx(cf,&cb_snd6xx);

#else

  osip_setcb_rcvinvite1xx(cf,&cb_rcv1xx);
  osip_setcb_rcvinvite2xx(cf,&cb_rcv2xx);
  osip_setcb_rcvinvite3xx(cf,&cb_rcv3xx);
  osip_setcb_rcvinvite4xx(cf,&cb_rcv4xx);
  osip_setcb_rcvinvite5xx(cf,&cb_rcv5xx);
  osip_setcb_rcvinvite6xx(cf,&cb_rcv6xx);
  
  osip_setcb_sndinvite1xx(cf,&cb_snd1xx);
  osip_setcb_sndinvite2xx(cf,&cb_snd2xx);
  osip_setcb_sndinvite3xx(cf,&cb_snd3xx);
  osip_setcb_sndinvite4xx(cf,&cb_snd4xx);
  osip_setcb_sndinvite5xx(cf,&cb_snd5xx);
  osip_setcb_sndinvite6xx(cf,&cb_snd6xx);

  osip_setcb_rcvack1xx(cf,&cb_rcv1xx);
  osip_setcb_rcvack2xx(cf,&cb_rcv2xx);
  osip_setcb_rcvack3xx(cf,&cb_rcv3xx);
  osip_setcb_rcvack4xx(cf,&cb_rcv4xx);
  osip_setcb_rcvack5xx(cf,&cb_rcv5xx);
  osip_setcb_rcvack6xx(cf,&cb_rcv6xx);
  
  osip_setcb_sndack1xx(cf,&cb_snd1xx);
  osip_setcb_sndack2xx(cf,&cb_snd2xx);
  osip_setcb_sndack3xx(cf,&cb_snd3xx);
  osip_setcb_sndack4xx(cf,&cb_snd4xx);
  osip_setcb_sndack5xx(cf,&cb_snd5xx);
  osip_setcb_sndack6xx(cf,&cb_snd6xx);

  osip_setcb_rcvbye1xx(cf,&cb_rcv1xx);
  osip_setcb_rcvbye2xx(cf,&cb_rcv2xx);
  osip_setcb_rcvbye3xx(cf,&cb_rcv3xx);
  osip_setcb_rcvbye4xx(cf,&cb_rcv4xx);
  osip_setcb_rcvbye5xx(cf,&cb_rcv5xx);
  osip_setcb_rcvbye6xx(cf,&cb_rcv6xx);
  
  osip_setcb_sndbye1xx(cf,&cb_snd1xx);
  osip_setcb_sndbye2xx(cf,&cb_snd2xx);
  osip_setcb_sndbye3xx(cf,&cb_snd3xx);
  osip_setcb_sndbye4xx(cf,&cb_snd4xx);
  osip_setcb_sndbye5xx(cf,&cb_snd5xx);
  osip_setcb_sndbye6xx(cf,&cb_snd6xx);

  osip_setcb_rcvcancel1xx(cf,&cb_rcv1xx);
  osip_setcb_rcvcancel2xx(cf,&cb_rcv2xx);
  osip_setcb_rcvcancel3xx(cf,&cb_rcv3xx);
  osip_setcb_rcvcancel4xx(cf,&cb_rcv4xx);
  osip_setcb_rcvcancel5xx(cf,&cb_rcv5xx);
  osip_setcb_rcvcancel6xx(cf,&cb_rcv6xx);
  
  osip_setcb_sndcancel1xx(cf,&cb_snd1xx);
  osip_setcb_sndcancel2xx(cf,&cb_snd2xx);
  osip_setcb_sndcancel3xx(cf,&cb_snd3xx);
  osip_setcb_sndcancel4xx(cf,&cb_snd4xx);
  osip_setcb_sndcancel5xx(cf,&cb_snd5xx);
  osip_setcb_sndcancel6xx(cf,&cb_snd6xx);

  osip_setcb_rcvinfo1xx(cf,&cb_rcv1xx);
  osip_setcb_rcvinfo2xx(cf,&cb_rcv2xx);
  osip_setcb_rcvinfo3xx(cf,&cb_rcv3xx);
  osip_setcb_rcvinfo4xx(cf,&cb_rcv4xx);
  osip_setcb_rcvinfo5xx(cf,&cb_rcv5xx);
  osip_setcb_rcvinfo6xx(cf,&cb_rcv6xx);
  
  osip_setcb_sndinfo1xx(cf,&cb_snd1xx);
  osip_setcb_sndinfo2xx(cf,&cb_snd2xx);
  osip_setcb_sndinfo3xx(cf,&cb_snd3xx);
  osip_setcb_sndinfo4xx(cf,&cb_snd4xx);
  osip_setcb_sndinfo5xx(cf,&cb_snd5xx);
  osip_setcb_sndinfo6xx(cf,&cb_snd6xx);

  osip_setcb_rcvoptions1xx(cf,&cb_rcv1xx);
  osip_setcb_rcvoptions2xx(cf,&cb_rcv2xx);
  osip_setcb_rcvoptions3xx(cf,&cb_rcv3xx);
  osip_setcb_rcvoptions4xx(cf,&cb_rcv4xx);
  osip_setcb_rcvoptions5xx(cf,&cb_rcv5xx);
  osip_setcb_rcvoptions6xx(cf,&cb_rcv6xx);

  osip_setcb_sndoptions1xx(cf,&cb_snd1xx);
  osip_setcb_sndoptions2xx(cf,&cb_snd2xx);
  osip_setcb_sndoptions3xx(cf,&cb_snd3xx);
  osip_setcb_sndoptions4xx(cf,&cb_snd4xx);
  osip_setcb_sndoptions5xx(cf,&cb_snd5xx);
  osip_setcb_sndoptions6xx(cf,&cb_snd6xx);

  osip_setcb_rcvregister1xx(cf,&cb_rcv1xx);
  osip_setcb_rcvregister2xx(cf,&cb_rcv2xx);
  osip_setcb_rcvregister3xx(cf,&cb_rcv3xx);
  osip_setcb_rcvregister4xx(cf,&cb_rcv4xx);
  osip_setcb_rcvregister5xx(cf,&cb_rcv5xx);
  osip_setcb_rcvregister6xx(cf,&cb_rcv6xx);
  
  osip_setcb_sndregister1xx(cf,&cb_snd1xx);
  osip_setcb_sndregister2xx(cf,&cb_snd2xx);
  osip_setcb_sndregister3xx(cf,&cb_snd3xx);
  osip_setcb_sndregister4xx(cf,&cb_snd4xx);
  osip_setcb_sndregister5xx(cf,&cb_snd5xx);
  osip_setcb_sndregister6xx(cf,&cb_snd6xx);

  osip_setcb_rcvprack1xx(cf,&cb_rcv1xx);
  osip_setcb_rcvprack2xx(cf,&cb_rcv2xx);
  osip_setcb_rcvprack3xx(cf,&cb_rcv3xx);
  osip_setcb_rcvprack4xx(cf,&cb_rcv4xx);
  osip_setcb_rcvprack5xx(cf,&cb_rcv5xx);
  osip_setcb_rcvprack6xx(cf,&cb_rcv6xx);
  
  osip_setcb_sndprack1xx(cf,&cb_snd1xx);
  osip_setcb_sndprack2xx(cf,&cb_snd2xx);
  osip_setcb_sndprack3xx(cf,&cb_snd3xx);
  osip_setcb_sndprack4xx(cf,&cb_snd4xx);
  osip_setcb_sndprack5xx(cf,&cb_snd5xx);
  osip_setcb_sndprack6xx(cf,&cb_snd6xx);
  
  osip_setcb_rcvunkrequest1xx(cf,&cb_rcv1xx);
  osip_setcb_rcvunkrequest2xx(cf,&cb_rcv2xx);
  osip_setcb_rcvunkrequest3xx(cf,&cb_rcv3xx);
  osip_setcb_rcvunkrequest4xx(cf,&cb_rcv4xx);
  osip_setcb_rcvunkrequest5xx(cf,&cb_rcv5xx);
  osip_setcb_rcvunkrequest6xx(cf,&cb_rcv6xx);
  
  osip_setcb_sndunkrequest1xx(cf,&cb_snd1xx);
  osip_setcb_sndunkrequest2xx(cf,&cb_snd2xx);
  osip_setcb_sndunkrequest3xx(cf,&cb_snd3xx);
  osip_setcb_sndunkrequest4xx(cf,&cb_snd4xx);
  osip_setcb_sndunkrequest5xx(cf,&cb_snd5xx);
  osip_setcb_sndunkrequest6xx(cf,&cb_snd6xx);

#endif

  osip_setcb_rcvinvite  (cf,&cb_rcvinvite);
  osip_setcb_rcvack     (cf,&cb_rcvack);
  osip_setcb_rcvbye     (cf,&cb_rcvbye);
  osip_setcb_rcvcancel  (cf,&cb_rcvcancel);
  osip_setcb_rcvinfo    (cf,&cb_rcvinfo);
  osip_setcb_rcvoptions (cf,&cb_rcvoptions);
  osip_setcb_rcvregister(cf,&cb_rcvregister);
  osip_setcb_rcvprack   (cf,&cb_rcvprack);
  osip_setcb_rcvunkrequest(cf,&cb_rcvunkrequest);
  
}

int
ua_start_design2(mt_ua_t *mt_ua)
{

  /* initialise the mutex that protects the access on */
  /* the list of transactions in osip_t structure.    */
  if (-1==osip_global_init())
    return -1; /* mutex is not initialised properly */

  set_all_callbacks(mt_ua->config);
#if defined(THREAD_PTH)
  pth_init();
#endif

  osipudpd_open(mt_ua);
  return 1; /* ok */
}

int
ua_init(mt_ua_t **mt_ua, int port, osip_t *config)
{
  (*mt_ua) = (mt_ua_t *)smalloc(sizeof(mt_ua_t));

  (*mt_ua)->port = port;

  (*mt_ua)->config = config;
  return 0;
}

#ifdef OSIP_MT

int
mt_ua_init(mt_ua_t **mt_ua, int port, osip_t *config)
{
  (*mt_ua) = (mt_ua_t *)smalloc(sizeof(mt_ua_t));

  (*mt_ua)->transactions_mt = (list_t *)
    smalloc(sizeof(list_t));
  if ((*mt_ua)==0) return -1;
  list_init((*mt_ua)->transactions_mt);
  (*mt_ua)->port = port;

  (*mt_ua)->config = config;
  return 0;
}


int
mt_ua_start_design1(mt_ua_t *mt_ua)
{
  /* initialise the mutex that protects the access on */
  /* the list of transactions in osip_t structure.    */
  if (-1==osip_global_init())
    return -1; /* mutex is not initialised properly */

  set_all_callbacks(mt_ua->config);
  timersd_start(mt_ua);

  /* start a thread on the given port. */
  sipd_start(mt_ua);

  return 1; /* ok */
}

/* temporary patch targeted to linphone before improvements */
#ifdef LINPHONE
int mt_ua_stop_design1(mt_ua_t *mt_ua)
{
  char a=0;
  if (control_fd)
    write(control_fd,&a,1);  /* unblock the select() in udp_listen.c*/
  return(0);
}
#endif


int
timersd_start(mt_ua_t *mt_ua)
{
  mt_ua->timerthread = sthread_create(20000,NULL,(void *(*)(void *))timers_thread,(void *)mt_ua->config);

  /* verify that this test also works on VxWorks */
  if (mt_ua->timerthread==NULL)
    return -1;
  return 0;
}


#endif
