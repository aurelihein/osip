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
#include <osip/fifo.h>


/* always use this method to initiate fifo_t.
*/
void
fifo_init (fifo_t * ff)
{
#ifdef OSIP_MT
  ff->qislocked = smutex_init ();
  /*INIT SEMA TO BLOCK ON GET() WHEN QUEUE IS EMPTY */
  ff->qisempty = ssem_init (0);
#endif
  ff->queue = (list_t *) smalloc (sizeof (list_t));
  list_init (ff->queue);
  /* ff->nb_elt = 0; */
  ff->etat = vide;
}

int
fifo_add (fifo_t * ff, void *el)
{
#ifdef OSIP_MT
  smutex_lock (ff->qislocked);
#endif

  if (ff->etat != plein)
    {
      /* ff->nb_elt++; */
      list_add (ff->queue, el, -1);	/* insert at end of queue */
    }
  else
    {
      OSIP_TRACE (osip_trace
		  (__FILE__, __LINE__, OSIP_WARNING, NULL,
		   "too much traffic in fifo.\n"));
#ifdef OSIP_MT
      smutex_unlock (ff->qislocked);
#endif
      return -1;		/* stack is full */
    }
  /* if (ff->nb_elt >= MAX_LEN) */
  if (list_size (ff->queue) >= MAX_LEN)
    ff->etat = plein;
  else
    ff->etat = ok;

#ifdef OSIP_MT
  ssem_post (ff->qisempty);
  smutex_unlock (ff->qislocked);
#endif
  return 0;
}


int
fifo_insert (fifo_t * ff, void *el)
{
#ifdef OSIP_MT
  smutex_lock (ff->qislocked);
#endif

  if (ff->etat != plein)
    {
      /* ff->nb_elt++; */
      list_add (ff->queue, el, 0);	/* insert at end of queue */
    }
  else
    {
      OSIP_TRACE (osip_trace
		  (__FILE__, __LINE__, OSIP_WARNING, NULL,
		   "too much traffic in fifo.\n"));
#ifdef OSIP_MT
      smutex_unlock (ff->qislocked);
#endif
      return -1;		/* stack is full */
    }
  /* if (ff->nb_elt >= MAX_LEN) */
  if (list_size (ff->queue) >= MAX_LEN)
    ff->etat = plein;
  else
    ff->etat = ok;

#ifdef OSIP_MT
  ssem_post (ff->qisempty);
  smutex_unlock (ff->qislocked);
#endif
  return 0;
}


int
fifo_size (fifo_t * ff)
{
  int i;

#ifdef OSIP_MT
  smutex_lock (ff->qislocked);
#endif

  i = list_size (ff->queue);
#ifdef OSIP_MT
  smutex_unlock (ff->qislocked);
#endif
  return i;
}

#ifdef OSIP_MT

void *
fifo_get (fifo_t * ff)
{
  void *el;
  int i = ssem_wait (ff->qisempty);

  if (i != 0)
    return NULL;
  smutex_lock (ff->qislocked);

  if (ff->etat != vide)
    {
      el = list_get (ff->queue, 0);
      list_remove (ff->queue, 0);
      /* ff->nb_elt--; */
    }
  else
    {
      OSIP_TRACE (osip_trace
		  (__FILE__, __LINE__, OSIP_ERROR, NULL,
		   "no element in fifo.\n"));
      smutex_unlock (ff->qislocked);
      return 0;			/* pile vide */
    }
  /* if (ff->nb_elt <= 0) */
  if (list_size (ff->queue) <= 0)
    ff->etat = vide;
  else
    ff->etat = ok;

  smutex_unlock (ff->qislocked);
  return el;
}

#endif

void *
fifo_tryget (fifo_t * ff)
{
  void *el;

#ifdef OSIP_MT
  if (0 != ssem_trywait (ff->qisempty))
    {				/* no elements... */
      return NULL;
    }
  smutex_lock (ff->qislocked);
#else
  if (ff->etat == vide)
    return NULL;
#endif

  if (ff->etat != vide)
    {
      el = list_get (ff->queue, 0);
      list_remove (ff->queue, 0);
      /* ff->nb_elt--; */
    }
#ifdef OSIP_MT
  else
    {				/* this case MUST never happen... */
      OSIP_TRACE (osip_trace
		  (__FILE__, __LINE__, OSIP_INFO4, NULL,
		   "no element in fifo.\n"));
      smutex_unlock (ff->qislocked);
      return 0;
    }
#endif

  /* if (ff->nb_elt <= 0) */
  if (list_size (ff->queue) <= 0)
    ff->etat = vide;
  else
    ff->etat = ok;

#ifdef OSIP_MT
  smutex_unlock (ff->qislocked);
#endif
  return el;
}

void
fifo_free (fifo_t * ff)
{
  if (ff == NULL)
    return;
#ifdef OSIP_MT
  smutex_destroy (ff->qislocked);
#ifndef __VXWORKS_OS__
  sfree (ff->qislocked);
#endif
  /* seems that pthread_mutex_destroy does not free space by itself */
  ssem_destroy (ff->qisempty);
  sfree (ff->qisempty);
#endif
  sfree (ff->queue);
}
