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

#ifdef OSIP_MT

#include <stdlib.h>
#include <stdio.h>

#include <osip/port.h>
#include <osip/sema.h>

/* Posix Thread API (use pthread.h) */
#if defined(THREAD_PTHREAD) && !defined(__VXWORKS_OS__)
smutex_t * smutex_init() {
  smutex_t *mut = (smutex_t *)smalloc(sizeof(smutex_t));
  pthread_mutex_init(mut, NULL);
  return mut;
}
void smutex_destroy(smutex_t *mut) { pthread_mutex_destroy(mut); }
int  smutex_lock(smutex_t *mut) {
  int i;
  i = pthread_mutex_lock(mut);
  if (i!=0)
    {
      fprintf(stdout, "ERROR while locking !!%i",i);
      if (i==EINVAL) fprintf(stdout, "ERROR while locking EINVAL!!");
      if (i==EDEADLK) fprintf(stdout, "ERROR while locking EDEADLK!!");
      if (i==EBUSY) fprintf(stdout, "ERROR while locking EBUSY!!");
      if (i==EINVAL) fprintf(stdout, "ERROR while locking EINVAL!!");
      return i;
    }
  return 0;
}

int smutex_unlock(smutex_t *mut) { return pthread_mutex_unlock(mut); }

/* Counting Semaphore is initialized to value */
ssem_t *ssem_init(unsigned int value) {
  ssem_t *sem = (ssem_t *)smalloc(sizeof(ssem_t));
  if (sem_init(sem, 0, value)==0) return sem;
  sfree(sem);
  return NULL;
}
int ssem_destroy(ssem_t *sem)           { return sem_destroy(sem); }
int ssem_post(ssem_t *sem)              { return sem_post(sem); }
int ssem_wait(ssem_t *sem)              { return sem_wait(sem); }
int ssem_trywait(ssem_t *sem)           { return sem_trywait(sem); }
#endif

#if defined(THREAD_PTH) && !defined(__VXWORKS_OS__)
smutex_t * smutex_init() {
  smutex_t *mut = (smutex_t *)smalloc(sizeof(smutex_t));
  pth_rwlock_init(mut);
  return mut;
}
void smutex_destroy(smutex_t *mut) { /* pth_mutex_destroy(mut);???? */ }
int  smutex_lock(smutex_t *mut) {
  int i;
  i = pth_rwlock_acquire(mut,PTH_RWLOCK_RW,FALSE,NULL);
  if (i==0)
    {
      fprintf(stdout, "ERROR while locking !!%i",errno);
      if (errno==EINVAL) fprintf(stdout, "ERROR while locking EINVAL!!");
      if (errno==EDEADLK) fprintf(stdout, "ERROR while locking EDEADLK!!");
      if (errno==EBUSY) fprintf(stdout, "ERROR while locking EBUSY!!");
      if (errno==EINVAL) fprintf(stdout, "ERROR while locking EINVAL!!");
      return errno;
    }
  return 0;
}

int smutex_unlock(smutex_t *mut)
{ return pth_rwlock_release(mut); }

/* Counting Semaphore is initialized to value */
ssem_t *ssem_init(unsigned int value) {
  ssem_t *sem = (ssem_t *)smalloc(sizeof(ssem_t));
  if (pth_mutex_init(sem)==0) return sem;
  sfree(sem);
  return NULL;
}
/* this method does not exist?? */
int ssem_destroy(ssem_t *sem)           { return 0; /* sem_destroy(sem);????? */ }
int ssem_post(ssem_t *sem)              { return pth_mutex_acquire(sem,TRUE,NULL); }
int ssem_wait(ssem_t *sem)              { return pth_mutex_release(sem); }
int ssem_trywait(ssem_t *sem)           { return pth_mutex_acquire(sem,FALSE,NULL); }

#endif



/* use VxWorks implementation */
#if defined(__VXWORKS_OS__) && defined(THREAD_PTHREAD)
smutex_t *smutex_init()                 { return semMCreate(0); }
void      smutex_destroy(smutex_t *mut) { semDelete(mut); }
int       smutex_lock(smutex_t *mut)    { return semTake(mut,WAIT_FOREVER); }
int       smutex_unlock(smutex_t *mut)  { return semGive(mut); }
#endif

#ifdef __VXWORKS_OS__
/* Counting Semaphore is initialized to value */
ssem_t *  ssem_init(unsigned int value) { return semCCreate(SEM_Q_FIFO,value); }
int       ssem_destroy(ssem_t *sem)     { semDelete(sem); return 0; }
int       ssem_post(ssem_t *sem)        { return semGive(sem); }
int       ssem_wait(ssem_t *sem)        { return semTake(sem,WAIT_FOREVER); }
int       ssem_trywait(ssem_t *sem)     { return semTake(sem,NO_WAIT); }
#endif

#ifdef WIN32
#include <limits.h>
smutex_t *smutex_init()
{
  smutex_t *mut = (smutex_t *)smalloc(sizeof(smutex_t));

  if ((mut->h = CreateMutex(NULL, FALSE, NULL)) != NULL) {
     return(mut);
  }

  sfree(mut);
  return(NULL);
}

void smutex_destroy(smutex_t *mut)
{
  CloseHandle(mut->h);
}

int smutex_lock(smutex_t *mut)
{
  DWORD err;

  if ((err = WaitForSingleObject(mut->h, INFINITE)) == WAIT_OBJECT_0) {
      return(0);
  }

  if (err == WAIT_TIMEOUT) {
      return(EBUSY);
  }

  return(EBUSY);
}

int smutex_unlock(smutex_t *mut)
{
  ReleaseMutex(mut->h);

  return(0);
}

ssem_t *ssem_init(unsigned int value)
{
  ssem_t *sem = (ssem_t *)smalloc(sizeof(ssem_t));
  
  if ((sem->h = CreateSemaphore(NULL, value, LONG_MAX, NULL)) != NULL) {
      return(sem);
  }
  
  sfree(sem);
  return(NULL);
}

int ssem_destroy(ssem_t *sem)
{
  CloseHandle(sem->h);
  
  return(0);
}

int ssem_post(ssem_t *sem)
{
  ReleaseSemaphore(sem->h, 1, NULL);

  return(0);
}

int ssem_wait(ssem_t *sem)
{
  DWORD err;

  if ((err = WaitForSingleObject(sem->h, INFINITE)) == WAIT_OBJECT_0) {
      return(0);
  }

  if (err == WAIT_TIMEOUT) {
      return(EBUSY);
  }

  return(EBUSY);
}

int ssem_trywait(ssem_t *sem)
{
  DWORD err;

  if ((err = WaitForSingleObject(sem->h, 0)) == WAIT_TIMEOUT) {
      return(-1);
  }

  if (err == WAIT_TIMEOUT) {
      return(0);
  }

  return(EBUSY);
}
#endif

#ifdef __PSOS__
smutex_t *smutex_init()
{
  smutex_t *mut = (smutex_t *)smalloc(sizeof(smutex_t));
  
   if (sm_create("mut", 1, 0, &mut->id) == 0)
      return(mut);
  
  sfree(mut);
  return(NULL);
}

void smutex_destroy(smutex_t *mut)
{
  if (mut) {
    sm_delete(mut->id);
    sfree(mut);
  }
}

int smutex_lock(smutex_t *mut)
{
  if (mut) {
    if (sm_p(mut->id, SM_WAIT, 0) != 0)
        return(-1);
  }

  return(0);
}

int smutex_unlock(smutex_t *mut)
{
  if (mut) {
    sm_v(mut->id);
  }

  return(0);
}

ssem_t *ssem_init(unsigned int value)
{
  ssem_t *sem = (ssem_t *)smalloc(sizeof(ssem_t));
  
  if (sm_create("sem", value, 0, &sem->id) == 0)
      return(sem);
  
  sfree(sem);
  return(NULL);
}

int ssem_destroy(ssem_t *sem)
{
  sm_delete(sem->id);
  sfree(sem);

  return(0);
}

int ssem_post(ssem_t *sem)
{
  return(sm_v(sem->id));
}

int ssem_wait(ssem_t *sem)
{
  if (sm_p(sem->id, SM_WAIT, 0) != 0)
      return(-1);
  return(0);
}

int ssem_trywait(ssem_t *sem)
{
  if (sm_p(sem->id, SM_NOWAIT, 0) != 0)
      return(-1);
  return(0);
}
#endif

#endif
