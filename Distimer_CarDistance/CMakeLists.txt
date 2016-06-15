CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )
SET(PROJ_PATH src)
if(UNIX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=gnu++0x")
endif()

include_directories(${PROJ_PATH})

SET(SOURCES src/CarAdjust.cpp
	src/CarDetector.cpp
	src/CarDistance.cpp
	src/CarDistanceCalculator.cpp
	src/CascadeClassifier.cpp)

SET(HEADER_FILES ${PROJ_PATH}/Camera.h
	${PROJ_PATH}/CarDistance.h
	${PROJ_PATH}/Cascade.h
	${PROJ_PATH}/CascadeClassifier.h
	${PROJ_PATH}/param.h)

project(Distimer_CarDistance)
find_package( OpenCV REQUIRED )
add_executable(Distimer_CarDistance ${SOURCES} ${HEADER_FILES})
target_link_libraries( Distimer_CarDistance ${OpenCV_LIBS} )
