cmake_minimum_required(VERSION 3.24)
project(hand-fibers)

set(CMAKE_CXX_STANDARD 20)

find_package( Boost REQUIRED COMPONENTS ALL )
include_directories( ${Boost_INCLUDE_DIRS} )
link_libraries(boost_fiber boost_context)
enable_language(C ASM)

set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")



add_executable(server sync/condvar.cpp context/context.cpp context/context.S
        fiber/fiber_impl.cpp fiber/fiber_manager.cpp sync/mutex.cpp waiter/waiter.cpp
        context/thread_local.cpp main.cpp)


add_executable(context_test context/context.cpp context/context.S tests/context_test.cpp context/fsgsbase.cpp context/thread_local.cpp)

add_executable(thread_local_test context/context.cpp context/context.S context/thread_local.cpp tests/thread_local_test.cpp)
