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


#ifdef ENABLE_MPATROL
#include <mpatrol.h>
#endif


#include <stdio.h>
#include <stdlib.h>
#include <osip/port.h>
#include <osip/smsg.h>


int
main (int argc, char **argv)
{
  FILE *routes_file;


  route_t *route;
  char *a_route;
  char *dest;
  char *res;

  routes_file = fopen (argv[1], "r");
  if (routes_file == NULL)
    {
      fprintf (stdout, "Failed to open %s file.\nUsage: troute routes.txt\n",
	       argv[1]);
      exit (0);
    }

  a_route = (char *) smalloc (200);
  res = fgets (a_route, 200, routes_file);	/* lines are under 200 */
  while (res != NULL)
    {

      int errcode;

      /* remove the last '\n' before parsing */
      strncpy (a_route + strlen (a_route) - 1, "\0", 1);

      if (0 != strncmp (a_route, "#", 1))
	{
	  /* allocate & init route */
	  route_init (&route);
	  printf ("=================================================\n");
	  printf ("ROUTE TO PARSE: |%s|\n", a_route);
	  errcode = route_parse (route, a_route);
	  if (errcode != -1)
	    {
	      if (route_2char (route, &dest) != -1)
		{
		  printf ("result:         |%s|\n", dest);
		  sfree (dest);
		}
	    }
	  else
	    printf ("Bad route format: %s\n", a_route);
	  route_free (route);
	  sfree (route);
	  printf ("=================================================\n");
	}
      res = fgets (a_route, 200, routes_file);	/* lines are under 200 */
    }
  sfree (a_route);
  return 0;
}
