/* To be placed in include/osip2 directory */

#ifndef _OSIP_TIME_H_
#define _OSIP_TIME_H_

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef WIN32
/* Windows */

  struct timeval;

/* Operations on struct timeval */
#define osip_timerisset(tvp)         ((tvp)->tv_sec || (tvp)->tv_usec)
#define osip_timercmp(tvp, uvp, cmp) \
        ((tvp)->tv_sec cmp (uvp)->tv_sec || \
         (tvp)->tv_sec == (uvp)->tv_sec && (tvp)->tv_usec cmp (uvp)->tv_usec)
#define osip_timerclear(tvp)         (tvp)->tv_sec = (tvp)->tv_usec = 0

/* gettimeofday() for Windows using struct timeval */
int osip_gettimeofday(struct timeval *tp,void *tz);

#else
/* Non-Windows with struct timeval defined in sys/time.h */

  struct timeval;
/* Or perhaps #include <osipparser2/osip_port.h> at the beginning of the file
if HAVE_SYS_TIME_H is defined??? */

/* Operations on struct timeval */
#define osip_timerisset(tvp)			timerisset(tvp)
#define osip_timercmp(tvp, uvp, cmp)	timercmp(tvp,uvp,cmp)
#define osip_timerclear(tvp)			timerclear(tvp)

/* osip_gettimeofday() */
#define osip_gettimeofday(tp,tz) gettimeofday((struct timeval *)(tp),tz)

#endif /* #ifdef WIN32 */

/* Windows-Linux common time-related functions */
void add_gettimeofday(struct timeval *atv,int ms);
void min_timercmp(struct timeval *tv1,struct timeval *tv2);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _OSIP_TIME_H_ */
