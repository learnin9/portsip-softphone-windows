# 版本限定
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)                      

# 项目名称
PROJECT(PortGo)

# 设置编译器选项：O3优化，显示所有警告
SET(CMAKE_C_FLAGS "$ENV{CFLAGS} -O3 -Wall") 
SET(CMAKE_EXE_LINKER_FLAGS     "${CMAKE_EXE_LINKER_FLAGS} /level='requireAdministrator' /uiAccess='false'")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
SET(CMAKE_MODULE_PATH 
	${CMAKE_MODULE_PATH}
)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/../cmake)
set(STDAFX_PCH_H ${CMAKE_SOURCE_DIR}/stdafx.h)
set(STDAFX_PCH_C ${CMAKE_SOURCE_DIR}/stdafx.cpp)

MESSAGE(STATUS "project source Path , ${CMAKE_MODULE_PATH}") 
MESSAGE(STATUS "STDAFX_PCH_H , ${STDAFX_PCH_H}") 
MESSAGE(STATUS "STDAFX_PCH_C , ${STDAFX_PCH_C}") 
INCLUDE(msvcpch)

#./*.h;./*.cpp;./*.ico;./*.rc
# 文件分组（过滤），同时必须加入到ADD_LIBRARY中才能显示
FILE(GLOB ROOT		 									./*.h;./*.cpp;./*.rc;./*.ico)
FILE(GLOB AVPLAY_RES     							    	AVPlay/*.cpp;AVPlay/*.h)
FILE(GLOB DB_RES      									    db/*.h;db/*.cpp)
FILE(GLOB LOG_RES       			   						./log/*.cpp;./log/*.h)
FILE(GLOB LOGIN_RES       									./Login/*.cpp;./Login/*.h)
FILE(GLOB MAIN_RES       									./Main/*.cpp;./Main/*.h)
FILE(GLOB MAIN_TAB_CTRL_CALLPANEL_RES       				./Main/MainFrameTabControl/CallPanel/*.cpp;./Main/MainFrameTabControl/CallPanel/*.h)
FILE(GLOB MAIN_TAB_CTRL_MAINPANEL_RES        				
./Main/MainFrameTabControl/MainPanel/*.cpp;/Main/MainFrameTabControl/MainPanel/*.h;
./Main/MainFrameTabControl/MainPanel/MainTabControl/ChatPanel/*.cpp;./Main/MainFrameTabControl/MainPanel/MainTabControl/ChatPanel/*.h;
./Main/MainFrameTabControl/MainPanel/MainTabControl/ChatPanel/CallingPageDialog/CallingPageTransfer/*.cpp;
./Main/MainFrameTabControl/MainPanel/MainTabControl/ChatPanel/CallingPageDialog/CallingPageTransfer/*.h;
./Main/MainFrameTabControl/MainPanel/MainTabControl/ContactsPanel/*.cpp;
./Main/MainFrameTabControl/MainPanel/MainTabControl/ContactsPanel/*.h;
./Main/MainFrameTabControl/MainPanel/MainTabControl/InboxPanel/*.cpp;
./Main/MainFrameTabControl/MainPanel/MainTabControl/InboxPanel/*.h
)
FILE(GLOB MAIN_TAB_CTRL_SEARCHPANEL_RES        		
./Main/MainFrameTabControl/SearchPanel/*.cpp;
./Main/MainFrameTabControl/SearchPanel/*.h;
)

FILE(GLOB SETTING_RES       				Settings/*.cpp;Settings/*.h)
FILE(GLOB SYSTEM_RES       					System/*.cpp;System/*.h;System/*.cxx;System/*.hxx)
FILE(GLOB UTILITY_RES       				Utility/*.*;Utility/*.*)
#筛选器
SOURCE_GROUP("other"							FILES 	${ROOT})
SOURCE_GROUP("AVPlay" 							FILES 	${AVPLAY_RES})
SOURCE_GROUP("db" 			FILES 	${DB_RES})
SOURCE_GROUP("Log" 	FILES 	${LOG_RES})
SOURCE_GROUP("Login" 	FILES 	${LOGIN_RES})
SOURCE_GROUP("Main" 	FILES 	${MAIN_RES})
SOURCE_GROUP("Main\\CallPanel" 	FILES 	${MAIN_TAB_CTRL_CALLPANEL_RES})
SOURCE_GROUP("Main\\MainPannel" 	FILES 	${MAIN_TAB_CTRL_MAINPANEL_RES})
SOURCE_GROUP("Main\\SearchPannel" 	FILES 	${MAIN_TAB_CTRL_SEARCHPANEL_RES})
SOURCE_GROUP("Main\\Setting" 	FILES 	${SETTING_RES})
SOURCE_GROUP("System" 	FILES 	${SYSTEM_RES})
SOURCE_GROUP("utility" 	FILES 	${UTILITY_RES})

# 增加头文件搜索路径
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../IUI)
#INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include/IUI)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../jsoncpp/include)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include/ICUC4)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include/libvlc_include)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include/breakpad)
#INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include/curl)
#INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include/log4cplus)
#INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include/PortSIPLib)

MESSAGE(STATUS " INCLUDE_DIRECTORIES Path, " ${CMAKE_CURRENT_SOURCE_DIR}) 
#这个好像没有使用
#INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include/SDL)

SET(PORTGO_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/;${CMAKE_CURRENT_SOURCE_DIR}/lib/;${CMAKE_CURRENT_SOURCE_DIR}/lib/x86;${CMAKE_CURRENT_SOURCE_DIR}/lib/libvlc_lib;)
##lib搜索路径
LINK_DIRECTORIES(
	${PORTGO_LIB_DIR}
)
MESSAGE(STATUS " PORTGO_LIB_DIR Path, " ${PORTGO_LIB_DIR}) 

#预处理宏定义
ADD_DEFINITIONS(-D_UNICODE -DUNICODE)
ADD_DEFINITIONS(-DBUILDING_LIBCURL)
ADD_DEFINITIONS(-DHTTP_ONLY)
ADD_DEFINITIONS(-D_WINDOWS)
ADD_DEFINITIONS(-DWIN32)
# 输出设置
SET(CMAKE_CXX_FLAGS_DEBUG "/D_DEBUG /MTd /Zi /Ob0 /Od /RTC1")# 直接抄gui的默认，然后 改成mtd
SET(CMAKE_CXX_FLAGS_RELEASE "/MT /Zi /O2 /Ob2 /D NDEBUG")
SET(CMAKE_CXX_FLAGS_MINSIZEREL "/MT /O1 /Ob1 /D NDEBUG")
SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/MT /O2 /Ob2 /D NDEBUG")

# 直/Zi /O2 /Ob1 /D NDEBUG

# 设置程序输出路径
#SET(EXECUTABLE_OUTPUT_PATH  ${PROJECT_BINARY_DIR}/bin) 
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)


# 生成静态库
ADD_EXECUTABLE(PortGo  WIN32  
			   ${ROOT};
			   ${AVPLAY_RES};
			   ${CONTROL_RES};
			   ${DB_RES};
			   ${LOG_RES};
			   ${LOGIN_RES};
			   ${MAIN_RES};
			   ${MAIN_TAB_CTRL_CALLPANEL_RES};
			   ${MAIN_TAB_CTRL_MAINPANEL_RES};
			   ${MAIN_TAB_CTRL_SEARCHPANEL_RES};
			   ${SETTING_RES};
			   ${SYSTEM_RES};
			   ${UTILITY_RES};
			   )
SET_TARGET_PROPERTIES(PortGo PROPERTIES OUTPUT_NAME "PortGoClient")
SET_TARGET_PROPERTIES(PortGo PROPERTIES DEBUG_POSTFIX "D")
SET_TARGET_PROPERTIES(PortGo PROPERTIES RELEASE_POSTFIX "")
SET_TARGET_PROPERTIES(PortGo PROPERTIES RELEASEWITHI_POSTFIX "")
#链接库
TARGET_INCLUDE_DIRECTORIES(PortGo PUBLIC ${PORTGO_LIB_DIR})
SET(PORTGO_LIBS libvlc.lib;libvlccore.LIB;ws2_32.lib;winmm.lib;sqlite3.lib;wldap32.lib;ExportClass.lib;libdisasmd.lib;)
TARGET_LINK_LIBRARIES(PortGo ${PORTGO_LIBS})

# 增加项目依赖项
#add_dependencies(PortGo  libjsoncpp)
#add_dependencies(PortGo  IUI_vs2017)


# 增加预编译头
use_precompiled_header(PortGo
   "${CMAKE_CURRENT_SOURCE_DIR}/stdafx.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/stdafx.cpp")
MESSAGE(STATUS "CMAKE_CURRENT_SOURCE_DIR Path, " ${CMAKE_CURRENT_SOURCE_DIR}) 

add_custom_command(TARGET PortGo 
					POST_BUILD 
					COMMAND call ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/PostBuild.cmd $(TargetDir)
					COMMENT "This command will be executed after build target PortGo"
					)
MESSAGE(STATUS "CMD COMMAND Path, " ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/PostBuild.cmd)