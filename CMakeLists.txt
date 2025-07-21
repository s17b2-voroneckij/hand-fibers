cmake_minimum_required(VERSION 3.24)

if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)
endif()

project(hand-fibers)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost REQUIRED COMPONENTS fiber context)

add_executable(hand-fibers main.cpp fiber_impl.cpp fiber_manager.cpp condvar.cpp mutex.cpp waiter.cpp)

target_link_libraries(hand-fibers PRIVATE Boost::fiber Boost::context)
