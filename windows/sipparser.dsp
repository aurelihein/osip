# Microsoft Developer Studio Project File - Name="sipparser" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=sipparser - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "sipparser.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "sipparser.mak" CFG="sipparser - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "sipparser - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "sipparser - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "sipparser - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "OSIP_MT" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "sipparser - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".." /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "ENABLE_DEBUG" /D "ENABLE_TRACE" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "sipparser - Win32 Release"
# Name "sipparser - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\parser\hdr_body.c
# End Source File
# Begin Source File

SOURCE=..\parser\hdr_callid.c
# End Source File
# Begin Source File

SOURCE=..\parser\hdr_contact.c
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

SOURCE=..\parser\hdr_from.c
# End Source File
# Begin Source File

SOURCE=..\parser\hdr_headers.c
# End Source File
# Begin Source File

SOURCE=..\parser\hdr_mimeversion.c
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

SOURCE=..\parser\hdr_callinfo.c
# End Source File
# Begin Source File

SOURCE=..\parser\hdr_contentdisposition.c
# End Source File
# Begin Source File

SOURCE=..\parser\hdr_contentencoding.c
# End Source File
# Begin Source File

SOURCE=..\parser\hdr_errorinfo.c
# End Source File
# Begin Source File

SOURCE=..\parser\hdr_proxyauthenticate.c
# End Source File
# Begin Source File

SOURCE=..\parser\hdr_proxyauthorization.c
# End Source File
# Begin Source File

SOURCE=..\parser\hdr_wwwauthenticate.c
# End Source File
# Begin Source File

SOURCE=..\parser\hdr_authorization.c
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

SOURCE=..\parser\port_list.c
# End Source File
# Begin Source File

SOURCE=..\parser\port_malloc.c
# End Source File
# Begin Source File

SOURCE=..\parser\port_misc.c
# End Source File
# Begin Source File

SOURCE=..\parser\sdp_rfc2327.c
# End Source File
# Begin Source File

SOURCE=..\parser\sdp_accessor.c
# End Source File
# Begin Source File

SOURCE=..\parser\urls.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\parser\msg.h
# End Source File
# End Group
# End Target
# End Project
