# CMakeLists.txt for <VirtualMic> 
cmake_minimum_required(VERSION 3.11)

# hints from https://github.com/robotology/how-to-export-cpp-library/blob/master/CMakeLists.txt
project(VirtualMic
        LANGUAGES CXX C
        VERSION 0.0.0.0)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_CRT_SECURE_NO_WARNINGS")

include(FetchContent)

FetchContent_Declare(
  DirectShowExt
  GIT_REPOSITORY https://github.com/CSIR-RTVC/DirectShowExt
)
# Declare dependencies
find_package(DirectShowExt 1.0.0 REQUIRED)

SET(FLT_HDRS
VersionInfo.h
VirtualMic.h
VirtualMicSourceStream.h
)

SET(FLT_SRCS 
DllSetup.cpp
VirtualMic.cpp
VirtualMic.def
VirtualMicSourceStream.cpp
)

ADD_LIBRARY(
VirtualMic SHARED ${FLT_SRCS} ${FLT_HDRS})

target_include_directories(VirtualMic
    PUBLIC 
        $<INSTALL_INTERFACE:install>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

LINK_DIRECTORIES(
    $ENV{MSSDK}/Lib
)

TARGET_LINK_LIBRARIES (
VirtualMic
DirectShowExt::DirectShowExt
) 

INSTALL(
  TARGETS VirtualMic
  ARCHIVE DESTINATION bin
  LIBRARY DESTINATION bin
)

IF (REGISTER_DS_FILTERS)
ADD_CUSTOM_COMMAND(
TARGET VirtualMic 
POST_BUILD COMMAND 
regsvr32 /s \"$(TargetPath)\"
)
ENDIF(REGISTER_DS_FILTERS)
