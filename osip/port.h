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


#ifndef _PORT_H_
#define _PORT_H_

#include <osip/const.h>

#include <stdio.h>
#include <osip/list.h>

#ifdef WIN32

#define STDC_HEADERS 1
#define HAVE_CTYPE_H 1
#define HAVE_STRING_H 1
#define HAVE_SYS_TYPES_H 1
#define HAVE_TIME_H 1
#define HAVE_STDARG_H 1

#elif defined _WIN32_WCE

#define STDC_HEADERS 1
#define HAVE_CTYPE_H 1
#define HAVE_STRING_H 1
#define HAVE_TIME_H 1
#define HAVE_STDARG_H 1

#define strnicmp	_strnicmp
#define stricmp		_stricmp
#define EBUSY           16

#endif

#ifdef __VXWORKS_OS__

#include <string.h>
#include <time.h>
#include <stdarg.h>
#define VA_START(a, f)  va_start(a, f)

#else /* end of __VXWORKS_OS__ */

#if defined (HAVE_CONFIG_H)
#include <config.h>
#  if defined (HAVE_STRING_H)
#    include <string.h>
#  else
#    include <strings.h>
#  endif /* HAVE_STRING_H */
#else
#  include <string.h>
#endif /* !HAVE_CONFIG_H */

#if defined (HAVE_SYS_TYPES_H)
#  include <sys/types.h>
#endif

#if STDC_HEADERS
#    include <stdlib.h>
#endif /* !STDC_HEADERS */

#if defined(HAVE_STDARG_H) || defined(WIN32)
#  include <stdarg.h>
#  define VA_START(a, f)  va_start(a, f)
#else
#  if HAVE_VARARGS_H
#    include <varargs.h>
#    define VA_START(a, f) va_start(a)
#  endif
#endif

#ifdef HAVE_TIME_H
#  include <time.h>
#endif

#if defined (HAVE_SYS_TIME_H)
#  include <sys/time.h>
#endif

#endif /* end of !__VXWORKS_OS__ */

#ifdef _WIN32_WCE
#define VA_START(a, f)  va_start(a, f)
#endif

#ifdef WIN32
#define VA_START(a, f)  va_start(a, f)
#endif

#ifdef __PSOS__
#define VA_START(a, f)  va_start(a, f)
#endif

#ifndef VA_START
#  error no variadic api
#endif

#if __STDC__
#  ifndef NOPROTOS
#    define PARAMS(args)   args
#  endif
#endif

#ifndef PARAMS
#  define PARAMS(args)     ()
#endif


#define SIP_SYNTAX_ERROR    (-1)
#define SIP_NETWORK_ERROR   (-2)
#define SIP_ECONNREFUSED    (-3)
#define SIP_RESSOURCE_ERROR (-4)
#define SIP_GLOBAL_ERROR    (-5)

#ifdef __cplusplus
extern "C"
{
#endif

/**************************/
/* MALLOC redirections    */
/**************************/

  void *smalloc (size_t size);
  void sfree (void *ptr);


/**************************/
/* RANDOM number support  */
/**************************/

  unsigned int new_random_number ();
  void init_random_number ();

/**************************/
/* TIMER support          */
/**************************/

#define SP   " \0"

  void susleep (int useconds);

/**************************/
/* STRING support         */
/**************************/

  int satoi (char *number);
  char *sstrncpy (char *dest, const char *src, int length);
  char *sgetcopy (const char *ch);
  char *sgetcopy_unquoted_string (const char *ch);
  int stolowercase (char *word);
  int sclrspace (char *word);
/* used by the sdp parser: */
  char *sdp_append_string (char *string, int size,
			   char *cur, char *string_to_append);
  int set_next_token (char **dest, char *buf, int end_separator, char **next);
/* find the next unescaped quote and  */
/* return its index.                  */
/* return NULL on error.              */
  char *quote_find (char *qstring);
  int sclrlws (char *word);

/**************************/
/* LOG&DEBUG support      */
/**************************/

/* define log possible value */
#if defined (WIN32) || defined (_WIN32_WCE)
  typedef unsigned char boolean;
#else
#ifndef __PSOS__
  typedef int boolean;
#endif
#endif

#define LOG_TRUE  1
#define LOG_FALSE 0
/* levels */
  typedef enum _trace_level
  {
    TRACE_LEVEL0 = 0,
#define OSIP_FATAL    0
    TRACE_LEVEL1 = 1,
#define OSIP_BUG      1
    TRACE_LEVEL2 = 2,
#define OSIP_ERROR    2
    TRACE_LEVEL3 = 3,
#define OSIP_WARNING  3
    TRACE_LEVEL4 = 4,
#define OSIP_INFO1    4
    TRACE_LEVEL5 = 5,
#define OSIP_INFO2    5
    TRACE_LEVEL6 = 6,
#define OSIP_INFO3    6
    TRACE_LEVEL7 = 7,
#define OSIP_INFO4    7
    END_TRACE_LEVEL = 8
  }
  trace_level_t;

/* these are defined in all cases, but are empty when oSIP is compiled
   without trace */
  void trace_initialize_syslog (trace_level_t level, char* ident);
  void trace_initialize (trace_level_t level, FILE * file);
  void trace_enable_level (trace_level_t level);
  void trace_disable_level (trace_level_t level);
  boolean is_trace_level_activate (trace_level_t level);

#ifndef ENABLE_TRACE

#define TRACE_INITIALIZE(level, file)  do { } while (0)
#define TRACE_ENABLE_LEVEL(level)      do { } while (0)
#define TRACE_DISABLE_LEVEL(level)     do { } while (0)
#define IS_TRACE_LEVEL_ACTIVATE(level) (-1)

#else

#define TRACE_INITIALIZE(level, file)  trace_initialize(level, file)
#define TRACE_ENABLE_LEVEL(level)      trace_enable_level ( level )
#define TRACE_DISABLE_LEVEL(level)     trace_disable_level ( level )
#define IS_TRACE_LEVEL_ACTIVATE(level) is_trace_level_activate( level )

#endif

/* log facility. */
/* if f is NULL, current default log file is used. */
/* INPUT: level | level of the trace               */
/* INPUT: f | use f instead of default log file    */
/* INPUT: chfr | format string for next args       */
  int
#if defined(HAVE_STDARG_H) || defined(WIN32) || defined(__VXWORKS_OS__)
   
    osip_trace (char *fi, int li, trace_level_t level, FILE * f, char *chfr,
		...);
#else
/* ERROR? I never tested this */
    osip_trace (fi, li, level, f, chfr, va_list);
#endif

#ifdef ENABLE_TRACE

  /* #ifndef TRACE ??? quick and temporary hack for VC++ */
#define OSIP_TRACE(P) P
  /* #endif */

#else

#define OSIP_TRACE(P) do {} while (0)

#endif

#ifdef __cplusplus
}
#endif

#endif				/* _PORT_H_ */
