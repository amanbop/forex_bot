cmake_minimum_required (VERSION 2.6) 
 
set (PROJECT CreateOTO) 
  
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

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/include
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../../threadlib/include
                    )   

link_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../../lib
				  ${CMAKE_CURRENT_SOURCE_DIR}/../../../threadlib/lib
				 )

set (HEADERS 
    include/Example.h
    include/ExampleBase.h
    include/header.h
    include/ResponseListener.h
    include/Session.h
    include/Table.h
    )
     
set (SOURCES
    source/Example.cpp
    source/ExampleBase.cpp
    source/main.cpp
    source/ResponseListener.cpp
    source/Session.cpp
    ) 

if (NOT CMAKE_CROSSCOMPILING)
	add_executable (${PROJECT} ${HEADERS} ${SOURCES})
else()
	add_library (${PROJECT} STATIC ${HEADERS} ${SOURCES})
endif()

if (${CMAKE_SYSTEM} MATCHES "Linux")
	target_link_libraries (${PROJECT} order2go2 threadlib)
elseif (${CMAKE_SYSTEM} MATCHES "Darwin")
	target_link_libraries (${PROJECT} liborder2go2.a libthreadlib.a)
endif()
