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


#ifndef _DOC_H_
#define _DOC_H_

/** @mainpage The GNU oSIP stack Documentation
 *
 * @section intro_sec Introduction
 *
 * oSIP stands for the GNU oSIP library.
 * 
 * It aims to implement the lower layers of the SIP protocol
 * defined by the <A HREF="http://www.ietf.org/rfc/rfc3261.txt">rfc3261</A>
 *
 ************************************************************
 ************************************************************
 *
 * <HR>
 * @section history_sec History
 *
 * The oSIP stack project has been started in September 2000
 * since the early days of the SIP protocol.
 * 
 * The initial version was quite close to the actual one in
 * terms of features. The SIP parser and the SIP state machines
 * were already provided. At this point and till version 0.9.7,
 * the API was not very clean but osip was already stable,
 * portable, flexible and compliant with SIP.
 * 
 * After rfc2543 were released, a lot of developments were made
 * to follow the drafts that were produced prior to the final
 * release of rfc3261. (around version 0.8.X).
 *
 * A lot of users were requesting help on the API and were
 * reporting their difficulties to use it. I have decided
 * to refine osip into a new osip2 version which would offer
 * a simpler and nicer API. With a very few exception, only
 * the API has been changed between 0.9.7 and 2.0.0. From
 * this version, osip has been much readable.
 *
 * However, oSIP is still complex to use. There are reasons
 * for that. oSIP was made to be flexible (which make the API
 * quite large). oSIP is architecture free (but you need to
 * understand how it works to use it correctly). oSIP is
 * implementing only low layers of SIP (so you still can
 * build malformed messages, or uncompliant ones). oSIP will
 * not warn you about most errors that you can make.
 *
 * <B>
 * Be aware, that if you use osip, you still have to read
 * carefully the rfc. oSIP is not easy to use, but I'm
 * convinced there are benefits in terms of flexibility.
 * If you find osip too complex to use, please look for
 * another adequate solution.
 * </B>
 *
 ************************************************************
 ************************************************************
 *
 * <HR>
 * @section install_sec Installation
 *
 * @subsection step1 Step 1:
 * 
 * If you are using the CVS:
 * <PRE>
 *   $> export CVS_RSH="ssh"
 *   $> cvs -z3 -d:ext:anoncvs@savannah.gnu.org:/webcvs/osip co osip
 *      
 *   The SSHv2 public key fingerprints for the machine hosting the cvs trees are:
 *
 *    RSA: 1024 80:5a:b0:0c:ec:93:66:29:49:7e:04:2b:fd:ba:2c:d5
 *    DSA: 1024 4d:c8:dc:9a:99:96:ae:cc:ce:d3:2b:b0:a3:a4:95:a5
 *
 *   $> ./autogen.sh
 * </PRE>
 *
 *     Note that you must have libtool, autoconf & automake to be able
 *     to run the autogen.sh script.
 *
 * If you have downloaded the archive, run the following command instead:
 *
 * <PRE>
 *   $> tar -xvzf libosip2-X.X.X.tar.gz
 * </PRE>
 *
 * @subsection step2 Step 2:
 *
 * As usual, run the following on unix platforms:
 *
 * <PRE>
 *   $> ./configure 
 *   $> make
 *   # make install
 * </PRE>
 *
 * @subsection step3 Step 3: compiling for embedded platforms.
 *
 *
 *
 ************************************************************
 ************************************************************
 *
 * <HR>
 * @section features_sec Features
 *
 *
 * oSIP is a not a complete SIP stack and will never implements
 * all the specifications. Instead, the goal is to provide
 * a limited set of features common to any kind of SIP Agents.
 * Thus oSIP is not oriented towards any particular implementations
 * and can be used for implementing SIP End-Point, Proxy
 * or any kind of more specific SIP Agent such as B2BUA.
 *
 * The minimal common required features for any SIP Agent
 * provided by osip are describe below.
 *
 * However, some extra facilities oriented towards SIP End-Points
 * are provided. Some are considered usefull and stable such as the
 * dialog management API. One features is particularly not flexible
 * such as the SDP negotiation facility and you should consider
 * implementing your own. If you still want 
 * 
 *
 * @subsection sip_parser_sec The SIP parser:
 *
 * The initial feature implemented in osip is a SIP parser. There
 * is not much to say about it: it is capable of parsing and
 * reformating SIP requests and answers.
 *
 * The details of the parsing tools available are listed below:
 *
 * <UL>
 * <LI>SIP request/answer</LI>
 * <LI>SIP uri</LI>
 * <LI>specific headers
 *  <UL>
 *  <LI>Via</LI>
 *  <LI>CSeq</LI>
 *  <LI>Call-ID</LI>
 *  <LI>To, From, Route, Record-Route...</LI>
 *  <LI>Authentication related headers</LI>
 *  <LI>Content related headers</LI>
 *  <LI>Accept related headers</LI>
 *  <LI>...</LI>
 *  <LI>Generic header</LI>
 *  </UL>
 * <LI>Attachement parser (should support mime) </LI>
 * <LI>SDP parser </LI>
 * </UL>
 *
 * @subsection sip_statemachine_sec The SIP transaction state machines:
 *
 * The interesting and somewhat complex feature implemented
 * by osip is the 4 states machines that applied to the different
 * transactions defined by the SIP rfc.
 *
 * SIP defines the following 4 state machines, abreviations
 * used in osip are provided below:
 *   <UL>
 *   <LI>ICT : Invite Client Transaction (Section 17.1.1) </LI>
 *   <LI>NICT: Non Invite Client Transaction (Section 17.1.2) </LI>
 *   <LI>IST : Invite Server Transaction (Section 17.2.1) </LI>
 *   <LI>NIST: Non Invite Server Transaction (Section 17.2.2) </LI>
 *   </UL>
 *
 * As you can notice if you have read the rfc (do it!), those
 * 4 state machines are provided as drawings within the SIP
 * rfc3261.txt (section 17.1 and 17.2)
 *
 * As an exemple of what you'll find in the rfc3261, here is the
 * drawing that apply to the "Invite Client Transaction" (page 127)
 * 
<PRE>
                               |INVITE from TU
             Timer A fires     |INVITE sent
             Reset A,          V                      Timer B fires
             INVITE sent +-----------+                or Transport Err.
               +---------|           |---------------+inform TU
               |         |  Calling  |               |
               +-------->|           |-------------->|
                         +-----------+ 2xx           |
                            |  |       2xx to TU     |
                            |  |1xx                  |
    300-699 +---------------+  |1xx to TU            |
   ACK sent |                  |                     |
resp. to TU |  1xx             V                     |
            |  1xx to TU  -----------+               |
            |  +---------|           |               |
            |  |         |Proceeding |-------------->|
            |  +-------->|           | 2xx           |
            |            +-----------+ 2xx to TU     |
            |       300-699    |                     |
            |       ACK sent,  |                     |
            |       resp. to TU|                     |
            |                  |                     |      NOTE:
            |  300-699         V                     |
            |  ACK sent  +-----------+Transport Err. |  transitions
            |  +---------|           |Inform TU      |  labeled with
            |  |         | Completed |-------------->|  the event
            |  +-------->|           |               |  over the action
            |            +-----------+               |  to take
            |              ^   |                     |
            |              |   | Timer D fires       |
            +--------------+   | -                   |
                               |                     |
                               V                     |
                         +-----------+               |
                         |           |               |
                         | Terminated|<--------------+
                         |           |
                         +-----------+

                 Figure 5: INVITE client transaction

</PRE>
 *
 * As you can expect, with osip an Invite Client Transaction may be
 * in the CALLING, PROCEEDING, COMPLETED or TERMINATED state. To
 * "execute" the state machine, you will build events, provide them
 * to the correct transaction context and the the state of the
 * transaction will be updated if the event is allowed in the current
 * state.
 * 
 * Events are divided in three categories:
 * <UL>
 * <LI>SIP messages</LI>
 * <LI>Timers</LI>
 * <LI>transport errors</LI>
 * </UL>
 *
 * @subsection sip_port_sec The supported platforms:
 *
 * To ease development of SIP portable application, osip
 * provide abstractions methods for threads, semaphore,
 * and mutex.
 *
 * You still have the choice to compile osip without thread
 * support which you may be required on some embedded platforms.
 *
 * Specific port of threads, semaphore and mutex exists for
 * the following platforms:
 * <UL>
 * <LI>linux/unix (posix thread)</LI>
 * <LI>Mac OS X (Darwin)</LI>
 * <LI>VxWorks</LI>
 * <LI>PSOS</LI>
 * <LI>Win32 (NT, 2000, XP, may be more) </LI>
 * <LI>WinCE (report to be possible)</LI>
 * </UL>
 *
 *
 * Please report any porting issue to the mailing list <osip@atosc.org>
 * (subscribe first!) or directly to me <jack@atosc.org>.
 *
 * If you are building a new port, I'll certainly think about merging
 * it.
 *
 * @subsection sip_facilities_sec The other facilities:
 *
 * oSIP also give small facilities which are completly optional.
 *
 * Among them, the <B>dialog facility(osip_dialog.h)</B>, mainly to
 * be used by SIP End-Points, is a stable extra feature. It will
 * allow you to build a structure described in the rfc3261 (Section
 * 12: Dialogs). Once built, you can reuse this information to match
 * received requests with a particular SIP call or to build a new
 * request within a particular SIP call.
 *
 * An old feature has been implemented in osip: <B>a SDP negotiator.
 * I advise you to not use this old facility which is not powerfull
 * often uncompliant and not flexible enough.</B> It's also quite
 * unreadable and poorly written (by me!). I'll delete it anyway in
 * future version of osip. I have warned you...
 *
 * However, I'm building a <B>new SDP negotiator (osip_rfc3264.h).</B>
 * While not being fully ready, I hope to get it completed by the
 * end of year 2004. It's promising work and I hope it will be 
 * enough flexible to fit any applications. Time will tell.
 *
 * oSIP finaly contains a <B>MD5 implementation (osip_md5.h)</B> which
 * you'll find usefull when implementing SIP Digest authentication.
 * 
 */
#endif
