
#include "app.h"

#ifdef OSIP_MT

void *
transaction_thread(transaction_t *transaction)
{
  sipevent_t *se;
#ifdef __VXWORKS_OS__
  taskSafe(); /* prevent from deletion */
#endif
  while (1)
    {
      se = (sipevent_t *)fifo_get(transaction->transactionff);
      if (se==NULL)
	return NULL;
      if (transaction_execute(transaction,se)<1) /* deletion asked */
	{
#ifdef __VXWORKS_OS__
	  taskUnsafe(); /* thread is safe for deletion */
#endif
	  return NULL;
	}
  }
}


int
transaction_mt_init(transaction_mt_t **transaction_mt)
{
  *transaction_mt = (transaction_mt_t *)smalloc(sizeof(transaction_mt_t));
  if (*transaction_mt==NULL)
    return -1;
  return 0;
}


void
transaction_mt_set_transaction(transaction_mt_t *transaction_mt,
			       transaction_t *transaction)
{
  transaction_mt->transaction = transaction;
}

int
transaction_mt_start_transaction(transaction_mt_t *transaction_mt)
{
  transaction_mt->thread = sthread_create(20000,NULL,(void *(*)(void *))transaction_thread,(void *)transaction_mt->transaction);
  /* try to give a highest priority to this task     */
  /* this is only supported by some OS and currently */
  /* only used on VxWorks. */

  if (transaction_mt->thread==NULL) return -1;

  sthread_setpriority(transaction_mt->thread, 1);
  transaction_mt->transaction->your_instance = transaction_mt;
  return 0;
}

void
transaction_mt_stop_transaction(transaction_mt_t *transaction_mt)
{
#ifndef __VXWORKS_OS__
  if (transaction_mt->thread!=NULL)
#else
  if (transaction_mt->thread!=ERROR)
#endif
    {
      int i;
      sipevent_t *sipevent;
      sipevent = osip_new_event(KILL_TRANSACTION,transaction_mt->transaction->transactionid);
      fifo_add(transaction_mt->transaction->transactionff,sipevent);

      /* arg is a pointer to a pthread on linux, solaris... */
      /* and an int on VxWorks */
      i = sthread_join(transaction_mt->thread); 
#ifndef __VXWORKS_OS__
      sfree(transaction_mt->thread);
#endif
    }
  sfree(transaction_mt);
}

#endif /* OSIP_MT */
