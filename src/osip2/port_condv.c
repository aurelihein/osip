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

#ifndef __PORT_CONDV_H__
#define __PORT_CONDV_H__

#ifdef ENABLE_MPATROL
#include <mpatrol.h>
#endif

#include <osip2/internal.h>
#include <osip2/osip_mt.h>
#include <osip2/osip_condv.h>

#if !defined(__VXWORKS_OS__) && !defined(WIN32) && !defined(_WIN32_WCE) && !defined(__PSOS__)
#if defined(HAVE_PTHREAD) || defined(HAVE_PTH_PTHREAD_H)

/*
  #include <sys/types.h>
  #include <sys/timeb.h>
*/
#include <pthread.h>

struct osip_cond *
osip_cond_init ()
{
  osip_cond_t *cond = (osip_cond_t *) osip_malloc (sizeof (osip_cond_t));
  if(cond && (pthread_cond_init(&cond->cv, NULL) == 0))
    {
      return (struct osip_cond *)(cond);
    }
  osip_free (cond);

  return NULL;
}

int
osip_cond_destroy (struct osip_cond * _cond)
{
  int ret;
  
  ret =	pthread_cond_destroy(&_cond->cv);
  osip_free(_cond);
  return ret;
}

int
osip_cond_signal (struct osip_cond * _cond)
{
  return pthread_cond_signal(&_cond->cv);
}


int
osip_cond_wait (struct osip_cond * _cond, struct osip_mutex * _mut)
{
  return pthread_cond_wait(&_cond->cv, (pthread_mutex_t *)_mut);
}


int
osip_cond_timedwait (struct osip_cond * _cond, struct osip_mutex * _mut,
                       const struct timespec *abstime)
{
 return pthread_cond_timedwait(&_cond->cv, (pthread_mutex_t *)_mut, 
	(const struct timespec *)abstime);
}
#else
#error NO thread implementation found
#endif
#endif

#if defined (WIN32) || defined (_WIN32_WCE)
#include <sys/types.h>
#include <sys/timeb.h>
struct osip_cond *
osip_cond_init ()
{
  osip_cond_t *cond = (osip_cond_t *) osip_malloc (sizeof (osip_cond_t));
  if((cond->mut = osip_mutex_init ()) != NULL)
    {
    cond->sem = osip_sem_init (0); /* initially locked */
    return (struct osip_cond *)(cond);
    }
  osip_free (cond);
 
  return NULL;
}

int
osip_cond_destroy (struct osip_cond * _cond)
{
   if(_cond->sem == NULL)
     return 0;

   osip_sem_destroy (_cond->sem);

   if(_cond->mut == NULL)
      return 0;

   osip_mutex_destroy (_cond->mut);

   return (0);
}

int
osip_cond_signal (struct osip_cond * _cond)
{
   return osip_sem_post(_cond->sem);
}


int
osip_cond_wait (struct osip_cond * _cond, struct osip_mutex * _mut)
{
   int ret1=0, ret2=0, ret3=0;

   if (osip_mutex_lock (_cond->mut))
      return -1;

   if (osip_mutex_unlock (_mut))
      return -1;

   ret1 = osip_sem_wait (_cond->sem);

   ret2 = osip_mutex_lock (_mut);

   ret3 = osip_mutex_unlock (_cond->mut);

   if (ret1 || ret2 || ret3)
      return -1;
   return 0;
}

#define OSIP_CLOCK_REALTIME 4002

int
__osip_clock_gettime (unsigned int clock_id, struct timespec *tp)
{
   struct _timeb time_val;

   if (clock_id != OSIP_CLOCK_REALTIME)
     return -1;

   if (tp == NULL)
     return -1;

   _ftime (&time_val);
   tp->tv_sec = time_val.time;
   tp->tv_nsec = time_val.millitm * 1000000;
   return 0;
}

static int
_delta_time (const struct timespec *start, const struct timespec *end)
{
   int difx;

   difx = ((end->tv_sec - start->tv_sec) * 1000) +
           ((end->tv_nsec - start->tv_nsec) / 1000000);

   return difx;
}


int
osip_cond_timedwait (struct osip_cond * _cond, struct osip_mutex * _mut,
                        const struct timespec *abstime)
{
   DWORD dwRet;
   struct timespec now;
   int timeout_ms;
   HANDLE sem = *((HANDLE *)_cond->sem);

   if (sem == NULL)
     return -1;

   if (abstime == NULL)
     return -1;

   __osip_clock_gettime (OSIP_CLOCK_REALTIME, &now);

   timeout_ms = _delta_time (&now, abstime);
   if (timeout_ms <= 0)
     return 1; /* ETIMEDOUT; */

   if (osip_mutex_unlock (_mut))
     return -1;

   dwRet = WaitForSingleObject (sem, timeout_ms);

   if (osip_mutex_lock (_mut))
     return -1;

   switch(dwRet)
   {
      case WAIT_OBJECT_0:
         return 0;
         break;
      case WAIT_TIMEOUT:
	     return 1; /* ETIMEDOUT; */
         break;
      default:
         return -1;
         break;
   }
}
#endif

#ifdef __PSOS__
/*TODO*/
#endif

/* use VxWorks implementation */
#ifdef __VXWORKS_OS__
/*TODO*/
#endif

#endif

#endif

