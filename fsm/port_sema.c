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

#ifdef OSIP_MT

#include <stdlib.h>
#include <stdio.h>

#include <osip/port.h>
#include <osip/sema.h>

#if !defined(__VXWORKS_OS__) && !defined(WIN32) && !defined(_WIN32_WCE) && !defined(__PSOS__)
#if defined(HAVE_PTHREAD) || defined(HAVE_PTH_PTHREAD_H)

smutex_t *
smutex_init ()
{
  smutex_t *mut = (smutex_t *) smalloc (sizeof (smutex_t));

  if (mut == NULL)
    return NULL;
  pthread_mutex_init (mut, NULL);
  return mut;
}

void
smutex_destroy (smutex_t * mut)
{
  if (mut == NULL)
    return;
  pthread_mutex_destroy (mut);
}

int
smutex_lock (smutex_t * mut)
{
  if (mut == NULL)
    return -1;
  return pthread_mutex_lock (mut);
}

int
smutex_unlock (smutex_t * mut)
{
  if (mut == NULL)
    return -1;
  return pthread_mutex_unlock (mut);
}

#else
#error NO thread implementation found
#endif

#if defined (HAVE_SEMAPHORE_H)

/* Counting Semaphore is initialized to value */
ssem_t *
ssem_init (unsigned int value)
{
  ssem_t *sem = (ssem_t *) smalloc (sizeof (ssem_t));

  if (sem_init (sem, 0, value) == 0)
    return sem;
  sfree (sem);
  return NULL;
}

int
ssem_destroy (ssem_t * sem)
{
  if (sem == NULL)
    return 0;
  return sem_destroy (sem);
}

int
ssem_post (ssem_t * sem)
{
  if (sem == NULL)
    return -1;
  return sem_post (sem);
}

int
ssem_wait (ssem_t * sem)
{
  if (sem == NULL)
    return -1;
  return sem_wait (sem);
}

int
ssem_trywait (ssem_t * sem)
{
  if (sem == NULL)
    return -1;
  return sem_trywait (sem);
}
#endif

#if defined (HAVE_SYS_SEM_H) && !defined (HAVE_SEMAPHORE_H)
/* support for semctl, semop, semget */

#define SEM_PERM 0600

ssem_t *
ssem_init (unsigned int value)
{
  int i;
  ssem_t *sem = (ssem_t *) smalloc (sizeof (ssem_t));

  sem->semid = semget (IPC_PRIVATE, 1, IPC_CREAT | SEM_PERM);
  if (sem->semid == -1)
    {
      perror ("semget error");
      sfree (sem);
      return NULL;
    }
  i = semctl (sem->semid, 0, SETVAL, value);
  if (i != 0)
    {
      perror ("semctl error");
      sfree (sem);
      return NULL;
    }
  return sem;
}

int
ssem_destroy (ssem_t * sem)
{
  if (sem == NULL)
    return 0;
  return semctl (sem->semid, 0, IPC_RMID);
}

int
ssem_post (ssem_t * sem)
{
  struct sembuf sb;

  if (sem == NULL)
    return -1;
  sb.sem_num = 0;
  sb.sem_op = 1;
  sb.sem_flg = 0;
  return semop (sem->semid, &sb, 1);
}

int
ssem_wait (ssem_t * sem)
{
  struct sembuf sb;

  if (sem == NULL)
    return -1;
  sb.sem_num = 0;
  sb.sem_op = -1;
  sb.sem_flg = 0;
  return semop (sem->semid, &sb, 1);
}

int
ssem_trywait (ssem_t * sem)
{
  struct sembuf sb;

  if (sem == NULL)
    return -1;
  sb.sem_num = 0;
  sb.sem_op = -1;
  sb.sem_flg = IPC_NOWAIT;
  return semop (sem->semid, &sb, 1);
}
#endif
#endif

/* use VxWorks implementation */
#ifdef __VXWORKS_OS__
smutex_t *
smutex_init ()
{
  return semMCreate (0);
}

void
smutex_destroy (smutex_t * mut)
{
  if (mut == NULL)
    return;
  semDelete (mut);
}

int
smutex_lock (smutex_t * mut)
{
  if (mut == NULL)
    return -1;
  return semTake (mut, WAIT_FOREVER);
}

int
smutex_unlock (smutex_t * mut)
{
  if (mut == NULL)
    return -1;
  return semGive (mut);
}

ssem_t *
ssem_init (unsigned int value)
{
  SEM_ID initsem;
  ssem_t *x;

  x = (ssem_t *) smalloc (sizeof (ssem_t));
  if (x == NULL)
    return NULL;
  initsem = semCCreate (SEM_Q_FIFO, value);
  x->semId = initsem;
  x->refCnt = value;
  x->sem_name = NULL;
  return x;
}

int
ssem_destroy (ssem_t * sem)
{
  if (sem == NULL)
    return 0;
  semDelete (sem->semId);
  return 0;
}

int
ssem_post (ssem_t * sem)
{
  if (sem == NULL)
    return -1;
  return semGive (sem->semId);
}

int
ssem_wait (ssem_t * sem)
{
  if (sem == NULL)
    return -1;
  return semTake (sem->semId, WAIT_FOREVER);
}

int
ssem_trywait (ssem_t * sem)
{
  if (sem == NULL)
    return -1;
  return semTake (sem->semId, NO_WAIT);
}
#endif

#if defined (WIN32) || defined (_WIN32_WCE)

#include <limits.h>
smutex_t *
smutex_init ()
{
  smutex_t *mut = (smutex_t *) smalloc (sizeof (smutex_t));

  if ((mut->h = CreateMutex (NULL, FALSE, NULL)) != NULL)
    return (mut);
  sfree (mut);
  return (NULL);
}

void
smutex_destroy (smutex_t * mut)
{
  if (mut == NULL)
    return;
  CloseHandle (mut->h);
}

int
smutex_lock (smutex_t * mut)
{
  DWORD err;

  if (mut == NULL)
    return -1;
  if ((err = WaitForSingleObject (mut->h, INFINITE)) == WAIT_OBJECT_0)
    return (0);
  return (EBUSY);
}

int
smutex_unlock (smutex_t * mut)
{
  if (mut == NULL)
    return -1;
  ReleaseMutex (mut->h);
  return (0);
}

ssem_t *
ssem_init (unsigned int value)
{
  ssem_t *sem = (ssem_t *) smalloc (sizeof (ssem_t));

  if ((sem->h = CreateSemaphore (NULL, value, LONG_MAX, NULL)) != NULL)
    return (sem);
  sfree (sem);
  return (NULL);
}

int
ssem_destroy (ssem_t * sem)
{
  if (sem == NULL)
    return 0;
  CloseHandle (sem->h);
  return (0);
}

int
ssem_post (ssem_t * sem)
{
  if (sem == NULL)
    return -1;
  ReleaseSemaphore (sem->h, 1, NULL);
  return (0);
}

int
ssem_wait (ssem_t * sem)
{
  DWORD err;

  if (sem == NULL)
    return -1;
  if ((err = WaitForSingleObject (sem->h, INFINITE)) == WAIT_OBJECT_0)
    return (0);
  if (err == WAIT_TIMEOUT)
    return (EBUSY);
  return (EBUSY);
}

int
ssem_trywait (ssem_t * sem)
{
  DWORD err;

  if (sem == NULL)
    return -1;
  if ((err = WaitForSingleObject (sem->h, 0)) == WAIT_OBJECT_0)
    return (0);
  return (EBUSY);
}
#endif

#ifdef __PSOS__
smutex_t *
smutex_init ()
{
  smutex_t *mut = (smutex_t *) smalloc (sizeof (smutex_t));

  if (sm_create ("mut", 1, 0, &mut->id) == 0)
    return (mut);
  sfree (mut);
  return (NULL);
}

void
smutex_destroy (smutex_t * mut)
{
  if (mut)
    {
      sm_delete (mut->id);
      /* sfree (mut); extra free removed */
    }
}

int
smutex_lock (smutex_t * mut)
{
  if (mut)
    {
      if (sm_p (mut->id, SM_WAIT, 0) != 0)
	return (-1);
    }
  return (0);
}

int
smutex_unlock (smutex_t * mut)
{
  if (mut)
    {
      sm_v (mut->id);
    }

  return (0);
}

ssem_t *
ssem_init (unsigned int value)
{
  ssem_t *sem = (ssem_t *) smalloc (sizeof (ssem_t));

  if (sm_create ("sem", value, 0, &sem->id) == 0)
    return (sem);
  sfree (sem);
  return (NULL);
}

int
ssem_destroy (ssem_t * sem)
{
  if (sem == NULL)
    return 0;
  sm_delete (sem->id);
  /* sfree (sem); */
  return (0);
}

int
ssem_post (ssem_t * sem)
{
  if (sem == NULL)
    return -1;
  return (sm_v (sem->id));
}

int
ssem_wait (ssem_t * sem)
{
  if (sem == NULL)
    return -1;
  if (sm_p (sem->id, SM_WAIT, 0) != 0)
    return (-1);
  return (0);
}

int
ssem_trywait (ssem_t * sem)
{
  if (sem == NULL)
    return -1;
  if (sm_p (sem->id, SM_NOWAIT, 0) != 0)
    return (-1);
  return (0);
}
#endif

#endif
