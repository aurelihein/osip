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

#ifndef __VXWORKS_OS__
#ifdef WIN32
#include <winsock.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif
#endif

#ifdef __VXWORKS_OS__
#include <vxWorks.h>
#include <sockLib.h>
#include <inetLib.h>
#include <hostLib.h>
#include <resolvLib.h>
#endif

#include <osip/port.h>

struct hostent *
resolv(char *name) {

  unsigned long int addr;
  struct hostent *hp = NULL;

  if ((int)(addr = inet_addr(name)) == -1)
    {
    /* The address is not of the form a.b.c.d */
#ifndef __VXWORKS_OS__
    hp = gethostbyname(name);
    /*    fprintf(stdout,"Address is of type name %li\n",addr); */
#endif
#ifdef __VXWORKS_OS__
    char *pbuf = (char *) smalloc(513*sizeof(char));
    hp = resolvGetHostByName(name,pbuf,512);
    /* we should check errno */
#endif
    }
  else
    {
#ifndef __VXWORKS_OS__
    hp = gethostbyaddr((char *)&addr, sizeof (addr), AF_INET);
    /* fprintf(stdout,"Address is of type a.c.d.e %li\n",addr); */
#endif
#ifdef __VXWORKS_OS__
    char *pbuf = (char *) smalloc(513*sizeof(char));
    /* hp = resolvGetHostByAddr((char *)&addr ,pbuf ,512); */
#endif
    }
  if (hp == NULL) {
    fprintf(stdout,"host information for %s not found\n",name);
    return NULL;
  }

  /* print information about IP address */
  /* little things to fix?
  {
  char *p;
  for (p = hp->h_addr_list; *p != 0; p++)
  {
    struct in_addr in;
    char **q;
    
    (void) memcpy(&in.s_addr, *p, sizeof (in.s_addr));
    (void) printf("%s\t%s", inet_ntoa(in), hp->h_name);
    for (q = hp->h_aliases; *q != 0; q++)
      (void) fprintf(stdout," %s", *q);
    (void) fprintf(stdout,"\n");
  }
  */
  return hp;
}


