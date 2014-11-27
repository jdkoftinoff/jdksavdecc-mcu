cmake_minimum_required (VERSION 2.8)
set(PROJECT jdksavdecc-mcu)

project(${PROJECT} C CXX)

option(JDKSAVDECC_C_LOCAL "embed jdksavdecc-c from jdksavdecc-c directory" "OFF")
option(CXX11 "Force C++11 compiler" "OFF")

IF(JDKSAVDECC_C_LOCAL MATCHES "ON")
    add_subdirectory( jdksavdecc-c )
ENDIF()

include_directories( "include" "jdksavdecc-c/include" "../jdksavdecc-c/include" )

set(LIBS ${LIBS} jdksavdecc-c)

if( WIN32 )
    include_directories($ENV{PCAP_INCLUDE})
    set(LIBS ${LIBS} wpcap)
    link_directories($ENV{PCAP_LIBRARY})
elseif( ${CMAKE_SYSTEM_NAME} MATCHES "Darwin" )
    set(LIBS ${LIBS} pcap)
endif()

INCLUDE (common.cmake)

