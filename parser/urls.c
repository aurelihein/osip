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

/* HISTORY:

   v0.5.0: created.

   v0.6.0: 14/07/2001 complete support for url:
   new structure
   new url_parse() method

   TODO:
   add support for optionnal value for paramaters
 */

#include <osip/port.h>
#include <osip/smsg.h>
#include <stdlib.h>
#include <stdio.h>

/* allocate a new url structure */
/* OUTPUT: url_t *url | structure to save results.   */
/* OUTPUT: err_t *err | structure to store error.   */
/* return -1 on error */
int
url_init (url_t ** url)
{
  *url = (url_t *) smalloc (sizeof (url_t));
  (*url)->scheme = NULL;
  (*url)->username = NULL;
  (*url)->password = NULL;
  (*url)->host = NULL;
  (*url)->port = NULL;

  (*url)->url_params = (list_t *) smalloc (sizeof (list_t));
  list_init ((*url)->url_params);

  (*url)->url_headers = (list_t *) smalloc (sizeof (list_t));
  list_init ((*url)->url_headers);

  (*url)->string = NULL;
  return 0;
}

/* examples:
   sip:j.doe@big.com;maddr=239.255.255.1;ttl=15
   sip:j.doe@big.com
   sip:j.doe:secret@big.com;transport=tcp
   sip:j.doe@big.com?subject=project
   sip:+1-212-555-1212:1234@gateway.com;user=phone
   sip:1212@gateway.com
   sip:alice@10.1.2.3
   sip:alice@example.com
   sip:alice@registrar.com;method=REGISTER

   NOT EQUIVALENT:
   SIP:JUSER@ExAmPlE.CoM;Transport=udp
   sip:juser@ExAmPlE.CoM;Transport=UDP
*/

/* this method search for the separator and   */
/* return it only if it is located before the */
/* second separator. */
char *
next_separator (char *ch, int separator_to_find, int before_separator)
{
  char *ind;
  char *tmp;

  ind = strchr (ch, separator_to_find);
  if (ind == NULL)
    return NULL;

  tmp = NULL;
  if (before_separator != 0)
    tmp = strchr (ch, before_separator);

  if (tmp != NULL)
    {
      if (ind < tmp)
        return ind;
  } else
    return ind;

  return NULL;
}

/* parse the sip url.                                */
/* INPUT : char *buf | url to be parsed.*/
/* OUTPUT: url_t *url | structure to save results.   */
/* OUTPUT: err_t *err | structure to store error.   */
/* return -1 on error */
int
url_parse (url_t * url, char *buf)
{
  char *username;
  char *password;
  char *host;
  char *port;
  char *params;
  char *headers;
  char *tmp;

  /* basic tests */
  if (buf == NULL)
    return -1;

  tmp = strchr (buf, ':');
  if (tmp == NULL)
    return -1;

  if (tmp - buf < 2)
    return -1;
  url->scheme = (char *) smalloc (tmp - buf + 1);
  sstrncpy (url->scheme, buf, tmp - buf);

  if (strlen (url->scheme) < 3 ||
      (0 != strncmp (url->scheme, "sip", 3)
       && 0 != strncmp (url->scheme, "sips", 4)))
    {                           /* Is not a sipurl ! */
      int i = strlen (tmp + 1);

      if (i < 2)
        return -1;
      url->string = (char *) smalloc (i + 1);
      sstrncpy (url->string, tmp + 1, i);
      return 0;
    }
  /*  law number 1:
     if ('?' exists && is_located_after '@')
     or   if ('?' exists && '@' is not there -no username-)
     =====>  HEADER_PARAM EXIST
     =====>  start at index(?)
     =====>  end at the end of url
   */

  /* find the beginning of host */
  username = strchr (buf, ':');
  /* if ':' does not exist, the url is not valid */
  if (username == NULL)
    return -1;

  host = strchr (buf, '@');

  if (host == NULL)
    host = username;
  else
    /* username exists */
    {
      password = next_separator (username + 1, ':', '@');
      if (password == NULL)
        password = host;
      else
        /* password exists */
        {
          if (host - password < 2)
            return -1;
          url->password = (char *) smalloc (host - password);
          sstrncpy (url->password, password + 1, host - password - 1);
          url_unescape (url->password);
        }
      if (password - username < 2)
        return -1;
      {
        url->username = (char *) smalloc (password - username);
        sstrncpy (url->username, username + 1, password - username - 1);
        url_unescape (url->username);
      }
    }


  /* search for header after host */
  headers = strchr (host, '?');

  if (headers == NULL)
    headers = buf + strlen (buf);
  else
    /* headers exist */
    url_parse_headers (url, headers);


  /* search for params after host */
  params = strchr (host, ';');  /* search for params after host */
  if (params == NULL)
    params = headers;
  else
    /* params exist */
    {
      if (headers - params + 1 < 2)
        return -1;
      tmp = smalloc (headers - params + 1);
      tmp = sstrncpy (tmp, params, headers - params);
      url_parse_params (url, tmp);
      sfree (tmp);
    }

  port = params - 1;
  while (port > host && *port != ']' && *port != ':')
    port--;
  if (*port == ':')
    {
      if (host == port)
        port = params;
      else
        {
          if ((params - port < 2) || (params - port > 8))
            return -1;          /* error cases */
          url->port = (char *) smalloc (params - port);
          sstrncpy (url->port, port + 1, params - port - 1);
          sclrspace (url->port);
        }
  } else
    port = params;
  /* adjust port for ipv6 address */
  tmp = port;
  while (tmp > host && *tmp != ']')
    tmp--;
  if (*tmp == ']')
    {
      port = tmp;
      while (host < port && *host != '[')
        host++;
      if (host >= port)
        return -1;
    }

  if (port - host < 2)
    return -1;
  url->host = (char *) smalloc (port - host);
  sstrncpy (url->host, host + 1, port - host - 1);
  sclrspace (url->host);

  return 0;
}

void
url_setscheme (url_t * url, char *scheme)
{
  url->scheme = scheme;
}

char *
url_getscheme (url_t * url)
{
  if (url == NULL)
    return NULL;
  return url->scheme;
}

void
url_setusername (url_t * url, char *username)
{
  url->username = username;
}

char *
url_getusername (url_t * url)
{
  if (url == NULL)
    return NULL;
  return url->username;
}

void
url_setpassword (url_t * url, char *password)
{
  url->password = password;
}

char *
url_getpassword (url_t * url)
{
  if (url == NULL)
    return NULL;
  return url->password;
}

void
url_sethost (url_t * url, char *host)
{
  url->host = host;
}

char *
url_gethost (url_t * url)
{
  if (url == NULL)
    return NULL;
  return url->host;
}

void
url_setport (url_t * url, char *port)
{
  url->port = port;
}

char *
url_getport (url_t * url)
{
  if (url == NULL)
    return NULL;
  return url->port;
}


int
url_parse_headers (url_t * url, char *headers)
{
  char *and;
  char *equal;

  /* find '=' wich is the separator for one header */
  /* find ';' wich is the separator for multiple headers */

  equal = strchr (headers, '=');
  and = strchr (headers + 1, '&');

  if (equal == NULL)            /* each header MUST have a value */
    return -1;

  do
    {
      char *hname;
      char *hvalue;

      hname = (char *) smalloc (equal - headers);
      sstrncpy (hname, headers + 1, equal - headers - 1);
      url_unescape (hname);

      if (and != NULL)
        {
          if (and - equal < 2)
            {
              sfree (hname);
              return -1;
            }
          hvalue = (char *) smalloc (and - equal);
          sstrncpy (hvalue, equal + 1, and - equal - 1);
          url_unescape (hvalue);
      } else
        {                       /* this is for the last header (no and...) */
          if (headers + strlen (headers) - equal + 1 < 2)
            {
              sfree (hname);
              return -1;
            }
          hvalue = (char *) smalloc (headers + strlen (headers) - equal + 1);
          sstrncpy (hvalue, equal + 1, headers + strlen (headers) - equal);
          url_unescape (hvalue);
        }

      url_uheader_add (url, hname, hvalue);

      if (and == NULL)          /* we just set the last header */
        equal = NULL;
      else                      /* continue on next header */
        {
          headers = and;
          equal = strchr (headers, '=');
          and = strchr (headers + 1, '&');
          if (equal == NULL)    /* each header MUST have a value */
            return -1;
        }
    }
  while (equal != NULL);
  return 0;
}

int
url_parse_params (url_t * url, char *params)
{
  char *pname;
  char *pvalue;

  char *comma;
  char *equal;

  /* find '=' wich is the separator for one param */
  /* find ';' wich is the separator for multiple params */

  equal = next_separator (params + 1, '=', ';');
  comma = strchr (params + 1, ';');

  while (comma != NULL)
    {
      if (equal == NULL)
        {
          equal = comma;
          pvalue = NULL;
      } else
        {
          if (comma - equal < 2)
            return -1;
          pvalue = (char *) smalloc (comma - equal);
          sstrncpy (pvalue, equal + 1, comma - equal - 1);
          url_unescape (pvalue);
        }

      if (equal - params < 2)
        {
          sfree (pvalue);
          return -1;
        }
      pname = (char *) smalloc (equal - params);
      sstrncpy (pname, params + 1, equal - params - 1);
      url_unescape (pname);

      url_uparam_add (url, pname, pvalue);

      params = comma;
      equal = next_separator (params + 1, '=', ';');
      comma = strchr (params + 1, ';');
    }

  /* this is the last header (comma==NULL) */
  comma = params + strlen (params);

  if (equal == NULL)
    {
      equal = comma;            /* at the end */
      pvalue = NULL;
  } else
    {
      if (comma - equal < 2)
        return -1;
      pvalue = (char *) smalloc (comma - equal);
      sstrncpy (pvalue, equal + 1, comma - equal - 1);
    }

  if (equal - params < 2)
    {
      sfree (pvalue);
      return -1;
    }
  pname = (char *) smalloc (equal - params);
  sstrncpy (pname, params + 1, equal - params - 1);

  url_uparam_add (url, pname, pvalue);

  return 0;
}

int
url_2char (url_t * url, char **dest)
{

  char *buf;

  if (url == NULL)
    return -1;
  if (url->host == NULL && url->string == NULL)
    return -1;
  if (url->scheme == NULL && url->string != NULL)
    return -1;
  if (url->string == NULL && url->scheme == NULL)
    url->scheme = sgetcopy ("sip");     /* default is sipurl */

  buf = (char *) smalloc (200 * sizeof (char));
  *dest = buf;
  if (url->string != NULL)
    {
      sprintf (buf, "%s:", url->scheme);
      buf = buf + strlen (url->scheme) + 1;
      sprintf (buf, "%s", url->string);
      buf = buf + strlen (url->string);
      return 0;
    }

  sprintf (buf, "%s:", url->scheme);
  buf = buf + strlen (url->scheme) + 1;

  if (url->username != NULL)
    {
      char *tmp = url_escape_userinfo (url->username);

      sprintf (buf, "%s", tmp);
      sfree (tmp);
      buf = buf + strlen (buf);
    }
  if ((url->password != NULL) && (url->username != NULL))
    {                           /* be sure that when a password is given, a username is also given */
      char *tmp = url_escape_password (url->password);

      sprintf (buf, ":%s", tmp);
      sfree (tmp);
      buf = buf + strlen (buf);
    }
  if (url->username != NULL)
    {                           /* we add a '@' only when username is present... */
      sprintf (buf, "@");
      buf++;
    }
  /*   if (url->host!=NULL)  mandatory */
  if (strchr (url->host, ':') != NULL)
    {
      sprintf (buf, "[%s]", url->host);
      buf = buf + strlen (url->host) + 2;
  } else
    {
      sprintf (buf, "%s", url->host);
      buf = buf + strlen (url->host);
    }
  if (url->port != NULL)
    {
      sprintf (buf, ":%s", url->port);
      buf = buf + strlen (buf);
    }
  {
    int pos = 0;
    url_param_t *u_param;

    while (!list_eol (url->url_params, pos))
      {
        char *tmp;

        u_param = (url_param_t *) list_get (url->url_params, pos);
        tmp = url_escape_uri_param (u_param->gname);
        if (u_param->gvalue != NULL)
          {
            char *tmp2 = url_escape_uri_param (u_param->gvalue);

            sprintf (buf, ";%s=%s", tmp, tmp2);
            sfree (tmp2);
        } else
          sprintf (buf, ";%s", tmp);
        sfree (tmp);
        buf = buf + strlen (buf);
        pos++;
      }
  }

  {
    int pos = 0;
    url_header_t *u_header;

    while (!list_eol (url->url_headers, pos))
      {
        char *tmp;
        char *tmp2;

        u_header = (url_header_t *) list_get (url->url_headers, pos);
        tmp = url_escape_header_param (u_header->gname);
        tmp2 = url_escape_header_param (u_header->gvalue);
        if (pos == 0)
          {
            sprintf (buf, "?%s=%s", u_header->gname, u_header->gvalue);
        } else
          sprintf (buf, "&%s=%s", u_header->gname, u_header->gvalue);
        sfree (tmp);
        sfree (tmp2);
        buf = buf + strlen (buf);
        pos++;
      }
  }

  /* we should free useless memory by reallocating a new */
  /*  char * wich is the right size and copy result into it? */

  return 0;
}


void
url_free (url_t * url)
{
  int pos = 0;

  if (url == NULL)
    return;
  sfree (url->scheme);
  sfree (url->username);
  sfree (url->password);
  sfree (url->host);
  sfree (url->port);

  url_param_freelist (url->url_params);
  sfree (url->url_params);

  {
    url_header_t *u_header;

    while (!list_eol (url->url_headers, pos))
      {
        u_header = (url_header_t *) list_get (url->url_headers, pos);
        list_remove (url->url_headers, pos);
        url_header_free (u_header);
        sfree (u_header);
      }
    sfree (url->url_headers);
  }

  sfree (url->string);

  url->scheme = NULL;
  url->username = NULL;
  url->password = NULL;
  url->host = NULL;
  url->port = NULL;
  url->url_params = NULL;
  url->url_headers = NULL;
  url->string = NULL;
}

int
url_clone (url_t * url, url_t ** dest)
{
  int i;
  url_t *ur;

  *dest = NULL;
  if (url == NULL)
    return -1;
  if (url->host == NULL && url->string == NULL)
    return -1;

  i = url_init (&ur);
  if (i == -1)                  /* allocation failed */
    return -1;
  if (url->scheme != NULL)
    ur->scheme = sgetcopy (url->scheme);
  if (url->username != NULL)
    ur->username = sgetcopy (url->username);
  if (url->password != NULL)
    ur->password = sgetcopy (url->password);
  if (url->host != NULL)
    ur->host = sgetcopy (url->host);
  if (url->port != NULL)
    ur->port = sgetcopy (url->port);
  if (url->string != NULL)
    ur->string = sgetcopy (url->string);

  {
    int pos = 0;
    url_param_t *u_param;
    url_param_t *dest_param;

    while (!list_eol (url->url_params, pos))
      {
        u_param = (url_param_t *) list_get (url->url_params, pos);
        i = url_param_clone (u_param, &dest_param);
        if (i != 0)
          return -1;
        list_add (ur->url_params, dest_param, -1);
        pos++;
      }
  }
  {
    int pos = 0;
    url_param_t *u_param;
    url_param_t *dest_param;

    while (!list_eol (url->url_headers, pos))
      {
        u_param = (url_param_t *) list_get (url->url_headers, pos);
        i = url_param_clone (u_param, &dest_param);
        if (i != 0)
          return -1;
        list_add (ur->url_headers, dest_param, -1);
        pos++;
      }
  }

  *dest = ur;
  return 0;
}

int
url_param_init (url_param_t ** url_param)
{
  *url_param = (url_param_t *) smalloc (sizeof (url_param_t));
  (*url_param)->gname = NULL;
  (*url_param)->gvalue = NULL;
  return 0;
}

void
url_param_free (url_param_t * url_param)
{
  sfree (url_param->gname);
  sfree (url_param->gvalue);

  url_param->gname = NULL;
  url_param->gvalue = NULL;
}

int
url_param_set (url_param_t * url_param, char *pname, char *pvalue)
{
  url_param->gname = pname;
  /* not needed for url, but for all other generic params */
  sclrspace (url_param->gname);
  url_param->gvalue = pvalue;
  if (url_param->gvalue != NULL)
    sclrspace (url_param->gvalue);
  return 0;
}

int
url_param_add (list_t * url_params, char *pname, char *pvalue)
{
  int i;
  url_param_t *url_param;

  i = url_param_init (&url_param);
  if (i != 0)
    return -1;
  i = url_param_set (url_param, pname, pvalue);
  if (i != 0)
    {
      url_param_free (url_param);
      sfree (url_param);
      return -1;
    }
  list_add (url_params, url_param, -1);
  return 0;
}

void
url_param_freelist (list_t * params)
{
  url_param_t *u_param;

  while (!list_eol (params, 0))
    {
      u_param = (url_param_t *) list_get (params, 0);
      list_remove (params, 0);
      url_param_free (u_param);
      sfree (u_param);
    }
}

int
url_param_getbyname (list_t * params, char *pname, url_param_t ** url_param)
{
  int pos = 0;
  url_param_t *u_param;

  *url_param = NULL;
  while (!list_eol (params, pos))
    {
      u_param = (url_param_t *) list_get (params, pos);
      if (strncmp (u_param->gname, pname, strlen (pname)) == 0)
        {
          *url_param = u_param;
          return 0;
        }
      pos++;
    }
  return -1;
}

int
url_param_clone (url_param_t * uparam, url_param_t ** dest)
{
  int i;
  url_param_t *up;

  *dest = NULL;
  if (uparam == NULL)
    return -1;
  if (uparam->gname == NULL)
    return -1;                  /* name is mandatory */

  i = url_param_init (&up);
  if (i != 0)                   /* allocation failed */
    return -1;
  up->gname = sgetcopy (uparam->gname);
  if (uparam->gvalue != NULL)
    up->gvalue = sgetcopy (uparam->gvalue);
  else
    up->gvalue = NULL;
  *dest = up;
  return 0;
}


#define _ALPHANUM_ "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890\0"
#define _RESERVED_ ";/?:@&=+$\0"
#define _MARK_ "-_.!~*'()\0"

#define _MARK__USER_UNRESERVED_ "-_.!~*'()&=+$,;?/\0"
#define _MARK__PWORD_UNRESERVED_ "-_.!~*'()&=+$,\0"
#define _MARK__URI_PARAM_UNRESERVED_ "-_.!~*'()[]/:&+$\0"
#define _MARK__HEADER_PARAM_UNRESERVED_ "-_.!~*'()[]/?:+$\0"

#define osip_is_alphanum(in) (  \
       (in >= 'a' && in <= 'z') || \
       (in >= 'A' && in <= 'Z') || \
       (in >= '0' && in <= '9'))

char *
url_escape_nonascii_and_nondef (const char *string, const char *def)
{
  int alloc = strlen (string) + 1;
  int length;
  char *ns = smalloc (alloc);
  unsigned char in;
  int newlen = alloc;
  int index = 0;
  const char *tmp;
  int i;

  length = alloc - 1;
  while (length--)
    {
      in = *string;

      i = 0;
      tmp = NULL;
      if (osip_is_alphanum (in))
        tmp = string;
      else
        {
          for (; def[i] != '\0' && def[i] != in; i++)
            {
            }
          if (def[i] != '\0')
            tmp = string;
        }
      if (tmp == NULL)
        {
          /* encode it */
          newlen += 2;          /* the size grows with two, since this'll become a %XX */
          if (newlen > alloc)
            {
              alloc *= 2;
              ns = realloc (ns, alloc);
              if (!ns)
                return NULL;
            }
          sprintf (&ns[index], "%%%02X", in);
          index += 3;
      } else
        {
          /* just copy this */
          ns[index++] = in;
        }
      string++;
    }
  ns[index] = 0;                /* terminate it */
  return ns;
}

/* user =  *( unreserved / escaped / user-unreserved ) */
const char *userinfo_def = /* implied _ALPHANUM_ */ _MARK__USER_UNRESERVED_;
char *
url_escape_userinfo (const char *string)
{
  return url_escape_nonascii_and_nondef (string, userinfo_def);
}

/* user =  *( unreserved / escaped / user-unreserved ) */
const char *password_def = _MARK__PWORD_UNRESERVED_;
char *
url_escape_password (const char *string)
{
  return url_escape_nonascii_and_nondef (string, password_def);
}

const char *uri_param_def = _MARK__URI_PARAM_UNRESERVED_;
char *
url_escape_uri_param (char *string)
{
  return url_escape_nonascii_and_nondef (string, uri_param_def);
}

const char *header_param_def = _MARK__HEADER_PARAM_UNRESERVED_;
char *
url_escape_header_param (char *string)
{
  return url_escape_nonascii_and_nondef (string, header_param_def);
}

void
url_unescape (char *string)
{
  int alloc = strlen (string) + 1;
  unsigned char in;
  int index = 0;
  unsigned int hex;
  char *ptr;

  ptr = string;
  while (--alloc > 0)
    {
      in = *ptr;
      if ('%' == in)
        {
          /* encoded part */
          if (sscanf (ptr + 1, "%02X", &hex))
            {
              in = hex;
              ptr += 2;
              alloc -= 2;
            }
        }

      string[index++] = in;
      ptr++;
    }
  string[index] = 0;            /* terminate it */
}
