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

#include <stdlib.h>
#include <stdio.h>

#ifndef __VXWORKS_OS__
#ifdef WIN32
#include <winsock.h>
#define close(s) closesocket(s)
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/time.h>
#endif
#endif

#ifdef __VXWORKS_OS__
#include "vxWorks.h" 
#include "sockLib.h" 
#include "inetLib.h" 
#endif

#include "app.h"
#include <osip/port.h>
#include <osip/const.h>

#ifdef OSIP_MT

void        *sipd_thread(mt_ua_t *mt_ua);

int sipd_start(mt_ua_t *mt_ua)
{
  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,stdout,"New UDP listener on port %i\n"
	  ,mt_ua->port));

  sthread_create(20000,NULL,(void *(*)(void *))sipd_thread
			 ,(void *)mt_ua);
  return 1; /* OK */
}

/* temporary patch targeted to linphone before improvements */
#ifdef LINPHONE
fd_set osip_fdset;
int control_fd=0;
int max_fd=0;
#endif

void *
sipd_thread(mt_ua_t *mt_ua)
{
  struct sockaddr_in  raddr;
  int	 sock;
  char *buf;

#ifdef LINPHONE
  int err;
  int control_fds[2];
  int option=1;

  FD_ZERO(&osip_fdset);
#endif

  /* create a socket */
  sock = (int)socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
  
  raddr.sin_addr.s_addr = htons(INADDR_ANY);
  raddr.sin_port = htons((short)mt_ua->port);
  raddr.sin_family = AF_INET;
  
  if (bind(sock, (struct sockaddr *)&raddr, sizeof(raddr)) < 0) {
     TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL0,NULL,"UDP listener bind failed %i!\n",mt_ua->port));

     exit(0);
  }
  

#ifdef LINPHONE
  err=setsockopt(sock, SOL_SOCKET , SO_REUSEADDR, (void*)&option, sizeof(option));
  if (err!=0) {DEBUG(fprintf(stdout,"Error in setsockopt()\n"));}
  max_fd=sock;
  /* create a set of file descritor to control the stack*/
  err=pipe(control_fds);
  if (err!=0)
    {
      perror("Error creating pipe");
      exit(1);
    }
  control_fd=control_fds[1]; /* the file descriptor where to write something to control the stack*/
  if (max_fd<control_fds[0]) max_fd=control_fds[0];
  
  FD_SET(sock,&osip_fdset );
  FD_SET(control_fds[0],&osip_fdset );
#endif

  
  buf = (char *)smalloc(SIP_MESSAGE_MAX_LENGTH*sizeof(char)+1);
  while(1)
    { /* SIP_MESSAGE_MAX_LENGTH should be set to 66001
	 to improve interopperability */
      int i;
      
#ifdef LINPHONE
      i=select( max_fd,&osip_fdset,NULL,NULL,NULL);
      /* if something to read on the control file descriptor, then exit thread*/
      if (FD_ISSET(control_fds[0],&osip_fdset))
	{
	  close(sock);
	  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,NULL,"Exiting thread!\n"));
	  sfree(buf); /* added by AMD 18/08/2001 */
	  return NULL;
	}
#endif

    i = recv(sock, buf, SIP_MESSAGE_MAX_LENGTH ,  0);
    if( i > 0 )
      {
	/* Message might not end with a "\0" but we know the number of */
	/* char received! */
	transaction_t *transaction;
	sipevent_t *sipevent;
	sstrncpy(buf+i,"\0",1);
	TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL2,NULL,"RCV UDP MESSAGE\n"));
	TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL4,stdout,"%s\n",buf));
	sipevent = osip_parse(buf);
	transaction = NULL;
	if (sipevent!=NULL)
	  transaction = osip_distribute_event(mt_ua->config,sipevent);

#ifdef OSIP_MT
      if (transaction!=NULL)
	{
	  if (transaction->your_instance==NULL)
	    {
	      transaction_mt_t *transaction_mt;
	      transaction_mt_init(&transaction_mt);
	      transaction_mt_set_transaction(transaction_mt,transaction);
	      transaction_mt_start_transaction(transaction_mt);
	    }
	}
#endif
      }
    else
      {
      TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL0,NULL,"UDP listener failed while receiving data!\n"));
      }
    }
  sfree(buf);
  return NULL;
}

#endif

int
osipudpd_close(mt_ua_t *mt_ua)
{
  int sock = osipudpd_open(mt_ua);
  close(sock);
  return 0;
}

int
osipudpd_open(mt_ua_t *mt_ua)
{
  static struct sockaddr_in raddr;
  static int udp_out_sock = 0;
  
  if (udp_out_sock==0)
    {
      udp_out_sock = (int)socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
      if (udp_out_sock==-1)
	{
	  perror("failed to open socket!");
	  return -1;
	}
      raddr.sin_addr.s_addr = htons(INADDR_ANY);
      raddr.sin_port = htons((short)mt_ua->port);
      raddr.sin_family = AF_INET;
      if (bind(udp_out_sock, (struct sockaddr *)&raddr, sizeof(raddr)) < 0)
	{
	  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL0,stdout,"UDP listener bind failed %i!\n",mt_ua->port));
	  exit(0);
	}
    }
  return udp_out_sock;
}

int
osipudpd_execute(mt_ua_t *mt_ua, int sec_max, int usec_max)
{
  char *buf;
  int sock = osipudpd_open(mt_ua);

  fd_set rfds;
  struct timeval tv;
  int retval;
  int i;

  /* Watch the udp port. */
  FD_ZERO(&rfds);
  FD_SET(sock, &rfds);
  /* Wait up to ? seconds. */
  tv.tv_sec = sec_max;
  tv.tv_usec = usec_max;

  if ((sec_max==-1)||(usec_max==-1))
    retval = select(sock+1, &rfds, NULL, NULL, NULL);
  else
    retval = select(sock+1, &rfds, NULL, NULL, &tv);
  /* Don't rely on the value of tv now! */

  if (0==retval)
    return -1; /* no message: timout expires */
  if (-1==retval)
    return -2; /* error */

  /* FD_ISSET(sock, &rfds) will be true. */

  buf = (char *)smalloc(SIP_MESSAGE_MAX_LENGTH*sizeof(char)+1);
  /* SIP_MESSAGE_MAX_LENGTH should be set to 66001
     to improve interopperability */
  i = recv(sock, buf, SIP_MESSAGE_MAX_LENGTH ,  0);
  if( i > 0 )
    {
      /* Message might not end with a "\0" but we know the number of */
      /* char received! */
      transaction_t *transaction;
      sipevent_t *sipevent;
      sstrncpy(buf+i,"\0",1);
      TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL2,NULL,"RCV UDP MESSAGE\n"));
      TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL4,stdout,"%s\n",buf));
      sipevent = osip_parse(buf);
      transaction = NULL;
      if (sipevent!=NULL)
	transaction = osip_distribute_event(mt_ua->config,sipevent);

#ifdef OSIP_MT
      if (transaction!=NULL)
	{
	  if (transaction->your_instance==NULL)
	    {
	      transaction_mt_t *transaction_mt;
	      transaction_mt_init(&transaction_mt);
	      transaction_mt_set_transaction(transaction_mt,transaction);
	      transaction_mt_start_transaction(transaction_mt);
	    }
	}
#endif
    }
  else
    {
      TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,NULL,"<udp_listen.c> UDP listener failed while receiving data!\n"));
      return -1;
    }

  sfree(buf);
  return 1;
}
