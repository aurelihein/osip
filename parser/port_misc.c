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

#ifdef _WIN32_WCE
#define _INC_TIME // for wce.h
#include <windows.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include <osip/port.h>

#ifdef HAVE_CTYPE_H
#include <ctype.h>
#endif

#include <time.h>

#if defined(__VXWORKS_OS__)
#include <selectLib.h>
#elif (!defined(WIN32) && !defined(_WIN32_WCE))
#include <sys/time.h>
#elif defined(WIN32)
#include <windows.h>
#endif

#if defined (HAVE_SYS_UNISTD_H)
#  include <sys/unistd.h>
#endif

#if defined (HAVE_UNISTD_H)
#  include <unistd.h>
#endif

#if defined (HAVE_SYSLOG_H)
#  include <syslog.h>
#endif

#if defined (HAVE_SYS_SELECT_H)
#  include <sys/select.h>
#endif

#ifdef HAVE_PTH_PTHREAD_H
#include <pthread.h>
#endif

FILE *logfile = NULL;
int tracing_table[END_TRACE_LEVEL];
static int use_syslog = 0;

/* a,b, and x are used to generate random callid with a long period. */
static unsigned int a = 0;
static unsigned int b = 0;
static unsigned int x = 0;

void
init_random_number ()
{
  srand ((unsigned int) time (NULL));
  a = 4 * (1 + (int) (20000000.0 * rand () / (RAND_MAX + 1.0))) + 1;
  b = 2 * (1 + (int) (30324000.0 * rand () / (RAND_MAX + 1.0))) + 1;
  x = (1 + (int) (23445234.0 * rand () / (RAND_MAX + 1.0)));
}

unsigned int
new_random_number ()
{
  x = a * x + b;
  return x;
}

#if defined(__linux)
#include <limits.h>
#endif

int
satoi (char *number)
{
#if defined(__linux) || defined(HAVE_STRTOL)
  int i;

  i = strtol (number, (char **) NULL, 10);
  if (i == LONG_MIN || i == LONG_MAX)
    return -1;
  return i;
#endif

  return atoi (number);
}

char *
sstrncpy (char *dest, const char *src, int length)
{
  memcpy (dest, src, length);
  dest[length] = '\0';
  return dest;
}

/* append string_to_append to string at position cur
   size is the current allocated size of the element
*/
char *
sdp_append_string (char *string, int size, char *cur, char *string_to_append)
{
  int length = strlen (string_to_append);

  if (cur + length - string > size)
    {
      int length2;

      length2 = cur - string;
      string = realloc (string, size + length + 10);
      cur = string + length2;	/* the initial allocation may have changed! */
    }
  sstrncpy (cur, string_to_append, length);
  return cur + strlen (cur);
}

void
susleep (int useconds)
{
#ifdef WIN32
  Sleep (useconds / 1000);
#else
  struct timeval delay;
  int sec;

  sec = (int) useconds / 1000000;
  if (sec > 0)
    {
      delay.tv_sec = sec;
      delay.tv_usec = 0;
    }
  else
    {
      delay.tv_sec = 0;
      delay.tv_usec = useconds;
    }
  select (0, 0, 0, 0, &delay);
#endif
}


char *
sgetcopy (const char *ch)
{
  char *copy;

  if (ch == NULL)
    return NULL;
  copy = (char *) smalloc (strlen (ch) + 1);
  sstrncpy (copy, ch, strlen (ch));
  return copy;
}

char *
sgetcopy_unquoted_string (const char *ch)
{
  char *copy = (char *) smalloc (strlen (ch) + 1);

  /* remove leading and trailing " */
  if ((*ch == '\"'))
    {
      sstrncpy (copy, ch + 1, strlen (ch + 1));
      sstrncpy (copy + strlen (copy) - 1, "\0", 1);
    }
  else
    sstrncpy (copy, ch, strlen (ch));
  return copy;
}

int
stolowercase (char *word)
{
#ifdef HAVE_CTYPE_H
  for (; *word; word++)
    *word = (char) tolower (*word);
#else
  int i;
  int len = strlen (word);

  for (i = 0; i <= len - 1; i++)
    {
      if ('A' <= word[i] && word[i] <= 'Z')
	word[i] = word[i] + 32;
    }
#endif
  return 0;
}

/* remove SPACE before and after the content */
int
sclrspace (char *word)
{
  char *pbeg;
  char *pend;
  int len;

  if (word == NULL)
    return -1;
  if (*word == '\0')
    return 0;
  len = strlen (word);

  pbeg = word;
  while ((' ' == *pbeg) || ('\r' == *pbeg) || ('\n' == *pbeg)
	 || ('\t' == *pbeg))
    pbeg++;

  pend = word + len - 1;
  while ((' ' == *pend) || ('\r' == *pend) || ('\n' == *pend)
	 || ('\t' == *pend))
    pend--;

  if (pend < pbeg)
    {
      *word = '\0';
      return 0;
    }

  /* Add terminating NULL only if we've cleared room for it */
  if (pend + 1 <= word + (len - 1))
    pend[1] = '\0';

  if (pbeg != word)
    memmove (word, pbeg, pend - pbeg + 2);

  return 0;
}

/* set_next_token:
   dest is the place where the value will be allocated
   buf is the string where the value is searched
   end_separator is the character that MUST be found at the end of the value
   next is the final location of the separator + 1

   the element MUST be found before any "\r" "\n" "\0" and
   end_separator

   return -1 on error
   return 1 on success
*/
int
set_next_token (char **dest, char *buf, int end_separator, char **next)
{
  char *sep;			/* separator */

  *next = NULL;

  sep = buf;
  while ((*sep != end_separator) && (*sep != '\0') && (*sep != '\r')
	 && (*sep != '\n'))
    sep++;
  if ((*sep == '\r') || (*sep == '\n'))
    {				/* we should continue normally only if this is the separator asked! */
      if (*sep != end_separator)
	return -1;
    }
  if (*sep == '\0')
    return -1;			/* value must not end with this separator! */
  if (sep == buf)
    return -1;			/* empty value (or several space!) */

  *dest = smalloc (sep - (buf) + 1);
  sstrncpy (*dest, buf, sep - buf);

  *next = sep + 1;		/* return the position right after the separator */
  return 0;
}

#if 0
/*  not yet done!!! :-)
 */
int
set_next_token_better (char **dest, char *buf, int end_separator,
		       int *forbidden_tab[], int size_tab, char **next)
{
  char *sep;			/* separator */

  *next = NULL;

  sep = buf;
  while ((*sep != end_separator) && (*sep != '\0') && (*sep != '\r')
	 && (*sep != '\n'))
    sep++;
  if ((*sep == '\r') && (*sep == '\n'))
    {				/* we should continue normally only if this is the separator asked! */
      if (*sep != end_separator)
	return -1;
    }
  if (*sep == '\0')
    return -1;			/* value must not end with this separator! */
  if (sep == buf)
    return -1;			/* empty value (or several space!) */

  *dest = smalloc (sep - (buf) + 1);
  sstrncpy (*dest, buf, sep - buf);

  *next = sep + 1;		/* return the position right after the separator */
  return 1;
}
#endif

/* in quoted-string, many characters can be escaped...   */
/* quote_find returns the next quote that is not escaped */
char *
quote_find (char *qstring)
{
  char *quote;

  quote = strchr (qstring, '"');
  if (quote == qstring)		/* the first char matches and is not escaped... */
    return quote;

  if (quote == NULL)
    return NULL;		/* no quote at all... */

  /* this is now the nasty cases where '"' is escaped
     '" jonathan ros \\\""'
     |                  |
     '" jonathan ros \\"'
     |                |
     '" jonathan ros \""'
     |                |
     we must count the number of preceeding '\' */
  {
    int i = 1;

    for (;;)
      {
	if (0 == strncmp (quote - i, "\\", 1))
	  i++;
	else
	  {
	    if (i % 2 == 1)	/* the '"' was not escaped */
	      return quote;

	    /* else continue with the next '"' */
	    quote = strchr (quote + 1, '"');
	    if (quote == NULL)
	      return NULL;
	    i = 1;
	  }
	if (quote - i == qstring - 1)
	  /* example: "\"john"  */
	  /* example: "\\"jack" */
	  {
	    /* special case where the string start with '\' */
	    if (*qstring=='\\')
	      i++; /* an escape char was not counted */
	    if (i % 2 == 0)	/* the '"' was not escaped */
	      return quote;
	    else
	      {			/* else continue with the next '"' */
		qstring = quote + 1; /* reset qstring because
					(*quote+1) may be also == to '\\' */
		quote = strchr (quote + 1, '"');
		if (quote == NULL)
		  return NULL;
		i = 1;
	      }

	  }
      }
    return NULL;
  }
}

/*
int sclrlws(char *word) {
  int i = 0;
  int j = 0;
  int len = strlen (word);
  char* r = (char*) smalloc (len+2);
  for (i=0 ; i<=len-1 ; i++) {
    if ((' '!=word[i])&&('\n'!=word[i])&&('\r'!=word[i])&&('\t'!=word[i]))
      {
	r[j] = word[i];
	j++;
      }
  }
  
  sstrncpy(word,r,j);
  sfree(r);
  return 0;
}
*/

/**********************************************************/
/* only MACROS from osip/trace.h should be used by others */
/* TRACE_INITIALIZE(level,file))                          */
/* TRACE_ENABLE_LEVEL(level)                              */
/* TRACE_DISABLE_LEVEL(level)                             */
/* IS_TRACE_LEVEL_ACTIVATE(level)                         */
/**********************************************************/
#ifndef ENABLE_TRACE
void
trace_initialize_syslog (trace_level_t level, char* ident)
{
}
void
trace_initialize (trace_level_t level, FILE * file)
{
}
void
trace_enable_level (trace_level_t level)
{
}
void
trace_disable_level (trace_level_t level)
{
}

boolean
is_trace_level_activate (trace_level_t level)
{
  return LOG_FALSE;
}

#else

/* initialize log */
/* all lower levels of level are logged in file. */
void
trace_initialize (trace_level_t level, FILE * file)
{
  int i = 0;

  /* enable trace in log file by default */
  logfile = NULL;
  if (file != NULL)
    logfile = file;
#ifndef SYSTEM_LOGGER_ENABLED
  else
    logfile = stdout;
#endif

  /* enable all lower levels */
  while (i < END_TRACE_LEVEL)
    {
      if (i < level)
	tracing_table[i] = LOG_TRUE;
      else
	tracing_table[i] = LOG_FALSE;
      i++;
    }
}

void
trace_initialize_syslog (trace_level_t level, char* ident)
{
  int i = 0;
#if defined (HAVE_SYSLOG_H)
  openlog(ident, LOG_CONS|LOG_PID, LOG_DAEMON);
  use_syslog=1;
#endif
  /* enable all lower levels */
  while (i < END_TRACE_LEVEL)
    {
      if (i < level)
	tracing_table[i] = LOG_TRUE;
      else
	tracing_table[i] = LOG_FALSE;
      i++;
    }
}

/* enable a special debugging level! */
void
trace_enable_level (trace_level_t level)
{
  tracing_table[level] = LOG_TRUE;
}

/* disable a special debugging level! */
void
trace_disable_level (trace_level_t level)
{
  tracing_table[level] = LOG_FALSE;
}

/* not so usefull? */
boolean
is_trace_level_activate (trace_level_t level)
{
  return (boolean) tracing_table[level];
}
#endif

int
osip_trace (char *fi, int li, trace_level_t level, FILE * f, char *chfr, ...)
     {
       va_list ap;
#ifdef ENABLE_TRACE

#if !defined(WIN32) && !defined(SYSTEM_LOGGER_ENABLED)
       if (logfile == NULL && use_syslog==0)
	 {			/* user did not initialize logger.. */
	   return 1;
	 }
#endif

       if (f == NULL)
	   f = logfile;

       if (tracing_table[level] == LOG_FALSE)
	 return 0;

       VA_START (ap, chfr);

#ifdef __VXWORKS_OS__
       /* vxworks can't have a local file */
       f = stdout;
#endif

       if (f&&use_syslog==0)
	 {
	   if (level == OSIP_FATAL)
	     fprintf (f, "| FATAL | <%s: %i> ", fi, li);
	   else if (level == OSIP_BUG)
	     fprintf (f, "|  BUG  | <%s: %i> ", fi, li);
	   else if (level == OSIP_ERROR)
	     fprintf (f, "| ERROR | <%s: %i> ", fi, li);
	   else if (level == OSIP_WARNING)
	     fprintf (f, "|WARNING| <%s: %i> ", fi, li);
	   else if (level == OSIP_INFO1)
	     fprintf (f, "| INFO1 | <%s: %i> ", fi, li);
	   else if (level == OSIP_INFO2)
	     fprintf (f, "| INFO2 | <%s: %i> ", fi, li);
	   else if (level == OSIP_INFO3)
	     fprintf (f, "| INFO3 | <%s: %i> ", fi, li);
	   else if (level == OSIP_INFO4)
	     fprintf (f, "| INFO4 | <%s: %i> ", fi, li);

	   vfprintf (f, chfr, ap);

	   fflush (f);
	 }
#if defined (HAVE_SYSLOG_H)
       else if (use_syslog==1)
	 {
	   char buffer[512];
	   int in = 0;
	   memset(buffer, 0, sizeof(buffer));
	   if (level == OSIP_FATAL)
	     in = snprintf (buffer, 511, "| FATAL | <%s: %i> ", fi, li);
	   else if (level == OSIP_BUG)
	     in = snprintf (buffer, 511, "|  BUG  | <%s: %i> ", fi, li);
	   else if (level == OSIP_ERROR)
	     in = snprintf (buffer, 511, "| ERROR | <%s: %i> ", fi, li);
	   else if (level == OSIP_WARNING)
	     in = snprintf (buffer, 511, "|WARNING| <%s: %i> ", fi, li);
	   else if (level == OSIP_INFO1)
	     in = snprintf (buffer, 511, "| INFO1 | <%s: %i> ", fi, li);
	   else if (level == OSIP_INFO2)
	     in = snprintf (buffer, 511, "| INFO2 | <%s: %i> ", fi, li);
	   else if (level == OSIP_INFO3)
	     in = snprintf (buffer, 511, "| INFO3 | <%s: %i> ", fi, li);
	   else if (level == OSIP_INFO4)
	     in = snprintf (buffer, 511, "| INFO4 | <%s: %i> ", fi, li);

	   vsnprintf (buffer + in, 511 - in, chfr, ap);
	   if (level == OSIP_FATAL)
	     syslog (LOG_ERR, "%s", buffer);
	   else if (level == OSIP_BUG)
	     syslog(LOG_ERR, "%s", buffer);
	   else if (level == OSIP_ERROR)
	     syslog(LOG_ERR, "%s", buffer);
	   else if (level == OSIP_WARNING)
	     syslog(LOG_WARNING, "%s", buffer);
	   else if (level == OSIP_INFO1)
	     syslog(LOG_INFO, "%s", buffer);
	   else if (level == OSIP_INFO2)
	     syslog(LOG_INFO, "%s", buffer);
	   else if (level == OSIP_INFO3)
	     syslog(LOG_DEBUG, "%s", buffer);
	   else if (level == OSIP_INFO4)
	     syslog(LOG_DEBUG, "%s", buffer);
	 }
#endif
#ifdef SYSTEM_LOGGER_ENABLED
       else
	 {
	   char buffer[512];
	   int in = 0;
	   memset(buffer, 0, sizeof(buffer));
	   if (level == OSIP_FATAL)
	     in = _snprintf (buffer, 511, "| FATAL | <%s: %i> ", fi, li);
	   else if (level == OSIP_BUG)
	     in = _snprintf (buffer, 511, "|  BUG  | <%s: %i> ", fi, li);
	   else if (level == OSIP_ERROR)
	     in = _snprintf (buffer, 511, "| ERROR | <%s: %i> ", fi, li);
	   else if (level == OSIP_WARNING)
	     in = _snprintf (buffer, 511, "|WARNING| <%s: %i> ", fi, li);
	   else if (level == OSIP_INFO1)
	     in = _snprintf (buffer, 511, "| INFO1 | <%s: %i> ", fi, li);
	   else if (level == OSIP_INFO2)
	     in = _snprintf (buffer, 511, "| INFO2 | <%s: %i> ", fi, li);
	   else if (level == OSIP_INFO3)
	     in = _snprintf (buffer, 511, "| INFO3 | <%s: %i> ", fi, li);
	   else if (level == OSIP_INFO4)
	     in = _snprintf (buffer, 511, "| INFO4 | <%s: %i> ", fi, li);

	   _vsnprintf (buffer + in, 511 - in, chfr, ap);
	   OutputDebugString (buffer);
	 }
#endif

       va_end (ap);
#endif
       return 0;
     }
