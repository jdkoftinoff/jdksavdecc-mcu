cmake_minimum_required (VERSION 2.8)
set(PROJECT jdksavdecc-mcu)
set(ADDITIONAL_INCLUDE_DIRECTORIES "jdksavdecc-c/include" )
set(ADDITIONAL_PROJECT_INCLUDES "jdkavdecc-c/include/*.h" "jdkavdecc-c/include/*.hpp")
set(ADDITIONAL_PROJECT_SRC "jdksavdecc-c/src/*.c" )

if(CMAKE_BUILD_TOOL MATCHES "(msdev|devenv|nmake|MSBuild)")
    include_directories($ENV{PCAP_INCLUDE})
    set(LIBS ${LIBS} wpcap)
    link_directories($ENV{PCAP_LIBRARY})
else()
    set(LIBS ${LIBS} pcap)
endif()

INCLUDE (common.cmake)

