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


#ifndef _LIST_H_
#define _LIST_H_

#ifdef ENABLE_MPATROL
#include <mpatrol.h>
#endif

/**
 * @file list.h
 * @brief oSIP list Routines
 *
 * This is a very simple implementation of a linked list.
 * <BR>There is not much to say about it... Except that it
 * could be a lot improved. Sadly, it would be difficult
 * to improve it without breaking the compatibility with
 * older version!
 */

/**
 * @defgroup oSIP_LIST oSIP list Handling
 * @ingroup oSIP
 * @{
 */

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef DOXYGEN
/**
 * Structure for referencing a node in a list_t element.
 * @defvar node_t
 */
  typedef struct node_t node_t;

  struct node_t
  {
    void *next;			/* next node_t */
    void *element;
  };
#endif

/**
 * Structure for referencing a list of elements.
 * @defvar list_t
 */
  typedef struct list_t list_t;

  struct list_t
  {

    int nb_elt;
    node_t *node;

  };

/**
 * Initialise a list_t element.
 * NOTE: this element MUST be previously allocated.
 * @param li The element to initialise.
 */
  int list_init (list_t * li);
/**
 * Free a list of element.
 * Each element will be free with the method given as the second parameter.
 * @param li The element to work on.
 * @param free_func The method that is able to release one element of the list.
 */
  void list_special_free (list_t * li, void *(*free_func) (void *));
/**
 * Free a list of element where elements are pointer to 'char'.
 * @param li The element to work on.
 */
  void listofchar_free (list_t * li);
/**
 * Get the size of a list of element.
 * @param li The element to work on.
 */
  int list_size (list_t * li);
/**
 * Check if the end of list is detected .
 * @param li The element to work on.
 * @param pos The index of the possible element.
 */
  int list_eol (list_t * li, int pos);
/**
 * Add an element in a list.
 * @param li The element to work on.
 * @param element The pointer on the element to add.
 * @param pos the index of the element to add. (or -1 to append the element at the end)
 */
  int list_add (list_t * li, void *element, int pos);
/**
 * Get an element from a list.
 * @param li The element to work on.
 * @param pos the index of the element to get.
 */
  void *list_get (list_t * li, int pos);
/**
 * Remove an element from a list.
 * @param li The element to work on.
 * @param pos the index of the element to remove.
 */
  int list_remove (list_t * li, int pos);


#ifdef __cplusplus
}
#endif


/** @} */

#endif
