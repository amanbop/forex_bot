cmake_minimum_required (VERSION 2.6) 
 
set (PROJECT TwoConnection) 
  
project (${PROJECT}) 

if(DEFINED Mac32)
  set(ARCHFLAGS "-arch i386 -DMac32")
elseif(${CMAKE_SYSTEM} MATCHES "Linux" AND ${CMAKE_SYSTEM_PROCESSOR} MATCHES "i[3-6]86")
  set(ARCHFLAGS "-march=i486")
else()
  set(ARCHFLAGS "")
endif()

set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} ${ARCHFLAGS}")
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} ${ARCHFLAGS}")

set (CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} ${ARCHFLAGS}")
set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} ${ARCHFLAGS}")

set (CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS} ${ARCHFLAGS}")
set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS} ${ARCHFLAGS}")

IF(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/source
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../../threadlib/include
                    )   

link_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../../lib
				  ${CMAKE_CURRENT_SOURCE_DIR}/../../../threadlib/lib
				 )

set (HEADERS 
    source/stdafx.h
	source/ResponseQueue.h
	source/SimpleLog.h
	source/SessionStatusListener.h
    source/EventListener.h
    source/ResponseListener.h
    source/PermissionCheckerSample.h
    source/CreateOrderSample.h
    source/TradingSettingsProviderSample.h
    )
     
set (SOURCES
    source/stdafx.cpp
    source/TwoConnection.cpp
    source/CreateOrderSample.cpp
	source/PermissionCheckerSample.cpp
    source/SimpleLog.cpp
	source/ResponseListener.cpp
	source/ResponseQueue.cpp
	source/EventListener.cpp
	source/SessionStatusListener.cpp
	source/TradingSettingsProviderSample.cpp
    ) 

if (NOT CMAKE_CROSSCOMPILING)
	add_executable (${PROJECT} ${HEADERS} ${SOURCES})
else()
	add_library (${PROJECT} STATIC ${HEADERS} ${SOURCES})
endif()

if (${CMAKE_SYSTEM} MATCHES "Linux")
	target_link_libraries (${PROJECT} order2go2 threadlib )
elseif (${CMAKE_SYSTEM} MATCHES "Darwin")
	target_link_libraries (${PROJECT} liborder2go2.a libthreadlib.a)
endif()