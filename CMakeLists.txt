cmake_minimum_required(VERSION 3.24)
project(boostProject)

set(CMAKE_CXX_STANDARD 20)

find_package( Boost REQUIRED COMPONENTS ALL )
include_directories( ${Boost_INCLUDE_DIRS} )
link_libraries(boost_fiber boost_context)

add_executable(boostProject main.cpp fiber_impl.cpp fiber_manager.cpp condvar.cpp mutex.cpp waiter.cpp)
