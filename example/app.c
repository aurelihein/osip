
#include "example_rcfile.h"
#include "app.h"
#include <osip/timers.h>
#include <osip/sdp_negoc.h>

#if defined(HAVE_SIGNAL_H) || defined(__VXWORKS_OS__) || defined(WIN32)
#include <signal.h>
#else /* HAVE_SYS_SIGNAL_H? */
#include <sys/signal.h>
#endif

session_t *sessiontest;
int global_static_code=200; /* default return code. Can be changed by user */

#ifdef WIN32
#include <stdio.h>
#include <stdlib.h>
#include <winsock.h>
#else
#include <unistd.h>

#include <sys/types.h>
#endif

#ifndef __VXWORKS_OS__
#ifndef WIN32
#include <sys/time.h>
#include <libgen.h>
#endif
#else
#include <selectLib.h>
#endif

osip_t *myconfig;

FILE *logfile;

void app_invite(char *tostring);
void app_bye();

#ifdef WIN32
#include <process.h>
#include <io.h>
#include <conio.h>
#define onexit my_onexit
#define getpid _getpid
#define read _read
#define fgets(s, max, fp) gets(s)
#endif

void onexit();
void onsignal(int s);
void onalarm(int sig);

void
set_debug_level(char *tmp)
{
  if (0==strncmp(tmp,"0",1)) {   TRACE_ENABLE_LEVEL(0); }
  if (0==strncmp(tmp,"1",1)) {   TRACE_ENABLE_LEVEL(1); }
  if (0==strncmp(tmp,"2",1)) {   TRACE_ENABLE_LEVEL(2); }
  if (0==strncmp(tmp,"3",1)) {   TRACE_ENABLE_LEVEL(3); }
  if (0==strncmp(tmp,"4",1)) {   TRACE_ENABLE_LEVEL(4); }
  if (0==strncmp(tmp,"5",1)) {   TRACE_ENABLE_LEVEL(5); }
  if (0==strncmp(tmp,"d",1)) {
  TRACE_DISABLE_LEVEL(0);
  TRACE_DISABLE_LEVEL(1);
  TRACE_DISABLE_LEVEL(2);
  TRACE_DISABLE_LEVEL(3);
  TRACE_DISABLE_LEVEL(4);
  TRACE_DISABLE_LEVEL(5);
  }
}
void
set_sipreturn_code(char *tmp)
{
  if (0==strncmp(tmp,"c",1))
    {
    int i;
    char *newcode;
    newcode = (char *)smalloc(11);
    printf("<app.c> Give the new return code to use:\n");
    fgets(newcode,10,stdin);
    i = atoi(newcode);
    sfree(newcode);
    if (100<=i&&i<700)
      {
      global_static_code = i;
      printf("<app.c> Status code changed!\n");
      }
    else
      printf("<app.c> error: Status code NOT changed!\n");
    
    }
}

#ifdef WIN32
char *
simple_readline(int descr, int forever)
{
  int i = 0;
  char *tmp = (char *)smalloc(201);

  if (forever) {
    fgets(tmp, 200, stdin);
  }
  else {
      while (kbhit()) {

        tmp[i] = (char)_getche();

        if (tmp[i] == '\r') {
        
	        printf("\n");

            tmp[i] = '\0';

            i = 0;
        }
        else {

            if (tmp[i] == '\b') {
                if (i > 0) {
                    printf(" ");
                    printf("\b");
                    i--;
                }
            }
            else {
                if (i < sizeof(tmp) - 2)
                    i++;
            }
        }
     }
      
     if (i == 0) {
       sfree(tmp);
       return(NULL);
     }
  }
      
  set_debug_level(tmp);
  set_sipreturn_code(tmp);
  return tmp;
}
#else
char *
simple_readline(int descr, int forever)
{
  int ret;
  struct timeval tv;
  fd_set fset;
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  FD_ZERO(&fset);
  FD_SET(descr, &fset);
  
  if (forever)
    ret = select(descr+1, &fset ,NULL ,
		 NULL, NULL );
  else
    ret = select(descr+1, &fset ,NULL ,
		 NULL, &tv );
  if (FD_ISSET(descr, &fset))
    {
    char *tmp;
    tmp = (char *)smalloc(201);
    read(descr,tmp,200);
    set_debug_level(tmp);
    set_sipreturn_code(tmp);
    return tmp;
    }
  return NULL;
}
#endif

void
usage()
{
  printf("usage: ua -m mode -f config_file -d trace_level -t to_field\n");
  printf("mode is mandatory:\n");
  printf("\t0 for command mode\n");
  printf("\t1 for slow test mode (2 transactions/sec)\n");
  printf("\t2 for high load test (200 transactions/20sec)\n");
  printf("\t3 for UAS behavior (only answer to transactions)\n");
  
  printf("config_file is mandatory:\n");
  printf("\tfile contains the whole configuration of SIP agent\n");
  printf("trace_level is the level of trace\n");
  printf("\ttrace_level is an integer between 0 and 5\n");
  
  printf("to_field is the url of the correspondant\n");
  printf("\turl looks like <sip:jack@127.0.0.1>\n");
}

int
ua_sdp_accept_audio_codec(sdp_context_t *context,
                       char *port, char *number_of_port,
                       int audio_qty, char *payload)
{
  /* this may come from buggy implementation who                 */
  /* propose several sdp lines while they only want 1 connection */
  if (0!=audio_qty) return -1;
  
  if (0==strncmp(payload,"0",1)||0==strncmp(payload,"3",1)||
      0==strncmp(payload,"7",1)||0==strncmp(payload,"4",1))
    return 0;
  return -1;
}

int
ua_sdp_accept_video_codec(sdp_context_t *context,
                       char *port, char *number_of_port,
                       int video_qty, char *payload)
{
  /* this may come from buggy implementation who                 */
  /* propose several sdp lines while they only want 1 connection */
  if (0!=video_qty) return -1;
  /* ... */
  return -1;
}

int
ua_sdp_accept_other_codec(sdp_context_t *context,
                          char *type, char *port,
                          char *number_of_port, char *payload)
{
  /* ... */
  return -1;
}

char *
ua_sdp_get_audio_port(sdp_context_t *context, int pos_media)
{
  return sgetcopy("23010"); /* this port should not be static ... */
  /* also, this method should be called more than once... */
  /* If there is more than one audio line, this may fail :( */
}

#ifdef __VXWORKS_OS__
int
main_osipstack(int mode, int trace_level, char *tostring)
{
  mt_ua_t *mt_ua;
  char *configfile = NULL;
  logfile = NULL;
  configfile = sgetcopy("empty");
#else
  int
    main(int argc, char **argv)
    {
     mt_ua_t *mt_ua;
    char *tostring = NULL;
    char *configfile = NULL;
    int trace_level = 2; /* should be between 0 and 5 */
    int mode = 0;        /* should be between 0 and 2 */
    int arg_num;
#ifdef WIN32
    int err;
    WSADATA wsdata;
    
    err = WSAStartup(MAKEWORD(2, 0), &wsdata);
#endif
    /*  atexit(&onexit); */
    
    /* signal(SIGSEGV, &onsignal); */
    printf("This is a facility to test the oSIP library!\n");
    if ((argc<2))
      {
      usage();
      exit(0);
      }
    
    arg_num = 1;
    while (arg_num<argc)
      {
      if (strlen(argv[arg_num])!=2)
	{
	usage();
	exit(0);
	}
      if (strncmp("-f",argv[arg_num],2)==0)
	{
	/* the next arg is the config file */
	arg_num++;
	configfile = sgetcopy(argv[arg_num]);
	}
      if (strncmp("-t",argv[arg_num],2)==0)
	{
	/* the next arg is the address in the to field */
	arg_num++;
	tostring =  sgetcopy(argv[arg_num]);
	}
      if (strncmp("-m",argv[arg_num],2)==0)
	{
	/* the next arg is the mode to start... */
	arg_num++;
	mode = atoi(argv[arg_num]);
	}
      if (strncmp("-d",argv[arg_num],2)==0)
	{
	/* the next arg is the trace level... */
	arg_num++;
	trace_level = atoi(argv[arg_num]);
	}
      arg_num++;
      }
    
#ifndef WIN32
    {
    char *tmp;
    tmp = smalloc(strlen(argv[0])+10);
    sprintf(tmp,"%s_%i",basename(argv[0]),(int)getpid());
    logfile = fopen(tmp, "w");
    /* init logger facility */
    sfree(tmp);
    }
#endif
    if (configfile==NULL)
      {
      usage();
      exit(0);
      }
#endif
    
    TRACE_INITIALIZE(trace_level,logfile);
    
    if ((mode!=3)&&(tostring==NULL))
      {
      usage();
      exit(0);
      }
    
    
    {
      int i;
      /* Load the configuration */
      i = loadsipconf(configfile);
      if (i!=0)
	{
	  usage();
	  exit(0);
	}
      sfree(configfile);
    }
    
    { /* set the proxy if you want... */
      int i;
      char *proxy = getsipconf("sipproxy");
      url_t *prox;
      osip_init(&myconfig);
      if (proxy==NULL)
	  fprintf(stdout,"<app.c> You will not use a proxy in this configuration\n");
      else
	{
	  i = url_init(&prox);
	  i = url_parse(prox,proxy);
	  osip_init_proxy(myconfig, prox);
	  if (i==-1)
	    {
	      fprintf(stdout,"<app.c> the URL of the proxy is not valid! check the value in the rc file\n");
	      return -1;
	    }
	}
      i = sdp_config_init();
      if (i!=0) {
	fprintf(stderr,"Failed to initialize the SDP negociator\n");
	exit(1);
      }
      sdp_config_set_o_username(sgetcopy("userX"));
      sdp_config_set_o_session_id(sgetcopy("20000001"));
      sdp_config_set_o_session_version(sgetcopy("20000001"));
      sdp_config_set_o_nettype(sgetcopy("IN"));
      sdp_config_set_o_addrtype(sgetcopy("IP4"));
      sdp_config_set_o_addr(sgetcopy("192.168.1.114"));
      
      sdp_config_set_c_nettype(sgetcopy("IN"));
      sdp_config_set_c_addrtype(sgetcopy("IP4"));
      sdp_config_set_c_addr(sgetcopy("192.168.1.114"));
      
      /* ALL CODEC MUST SHARE THE SAME "C=" line and proto as the media 
	 will appear on the same "m" line... */
      sdp_config_add_support_for_audio_codec(sgetcopy("0"),
					     NULL,
					     sgetcopy("RTP/AVP"),
					     NULL, NULL, NULL,
					     NULL,NULL,
					     sgetcopy("0 PCMU/8000"));
      sdp_config_add_support_for_audio_codec(sgetcopy("3"),
					     NULL,
					     sgetcopy("RTP/AVP"),
					     NULL, NULL, NULL,
					     NULL,NULL,
					     sgetcopy("3 GSM/8000"));
      sdp_config_add_support_for_audio_codec(sgetcopy("7"),
					     NULL,
					     sgetcopy("RTP/AVP"),
					     NULL, NULL, NULL,
					     NULL,NULL,
					     sgetcopy("7 LPC/8000"));
      sdp_config_add_support_for_audio_codec(sgetcopy("8"),
					     NULL,
					     sgetcopy("RTP/AVP"),
					     NULL, NULL, NULL,
					     NULL,NULL,
					     sgetcopy("8 PCMA/8000"));

      sdp_config_set_fcn_accept_audio_codec(&ua_sdp_accept_audio_codec);
      sdp_config_set_fcn_accept_video_codec(&ua_sdp_accept_video_codec);
      sdp_config_set_fcn_accept_other_codec(&ua_sdp_accept_other_codec);
      sdp_config_set_fcn_get_audio_port(&ua_sdp_get_audio_port);
    }


#ifndef OSIP_MT
    ua_init(&mt_ua, atoi(getsipconf("localport")), myconfig);

    /* start the main loop */
    ua_start_design2(mt_ua);
    {
      clock_t clock1,clock2;
      clock1 = time(NULL);
      while (1)
	{
	  transaction_t *transaction;
	  int max;
	  
	  /* do it only each 500ms */
	  /* this may not be possible on non-real time OS */
	  clock2 = time(NULL);
	  if (clock2-clock1>2) /* more than one sec */
	    {
	      TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL3,stdout,"start timers management:)\n"));
	      clock1 = clock2;
	      timers_execute(myconfig->uas_transactions);
	      timers_execute(myconfig->uac_transactions);
	    }

	  osip_execute(myconfig);

	  max = 10;
	  while ((osipudpd_execute(mt_ua,0,0)>0)&&(max!=0))
	    {
	      max--;
	    }

	  transaction = (transaction_t *)fifo_tryget(myconfig->uas_timerff);
	  while (transaction!=NULL)
	    {
	      list_add(myconfig->uas_transactions,transaction,-1);
	      transaction = (transaction_t *)fifo_tryget(myconfig->uas_timerff);
	    }
	  
	  transaction = (transaction_t *)fifo_tryget(myconfig->uac_timerff);
	  while (transaction!=NULL)
	    {
	      list_add(myconfig->uac_transactions,transaction,-1);
	      transaction = (transaction_t *)fifo_tryget(myconfig->uac_timerff);
	    }	  
	    
	} 
    }
#endif
#ifdef OSIP_MT
    mt_ua_init(&mt_ua, atoi(getsipconf("localport")), myconfig);
        
    mt_ua_start_design1(mt_ua);
    
    
    if (mode==2) /* high load test */
      {
      int active = 1;
      int i=10;
      printf("Entering high load test (launch 200 transactions each 20s!\n");
      printf("keystrokes:\n");
      printf("0 1 2 3 4 5: enable trace levels   d: disable trace\n");
      printf("c: new static return code wanted\n");
      printf("s: stop test\n");
      printf("r: restart test\n");
      while (i!=0)
	{
	int j=50;
	char *tmp;
	
	tmp = simple_readline(0,0);
	if (tmp!=NULL)
	  {
	  if (strncmp(tmp,"s",1)==0)
	    active = 0;
	  if (strncmp(tmp,"r",1)==0)
	    active = 1;
	  sfree(tmp);
	  }
	if (active==1)
	  {
	    int pending_transactions = list_size(myconfig->uac_transactions)
	      + list_size(myconfig->uas_transactions);
	    /* limit the number of transactions */
	  while (j!=0)
	    {
	      if (pending_transactions<600)
		app_invite(tostring);
	      else
		susleep(200000); /* the application is overloeaded... */
	      j--;
	    }
	  /*  i--; */ /* loop for ever... */
	  }
	/* susleep(1000000); */
	}
      }
    if (mode==1) /* slower test */
      {
      int active = 1;
      char *tmp;
      printf("keystrokes:\n");
      printf("0 1 2 3 4 5: enable trace levels   d: disable trace\n");
      printf("c: new static return code wanted\n");
      printf("s: stop test\n");
      printf("r: restart test\n");
      while (1)
	{
	tmp = simple_readline(0,0);
	if (tmp!=NULL)
	  {
	  if (strncmp(tmp,"s",1)==0)
	    active = 0;
	  if (strncmp(tmp,"r",1)==0)
	    active = 1;
	  sfree(tmp);
	  }
	susleep(500000); /* ok for 1s */
	if (active==1)
	  {
	  app_invite(tostring);
	  /* i++; */
	  }
	}
      }
    
    if (mode==0) /* Command mode */
      {
      char *tmp;
      int i=1;
      
      while (i!=0)
	{
	printf("keystrokes:\n");
	printf("0 1 2 3 4 5: enable trace levels   d: disable trace\n");
	printf("c: new static return code wanted\n");
	printf("i: start an INVITE transaction    t: start N INVITE trans\n");
	printf("r: start a REGISTER transaction   b: start a BYE trans\n");
	printf("osip command> ");
	tmp = simple_readline(0,1);
	/*fgets(tmp, 100, stdin); */
	
	if (0==strncmp(tmp,"i",1))
	  {
	  fprintf(stdout,"INVITE\n");
	  app_invite(tostring);
	  }
	if (0==strncmp(tmp,"r",1))
	  {
	  char *expires;
	  expires = (char *) smalloc(5);
	  sprintf(expires,"%i",3600);
	  fprintf(stdout,"REGISTER expires after: %s\n",expires);
	  sip_register(expires);
	  sfree(expires);
	  } 
	if (0==strncmp(tmp,"b",1))
	  {
	  fprintf(stdout,"BYE transaction: \n");
	  app_bye();
	  }
	if (0==strncmp(tmp,"t",1))
	  {
	  int j;
	  sfree(tmp);
	  tmp = (char *)smalloc(100);
	  fprintf(stdout,"Give the number of transactions to start\n");
	  fgets(tmp, 100, stdin);
	  j = atoi(tmp);
	  fprintf(stdout,"INVITE * %i\n",j);
	  while (j!=0)
	    {
	    app_invite(tostring);
	    j--;
	    }
	  }
	if (tmp!=NULL) sfree(tmp);
	}
      }
    
    while (mode==3) /* only a UAC behavior */
      {
      char *tmp;
      printf("keystrokes:\n");
      printf("0 1 2 3 4 5: enable trace levels   d: disable trace\n");
      printf("c: new static return code wanted\n");
      tmp = simple_readline(0,1);
      if (tmp!=NULL) sfree(tmp);
      }
#endif    
    return 1; /* ok */

    }
  
  
  void
    uaapp_annouceoutinvite(sip_t *invite,transaction_t *tr)
    {
    }
  
  void
    uaapp_annouceoutrequest(sip_t *request,transaction_t *tr)
    {
    }
  
  void
    uaapp_annouceincresponse(sip_t *response,transaction_t *tr)
    {
    /* Here we SHOULD give order to send ack.         */
    /* (only for INVITE's response and for code>=200) */
    
    if (MSG_IS_RESPONSEFOR(response,"INVITE")
	&& !MSG_TEST_CODE(response,100)
	&& !MSG_TEST_CODE(response,180))
      respondto200forinvite(tr,response);
    }
  
  void
    uaapp_annouceincack(sip_t *request,transaction_t *tr)
    {
    /* do nothing */
    }
  
  void
    uaapp_annouceincrequest(sip_t *request,transaction_t *tr)
    {
    /* autoresponse 200 to all request except ACK */
    if (!MSG_IS_ACK(request))
      respondtorequest(tr,global_static_code);
    }
  
  void
    uaapp_annouceincinvite(sip_t *invite,transaction_t *tr)
    {
    respondtoinvite(tr,180);
    /* usleep(10000000); timeout to allow cancel*/ 
    respondtoinvite(tr,global_static_code);
    }
  void
    uaapp_annouceoutresponse(sip_t *response,transaction_t *tr)
    {
    
    }
  
  void
    app_invite(char *tostring)
    {
    static int only_once = 0;
    
    if (only_once == 0)
      {
      only_once = 1;
      sessiontest   = (session_t *)smalloc(sizeof(session_t));
      to_init(&(sessiontest->to));
      to_parse(sessiontest->to, tostring);

      {
      header_t *header;
      header_init(&header);
      
      header->hname = (char *) smalloc(strlen("subject")+1);
      sstrncpy(header->hname,"subject",strlen("subject"));
      header->hvalue = (char *) smalloc(strlen("Hello.")+1);
      sstrncpy(header->hvalue,"Hello.",strlen("Hello."));
      
      sessiontest->headers = (list_t *) smalloc(sizeof(list_t));
      list_init(sessiontest->headers);
      list_add(sessiontest->headers,header,-1);
      }
      {
      header_t *header;
      header_init(&header);
      
      header->hname = (char *) smalloc(strlen("User-Agent")+1);
      sstrncpy(header->hname,"User-Agent",strlen("User-Agent"));
      header->hvalue = (char *) smalloc(strlen("oSIP-UA/0.6.0")+1);
      sstrncpy(header->hvalue,"oSIP-UA/0.6.0",strlen("oSIP-UA/0.6.0"));
      
      sessiontest->headers = (list_t *) smalloc(sizeof(list_t));
      list_init(sessiontest->headers);
      list_add(sessiontest->headers,header,-1);
      }

      sessiontest->from = NULL;
      sessiontest->contact = NULL;
      sessiontest->proxy = NULL;
      }
    sessiontest->transactionid = sip_invite(&sessiontest);
    
    }
  
  
  void app_bye()
    {
    sip_bye(&sessiontest);
    }
  
  /* for use of int atexit(void (*func)(void) */
  void
    onexit(){
  fclose(logfile);
  /* close(currentsipconfig->udpout_sock); to be done in stack? */
  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,stdout,"program end normally :)\n"));
  exit(1);
  }
  
#if defined HAVE_SIGNAL_H || defined HAVE_SYS_SIGNAL_H 
  /* for use of void (*signal(int sig, void (*func)(int)))(int);  */
  void
    onsignal(int s){
  TRACE(trace(__FILE__,__LINE__,TRACE_LEVEL1,stdout,"program end abnormally (%i) :(\n",s));
  exit(1);
  }
#endif
  

