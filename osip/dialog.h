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

#ifndef _DIALOG_H_
#define _DIALOG_H_

#include <osip/osip.h>
#include <osip/port.h>

typedef enum _dlg_type_t {
  CALLER,
  CALLEE
} dlg_type_t;

typedef struct _dialog_t {
  /*  char *dialog_id; ***implied*** */  /* call-id:local_tag:remote-tag */
  char *call_id;
  char *local_tag;
  char *remote_tag;
  list_t *route_set;
  int local_cseq;
  int remote_cseq;
  to_t *remote_uri;
  from_t *local_uri;
  contact_t *remote_contact_uri;
  int secure;

  /* type of dialog (CALLEE or CALLER) */
  dlg_type_t type;
  /* fsm for dialog? */
  state_t state; /* DIALOG_EARLY || DIALOG_CONFIRMED || DIALOG_CLOSED */

} dialog_t;

int dialog_init_as_uac(dialog_t **dialog, sip_t *response);
int dialog_init_as_uas(dialog_t **dialog, sip_t *invite, sip_t *response);
void dialog_free(dialog_t *dialog);
void dialog_set_state(dialog_t *dialog, dlg_type_t type);
int dialog_update_route_set_as_uas(dialog_t *dialog, sip_t *invite);
int dialog_update_cseq_as_uas(dialog_t *dialog, sip_t *invite);

int dialog_match_as_uac(dialog_t *dlg, sip_t *answer);
int dialog_update_tag_as_uac(dialog_t *dialog, sip_t *response);
int dialog_update_route_set_as_uac(dialog_t *dialog, sip_t *reponse);

int dialog_match_as_uas(dialog_t *dlg, sip_t *answer);

int dialog_is_originator(dialog_t *dlg);
int dialog_is_callee(dialog_t *dlg);

#endif
