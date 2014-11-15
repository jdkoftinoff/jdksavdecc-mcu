cmake_minimum_required (VERSION 2.8)
set(PROJECT jdksavdecc-mcu)

include_directories( "include" "../jdksavdecc-c/include" )

file(GLOB PROJECT_INCLUDES "jdkavdecc-c/include/*.h" "jdkavdecc-c/include/*.hpp" "include/*.hpp" "include/tests/*.hpp" "include/*.h" "include/tests/*.h" )
file(GLOB PROJECT_SRC "src/*.c" "src/test/*.c" "src/*.cpp" "src/test/*.cpp" "jdksavdecc-c/src/*.c" "jdksavdecc-c/src/*.cpp" "jdkavdecc-c/include/*.h" "jdkavdecc-c/include/*.hpp" )

if(CMAKE_BUILD_TOOL MATCHES "(msdev|devenv|nmake|MSBuild)")
    include_directories($ENV{PCAP_INCLUDE})
    set(LIBS ${LIBS} wpcap)
    link_directories($ENV{PCAP_LIBRARY})
else()
    set(LIBS ${LIBS} pcap)
endif()

INCLUDE (common.cmake)

