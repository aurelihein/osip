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
  FILE *vias_file;


  via_t *via;
  char *a_via;
  char *dest;
  char *res;

  vias_file = fopen (argv[1], "r");
  if (vias_file == NULL)
    {
      fprintf (stdout, "Failed to open %s file.\nUsage: tvia vias.txt\n",
	       argv[1]);
      exit (0);
    }

  a_via = (char *) smalloc (200);
  res = fgets (a_via, 200, vias_file);	/* lines are under 200 */
  while (res != NULL)
    {

      int errcode;

      /* remove the last '\n' before parsing */
      sstrncpy (a_via + strlen (a_via) - 1, "\0", 1);

      if (0 != strncmp (a_via, "#", 1))
	{
	  /* allocate & init via */
	  via_init (&via);
	  printf ("=================================================\n");
	  printf ("VIA TO PARSE: |%s|\n", a_via);
	  errcode = via_parse (via, a_via);
	  if (errcode != -1)
	    {
	      if (via_2char (via, &dest) != -1)
		{
		  printf ("result:       |%s|\n", dest);
		  sfree (dest);
		}
	    }
	  else
	    printf ("Bad via format: %s\n", a_via);
	  via_free (via);
	  sfree (via);
	  printf ("=================================================\n");
	}
      res = fgets (a_via, 200, vias_file);	/* lines are under 200 */
    }
  sfree (a_via);

  return 0;
}
