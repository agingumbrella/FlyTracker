PROJECT(FlyTracker)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
FIND_PACKAGE(OpenCV REQUIRED)
FIND_PACKAGE(Boost REQUIRED)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_FILE_OFFSET_BITS=64")
FILE(GLOB SRC . *.cpp)
SET(SRC ${SRC} ${CMAKE_CURRENT_SOURCE_DIR}/hungarian/asp.cpp ${CMAKE_CURRENT_SOURCE_DIR}/hungarian/hungarian.cpp)
ADD_EXECUTABLE(flytracker ${SRC})
TARGET_LINK_LIBRARIES(flytracker ${OpenCV_LIBS})
