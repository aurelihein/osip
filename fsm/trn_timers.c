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
#include <osip/ua.h>
#include <osip/port.h>


int
timers_4uas_needretransmission(transaction_t *transaction)
{
  int counter = transaction->retransmissioncounter;
  transaction->retransmissioncounter++; 
  if (counter==1)   /* 500.000s    */
    return 1;
  if (counter==3)  /* 1.000.000s  */
     return 1;
  if (counter==7)  /* 2.000.000s  */
     return 1;
  if (counter==15) /* 4.000.000s  */
     return 1;
  if (counter==23) /* 8.000.000s  */
     return 1;
  if (counter==31) /* 12.000.000s */
     return 1;
  if (counter==39) /* 16.000.000s */
     return 1;
  if (counter==47) /* 20.000.000s */
     return 1;
  if (counter==55) /* 24.000.000s */
     return 1;
  if (counter==63) /* 28.000.000s */
     return 1;
  if (counter==71) /* 32.000.000s */
     return 1;

  return 0;
}

int
timers_4uac_needretransmission(transaction_t *transaction)
{
  int counter = transaction->retransmissioncounter;
  transaction->retransmissioncounter++; 
  if (counter==1)   /* 500.000s    */
    return 1;
  if (counter==3)  /* 1.000.000s  */
     return 1;
  if (counter==7)  /* 2.000.000s  */
     return 1;
  if (counter==15) /* 4.000.000s  */
     return 1;
  if (counter==23) /* 8.000.000s  */
     return 1;
  if (counter==39) /* 16.000.000s */
     return 1;
  if (counter==71) /* 32.000.000s */
     return 1;
  return 0;
}

int
timers_execute(list_t *transactions)
{
  transaction_t *transaction;
  long cur_time;
  int pos = 0;

  cur_time = time(NULL);
  while (!list_eol(transactions,pos))
    {
      int i;
      transaction = list_get(transactions,pos);
      i = cur_time-transaction->birth_time;
      if (transaction->state!=COMPLETED)
	{
	  if (i>NO_RESPONSE_TIMEOUT)
	    {
	      static int lost = 0;
	      lost++;
	      if (lost%10==0)
		printf("number of lost transaction %i\n",lost);
	      TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL0,NULL,"(UAX) killing lost transaction %i %s\n",transaction->transactionid,transaction->callid->number));
	      
	      list_remove(transactions,pos);
	      if (((osip_t*)transaction->config)->cb_killtransaction != NULL)
		((osip_t*)transaction->config)->cb_killtransaction(transaction);
	      transaction_free(transaction);
	      sfree(transaction);
	    }
	  else
	    {
	      /* retransmission is only for INVITE request    */
	      /* (for other requests, ANSWERED is never used) */
	      if (transaction->state==ANSWERED)
		{
		  if (1==timers_4uas_needretransmission(transaction))
		    {
		      sipevent_t *sipevent;
		      sipevent = osip_new_event(TIMEOUT,transaction->transactionid);
		      fifo_add(transaction->transactionff,sipevent);
		    }
		}
	      if (transaction->state==CALLING)  /* retransmission case */
		{
		  if (1==timers_4uac_needretransmission(transaction))
		    {
		      sipevent_t *sipevent;
		      sipevent = osip_new_event(TIMEOUT,transaction->transactionid);
		      fifo_add(transaction->transactionff,sipevent);
		    }
		}
	    }
	}
      else 
	{ /*  (transaction->state==COMPLETED) */
	  i = cur_time-transaction->completed_time;
	  if (i>END_TRANSACTION_TIMEOUT)
	    {
#ifdef ENABLE_DEBUG
	      static int succ = 0;
	      succ++;
	      if (succ%100==0)
		printf("number of successfull transaction %i\n",succ);
#endif
	      TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL2,NULL,"(UAX) Time to kill transaction %i %s\n",transaction->transactionid,transaction->callid->number));
	      
	      list_remove(transactions,pos);
	      if (((osip_t*)transaction->config)->cb_endoftransaction != NULL)
		((osip_t*)transaction->config)->cb_endoftransaction(transaction);
	      transaction_free(transaction);
	      sfree(transaction);
	    }
	}
      pos++;
    }
  return 0;
}

void *
timers_thread(osip_t *config)
{
  transaction_t *transaction;

  while (1) {
    susleep(500000);

    timers_execute(config->uas_transactions);
    timers_execute(config->uac_transactions);

    transaction = (transaction_t *)fifo_tryget(config->uas_timerff);
    while (transaction!=NULL)
      {
	list_add(config->uas_transactions,transaction,-1);
	transaction = (transaction_t *)fifo_tryget(config->uas_timerff);
      }

    transaction = (transaction_t *)fifo_tryget(config->uac_timerff);
    while (transaction!=NULL)
      {
	list_add(config->uac_transactions,transaction,-1);
	transaction = (transaction_t *)fifo_tryget(config->uac_timerff);
      }
  }
}
