# Microsoft Developer Studio Project File - Name="osipparser" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=osipparser - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "osipparser.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "osipparser.mak" CFG="osipparser - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "osipparser - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "osipparser - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "osipparser - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".libs"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "OSIPPARSER_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W4 /GX /O2 /I "..\\" /D "SYSTEM_LOGGER_ENABLED" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "OSIPPARSER_EXPORTS" /D "ENABLE_TRACE" /D "OSIP_MT" /D "USE_TMP_BUFFER" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 msvcrt.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /nodefaultlib

!ELSEIF  "$(CFG)" == "osipparser - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".libs"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "OSIPPARSER_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GX /ZI /Od /I "..\\" /D "SYSTEM_LOGGER_ENABLED" /D "_DEBUG" /D "ENABLE_TRACE" /D "ENABLE_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "OSIPPARSER_EXPORTS" /D "OSIP_MT" /D "USE_TMP_BUFFER" /FR /YX /FD /GZ /c
# SUBTRACT CPP /WX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 msvcrtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /nodefaultlib /pdbtype:sept

!ENDIF 

# Begin Target

# Name "osipparser - Win32 Release"
# Name "osipparser - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\parser\hdr_accept.c
# End Source File
# Begin Source File

SOURCE=..\parser\hdr_acceptencoding.c
# End Source File
# Begin Source File

SOURCE=..\parser\hdr_acceptlanguage.c
# End Source File
# Begin Source File

SOURCE=..\parser\hdr_alertinfo.c
# End Source File
# Begin Source File

SOURCE=..\parser\hdr_allow.c
# End Source File
# Begin Source File

SOURCE=..\parser\hdr_authorization.c
# End Source File
# Begin Source File

SOURCE=..\parser\hdr_body.c
# End Source File
# Begin Source File

SOURCE=..\parser\hdr_callid.c
# End Source File
# Begin Source File

SOURCE=..\parser\hdr_callinfo.c
# End Source File
# Begin Source File

SOURCE=..\parser\hdr_contact.c
# End Source File
# Begin Source File

SOURCE=..\parser\hdr_contentdisposition.c
# End Source File
# Begin Source File

SOURCE=..\parser\hdr_contentencoding.c
# End Source File
# Begin Source File

SOURCE=..\parser\hdr_contentlength.c
# End Source File
# Begin Source File

SOURCE=..\parser\hdr_contenttype.c
# End Source File
# Begin Source File

SOURCE=..\parser\hdr_cseq.c
# End Source File
# Begin Source File

SOURCE=..\parser\hdr_errorinfo.c
# End Source File
# Begin Source File

SOURCE=..\parser\hdr_from.c
# End Source File
# Begin Source File

SOURCE=..\parser\hdr_headers.c
# End Source File
# Begin Source File

SOURCE=..\parser\hdr_mimeversion.c
# End Source File
# Begin Source File

SOURCE=..\parser\hdr_proxyauthenticate.c
# End Source File
# Begin Source File

SOURCE=..\parser\hdr_proxyauthorization.c
# End Source File
# Begin Source File

SOURCE=..\parser\hdr_recordroute.c
# End Source File
# Begin Source File

SOURCE=..\parser\hdr_route.c
# End Source File
# Begin Source File

SOURCE=..\parser\hdr_to.c
# End Source File
# Begin Source File

SOURCE=..\parser\hdr_via.c
# End Source File
# Begin Source File

SOURCE=..\parser\hdr_wwwauthenticate.c
# End Source File
# Begin Source File

SOURCE=..\parser\md5c.c
# End Source File
# Begin Source File

SOURCE=..\parser\msg_cparser.c
# End Source File
# Begin Source File

SOURCE=..\parser\msg_parser.c
# End Source File
# Begin Source File

SOURCE=..\parser\msg_read.c
# End Source File
# Begin Source File

SOURCE=..\parser\msg_write.c
# End Source File
# Begin Source File

SOURCE=.\osipparser.def
# End Source File
# Begin Source File

SOURCE=..\parser\port_list.c
# End Source File
# Begin Source File

SOURCE=..\parser\port_malloc.c
# End Source File
# Begin Source File

SOURCE=..\parser\port_misc.c
# End Source File
# Begin Source File

SOURCE=..\parser\sdp_accessor.c
# End Source File
# Begin Source File

SOURCE=..\parser\sdp_rfc2327.c
# End Source File
# Begin Source File

SOURCE=..\parser\urls.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\osip\const.h
# End Source File
# Begin Source File

SOURCE=..\osip\dialog.h
# End Source File
# Begin Source File

SOURCE=..\osip\fifo.h
# End Source File
# Begin Source File

SOURCE=..\osip\global.h
# End Source File
# Begin Source File

SOURCE=..\osip\list.h
# End Source File
# Begin Source File

SOURCE=..\osip\md5.h
# End Source File
# Begin Source File

SOURCE=..\parser\msg.h
# End Source File
# Begin Source File

SOURCE=..\osip\osip.h
# End Source File
# Begin Source File

SOURCE=..\osip\port.h
# End Source File
# Begin Source File

SOURCE=..\osip\sdp.h
# End Source File
# Begin Source File

SOURCE=..\osip\sdp_negoc.h
# End Source File
# Begin Source File

SOURCE=..\osip\sema.h
# End Source File
# Begin Source File

SOURCE=..\osip\smsg.h
# End Source File
# Begin Source File

SOURCE=..\osip\smsgtypes.h
# End Source File
# Begin Source File

SOURCE=..\osip\thread.h
# End Source File
# Begin Source File

SOURCE=..\osip\urls.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
