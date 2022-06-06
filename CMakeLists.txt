cmake_minimum_required(VERSION 3.10)

project(PracticalParallelProgrammingInCPP
        VERSION 1.0
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)
find_package(MPI REQUIRED)

add_subdirectory(intro)
if(MSVC)
    add_subdirectory(windows_threads)
endif()
add_subdirectory(pthreads)
add_subdirectory(parallel_algorithms)
add_subdirectory(mpi)