cmake_minimum_required(VERSION 2.8)

project( KinectV2Module )

SET(BUILD_SHARED_LIBS ON)

set(KinectSDK2_FOUND OFF CACHE BOOL "Kinect 2.x SDK found")

if(WIN32)
    if(EXISTS $ENV{KINECTSDK20_DIR})
        message(STATUS KINECT_FOUND)
		message(STATUS $ENV{KINECTSDK20_DIR})
        set(KinectSDK2_FOUND ON CACHE BOOL "Kinect 2.x SDK found" FORCE)
        set(KinectSDK2_DIR $ENV{KinectSDK2_DIR} CACHE PATH "Kinect 2.x SDK path" FORCE)
        if(EXISTS $ENV{KINECTSDK20_DIR}/inc)
            set(KinectSDK2_INCLUDE_DIRS $ENV{KINECTSDK20_DIR}/inc)
			include_directories( ${KinectSDK2_INCLUDE_DIRS})
            message(STATUS KINECT_INC_FOUND)
        endif()
        if(EXISTS $ENV{KINECTSDK20_DIR}/lib/x64/)
            set(KinectSDK2_LIBRARY_DIRS $ENV{KINECTSDK20_DIR}/lib/x64/)
            message(STATUS KINECT_LIB_DIR_FOUND)
        endif()
        if(EXISTS $ENV{KINECTSDK20_DIR}/lib/x86/Kinect20.lib)
            set(KinectSDK2_LIBRARIES $ENV{KINECTSDK20_DIR}/lib/x64/Kinect20.lib)
            message(STATUS KINECT_LIB_FOUND)
        endif()
    endif()
else()
message(KINECT_NOT_FOUND)
endif()


find_package( OpenCV )
include_directories( ${OpenCV_INCLUDE_DIRS} )
set(src 
		main.cpp
		KinectColorStream.cpp
		)
add_executable( KinectV2Module ${src} )
target_link_libraries( KinectV2Module ${OpenCV_LIBS} ${KinectSDK2_LIBRARIES})
