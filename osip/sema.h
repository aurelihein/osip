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

#ifndef _SEMA_H_
#define _SEMA_H_

#ifdef OSIP_MT

#include <osip/port.h>
#include <errno.h>

/**
 * @file sema.h
 * @brief oSIP semaphore definitions
 *
 * Those methods are only available if the library is compile
 * in multi threaded mode. This is the default for oSIP.
 */

/**
 * @defgroup oSIP_SEMA oSIP semaphore definitions
 * @ingroup oSIP
 * @{
 */

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(WIN32) || defined(_WIN32_WCE)
#include <windows.h>
  typedef struct
  {
    HANDLE h;
  }
  smutex_t;
  typedef struct
  {
    HANDLE h;
  }
  ssem_t;
#endif

#if (!(defined(WIN32) || defined (_WIN32_WCE)) && defined(__PSOS__))
#include <Types.h>
#include <os.h>
  typedef struct
  {
    UInt32 id;
  }
  smutex_t;
  typedef struct
  {
    UInt32 id;
  }
  ssem_t;
#endif

#ifdef __VXWORKS_OS__
#include <semaphore.h>
#include <semLib.h>
  typedef struct semaphore smutex_t;
  typedef sem_t ssem_t;
#endif

#ifdef __sun__
#include <semaphore.h>
#undef getdate
#include <synch.h>
#endif

/* HAVE_PTHREAD_H is not used any more! I keep it for a while... */
#if defined(HAVE_PTHREAD) || defined(HAVE_PTHREAD_H) || defined(HAVE_PTH_PTHREAD_H)
#include <pthread.h>
/**
 * Structure for referencing a semaphore element.
 * @defvar smutex_t
 */
  typedef pthread_mutex_t smutex_t;
#endif

#if defined(HAVE_SEMAPHORE_H)
#include <semaphore.h>
#ifdef __sun__
#undef getdate
#include <synch.h>
#endif
/**
 * Structure for referencing a semaphore element.
 * @defvar ssem_t
 */
  typedef sem_t ssem_t;
#endif

#if !defined(HAVE_SEMAPHORE_H) && defined(HAVE_SYS_SEM_H)
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
  typedef struct _ssem_t
  {
    int semid;
  }
  ssem_t;
#endif

#if (!defined(HAVE_SEMAPHORE_H) && !defined(HAVE_SYS_SEM_H) && !defined(WIN32) && !defined(_WIN32_WCE) && !defined(__PSOS__) && !defined(__VXWORKS_OS__))
#error No semaphore implementation found
#endif

/**
 * Allocate and Initialise a semaphore.
 */
  smutex_t *smutex_init ();
/**
 * Destroy the mutex.
 * @param mut The mutex to destroy.
 */
  void smutex_destroy (smutex_t * mut);
/**
 * Lock the mutex.
 * @param mut The mutex to lock.
 */
  int smutex_lock (smutex_t * mut);
/**
 * Unlock the mutex.
 * @param mut The mutex to unlock.
 */
  int smutex_unlock (smutex_t * mut);

/**
 * Allocate and Initialise a semaphore.
 * @param value The initial value for the semaphore.
 */
  ssem_t *ssem_init (unsigned int value);
/**
 * Destroy a semaphore.
 * @param sem The semaphore to destroy.
 */
  int ssem_destroy (ssem_t * sem);
/**
 * Post operation on a semaphore.
 * @param sem The semaphore to destroy.
 */
  int ssem_post (ssem_t * sem);
/**
 * Wait operation on a semaphore.
 * NOTE: this call will block if the semaphore is at 0.
 * @param sem The semaphore to destroy.
 */
  int ssem_wait (ssem_t * sem);
/**
 * Wait operation on a semaphore.
 * NOTE: if the semaphore is at 0, this call won't block.
 * @param sem The semaphore to destroy.
 */
  int ssem_trywait (ssem_t * sem);


#ifdef __cplusplus
}
#endif

/** @} */


#endif				/* OSIP_MT */

#endif				/* _SEMA_H_ */
