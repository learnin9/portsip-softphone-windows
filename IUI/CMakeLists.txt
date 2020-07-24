# 版本限定
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)                      

# 项目名称
PROJECT(IUI)

# 设置编译器选项：O3优化，显示所有警告
SET(CMAKE_C_FLAGS "$ENV{CFLAGS} -O3 -Wall") 

SET(CMAKE_MODULE_PATH 
	${CMAKE_MODULE_PATH}
)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/../cmake)
MESSAGE(STATUS "project source Path , ${CMAKE_MODULE_PATH}") 
INCLUDE(msvcpch)

# 文件分组（过滤），同时必须加入到ADD_LIBRARY中才能显示
FILE(GLOB ROOT		 									./*.h;./*.rc;./*.cpp)
FILE(GLOB CONTROL_RES     							    Control/*.cpp)
#CONTROL CORE
FILE(GLOB CONTROL_CORE_RES      						Control/ControlCore/*.h;Control/ControlCore/*.cpp)
FILE(GLOB CONTROL_CORE_BUTTON_RES       			    Control/ControlCore/Button/*.cpp)
FILE(GLOB CONTROL_CORE_COLUMNHEADER_RES       			Control/ControlCore/ColumnHeader/*.cpp)
FILE(GLOB CONTROL_CORE_COMBOX_RES       				Control/ControlCore/ComboxBox/*.cpp)
FILE(GLOB CONTROL_CORE_DATETIMEPICKER_RES       		Control/ControlCore/DateTimePicker/*.cpp)
FILE(GLOB CONTROL_CORE_HTMLVIEW_RES       				Control/ControlCore/HtmlView/*.cpp)
FILE(GLOB CONTROL_CORE_LABEL_RES       				    Control/ControlCore/Label/*.cpp)
FILE(GLOB CONTROL_CORE_LISTVIEW_RES       				Control/ControlCore/ListView/*.cpp)
FILE(GLOB CONTROL_CORE_MENU_RES       					Control/ControlCore/Menu/*.h;Control/ControlCore/Menu/*.cpp)
FILE(GLOB CONTROL_CORE_PROGRESS_RES       				Control/ControlCore/ProgressBar/*.cpp)
FILE(GLOB CONTROL_CORE_RICHTEXTBOX_RES       			Control/ControlCore/RichTextBox/*.cpp)
FILE(GLOB CONTROL_CORE_RICHTEXTBOX_SRC       			Control/ControlCore/RichTextBox/RichEditSrc/*.*)
FILE(GLOB CONTROL_CORE_RICHTEXTBOX_INC       			Control/ControlCore/RichTextBox/RichEditInc/*.h;Control/ControlCore/RichTextBox/RichEditInc/version/*.h;Control/ControlCore/RichTextBox/RichEditInc/macos/*.h)

FILE(GLOB CONTROL_CORE_SCROLLBAR_RES       				Control/ControlCore/ScrollBar/*.cpp)
FILE(GLOB CONTROL_CORE_SLIDER_RES       				Control/ControlCore/Slider/*.cpp)
FILE(GLOB CONTROL_CORE_SPINBUTTON_RES       				Control/ControlCore/SpinButton/*.cpp)
FILE(GLOB CONTROL_CORE_SPLITTERBAR_RES       				Control/ControlCore/SplitterBar/*.cpp)
FILE(GLOB CONTROL_CORE_TOOLTIPS_RES       				Control/ControlCore/Tooltips/*.cpp)
FILE(GLOB CONTROL_CORE_TREEVIEW_RES       				Control/ControlCore/TreeView/*.cpp)
FILE(GLOB CONTROL_CORE_WINDOW_RES       				Control/ControlCore/Window/*.cpp)
FILE(GLOB CONTROL_CORE_WINDOWLESS_RES       				Control/ControlCore/Windowless/*.cpp)
#CONTROL LAYOUT
FILE(GLOB CONTROL_LAYOUT_RES       					   Control/ControlCore/Layout/*.cpp)
#CONTROL WINDOW
FILE(GLOB CONTROL_WINDOW_RES       						Control/ControlCore/Window/*.cpp)
#CORE
FILE(GLOB CONTROL_CORE_RES       						Core/*.*)
SOURCE_GROUP("other"							FILES 	${ROOT})
#筛选器Control
SOURCE_GROUP("Control" 							FILES 	${CONTROL_RES})
SOURCE_GROUP("Control\\ControlCore" 			FILES 	${CONTROL_CORE_RES})
SOURCE_GROUP("Control\\ControlCore\\Button" 	FILES 	${CONTROL_CORE_BUTTON_RES})
SOURCE_GROUP("Control\\ControlCore\\ColumnHeader" 	FILES 	${CONTROL_CORE_COLUMNHEADER_RES})
SOURCE_GROUP("Control\\ControlCore\\ComboxBox" 	FILES 	${CONTROL_CORE_COMBOX_RES})
SOURCE_GROUP("Control\\ControlCore\\DateTimePicker" 	FILES 	${CONTROL_CORE_DATETIMEPICKER_RES})
SOURCE_GROUP("Control\\ControlCore\\HtmlView" 	FILES 	${CONTROL_CORE_HTMLVIEW_RES})
SOURCE_GROUP("Control\\ControlCore\\Label" 	FILES 	${CONTROL_CORE_LABEL_RES})
SOURCE_GROUP("Control\\ControlCore\\ListView" 	FILES 	${CONTROL_CORE_LISTVIEW_RES})
SOURCE_GROUP("Control\\ControlCore\\Menu" 	FILES 	${CONTROL_CORE_MENU_RES})
SOURCE_GROUP("Control\\ControlCore\\ProgressBar" 							FILES 	${CONTROL_CORE_PROGRESS_RES})
SOURCE_GROUP("Control\\ControlCore\\RichTextBox" 							FILES 	${CONTROL_CORE_RICHTEXTBOX_RES})
SOURCE_GROUP("Control\\ControlCore\\RichTextBox\\RichEditSrc" 							FILES 	${CONTROL_CORE_RICHTEXTBOX_SRC})
SOURCE_GROUP("Control\\ControlCore\\RichTextBox\\RichEditInc" 							FILES 	${CONTROL_CORE_RICHTEXTBOX_INC})

SOURCE_GROUP("Control\\ControlCore\\ScrollBar" 			FILES 	${CONTROL_CORE_SCROLLBAR_RES})
SOURCE_GROUP("Control\\ControlCore\\Slider" 	FILES 	${CONTROL_CORE_SLIDER_RES})
SOURCE_GROUP("Control\\ControlCore\\SpinButton" 			FILES 	${CONTROL_CORE_SPINBUTTON_RES})
SOURCE_GROUP("Control\\ControlCore\\SplitterBar" 	FILES 	${CONTROL_CORE_SPLITTERBAR_RES})
SOURCE_GROUP("Control\\ControlCore\\Tooltips" 			FILES 	${CONTROL_CORE_TOOLTIPS_RES})
SOURCE_GROUP("Control\\ControlCore\\TreeView" 	FILES 	${CONTROL_CORE_TREEVIEW_RES})
SOURCE_GROUP("Control\\ControlCore\\Window" 			FILES 	${CONTROL_CORE_WINDOW_RES})
SOURCE_GROUP("Control\\ControlCore\\Windowless" 	FILES 	${CONTROL_CORE_WINDOWLESS_RES})
SOURCE_GROUP("Control\\Layout" 			FILES 	${CONTROL_CORE_RES})
SOURCE_GROUP("Control\\Window" 	FILES 	${CONTROL_CORE_BUTTON_RES})
#筛选器 Core
SOURCE_GROUP("Core" 							FILES 	${CONTROL_CORE_RES})#必须用\\,不能用/



# 增加头文件搜索路径
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
#INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/Control/ControlCore/RichTextBox/RichEditSrc)
#INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/Control/ControlCore/RichTextBox/RichEditInc)

MESSAGE(STATUS "INCLUDE_DIRECTORIES Output Path, " ${CONTROL_CORE_RICHTEXTBOX_SRC}) 
#工程采用Unicode
ADD_DEFINITIONS(-D_UNICODE -DUNICODE)
# 输出设置
SET(CMAKE_CXX_FLAGS_DEBUG "/D_DEBUG /MTd /Zi /Ob0 /Od /RTC1")# 直接抄gui的默认，然后 改成mtd
SET(CMAKE_CXX_FLAGS_RELEASE "/MT /O2 /Ob2 /D NDEBUG")
SET(CMAKE_CXX_FLAGS_MINSIZEREL "/MT /O1 /Ob1 /D NDEBUG")
SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/MT /Zi /O2 /Ob1 /D NDEBUG")

# 设置动态库输出路径
SET(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib) 
MESSAGE(STATUS "222Library Output Path, " ${CMAKE_CURRENT_SOURCE_DIR}/lib) 
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/lib)

# 生成静态库
ADD_LIBRARY(IUI_vs2017  STATIC  ${ROOT};
			   ${CONTROL_RES};
			   ${CONTROL_CORE_BUTTON_RES};
			   ${CONTROL_CORE_COLUMNHEADER_RES};
			   ${CONTROL_CORE_COMBOX_RES};
			   ${CONTROL_CORE_DATETIMEPICKER_RES};
			   ${CONTROL_CORE_HTMLVIEW_RES};
			   ${CONTROL_CORE_LABEL_RES};
			   ${CONTROL_CORE_LISTVIEW_RES};
			   ${CONTROL_CORE_MENU_RES};
			   ${CONTROL_CORE_PROGRESS_RES};
			   ${CONTROL_CORE_RICHTEXTBOX_RES};
#			   ${CONTROL_CORE_RICHTEXTBOX_INC};
#			   ${CONTROL_CORE_RICHTEXTBOX_SRC};
			   ${CONTROL_CORE_SCROLLBAR_RES};
			   ${CONTROL_CORE_SLIDER_RES};
			   ${CONTROL_CORE_SPINBUTTON_RES};
			   ${CONTROL_CORE_TOOLTIPS_RES};
			   ${CONTROL_CORE_TREEVIEW_RES};
			   ${CONTROL_CORE_WINDOW_RES};
			   ${CONTROL_CORE_WINDOWLESS_RES};
			   ${CONTROL_LAYOUT_RES};
			   ${CONTROL_WINDOW_RES};
			   ${CONTROL_CORE_RES};
			   )
SET_TARGET_PROPERTIES(IUI_vs2017 PROPERTIES OUTPUT_NAME "IUI_vs2017")
# 增加预编译头
use_precompiled_header(IUI_vs2017
   "${CMAKE_CURRENT_SOURCE_DIR}/stdafx.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/stdafx.cpp")


