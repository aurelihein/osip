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


#ifndef _THREAD_H_
#define _THREAD_H_
#include <stdio.h>
#include <errno.h>


#ifdef __VXWORKS_OS__
#include <taskLib.h>
typedef int sthread_t;
#else
#ifdef WIN32
#include <windows.h>
typedef struct {
    unsigned long h;
    unsigned int ID;
} sthread_t;
#else
#ifdef __PSOS__
#include <psos.h>
typedef struct {
    unsigned long tid;
} sthread_t;
#else
#ifdef THREAD_PTHREAD
#include <pthread.h>
typedef pthread_t sthread_t;
#endif
#ifdef THREAD_PTH
#include <pth.h>
typedef struct pth_st sthread_t;
#endif
#endif
#endif

#endif

#ifdef __VXWORKS_OS__
int sthread_create(int stacksize,
                   sthread_t *thread,  /* MUST BE NULL ON VxWorks */
                   void *(*func)(void *), void *arg);
int sthread_join(int thread);
int sthread_setpriority(int thread, int priority);
void sthread_exit();
#endif
#if !defined(__VXWORKS_OS__)
#if defined(THREAD_PTH) || defined(THREAD_PTHREAD) || defined(WIN32)
sthread_t *sthread_create(int stacksize, sthread_t *thread,
                          void *(*func)(void *),  void *arg);
int sthread_join(sthread_t *thread);
/* WARNING UNUSED METHOD */
int sthread_setpriority(sthread_t *thread, int priority);
void sthread_exit();
#endif
#endif




#endif /* end of _THREAD_H */
