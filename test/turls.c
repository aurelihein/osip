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
#include <osip/port.h>
#include <osip/urls.h>


int
main(int argc, char **argv)
{
  FILE *urls_file;


  url_t *url;
  char *a_url;
  char *dest;
  char *res;
  
  urls_file = fopen(argv[1],"r");
  if (urls_file==NULL)
    {
    fprintf(stdout,"Failed to open %s file.\nUsage: turls urls.txt\n",argv[1]);
    exit(0);
    }

  a_url = (char *)smalloc(200);
  res = fgets(a_url, 200, urls_file); /* lines are under 200 */
  while (res !=NULL)
    {
      int errcode;
      /* remove the last '\n' before parsing */
      sstrncpy(a_url+strlen(a_url)-1,"\0",1);

      if (0!=strncmp(a_url, "#", 1))
	{
	  /* allocate & init url */
	  url_init(&url);
	  printf ("=================================================\n");
	  printf ("URL TO PARSE: |%s|\n",a_url);
	  errcode = url_parse(url, a_url);
	  if (errcode!=-1)
	    {
	      if (url_2char(url, &dest)!=-1)
		{
		  printf ("result:       |%s|\n", dest);
		  sfree(dest);
		}
	    }
	  else
	    printf("Bad url format: %s\n",a_url);
	  url_free(url);
	  sfree(url);
	  printf ("=================================================\n");
	}
      res = fgets(a_url, 200, urls_file); /* lines are under 200 */
    }
  sfree(a_url);
  return 0;
}
