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

#ifndef _FIFO_H_
#define _FIFO_H_

#ifdef OSIP_MT
#include <osip/sema.h>
#endif
#include <osip/list.h>


#define MAX_LEN 1000
typedef enum {ok , plein , vide} fifo_etat;
typedef struct  {

#ifdef OSIP_MT
  smutex_t *qislocked;
  ssem_t  *qisempty;
#endif
  list_t *queue;
  int     nb_elt;
  fifo_etat etat;
  
} fifo_t;

void  fifo_init(fifo_t *ff);
void  fifo_free(fifo_t *ff);
int   fifo_add(fifo_t *ff , void *el);
#ifdef OSIP_MT
void *fifo_get(fifo_t *ff);
#endif
void *fifo_tryget(fifo_t *ff);


#endif
