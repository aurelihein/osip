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

#ifndef _SEMA_H_
#define _SEMA_H_

#ifdef OSIP_MT

#include <errno.h>
#ifdef WIN32
#include <Windows.h>
typedef struct {
    HANDLE h;
} smutex_t;
typedef struct {
    HANDLE h;
} ssem_t;
#else
#ifdef __PSOS__
#include <Types.h>
#include <os.h>
typedef struct {
    UInt32 id;
} smutex_t;
typedef struct {
    UInt32 id;
} ssem_t;
#else
#include <semaphore.h>
#endif
#endif

#ifdef __VXWORKS_OS__
#include <semLib.h>
typedef struct semaphore smutex_t;
typedef sem_t ssem_t;
#else

#ifdef __sun__
#undef getdate
#include <synch.h>
#endif

#ifdef THREAD_PTHREAD
#include <pthread.h>
typedef pthread_mutex_t smutex_t;
typedef sem_t  ssem_t;
#endif
#ifdef THREAD_PTH
#include <pth.h>
typedef pth_rwlock_t smutex_t;
typedef pth_mutex_t  ssem_t;
#endif

#endif

smutex_t     *smutex_init();
void         smutex_destroy(smutex_t *mut);
int          smutex_lock(smutex_t *mut);
int          smutex_unlock(smutex_t *mut);

ssem_t      *ssem_init(unsigned int value);
int          ssem_destroy(ssem_t *sem);
int          ssem_post(ssem_t *sem);
int          ssem_wait(ssem_t *sem);
int          ssem_trywait(ssem_t *sem);


#endif /* OSIP_MT */

#endif /* _SEMA_H_ */

