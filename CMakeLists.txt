# this cmake file is currently only for non-cross compiles for testing
# and is not used for creating avr binaries

cmake_minimum_required (VERSION 2.8)
project (JDKSAvdeccArduino)
enable_testing()

set(LIBS ${LIBS} ${CHECK_LIBRARIES} JDKSAvdeccArduino)

include_directories( "jdksavb/jdksavdecc-c/include" "jdksavb/jdksavb/include" )

file(GLOB JDKSAVDECC_ARDUINO_INCLUDES "*.h" "*.hpp" "jdksavb/jdksavb/include/*.h" "jdksavb/jdksavdecc-c/include/*.h" )

file(GLOB JDKSAVDECC_ARDUINO_SRC "*.cpp" "*.c" )

add_library (JDKSAvdeccArduino ${JDKSAVDECC_ARDUINO_SRC} ${JDKSAVDECC_ARDUINO_INCLUDES})

INSTALL(TARGETS JDKSAvdeccArduino DESTINATION "lib")

INSTALL(FILES
   ${JDKSAVDECC_ARDUINO_INCLUDES}
   DESTINATION include
)

link_directories( ${JDKSAVDECC_ARDUINO_BINARY_DIR} )


