cmake_minimum_required(VERSION 2.8)
project(collisions)

set(CC mpicc)
set(CXX mpicxx)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O2")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -O2")

find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})

set(SOURCE_FILES main.cpp Point.cc Point.hh PMISerializable.cc PMISerializable.hh cli.cc cli.hh FileHeader.cc FileHeader.hh FileReader.cc FileReader.hh version.hh errors.cc errors.hh)
add_executable(collisions ${SOURCE_FILES})
target_link_libraries(collisions ${MPI_LIBRARIES})


if(MPI_COMPILE_FLAGS)
    set_target_properties(collisions PROPERTIES COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()

if(MPI_LINK_FLAGS)
    set_target_properties(collisions PROPERTIES LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()