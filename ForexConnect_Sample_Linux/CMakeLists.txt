cmake_minimum_required (VERSION 2.6) 
 
set (PROJECT ForexConnect_Sample_Linux) 
  
project (${PROJECT}) 

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/source
                     ${CMAKE_CURRENT_SOURCE_DIR}/../include
                    )   

link_directories (${CMAKE_CURRENT_SOURCE_DIR}/lib 
                 )

set (HEADERS 
    source/stdafx.h
    )
     
set (SOURCES
    source/stdafx.cpp
    source/sample.cpp
    ) 

if (NOT CMAKE_CROSSCOMPILING)
	add_executable (${PROJECT} ${HEADERS} ${SOURCES})
else()
	add_library (${PROJECT} STATIC ${HEADERS} ${SOURCES})
endif()

add_definitions (-g)


if (${CMAKE_SYSTEM} MATCHES "Linux")
	target_link_libraries (${PROJECT} liborder2go2.so)
elseif (${CMAKE_SYSTEM} MATCHES "Darwin")
	target_link_libraries (${PROJECT} liborder2go2.a)
endif()