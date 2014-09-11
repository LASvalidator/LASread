# Microsoft Developer Studio Project File - Name="LASread" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=LASread - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LASread.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LASread.mak" CFG="LASread - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LASread - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "LASread - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LASread - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /I "inc" /I "..\lasread\inc" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "LASZIP_ENABLED" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy Release\LASread.lib lib\LASread.lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "LASread - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "inc" /I "..\lasread\inc" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "LASZIP_ENABLED" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy Debug\LASread.lib lib\LASreadD.lib
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "LASread - Win32 Release"
# Name "LASread - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\arithmeticdecoder.cpp
# End Source File
# Begin Source File

SOURCE=.\src\arithmeticmodel.cpp
# End Source File
# Begin Source File

SOURCE=.\src\integerdecompressor.cpp
# End Source File
# Begin Source File

SOURCE=.\src\lasheader.cpp
# End Source File
# Begin Source File

SOURCE=.\src\laspoint.cpp
# End Source File
# Begin Source File

SOURCE=.\src\lasreader.cpp
# End Source File
# Begin Source File

SOURCE=.\src\lasreaditemcompressed_v1.cpp
# End Source File
# Begin Source File

SOURCE=.\src\lasreaditemcompressed_v2.cpp
# End Source File
# Begin Source File

SOURCE=.\src\lasreadopener.cpp
# End Source File
# Begin Source File

SOURCE=.\src\lasreadpoint.cpp
# End Source File
# Begin Source File

SOURCE=.\src\lasutility.cpp
# End Source File
# Begin Source File

SOURCE=.\src\laswaveformreader.cpp
# End Source File
# Begin Source File

SOURCE=.\src\laszip.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ".h"
# Begin Source File

SOURCE=.\src\arithmeticdecoder.hpp
# End Source File
# Begin Source File

SOURCE=.\src\arithmeticmodel.hpp
# End Source File
# Begin Source File

SOURCE=.\src\bytestreamin.hpp
# End Source File
# Begin Source File

SOURCE=.\src\bytestreamin_file.hpp
# End Source File
# Begin Source File

SOURCE=.\src\bytestreamin_istream.hpp
# End Source File
# Begin Source File

SOURCE=.\src\integerdecompressor.hpp
# End Source File
# Begin Source File

SOURCE=.\inc\lasdefinitions.hpp
# End Source File
# Begin Source File

SOURCE=.\inc\lasheader.hpp
# End Source File
# Begin Source File

SOURCE=.\inc\laspoint.hpp
# End Source File
# Begin Source File

SOURCE=.\inc\lasreader.hpp
# End Source File
# Begin Source File

SOURCE=.\src\lasreaditem.hpp
# End Source File
# Begin Source File

SOURCE=.\src\lasreaditemcompressed_v1.hpp
# End Source File
# Begin Source File

SOURCE=.\src\lasreaditemcompressed_v2.hpp
# End Source File
# Begin Source File

SOURCE=.\src\lasreaditemraw.hpp
# End Source File
# Begin Source File

SOURCE=.\inc\lasreadopener.hpp
# End Source File
# Begin Source File

SOURCE=.\src\lasreadpoint.hpp
# End Source File
# Begin Source File

SOURCE=.\inc\lasutility.hpp
# End Source File
# Begin Source File

SOURCE=.\inc\laswaveformreader.hpp
# End Source File
# Begin Source File

SOURCE=.\inc\laszip.hpp
# End Source File
# Begin Source File

SOURCE=.\src\laszip_common_v1.hpp
# End Source File
# Begin Source File

SOURCE=.\src\laszip_common_v2.hpp
# End Source File
# Begin Source File

SOURCE=.\inc\mydefs.hpp
# End Source File
# End Group
# End Target
# End Project
