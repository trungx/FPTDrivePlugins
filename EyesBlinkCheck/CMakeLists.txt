cmake_minimum_required(VERSION 2.8)

project( EyesBlinkDetect )

if(UNIX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=gnu++0x")
endif()

find_package( OpenCV REQUIRED )
add_executable( EyesBlinkDetect main.cpp frm.cpp eye_blink_detector.cpp)
target_link_libraries( EyesBlinkDetect ${OpenCV_LIBS} )
