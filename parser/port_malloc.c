/*
  The oSIP library implements the Session Initiation Protocol (SIP -rfc3261-)
  Copyright (C) 2001,2002,2003  Aymeric MOIZARD jack@atosc.org
  
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

#include <osip/port.h>

/* to enable memory leak detection: */
/* #define ENABLE_LEAKD */

#ifdef ENABLE_LEAKD
static int malloccomptr = 0;
#endif


void *
smalloc (size_t size)
{
#ifdef ENABLE_LEAKD
  static int comptr = 0;
#endif
  void *value;

  value = malloc (size);
#ifdef ENABLE_LEAKD
  comptr++;
  malloccomptr++;

  fprintf (stdout,
	   "INITMALLOC (%i) (%x) =(size,value)| (%i,%i)=(el_freed,difference)\n",
	   size, (char *) value, comptr, malloccomptr);
  fflush (stdout);
#endif
  if (value == 0)
    {
      fprintf (stdout, "<port_malloc.c> virtual memory exhausted\n");
      /* How can I handle that case properly? */
      exit (0);
    }

  return value;
}

void
sfree (void *ptr)
{
#ifdef ENABLE_LEAKD
  static int comptr = 0;

  if (ptr != NULL)
    comptr++;
  if (ptr != NULL)
    malloccomptr--;
  fprintf (stdout, "FREEMALLOC (%x) comptr: %i existing element %i\n",
	   (char *) ptr, comptr, malloccomptr);
  fflush (stdout);
#endif

/* SPD added NULL check just for grins :-) */

  if (ptr != NULL)
    free (ptr);
/* 
   Robin Nayathodan <roooot@softhome.net> 
   N.K Electronics INDIA

   Most People Check This Condition 
 */
  ptr = NULL;

}
