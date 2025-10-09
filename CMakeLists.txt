cmake_minimum_required(VERSION 3.29)
project(TempCalculator CXX)
set(CMAKE_CXX_STANDARD 20)

set(Boost_MP11_Include_Dirs "/opt/homebrew/opt/boost/include/")
set(Calculator_Include_Dirs "${CMAKE_SOURCE_DIR}/include")

add_library(calc_headers INTERFACE)

target_include_directories(calc_headers
    INTERFACE
        ${Boost_MP11_Include_Dirs}      
        ${Calculator_Include_Dirs}  
)

add_executable(run calcmeta.cpp)

target_link_libraries(run PRIVATE calc_headers)

add_definitions(-DNUMBER1="${NUMBER1}")
add_definitions(-DNUMBER2="${NUMBER2}")
add_definitions(-DOPERATION="${OPERATION}")

if (NOT DEFINED PRECISION)
    add_definitions(-DPRECISION=100)
else()
    add_definitions(-DPRECISION=${PRECISION})
endif()

if(OPERATION STREQUAL "*")
    add_definitions(-DMULTIPLY)
elseif(OPERATION STREQUAL "+")
    add_definitions(-DADD)
elseif(OPERATION STREQUAL "-")
    add_definitions(-DSUBTRACT)
elseif(OPERATION STREQUAL "/")
    add_definitions(-DDIVIDE)
elseif(OPERATION STREQUAL "b")
    add_definitions(-DBINARY)
elseif(OPERATION STREQUAL "^")
    add_definitions(-DEXPONENT)
else()
    message(FATAL_ERROR "Unknown OPERATION: ${OPERATION}")
endif()


