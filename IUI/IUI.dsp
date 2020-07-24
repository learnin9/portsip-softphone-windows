# Microsoft Developer Studio Project File - Name="IUI" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=IUI - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "IUI.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "IUI.mak" CFG="IUI - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "IUI - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "IUI - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "IUI - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "IUI - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GR /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Lib\IUI_60dMTd.lib"

!ENDIF 

# Begin Target

# Name "IUI - Win32 Release"
# Name "IUI - Win32 Debug"
# Begin Group "Control"

# PROP Default_Filter ""
# Begin Group "ControlCore"

# PROP Default_Filter ""
# Begin Group "Button"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Control\ControlCore\Button\ButtonCore.cpp
# End Source File
# End Group
# Begin Group "ColumnHeader"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Control\ControlCore\ColumnHeader\header.cpp
# End Source File
# End Group
# Begin Group "ComboBox"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Control\ControlCore\ComboBox\Combo.h
# End Source File
# Begin Source File

SOURCE=.\Control\ControlCore\ComboBox\ComboBoxCore.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\ControlCore\ComboBox\combodir.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\ControlCore\ComboBox\comboini.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\ControlCore\ComboBox\lb1.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\ControlCore\ComboBox\lboxctl1.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\ControlCore\ComboBox\lboxctl2.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\ControlCore\ComboBox\lboxrare.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\ControlCore\ComboBox\lboxvar.cpp
# End Source File
# End Group
# Begin Group "Label"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Control\ControlCore\Label\LabelCore.cpp
# End Source File
# End Group
# Begin Group "ListView"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Control\ControlCore\ListView\ListViewCore.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\ControlCore\ListView\ListViewInc.h
# End Source File
# Begin Source File

SOURCE=.\Control\ControlCore\ListView\lvicon.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\ControlCore\ListView\lvlist.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\ControlCore\ListView\lvrept.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\ControlCore\ListView\lvsmall.cpp
# End Source File
# End Group
# Begin Group "Menu"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Control\ControlCore\Menu\menu.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\ControlCore\Menu\Menu.h
# End Source File
# Begin Source File

SOURCE=.\Control\ControlCore\Menu\menuc.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\ControlCore\Menu\mnchange.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\ControlCore\Menu\mncreate.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\ControlCore\Menu\mndraw.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\ControlCore\Menu\mndstry.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\ControlCore\Menu\mnkey.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\ControlCore\Menu\mnloop.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\ControlCore\Menu\mnpopup.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\ControlCore\Menu\mnstate.cpp
# End Source File
# End Group
# Begin Group "ScrollBar"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Control\ControlCore\ScrollBar\ScrollBarCore.cpp
# End Source File
# End Group
# Begin Group "TreeView"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Control\ControlCore\TreeView\treeview.h
# End Source File
# Begin Source File

SOURCE=.\Control\ControlCore\TreeView\TreeViewCore.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\ControlCore\TreeView\tvmem.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\ControlCore\TreeView\tvpaint.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\ControlCore\TreeView\tvscroll.cpp
# End Source File
# End Group
# Begin Group "Windowless"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Control\ControlCore\Windowless\WindowlessCore.cpp
# End Source File
# End Group
# Begin Group "HtmlView"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Control\ControlCore\HtmlView\HtmlViewCore.cpp
# End Source File
# End Group
# Begin Group "ProgressBar"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Control\ControlCore\ProgressBar\ProgressBarCore.cpp
# End Source File
# End Group
# Begin Group "RichTextBox"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Control\ControlCore\RichTextBox\ImageOleCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\ControlCore\RichTextBox\IMRichTextBoxCore.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\ControlCore\RichTextBox\OleControlCore.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\ControlCore\RichTextBox\ProtectedDragSource.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\ControlCore\RichTextBox\RichTextBoxCore.cpp
# End Source File
# End Group
# Begin Group "Slider"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Control\ControlCore\Slider\trackbar.cpp
# End Source File
# End Group
# Begin Group "SplitterBar"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Control\ControlCore\SplitterBar\SplitterBarCore.cpp
# End Source File
# End Group
# Begin Group "Tooltips"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Control\ControlCore\Tooltips\TooltipsCore.cpp
# End Source File
# End Group
# Begin Group "Window"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Control\ControlCore\Window\WindowHelper.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\Control\ControlCore\ControlMember.h
# End Source File
# Begin Source File

SOURCE=.\Control\ControlCore\Win32Global.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\ControlCore\Win32Global.h
# End Source File
# End Group
# Begin Group "Layout"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Control\Layout\Canvas.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\Layout\DockPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\Layout\StackPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\Layout\TabControl.cpp
# End Source File
# End Group
# Begin Group "Window No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Control\Window\Tooltips.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\Window\UIColorDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\Window\UIFileDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\Window\UIFontDialog.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\Control\Button.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\ColumnHeader.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\ComboBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\HostWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\HtmlView.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\IMRichTextBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\Label.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\ListView.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\ProgressBar.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\RepeatButton.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\RichTextBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\ScrollBar.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\Slider.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\SplitterBar.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\TreeView.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\Window.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\WLWnd.cpp
# End Source File
# End Group
# Begin Group "Core"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Core\Animation.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\ControlProp.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\CreateControls.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\crit.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\DropTargetEx.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\DropTargetEx.h
# End Source File
# Begin Source File

SOURCE=.\Core\Global.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\Global.h
# End Source File
# Begin Source File

SOURCE=.\Core\IUIFont.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\IUIImage.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\IUIProject.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\IUIStringA.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\IUIStringW.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\JScript.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\MemStdioFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\ParseLanguageFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\ParseResourceFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\Render.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\ResourceMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\ResourceMgr.h
# End Source File
# Begin Source File

SOURCE=.\Core\State.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\tls.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\UIParse.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\UIVariant.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\Utils.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\WLWndMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\WLWndMgr.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\IUI.h
# End Source File
# Begin Source File

SOURCE=.\Readme.txt
# End Source File
# Begin Source File

SOURCE=.\stdafx.cpp

!IF  "$(CFG)" == "IUI - Win32 Release"

!ELSEIF  "$(CFG)" == "IUI - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Target
# End Project
