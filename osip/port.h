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


#ifndef _PORT_H_
#define _PORT_H_

#include <osip/const.h>

#include <stdio.h>
#include <osip/list.h>

#ifdef ENABLE_TRACE
#define DEBUG(P) P
#else
#define DEBUG(P) do { } while(0)
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

#if HAVE_STDARG_H
#  include <stdarg.h>
#  define VA_START(a, f)  va_start(a, f)
#else
#  if HAVE_VARARGS_H
#    include <varargs.h>
#    define VA_START(a, f) va_start(a)
#  endif
#endif

#if defined (HAVE_SYS_TIME_H)
#  include <sys/time.h>
#endif

#endif /* end of !__VXWORKS_OS__ */

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
extern "C" {
#endif

/**************************/
/* MALLOC redirections    */
/**************************/

void      *smalloc(size_t size);
void       sfree  (void *ptr);


/**************************/
/* RANDOM number support  */
/**************************/

unsigned int new_random_number();
void         init_random_number();

/**************************/
/* TIMER support          */
/**************************/

#define SP   " \0"

void       susleep(int useconds);

/**************************/
/* STRING support         */
/**************************/

int        satoi(char *number);
char      *sstrncpy(char *dest,const char *src,int length);
char      *sgetcopy(char *ch);
char      *sgetcopy_unquoted_string(char *ch);
int        stolowercase(char *word);
int        sclrspace(char *word);
/* used by the sdp parser: */
char      *sdp_append_string(char *string, int size,
			     char *cur, char *string_to_append);
int        set_next_token(char **dest, char *buf,
			  int end_separator, char **next);
/* find the next unescaped quote and  */
/* return its index.                  */
/* return NULL on error.              */
char      *quote_find(char *qstring);
int        sclrlws(char *word);

/**************************/
/* LOG&DEBUG support      */
/**************************/

/* define log possible value */
#ifdef WIN32
typedef unsigned char boolean;
#else
#ifndef __PSOS__
typedef int boolean;
#endif
#endif
#define LOG_TRUE  1
#define LOG_FALSE 0
/* levels */
typedef enum _trace_level {
  TRACE_LEVEL0      = 0,
  TRACE_LEVEL1      = 1,
  TRACE_LEVEL2      = 2,
  TRACE_LEVEL3      = 3,
  TRACE_LEVEL4      = 4,
  TRACE_LEVEL5      = 5,
  END_TRACE_LEVEL   = 6
} trace_level_t;

#ifndef ENABLE_TRACE
#define TRACE_INITIALIZE(level, file) do { \
fprintf(stderr, \
"Error: TRACE_INITIALIZE() is disabled. (use configure --enable-trace)\n"); \
                                      } while (0)
#define TRACE_ENABLE_LEVEL(level)      do { } while (0)
#define TRACE_DISABLE_LEVEL(level)     do { } while (0)
#define IS_TRACE_LEVEL_ACTIVATE(level) (-1)

#else

#define TRACE_INITIALIZE(level, file)  trace_initialize(level, file)
#define TRACE_ENABLE_LEVEL(level)      trace_enable_level ( level )
#define TRACE_DISABLE_LEVEL(level)     trace_disable_level ( level )
#define IS_TRACE_LEVEL_ACTIVATE(level) is_trace_level_activate( level )

extern void    trace_initialize        ( trace_level_t level, FILE *file );
extern void    trace_enable_level      ( trace_level_t level );
extern void    trace_disable_level     ( trace_level_t level );
extern boolean is_trace_level_activate ( trace_level_t level );
#endif

/* log facility. */
/* if f is NULL, current default log file is used. */
/* INPUT: level | level of the trace               */
/* INPUT: f | use f instead of default log file    */
/* INPUT: chfr | format string for next args       */
int
#if defined(HAVE_STDARG_H) || defined(__VXWORKS_OS__)
trace(char *fi, int li,trace_level_t level,FILE *f,char *chfr, ...);
#else
/* ERROR? I never tested this */
trace(fi,li,level,f,chfr,va_list);
#endif

#ifdef ENABLE_TRACE
#define TRACE(P) P
#else
#define TRACE(P) do {} while (0)
#endif

#ifdef __cplusplus
}
#endif

#endif  /* _PORT_H_ */
