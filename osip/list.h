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


#ifndef _LIST_H_
#define _LIST_H_

typedef struct _node_t {
  void *next;  /* next node_t */
  void *element;
} node_t;

typedef struct _list_t {

  int  nb_elt;
  node_t *node;

  /* would be cool to add ref to lastnode and curnode... */

} list_t;

int   list_init  (list_t *li);
void  list_special_free(list_t *li,void *(*free_func)(void *));
void  listofchar_free(list_t *li);
int   list_size  (list_t *li);
int   list_eol   (list_t *li,int i);
int   list_add   (list_t *li,void *el,int pos);
void *list_get   (list_t *li,int pos);
int   list_remove(list_t *li,int pos);


#endif
