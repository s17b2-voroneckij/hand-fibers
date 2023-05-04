cmake_minimum_required(VERSION 3.24)
project(hand-fibers)

set(CMAKE_CXX_STANDARD 20)

find_package( Boost REQUIRED COMPONENTS ALL )
include_directories( ${Boost_INCLUDE_DIRS} )
link_libraries(boost_fiber boost_context)
enable_language(C ASM)


add_executable(boostProject sync/condvar.cpp context/context.cpp context/context.S fiber/fiber_impl.cpp fiber/fiber_manager.cpp sync/mutex.cpp waiter/waiter.cpp main.cpp)
