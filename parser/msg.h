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


#ifndef _MSG_H_
#define _MSG_H_

#define NUMBER_OF_HEADERS 31

/* internal type for parser's config */
typedef struct _parser_config_t
{
  char *hname;
  int (*setheader) (sip_t *, char *);
}
parser_config_t;

void msg_startline_init (startline_t ** strtline);

int parser_callmethod (int i, sip_t * dest, char *hvalue);
int parser_isknownheader (char *hname);

int find_next_occurence (char *str, char *buf, char **index_of_str);
int find_next_crlf (char *start_of_header, char **end_of_header);
int find_next_crlfcrlf (char *start_of_part, char **end_of_part);

int quoted_string_set (char *name, char *str, char **result, char **next);
int token_set (char *name, char *str, char **result, char **next);


int generic_param_parseall (list_t * gen_params, char *params);

#endif
