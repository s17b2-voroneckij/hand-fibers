cmake_minimum_required(VERSION 3.24)
project(hand-fibers)

set(CMAKE_CXX_STANDARD 20)

find_package( Boost REQUIRED COMPONENTS ALL )
include_directories( ${Boost_INCLUDE_DIRS} )
link_libraries(boost_fiber boost_context)
enable_language(C ASM)


add_executable(boostProject condvar.cpp context.cpp context.S fiber_impl.cpp fiber_manager.cpp mutex.cpp waiter.cpp main.cpp)
