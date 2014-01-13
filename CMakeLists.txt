cmake_minimum_required(VERSION 2.6)

project(pidgin-leaguedecline)

add_definitions(-DPACKAGE="${CMAKE_PROJECT_NAME}" -DLOCALEDIR=NULL)

set(CMAKE_C_FLAGS_DEBUG "-O0 -ggdb -Wall" CACHE STRING "" FORCE)

add_library(leaguedecline SHARED leaguedeny.c strings.c)

find_package( PkgConfig REQUIRED )
find_package( Threads REQUIRED )

pkg_check_modules( LIBPURPLE REQUIRED purple>=2.10.7 )

include_directories(${LIBPURPLE_INCLUDE_DIRS} ${CMAKE_THREAD_INCLUDE_DIRS})
target_link_libraries(leaguedecline ${LIBPURPLE_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

install(TARGETS leaguedecline
		LIBRARY DESTINATION ${LIBPURPLE_LIBDIR}/pidgin NAMELINK_SKIP
		ARCHIVE DESTINATION ${LIBPURPLE_LIBDIR})
