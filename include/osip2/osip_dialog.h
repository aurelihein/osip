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

#include <osip2/osip.h>

/**
 * @file osip_dialog.h
 * @brief oSIP dialog Routines
 *
 * Dialog management is a powerful facility given by oSIP. This feature is
 * needed by SIP end point who has the capability to answer calls. (i.e.
 * answering 200 OK to an INVITE).
 * <BR>
 * A Dialog is a context for a call establishment in oSIP. It's not useless
 * to say that ONE invite request can lead to several call establishment.
 * This can happen if your call has been forked by a proxy and several
 * user agent was contacted and replied at the same time. It is true that
 * this case won't probably happen several times a month...
 * <BR>
 * There is two ways of creating a dialog. In one case, you are the CALLER
 * and in the other case, you will be the CALLEE.
 * <UL>
 * <LI>Creating a dialog as a CALLER
 * <BR>In this case, you have to create a dialog each time you receive
 * an answer with a code between 101 and 299. The best place in oSIP to
 * actually create a dialog is of course in the callback that announce
 * such SIP messages. Of course, each time you receive a response, you have
 * to check for an existing dialog associated to this INVITE that can have
 * been created by earlier SIP answer coming from the same User Agent. The
 * code in the callback will look like the following:
 * <BR> void cb_rcv1xx(osip_transaction_t *tr,osip_message_t *sip)
 * <BR> {
 * <BR>   osip_dialog_t *dialog;
 * <BR>   if (MSG_IS_RESPONSEFOR(sip, "INVITE")&&!MSG_TEST_CODE(sip, 100)) {
 * <BR>     dialog = my_application_search_existing_dialog(sip);
 * <BR>     if (dialog==NULL) //NO EXISTING DIALOG
 * <BR>       {
 * <BR>        i = osip_dialog_init_as_uac(&dialog, sip);
 * <BR>        my_application_add_existing_dialog(dialog);
 * <BR>       }
 * <BR>   } else {
 * <BR>     // no dialog establishment for other REQUEST
 * <BR> }
 * </LI>
 * <LI>Creating a dialog as a CALLEE
 * <BR>In this case, you will have to create a dialog upon receiving the first
 * transmission of the INVITE request. The correct place to do that is inside
 * the callback previously registered to announce new INVITE. First, you will
 * build a SIP answer like 180 or 200 and you'll be able to create a dialog
 * by calling the following code:
 * <BR>osip_dialog_t *dialog;
 * <BR>osip_dialog_init_as_uas(&dialog, original_invite, response_that_you_build);
 * <BR>To make things working, you MUST create a VALID response: do not
 * forget to create a new tag and put it in the 'To' header. The dialog
 * management heavily depends on this tag.
 * </LI>
 * </UL>
 * <P>The dialog management is compliant with the latest SIP draft
 * (rfc2543bis-09). It should handle successfully most cases where
 * a remote UA is not compliant (no tag in the To of a final response!)
 * But for example, if you receive 2 answers from 2 uncompliant
 * UA, they will be detected as being related to the same dialog...
 * Do not change any code in oSIP or in your application... instead, you
 * should boycott such implementation. :-
 */

/**
 * @defgroup oSIP_DIALOG oSIP dialog Handling
 * @ingroup oSIP
 * @{
 */

#ifdef __cplusplus
extern "C"
{
#endif


#ifndef DOXYGEN
  typedef enum _osip_dialog_type_t
  {
    CALLER,
    CALLEE
  }
  osip_dialog_type_t;
#endif


/**
 * Structure for referencing a dialog.
 * @var osip_dialog_t
 */
  typedef struct osip_dialog osip_dialog_t;


/**
 * Structure for referencing a dialog.
 */
  struct osip_dialog
  {
    /*  char *dialog_id; ***implied*** *//* call-id:local_tag:remote-tag */
    char *call_id;
    char *local_tag;
    char *remote_tag;
    osip_list_t *route_set;
    int local_cseq;
    int remote_cseq;
    osip_to_t *remote_uri;
    osip_from_t *local_uri;
    osip_contact_t *remote_contact_uri;
    int secure;

    /* type of dialog (CALLEE or CALLER) */
    osip_dialog_type_t type;
    state_t state;		/* DIALOG_EARLY || DIALOG_CONFIRMED || DIALOG_CLOSED */
  };

/**
 * Allocate a osip_dialog_t element as a UAC.
 * <UL><LI>NOTE1: Only INVITE transactions can create a dialog.</LI>
 * <LI>NOTE2: The dialog should be created when the first response is received.
 *        (except for a 100 Trying)</LI>
 * <LI>NOTE3: Remote UA should be compliant! If not (not tag in the to header?)
 *        the old mechanism is used to match the request but if 2 uncompliant
 *        UA both answer 200 OK for the same transaction, they won't be detected.
 *        This is a major BUG in the old rfc.</LI></UL>
 * @param dialog The element to allocate.
 * @param response The response containing the informations.
 */
  int osip_dialog_init_as_uac (osip_dialog_t ** dialog,
			       osip_message_t * response);
/**
 * Allocate a osip_dialog_t element as a UAC.
 * <UL><LI>This could be used to initiate dialog with a NOTIFY coming
 * before the answer for a subscribe has reached us.</LI>
 * @param dialog The element to allocate.
 * @param next_request The response containing the informations.
 * @param local_cseq The local cseq
 */
  int osip_dialog_init_as_uac_with_remote_request (osip_dialog_t ** dialog,
						   osip_message_t *
						   next_request,
						   int local_cseq);

/**
 * Allocate a osip_dialog_t element as a UAS.
 * NOTE1: Only INVITE transactions can create a dialog.
 * NOTE2: The dialog should be created when the first response is sent.
 *        (except for a 100 Trying)
 * @param dialog The element to allocate.
 * @param invite The INVITE request containing some informations.
 * @param response The response containing other informations.
 */
  int osip_dialog_init_as_uas (osip_dialog_t ** dialog,
			       osip_message_t * invite,
			       osip_message_t * response);
/**
 * Free all resource in a osip_dialog_t element.
 * @param dialog The element to free.
 */
  void osip_dialog_free (osip_dialog_t * dialog);
/**
 * Set the state of the dialog.
 * This is useful to keep information on who is the initiator of the call.
 * @param dialog The element to work on.
 * @param type The type of dialog (CALLEE or CALLER).
 */
  void osip_dialog_set_state (osip_dialog_t * dialog, state_t type);
/**
 * Update the Route-Set as UAS of a dialog.
 * NOTE: bis-09 says that only INVITE transactions can update the route-set.
 * NOTE: bis-09 says that updating the route-set means: update the contact
 *       field only (AND NOT THE ROUTE-SET). This method follow this behaviour.
 * NOTE: This method should be called for each request (except 100 Trying)
 *       received for a dialog.
 * @param dialog The element to work on.
 * @param invite The invite received.
 */
  int osip_dialog_update_route_set_as_uas (osip_dialog_t * dialog,
					   osip_message_t * invite);
/**
 * Update the CSeq (remote cseq) during a UAS transaction of a dialog.
 * NOTE: All INCOMING transactions MUST update the remote CSeq.
 * @param dialog The element to work on.
 * @param request The request received.
 */
  int osip_dialog_update_osip_cseq_as_uas (osip_dialog_t * dialog,
					   osip_message_t * request);

/**
 * Match a response received with a dialog.
 * @param dialog The element to work on.
 * @param response The response received.
 */
  int osip_dialog_match_as_uac (osip_dialog_t * dialog,
				osip_message_t * response);
/**
 * Update the tag as UAC of a dialog?. (this could be needed if the 180
 * does not contains any tag, but the 200 contains one.
 * @param dialog The element to work on.
 * @param response The response received.
 */
  int osip_dialog_update_tag_as_uac (osip_dialog_t * dialog,
				     osip_message_t * response);
/**
 * Update the Route-Set as UAC of a dialog.
 * NOTE: bis-09 says that only INVITE transactions can update the route-set.
 * NOTE: bis-09 says that updating the route-set means: update the contact
 *       field only (AND NOT THE ROUTE-SET). This method follow this behaviour.
 * NOTE: This method should be called for each request (except 100 Trying)
 *       received for a dialog.
 * @param dialog The element to work on.
 * @param response The response received.
 */
  int osip_dialog_update_route_set_as_uac (osip_dialog_t * dialog,
					   osip_message_t * response);

/**
 * Match a request (response sent??) received with a dialog.
 * @param dialog The element to work on.
 * @param request The request received.
 */
  int osip_dialog_match_as_uas (osip_dialog_t * dialog,
				osip_message_t * request);

#ifndef DOXYGEN
  int osip_dialog_is_originator (osip_dialog_t * dialog);
  int osip_dialog_is_callee (osip_dialog_t * dialog);
#endif


#ifdef __cplusplus
}
#endif

/** @} */


#endif
