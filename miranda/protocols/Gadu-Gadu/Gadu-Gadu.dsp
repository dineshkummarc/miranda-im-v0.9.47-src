# Microsoft Developer Studio Project File - Name="GG" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=GG - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Gadu-Gadu.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Gadu-Gadu.mak" CFG="GG - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GG - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "GG - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "GG - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "GG_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O1 /I "../../include" /I "libgadu" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "GG_EXPORTS" /FAcs /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "../../include" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib version.lib /nologo /base:"0x32500000" /dll /map /debug /machine:I386 /out:"../../bin/release/plugins/GG.dll" /ALIGN:4096 /ignore:4108
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "GG - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "GG_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../include" /I "libgadu" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "GG_EXPORTS" /FAcs /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "../../include" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib version.lib /nologo /base:"0x32500000" /dll /map /debug /machine:I386 /out:"../../bin/debug/plugins/GG.dll" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "GG - Win32 Release"
# Name "GG - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "libgadu"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\libgadu\common.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=.\libgadu\compat.h
# End Source File
# Begin Source File

SOURCE=.\libgadu\dcc.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=.\libgadu\dcc7.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=.\libgadu\events.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=.\libgadu\http.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=.\libgadu\internal.h
# End Source File
# Begin Source File

SOURCE=.\libgadu\libgadu.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=.\libgadu\libgadu.h
# End Source File
# Begin Source File

SOURCE=.\libgadu\protocol.h
# End Source File
# Begin Source File

SOURCE=.\libgadu\pthread.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=.\libgadu\pthread.h
# End Source File
# Begin Source File

SOURCE=.\libgadu\pubdir.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=.\libgadu\pubdir50.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=.\libgadu\resolver.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=.\libgadu\resolver.h
# End Source File
# Begin Source File

SOURCE=.\libgadu\sha1.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=.\libgadu\win32.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=.\libgadu\win32.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\avatar.c
# End Source File
# Begin Source File

SOURCE=.\core.c
# End Source File
# Begin Source File

SOURCE=.\dialogs.c
# End Source File
# Begin Source File

SOURCE=.\dynstuff.c
# End Source File
# Begin Source File

SOURCE=.\filetransfer.c
# End Source File
# Begin Source File

SOURCE=.\gg.c
# ADD CPP /Yc"gg.h"
# End Source File
# Begin Source File

SOURCE=.\groupchat.c
# End Source File
# Begin Source File

SOURCE=.\icolib.c
# End Source File
# Begin Source File

SOURCE=.\image.c
# End Source File
# Begin Source File

SOURCE=.\import.c
# End Source File
# Begin Source File

SOURCE=.\keepalive.c
# End Source File
# Begin Source File

SOURCE=.\links.c
# End Source File
# Begin Source File

SOURCE=.\oauth.c
# End Source File
# Begin Source File

SOURCE=.\ownerinfo.c
# End Source File
# Begin Source File

SOURCE=.\services.c
# End Source File
# Begin Source File

SOURCE=.\sessions.c
# End Source File
# Begin Source File

SOURCE=.\token.c
# End Source File
# Begin Source File

SOURCE=.\userutils.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\dynstuff.h
# End Source File
# Begin Source File

SOURCE=.\gg.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\version.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\icons\block.ico
# End Source File
# Begin Source File

SOURCE=.\icons\clear_ignored_conference.ico
# End Source File
# Begin Source File

SOURCE=.\icons\conference.ico
# End Source File
# Begin Source File

SOURCE=.\icons\delete.ico
# End Source File
# Begin Source File

SOURCE=.\icons\export_list_to_server.ico
# End Source File
# Begin Source File

SOURCE=.\icons\export_list_to_txt_file.ico
# End Source File
# Begin Source File

SOURCE=.\icons\gg.ico
# End Source File
# Begin Source File

SOURCE=.\icons\image.ico
# End Source File
# Begin Source File

SOURCE=.\icons\import_list_from_server.ico
# End Source File
# Begin Source File

SOURCE=.\icons\import_list_from_txt_file.ico
# End Source File
# Begin Source File

SOURCE=.\icons\list.ico
# End Source File
# Begin Source File

SOURCE=.\icons\next.ico
# End Source File
# Begin Source File

SOURCE=.\icons\previous.ico
# End Source File
# Begin Source File

SOURCE=.\icons\remove_list_from_server.ico
# End Source File
# Begin Source File

SOURCE=.\resource.rc
# End Source File
# Begin Source File

SOURCE=.\icons\save.ico
# End Source File
# Begin Source File

SOURCE=.\icons\sessions.ico
# End Source File
# Begin Source File

SOURCE=.\icons\settings.ico
# End Source File
# End Group
# Begin Source File

SOURCE=".\docs\build-howto.txt"
# End Source File
# Begin Source File

SOURCE=".\docs\gg-license.txt"
# End Source File
# Begin Source File

SOURCE=".\docs\gg-readme.txt"
# End Source File
# Begin Source File

SOURCE=".\docs\gg-translation-sample.txt"
# End Source File
# End Target
# End Project
