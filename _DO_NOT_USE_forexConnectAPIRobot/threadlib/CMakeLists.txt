cmake_minimum_required (VERSION 2.6) 
 
set (PROJECT threadlib) 
  
project (${PROJECT}) 

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/include
					)   

set (HEADERS 
	include/AThread.h
	include/CWinEventHandle.h
	include/Mutex.h
	source/stdhead.h
	include/winEmul.h
	)
     
set (SOURCES
	source/AThread.cpp
	source/CWinEventHandle.cpp
	source/Mutex.cpp
	source/winevent.cpp
	) 

#add_definitions("-fvisibility=hidden")

if (${CMAKE_SYSTEM} MATCHES "Linux")
  if (CMAKE_COMPILER_IS_GNUCXX) 
	add_definitions (-fPIC)
  elseif (CMAKE_COMPILER_IS_GNUCC)
	add_definitions (-fPIC)
  endif()
endif()

add_library (${PROJECT} STATIC ${HEADERS} ${SOURCES})
