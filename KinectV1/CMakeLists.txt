cmake_minimum_required(VERSION 2.8)

project( KinectV1Module )

SET(BUILD_SHARED_LIBS ON)

#set(KinectSDK18_FOUND OFF CACHE BOOL "Kinect 1.8 SDK found")

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
		else()
			message("Kinect SDK directory does not contain an include folder.")
        endif()
		if(CMAKE_CL_64)
			message(STATUS "arch 64")
			if(EXISTS $ENV{KINECTSDK10_DIR}/lib/amd64/Kinect10.lib)
				set(KinectSDK18_LIBRARIES $ENV{KINECTSDK10_DIR}/lib/amd64/Kinect10.lib)
				message(STATUS KINECT_LIB_FOUND)
			else()
				message(FATAL_ERROR "Chosen arch is 64 but Kinect library 64 bit version not found, check that" $ENV{KINECTSDK10_DIR} " contains /lib/amd64  folder")
			endif()
		else()
			message(STATUS "arch x86")
			if(EXISTS $ENV{KINECTSDK10_DIR}/lib/x86/Kinect10.lib)
				set(KinectSDK18_LIBRARIES $ENV{KINECTSDK10_DIR}/lib/x86/Kinect10.lib)
				message(STATUS KINECT_LIB_FOUND)
			else()
				message(FATAL_ERROR "Chosen arch is x86 but Kinect library 32 bit version not found, check that" $ENV{KINECTSDK10_DIR} " contains /lib/x86  folder")
			endif()
		endif() 
    endif()
else()
	message(KINECT_NOT_FOUND)
endif()


find_package( OpenCV )

include_directories( ${OpenCV_INCLUDE_DIRS} )

set(src 
		main.cpp
        KinectV1Sensor.cpp
		)
		
add_executable( KinectV1Module ${src} )

target_link_libraries( KinectV1Module ${OpenCV_LIBS} ${KinectSDK18_LIBRARIES})
