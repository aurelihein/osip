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


#ifndef _THREAD_H_
#define _THREAD_H_


#ifdef ENABLE_MPATROL
#include <mpatrol.h>
#endif

#include <stdio.h>
#include <errno.h>

#ifdef __VXWORKS_OS__
#include <taskLib.h>
#endif

#if defined(WIN32) || defined(_WIN32_WCE)

#include <windows.h>
#endif

#if !defined(WIN32) && !defined(_WIN32_WCE) && defined(__PSOS__)
#include <psos.h>
#endif

#if !defined(WIN32) && !defined(_WIN32_WCE) && !defined(__VXWORKS_OS__) && !defined(__POS__)

/* HAVE_PTHREAD_H is not used any more! I keep it for a while... */
#if defined(HAVE_PTHREAD) || defined(HAVE_PTHREAD_H) || defined(HAVE_PTH_PTHREAD_H)
#include <pthread.h>
#endif
#endif

/**
 * @file thread.h
 * @brief oSIP Thread Routines
 *
 * Those methods are only available if the library is compile
 * in multi threaded mode. This is the default for oSIP.
 */

/**
 * @defgroup oSIP_THREAD oSIP Thread Routines
 * @ingroup oSIP
 * @{
 */

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __VXWORKS_OS__
  typedef struct _sthread_t
  {
    int id;
  }
  sthread_t;
#endif

#if defined(WIN32) || defined(_WIN32_WCE)
  typedef struct 
  {
    HANDLE h;
    unsigned id;
   }
   sthread_t;
  
#elif defined(__PSOS__)
    typedef struct
  {
    unsigned long tid;
  }
  sthread_t;
#endif

#if !defined(WIN32) && !defined(_WIN32_WCE) && !defined(__VXWORKS_OS__) && !defined(__POS__)
/* HAVE_PTHREAD_H is not used any more! I keep it for a while... */
#if defined(HAVE_PTHREAD) || defined(HAVE_PTHREAD_H) || defined(HAVE_PTH_PTHREAD_H)
/**
 * Structure for referencing a thread
 * @var sthread_t
 */
  typedef pthread_t sthread_t;
#else
#error no thread implementation found!
#endif
#endif

/**
 * Allocate (or initialise if a thread address is given)
 * @param stacksize The stack size of the thread. (20000 is a good value)
 * @param thread The thread to create. (if it is NULL, a new thread is returned)
 * @param func The method where the thread start.
 * @param arg A pointer on the argument given to the method 'func'.
 */
  sthread_t *sthread_create (int stacksize, sthread_t * thread,
			     void *(*func) (void *), void *arg);
/**
 * Join a thread.
 * @param thread The thread to join.
 */
  int sthread_join (sthread_t * thread);
/* this method is not implemented on all systems */
/**
 * Set the priority of a thread.
 * @param thread The thread to work on.
 * @param priority The priority value to set.
 */
  int sthread_setpriority (sthread_t * thread, int priority);
/**
 * Exit from a thread.
 */
  void sthread_exit ();

#ifdef __cplusplus
}
#endif


/** @} */


#endif				/* end of _THREAD_H_ */
