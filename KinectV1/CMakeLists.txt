cmake_minimum_required(VERSION 2.8)

project( KinectV1Module )

SET(BUILD_SHARED_LIBS ON)

set(KinectSDK18_FOUND OFF CACHE BOOL "Kinect 1.8 SDK found")

if(WIN32)
    if(EXISTS $ENV{KINECTSDK10_DIR})
        message(STATUS KINECT_FOUND)
		message(STATUS $ENV{KINECTSDK10_DIR})
        set(KinectSDK18_FOUND ON CACHE BOOL "Kinect 1.8 SDK found" FORCE)
        set(KinectSDK18_DIR $ENV{KINECTSDK10_DIR} CACHE PATH "Kinect 1.8 SDK path" FORCE)

        if(EXISTS $ENV{KINECTSDK10_DIR}/inc)
            set(KinectSDK18_INCLUDE_DIRS $ENV{KINECTSDK10_DIR}/inc)
			include_directories( ${KinectSDK18_INCLUDE_DIRS})
            message(STATUS KINECT_INC_FOUND)
        endif()
        if(EXISTS $ENV{KINECTSDK10_DIR}/lib/amd64/)
            set(KinectSDK18_LIBRARY_DIRS $ENV{KINECTSDK10_DIR}/lib/amd64/)
            message(STATUS KINECT_LIB_DIR_FOUND)
        endif()
        if(EXISTS $ENV{KINECTSDK10_DIR}/lib/x86/Kinect10.lib)
            set(KinectSDK18_LIBRARIES $ENV{KINECTSDK10_DIR}/lib/amd64/Kinect10.lib)
            message(STATUS KINECT_LIB_FOUND)
        endif()
    endif()
else()
message(KINECT_NOT_FOUND)
endif()


find_package( OpenCV )

include_directories( ${OpenCV_INCLUDE_DIRS} )

INCLUDE_DIRECTORIES( ${OPENNI2_INCLUDE_DIR} ${NITE2_INCLUDE_DIR} )

set(src 
		src/main.cpp
		)
		
add_executable( KinectModule ${src} )

target_link_libraries( KinectModule ${OpenCV_LIBS} ${KinectSDK18_LIBRARIES})
