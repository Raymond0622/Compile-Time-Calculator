cmake_minimum_required(VERSION 3.29)
project(TempCalculator CXX)
set(CMAKE_CXX_STANDARD 20)

set(Boost_MP11_Include_Dirs "/opt/homebrew/opt/boost/include/")
add_executable(run calcmeta.cpp)
target_include_directories(run PUBLIC ${Boost_MP11_Include_Dirs})