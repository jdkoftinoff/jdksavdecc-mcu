cmake_minimum_required (VERSION 2.8)
set(PROJECT jdksavdecc-mcu)

project(${PROJECT} C CXX)

option(CXX11 "C++11 mode" "OFF")
option(PCAP "Enable/Link with PCAP library" "ON")

include_directories( "include" "jdksavdecc-c/include" )

file(GLOB PROJECT_SRC "jdksavdecc-c/src/*.c" )

set(PROJECT_INCLUDES_GLOBS "jdksavdecc-c/include/*.h" "jdksavdecc-c/include/*.hpp")
set(PROJECT_SRC_GLOBS "jdksavdecc-c/src/*.c" "jdksavdecc-c/src/*.cpp" )

if( PCAP )
    add_definitions("-DJDKSAVDECCMCU_ENABLE_PCAP=1")
    if( WIN32 )
	include_directories($ENV{PCAP_INCLUDE})
	set(LIBS ${LIBS} wpcap)
	link_directories($ENV{PCAP_LIBRARY})
    elseif( ${CMAKE_SYSTEM_NAME} MATCHES "Darwin" )
	set(LIBS ${LIBS} pcap)
    endif()
else()
    add_definitions("-DJDKSAVDECCMCU_ENABLE_PCAP=0")
endif()

INCLUDE (common.cmake)

