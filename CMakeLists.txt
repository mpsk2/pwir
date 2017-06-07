cmake_minimum_required(VERSION 2.8)
project(collisions)

set(CC mpicc)
set(CXX mpicxx)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O2 -Wno-invalid-offsetof -Wno-format -DDEBUG")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -O2")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__FILENAME__='\"$(subst ${CMAKE_SOURCE_DIR}/,,$(abspath $<))\"'")

find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH} common/src common/include)

set(COMMON_SRC Algorithm.cc bounds.cc errors.cc cli.cc FileHeader.cc FileReader.cc Point.cc PMISerializable.cc Verbose.cc)
set(COMMON_INCLUDE bounds.hh Algorithm.hh errors.hh cli.hh FileHeader.hh FileReader.hh Point.hh PMISerializable.hh Verbose.hh)
add_library(COMMON ${COMMON_SRC} ${COMMON_INCLUDE})

set(SOURCE_FILES main.cpp
        version.hh
        Sender.cc Sender.hh
        )
add_executable(collisions-1 ${SOURCE_FILES})
add_executable(collisions-2 ${SOURCE_FILES})
add_executable(collisions-3 ${SOURCE_FILES})
target_link_libraries(collisions-1 ${MPI_LIBRARIES} COMMON)
target_link_libraries(collisions-2 ${MPI_LIBRARIES} COMMON)
target_link_libraries(collisions-3 ${MPI_LIBRARIES} COMMON)

target_compile_definitions(collisions-1 PUBLIC -DALG_V=1)
target_compile_definitions(collisions-2 PUBLIC -DALG_V=2)
target_compile_definitions(collisions-3 PUBLIC -DALG_V=3)

if(MPI_COMPILE_FLAGS)
    set_target_properties(collisions-1 PROPERTIES COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
    set_target_properties(collisions-2 PROPERTIES COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
    set_target_properties(collisions-3 PROPERTIES COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()

if(MPI_LINK_FLAGS)
    set_target_properties(collisions-1 PROPERTIES LINK_FLAGS "${MPI_LINK_FLAGS}")
    set_target_properties(collisions-2 PROPERTIES LINK_FLAGS "${MPI_LINK_FLAGS}")
    set_target_properties(collisions-3 PROPERTIES LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()