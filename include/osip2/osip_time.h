/* To be placed in include/osip2 directory */

#ifndef _OSIP_TIME_H_
#define _OSIP_TIME_H_

#ifdef __cplusplus
extern "C"
{
#endif

/* Common time-related functions and data types */

/* struct timeval, as defined in <sys/time.h>, <winsock.h> or <winsock2.h> */
struct timeval;

/* Time manipulation functions */
void add_gettimeofday(struct timeval *atv,int ms);
void min_timercmp(struct timeval *tv1,struct timeval *tv2);

/* OS-dependent */
#if defined(WIN32) || defined(_WIN32_WCE)
/* Operations on struct timeval */
#define osip_timerisset(tvp)         ((tvp)->tv_sec || (tvp)->tv_usec)
#define osip_timercmp(tvp, uvp, cmp) \
        ((tvp)->tv_sec cmp (uvp)->tv_sec || \
         (tvp)->tv_sec == (uvp)->tv_sec && (tvp)->tv_usec cmp (uvp)->tv_usec)
#define osip_timerclear(tvp)         (tvp)->tv_sec = (tvp)->tv_usec = 0

/* osip_gettimeofday() for Windows */
int osip_gettimeofday(struct timeval *tp,void *tz);

#else
/* Operations on struct timeval */
#define osip_timerisset(tvp)            timerisset(tvp)
#define osip_timercmp(tvp, uvp, cmp)    timercmp(tvp,uvp,cmp)
#define osip_timerclear(tvp)            timerclear(tvp)

/* osip_gettimeofday() == gettimeofday() */
#define osip_gettimeofday gettimeofday

#endif /* #ifdef WIN32 */

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _OSIP_TIME_H_ */
