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

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include <osip/port.h>

#ifdef HAVE_CTYPE_H
#include <ctype.h>
#endif

#include <time.h>
#ifndef __VXWORKS_OS__
#ifndef WIN32
#include <sys/time.h>
#else
#include <winsock.h>
#endif
#else
#include <selectLib.h>
#endif

#if defined (HAVE_UNISTD_H)
#  include <sys/unistd.h>
#endif

#if defined (HAVE_SYS_SELECT_H)
#  include <sys/select.h>
#endif

#ifdef HAVE_PTH_PTHREAD_H
#include <pthread.h>
#endif

FILE *logfile = NULL;
int tracing_table[END_TRACE_LEVEL];

/* a,b, and x are used to generate random callid with a long period. */
static unsigned int a = 0;
static unsigned int b = 0;
static unsigned int x = 0;

void
init_random_number()
{
  srand((unsigned int) time(NULL));
  a  = 4*(1+(int) (20000000.0*rand()/(RAND_MAX+1.0)))+1;
  b  = 2*(1+(int) (30324000.0*rand()/(RAND_MAX+1.0)))+1;
  x = (1+(int)   (23445234.0*rand()/(RAND_MAX+1.0)));
}

unsigned int
new_random_number()
{
  x = a*x + b;
  return x;
}

int
satoi(char *number)
{
  return atoi(number);
}

char *
sstrncpy(char *dest,const char *src,int length)
{
  memcpy(dest,src,length);
  dest[length]='\0';
  return dest;
}

/* append string_to_append to string at position cur
   size is the current allocated size of the element
*/
/* in fact, I don't know how to use realloc! so this
   method does not work... Also, the changed size
   is not returned!!!
   THIS METHOD MUST BE CHANGED!
*/
char *
sdp_append_string(char *string, int size, char *cur, char *string_to_append)
{
  int length = strlen(string_to_append);
  if (cur+length-string>size)
    {
      string = realloc(string,size+length+10);
      /* string may be another pointer?? does it contains
	 the previous element?? */
      /* can we use cur??? after such a call?? */
    }
  sstrncpy(cur,string_to_append,length);
  return cur+strlen(cur);
}

void
susleep(int useconds)
{
#ifdef WIN32
  Sleep(useconds/1000);
#else
  struct timeval delay;
  int sec;
  sec = (int)useconds/1000000;
  if (sec>0)
    {
    delay.tv_sec = sec;
    delay.tv_usec = 0;
    }
  else
    {
    delay.tv_sec = 0;
    delay.tv_usec =useconds;
    }
  select(0,0,0,0,&delay);
#endif  
}


char *
sgetcopy(char *ch)
{
  char *copy;
  if (ch==NULL) return NULL;
  copy = (char *)smalloc(strlen(ch)+1);
  sstrncpy(copy,ch,strlen(ch));
  return copy;
}

char *
sgetcopy_unquoted_string(char *ch)
{
  char *copy = (char *)smalloc(strlen(ch)+1);
  /* remove leading and trailing " */
  if ((*ch=='\"'))
    {
      sstrncpy(copy,ch+1,strlen(ch+1));
      sstrncpy(copy+strlen(copy)-1,"\0",1);
    }
  else
    sstrncpy(copy,ch,strlen(ch));
  return copy;
}

int stolowercase(char *word)
{
#ifdef HAVE_CTYPE_H
  for ( ; *word; word++)
    *word = tolower(*word);
#else
  int i;
  int len = strlen(word);
  for (i=0 ; i<=len-1 ; i++) {
  if ('A' <= word[i] && word[i] <= 'Z')
    word[i] = word[i]+32;
  }
#endif
  return 0;
}

/* remove SPACE before and after the content */
int sclrspace(char *word)
{
  char *pbeg;
  char *pend;
  int len;
  if (word==NULL) return -1;
  len = strlen(word);

  pbeg = word;
  while ((' '== *pbeg)||('\r'== *pbeg)||('\n'== *pbeg)||('\t'== *pbeg)) pbeg++;

  pend = word + len -1;
  while ((' '== *pend)||('\r'== *pend)||('\n'== *pend)||('\t'== *pend)) pend--;

  /* Add terminating NULL only if we've cleared room for it */
  if (pend+1 <= word+(len-1))
    pend[1]='\0';
  
  if (pbeg!=word)
    memmove(word,pbeg,pend-pbeg+2);

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
set_next_token(char **dest, char *buf, int end_separator, char **next)
{
  char *sep; /* separator */
  *next=NULL;

  sep = buf;
  while ((*sep!=end_separator)&&(*sep!='\0')&&(*sep!='\r')&&(*sep!='\n'))
    sep++;
  if ((*sep=='\r')||(*sep=='\n'))
    { /* we should continue normally only if this is the separator asked! */
      if (*sep!=end_separator) return -1;
    }
  if (*sep=='\0') return -1; /* value must not end with this separator! */
  if (sep==buf) return -1;  /* empty value (or several space!) */

  *dest = smalloc(sep-(buf)+1);
  sstrncpy(*dest,buf,sep-buf);
 
  *next = sep+1;  /* return the position right after the separator */
  return 0;
}

/*  not yet done!!! :-)
 */
int
set_next_token_better(char **dest, char *buf, int end_separator,
		      int *forbidden_tab[], int size_tab,
		      char **next)
{
  char *sep; /* separator */
  *next=NULL;

  sep = buf;
  while ((*sep!=end_separator)&&(*sep!='\0')&&(*sep!='\r')&&(*sep!='\n'))
    sep++;
  if ((*sep=='\r')&&(*sep=='\n'))
    { /* we should continue normally only if this is the separator asked! */
      if (*sep!=end_separator) return -1;
    }
  if (*sep=='\0') return -1; /* value must not end with this separator! */
  if (sep==buf) return -1;  /* empty value (or several space!) */

  *dest = smalloc(sep-(buf)+1);
  sstrncpy(*dest,buf,sep-buf);
 
  *next = sep+1;  /* return the position right after the separator */
  return 1;
}

/* in quoted-string, many characters can be escaped...   */
/* quote_find returns the next quote that is not escaped */
char *
quote_find(char *qstring)
{
  char *quote;
  quote = strchr(qstring,'"');
  if (quote==qstring) /* the first char matches and is not escaped...*/
    return quote;

  if (quote==NULL)
    return NULL; /* no quote at all... */

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
      while (1)
	{
	  if (0==strncmp(quote-i,"\\",1))
	    i++;
	  else
	    {
	      if (i%2==1) /* the '"' was not escaped */
		return quote;

	      /* else continue with the next '"'*/
	      quote = strchr(quote+1,'"');
	      if (quote==NULL) return NULL;
	      i = 1;
	    }
	  if (quote-i==qstring+1)
	    /* example: "\"john"  */
	    /* example: "\\"jack" */
	    {
	      if (i%2==0) /* the '"' was not escaped */
		return quote;
	      else
		{/* else continue with the next '"'*/
		  quote = strchr(quote+1,'"');
		  if (quote==NULL) return NULL;
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
#ifdef ENABLE_TRACE
/********************************************************/
/* only MACROS from osip/trace.h must be used by others */
/* TRACE_INITIALIZE(level,file))                        */
/* TRACE_ENABLE_LEVEL(level)                            */
/* TRACE_DISABLE_LEVEL(level)                           */
/* IS_TRACE_LEVEL_ACTIVATE(level)                       */
/********************************************************/

/* initialize log */
/* all lower levels of level are logged in file. */
void trace_initialize ( trace_level_t level , FILE *file )
{
  int i = 0;
  /* enable trace in log file by default */
  if (file!=NULL)
    logfile = file;
  else
    logfile = stdout;

  /* enable all lower levels */
  while (i<END_TRACE_LEVEL)
    {
      if (i<level)
	tracing_table[i]=LOG_TRUE;
      else
	tracing_table[i]=LOG_FALSE;
      i++;
    }
}

/* enable a special debugging level! */
void trace_enable_level ( trace_level_t level )
{
  tracing_table[level]=LOG_TRUE;
}

/* disable a special debugging level! */
void trace_disable_level (trace_level_t level)
{
  tracing_table[level]=LOG_FALSE;
}

/* not so usefull? */
boolean is_trace_level_activate ( trace_level_t level )
{
  return tracing_table[level];
}
#endif

int
#if defined(HAVE_STDARG_H) || defined(__VXWORKS_OS__) || defined(WIN32)
trace(char *fi, int li, trace_level_t level, FILE *f, char *chfr, ...)
#else
trace(fi,li,level,f,chfr,va_list)
     char *fi;
     char *li;
     int level;
     FILE *f;
     char *chfr
#endif
{
  va_list ap;
#ifdef ENABLE_TRACE
  time_t ti;

  if (logfile==NULL)
    { /* user did not initialize logger.. */
      return 1;
    }
  if (f==NULL)
    f = logfile;
  if (tracing_table[level]==LOG_FALSE)
    return 0;

  ti = time(NULL);
  VA_START(ap, chfr);

#ifdef __VXWORKS_OS__
  /* vxworks can't have a local file */
  f = stdout;
#endif
  fprintf(f, "L%i -%li- <%s: %i> ", level, (long int)ti, fi, li);
  vfprintf(f, chfr,ap);

  fflush(f);
#endif
  va_end(ap);
  return 0;
}



