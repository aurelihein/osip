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

#ifndef __VXWORKS_OS__
#include <errno.h>
#include <ctype.h>
#include <time.h>
#include <sys/types.h>
#include <stdlib.h>
#ifdef WIN32
#include <winsock.h>
#else
#include <sys/time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif
#endif

#ifdef __VXWORKS_OS__
#include "vxWorks.h" 
#include "sockLib.h" 
#include "inetLib.h" 
#include "stdioLib.h" 
#include "strLib.h" 
#include "ioLib.h" 
#include "fioLib.h"
#endif

#include <osip/port.h>
#include <osip/const.h>
#include <osip/smsg.h>


/* create one outgoing socket for the whole session.  */
/* return the socket. */
/* This is only temporary. a new transport layer will */
/* be added in the future. */
int
getsock()
{
  static int udpout_sock = 0;
  if (udpout_sock==0)
    {
      udpout_sock = (int)socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
      if (udpout_sock==-1)
	{
	  perror("failed to open socket!");
	  return -1;
	}
    }
  return udpout_sock;
}

/* send a message to host:port on UDP.          */
/* INPUT : char *host | host to be used.        */
/* INPUT : int port   | port to be use.         */
/* INPUT : char *message | message to be sent.  */
/* returns null on error. */
int
udp_send(char *message,char *host,int port)
{
  struct hostent     *hp;
#ifdef __linux /* ok for GNU/linux */
  struct hostent     **hp_ptr;
#else
#  ifdef __sun__ /* ok for solaris (Not yet tested MACRO??) */
  struct hostent     *hp_2;
#  else 
#    ifdef __unix
  struct hostent     *hp_2;
#    endif
#  endif
#endif
  struct sockaddr_in addr;
  unsigned long int  one_inet_addr;
  int sock;
  
  hp = NULL;

#ifdef __linux /* avoid warning before replacing the gethostbyname_r */
  hp_ptr = NULL;
#endif
  /* TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,stdout
      ,"<udp_send.c> sending message to %s on port %i\n"
      ,host,port)); */

  if ((int)(one_inet_addr = inet_addr(host)) == -1)
    {
      DEBUG(fprintf(stdout,"<udp_send.c> TODO: There is no support for name resolution!!!\n"));
      /* this has to be moved to the sutil module */
      /* Not implemented yet
#ifndef __VXWORKS_OS__
      int i;
      int *herrno = (int *) smalloc(sizeof(int ));
      char *pbuf = (char *) smalloc(513*sizeof(char));
      hp = (struct hostent *)smalloc(sizeof(struct hostent));
      TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,stdout
	  ,"<udp_send.c>  resolving address! !?\n"));
#ifdef __linux
      hp_ptr = (struct hostent **)smalloc(sizeof(struct hostent*));
      i = gethostbyname_r(host,hp,pbuf,512,hp_ptr,herrno);
#else
#  ifdef __sun__ 
      hp_2 = gethostbyname_r(host,hp,pbuf,512,herrno);
#  else
#    ifdef __unix 
      hp_2 = gethostbyname_r(host,hp,pbuf,512,herrno);
#    endif
#  endif
#endif

      if (*herrno<0)
	{
	  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,stdout
	      ,"<udp_send.c> Could not resolve address (error:%i)!!?\n",herrno));
	  return -1;
	}
      addr.sin_addr.s_addr = *((long *)(hp->h_addr));
      sfree(pbuf);
      sfree(hp);
#ifdef  __linux
      sfree(hp_ptr);
#else
#  ifdef __sun__
      sfree(herrno);
#  else
#    ifdef __unix 
      sfree(herrno);
#    endif
#  endif
#endif

#endif
 */
    }
  else
    { 
      addr.sin_addr.s_addr = one_inet_addr;
    }

  addr.sin_port        = htons((short)port);
  addr.sin_family      = AF_INET;

  sock = getsock();

  /* If we want to detect ICMP message and ECONNREFUSED
     errors, this line must be commented. */
#ifndef WIN32
  connect(sock,(struct sockaddr *) &addr,sizeof(addr));
#endif
  if (0  > sendto (sock, (const void*) message, strlen (message), 0, 
		      (struct sockaddr *) &addr, sizeof(addr))) 
    /* if (sendto (sock, (caddr_t) message, strlen (message), 0, 
       (struct sockaddr *) &addr, sizeof(addr)) < 0) */
    {
#ifdef WIN32
      if (WSAECONNREFUSED==WSAGetLastError())
#else
      if (ECONNREFUSED==errno)
#endif
	{
	  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL3,stdout,
		"SIP_ECONNREFUSED - No remote server.\n"));
	  /* This can be considered as an error, but for the moment,
	     I prefer that the application continue to try sending
	     message again and again... so we are not in a error case.
	     Nevertheless, this error should be announced!
	     ALSO, UAS may not have any other options than retry always
	     on the same port.
	  */
	  return 1;
	}
      else
	{
	  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL3,stdout,
		"SIP_NETWORK_ERROR - Network error.\n"));
	  /* SIP_NETWORK_ERROR; */
	  return -1;
	}
    }

  /* #endif */
  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL2, NULL,
	      "SND UDP MESSAGE.\n"));

  return 0;
}


int
udp_send_request(sip_t *request, url_t *proxy)
/* day3: support for proxy usage
   udp_send_request(sip_t *request) */
{
  char *mess;
  url_t *url;
  int port;
  int i;

   if (proxy==NULL)
    url = request->strtline->rquri;
   else
    url = proxy;

  if (url->port!=NULL)
    port = atoi(url->port);
  else
    port = 5060;
  i = msg_2char(request, &mess);
  if (i==-1)
    {
    TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL2,stdout,"<udp_send.c> Error while creating REQUEST\n"));
    return -1; /* error */
    }
    
  i = udp_send(mess, url->host, port);
  sfree(mess);
  if (i==-1)
    {
      TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL2,stdout,"<udp_send.c> Error while sending REQUEST\n"));
      return -1; /* error */
    }
  DEBUG(msg_logrequest(request,"SND %s f:%s t:%s cseq:%s callid:%s\n"));
  return 0;
}

int
udp_send_response(sip_t *response)
{
  char *mess;
  via_t *via;
  int port;
  int i;

  /*  via = (via_t *)smalloc(sizeof(via_t)); */
  if (-1==msg_getvia(response,0,&via))
    {
      TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL2,stdout,"<udp_send.c> Error in via header\n"));
      return -1;
    }
  i = msg_2char(response, &mess);
  if (i==-1)
    {
      TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL2,stdout,"<udp_send.c> Error could not create message\n"));
      return -1;
    }
  if (via->port!=NULL)
    port = atoi(via->port);
  else
    port = 5060;

  i = udp_send(mess,via->host, port);
  sfree(mess);
  if (i==-1)
    {
      TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL2,stdout,"<udp_send.c> Error while sending REQUEST\n"));
      return -1; /* error */
    }
  DEBUG(msg_logresponse(response,"SND %s %s (%s) f:%s t:%s cseq:%s callid:%s\n"));
  return 0; /* return 1 on success and i on error */
}


