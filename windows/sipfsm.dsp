# Microsoft Developer Studio Project File - Name="sipfsm" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=sipfsm - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "sipfsm.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "sipfsm.mak" CFG="sipfsm - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "sipfsm - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "sipfsm - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "sipfsm - Win32 Release"

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

!ELSEIF  "$(CFG)" == "sipfsm - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".." /I "../../../include/" /D "_DEBUG" /D "ENABLE_DEBUG" /D "ENABLE_TRACE" /D "WIN32" /D "_MBCS" /D "_LIB" /D "OSIP_MT" /YX /FD /GZ /c
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

# Name "sipfsm - Win32 Release"
# Name "sipfsm - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\fsm\evt_server.c
# End Source File
# Begin Source File

SOURCE=..\fsm\fsm_misc.c
# End Source File
# Begin Source File

SOURCE=..\fsm\fsm_uac4inv.c
# End Source File
# Begin Source File

SOURCE=..\fsm\fsm_uac4req.c
# End Source File
# Begin Source File

SOURCE=..\fsm\fsm_uas4inv.c
# End Source File
# Begin Source File

SOURCE=..\fsm\fsm_uas4req.c
# End Source File
# Begin Source File

SOURCE=..\fsm\osip.c
# End Source File
# Begin Source File

SOURCE=..\fsm\port_fifo.c
# End Source File
# Begin Source File

SOURCE=..\fsm\port_sema.c
# End Source File
# Begin Source File

SOURCE=..\fsm\port_thread.c
# End Source File
# Begin Source File

SOURCE=..\fsm\trn_manager.c
# End Source File
# Begin Source File

SOURCE=..\fsm\sdp_negoc.c
# End Source File
# Begin Source File

SOURCE=..\fsm\trn_timers.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\fsm\fsm.h
# End Source File
# Begin Source File

SOURCE=..\osip\sdp_negoc.h
# End Source File
# Begin Source File

SOURCE=..\osip\osip.h
# End Source File
# Begin Source File

SOURCE=..\osip\sdp.h
# End Source File
# End Group
# End Target
# End Project
