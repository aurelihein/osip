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

#include <osip/osip.h>
#include <osip/port.h>
#include "fsm.h"
 

/* Create a sipevent according to the SIP message buf. */
/* INPUT : char *buf | message as a string.            */
/* return NULL  if message cannot be parsed            */
sipevent_t *
osip_parse(char *buf)
{
    sipevent_t *se;
    int i;
    /* init */ 
    se      = (sipevent_t *)smalloc(sizeof(sipevent_t));
    /* se->sip = (sip_t *)     smalloc(sizeof(sip_t)); */
#ifdef TEST_PARSER_SPEED
    {
    int kk;
    int pstime1, pstime;
    struct timespec tv1;
    clock_gettime(CLOCK_REALTIME, &tv1);
    pstime = ((tv1.tv_sec*1000)+(tv1.tv_nsec/1000000));
    for (kk=0;kk<10000;kk++) {

    i = msg_init(&(se->sip));

    if (msg_parse(se->sip, buf)==-1)
      {
      fprintf(stdout,"msg_parse retrun -1\n");
      msg_free(se->sip);
      sfree(se->sip);
      }
    else
      { /* msg is parsed */
      msg_free(se->sip);
      sfree(se->sip);      
      }
    }
    clock_gettime(CLOCK_REALTIME, &tv1);
    pstime1 = ((tv1.tv_sec*1000)+(tv1.tv_nsec/1000000));
    fprintf(stdout,"CPU clock ticks for 10000 messages - T1: %i - T2: %i\n", pstime1,pstime);
    fprintf(stdout,"CPU time for 10000 messages - %d\n", (pstime1-pstime));
    }
    sfree(se);
    return NULL;
#endif
    /* parse message and set up an event */
    i = msg_init(&(se->sip));
    if (msg_parse(se->sip, buf)==-1)
      {
	TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,NULL,"could not parse message :-(\n"));
	msg_free(se->sip);
	sfree(se->sip);
	sfree(se);
	return NULL;
      }
	else
      {
	TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL5,NULL,"MESSAGE REC. CALLID:%s :-(\n",se->sip->call_id->number));
                   
	se->type = evt_settype_incoming_sipmessage(se->sip);
	return se;
      }
}

/* allocates an event from retransmitter.             */
/* USED ONLY BY THE STACK.                            */
/* INPUT : int transactionid | id of the transaction. */
/* INPUT : type_t type | type of event.               */
/* returns null on error. */
sipevent_t *
osip_new_event(type_t type,int transactionid)
{
  sipevent_t *sipevent;
  sipevent = (sipevent_t *) smalloc(sizeof(sipevent_t));

  sipevent->type = type;
  sipevent->sip = NULL;
  sipevent->transactionid = transactionid;
  return sipevent;
}

/* allocates an event from user.                      */
/* USED ONLY BY THE USER.                             */
/* INPUT : sip_t *sip | sip message for transaction.  */
/* returns null on error. */
sipevent_t *
osip_new_outgoing_sipmessage(sip_t *sip)
{
  sipevent_t *sipevent;
  sipevent = (sipevent_t *) smalloc(sizeof(sipevent_t));
  
  sipevent->sip = sip;
  sipevent->type = evt_settype_outgoing_sipmessage(sip);
  sipevent->transactionid = 0;
  return sipevent;
}

/* allocates an event from transport.                 */
/* USED ONLY BY THE TRANSPORT LAYER.                  */
/* INPUT : sip_t *sip | sip message for transaction.  */
/* returns null on error. */
sipevent_t *
osip_new_incoming_sipmessage(sip_t *sip)
{
  sipevent_t *sipevent;
  sipevent = (sipevent_t *) smalloc(sizeof(sipevent_t));
  
  sipevent->sip  = sip;
  sipevent->type = evt_settype_incoming_sipmessage(sip);
  sipevent->transactionid = 0;
  return sipevent;
}

/* finds the transaction context and add the sipevent in its fifo. */
/* USED ONLY BY THE TRANSPORT LAYER.                               */
/* INPUT : osip_t *config | config. contains the list of tr. context*/
/* INPUT : sipevent_t* sipevent | event to dispatch.               */
transaction_t *
osip_distribute_event(osip_t *config,sipevent_t* sipevent)
{
  transaction_t *transaction;
  transaction = NULL;
  
  if (EVT_IS_INCOMINGMSG(sipevent))
    {
      transaction = osip_find_asincomingmessage(config,
						sipevent);
      if (transaction!=NULL)
	{
	  /* be sure that the transaction know its mt_ua->config */
	  /* is it reall needed ?? */
	  transaction->config = config;
	  /* add event in fifo_t reserved for the new call */
	  sipevent->transactionid = transaction->transactionid;
	  fifo_add(transaction->transactionff,sipevent);
	  return transaction;
	  }
      else
	{
	  /* this happens some times on local test. */
	  /* It should be related to scheduling issue! */
	  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL2,NULL,"warning: transaction does not yet exist... %x callid:%s\n",sipevent,sipevent->sip->call_id->number));
	  
	  msg_free(sipevent->sip);
	  sfree(sipevent->sip);
	  sfree(sipevent); /* transaction thread will not delete it */
	  return NULL;
	}
    }
  else 
    {
      TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL2,stdout,"error: dispatcher No event recognized %x\n",sipevent));
      return NULL;
    }
}

type_t
evt_settype_incoming_sipmessage(sip_t *sip)
{
  if (MSG_IS_REQUEST(sip))
    {
      if (MSG_IS_INVITE(sip))
	return RCV_REQINVITE;
      if (MSG_IS_ACK(sip))
	return RCV_REQACK;
      return RCV_REQUEST;
    }
  else
    {
      if (MSG_IS_STATUS_1XX(sip))
	return RCV_STATUS_1XX;
      return RCV_STATUS_23456XX;
    }
}

type_t
evt_settype_outgoing_sipmessage(sip_t *sip)
{

  if (MSG_IS_REQUEST(sip))
    {
      if (MSG_IS_INVITE(sip))
	return SND_REQINVITE;
      if (MSG_IS_ACK(sip))
	return SND_REQACK;
      return SND_REQUEST;
    }
  else
    {
      if (MSG_IS_STATUS_1XX(sip))
	return SND_STATUS_1XX;
      return SND_STATUS_23456XX;
    }
}

