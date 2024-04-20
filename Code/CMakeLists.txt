cmake_minimum_required(VERSION 3.20)
project(mlgDensest)

set(CMAKE_CXX_STANDARD 20)

if (MPFR)
    add_compile_definitions(MPFR)
    find_package(MPFR REQUIRED)
endif()

find_package (gflags REQUIRED)
include_directories (${gflags_INCLUDE_DIR})

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DMY_DEBUG")

add_executable(mlgDensest main.cpp Utils/Timer.h Header.h Utils/StrOps.h Utils/ArrOps.h Graphs/MLG.cpp Graphs/MLG.h Structs/IntHeap.cpp Structs/IntHeap.h Structs/RealHeap.h Structs/SetHeap.cpp Structs/SetHeap.h Structs/MixHeap.cpp Structs/MixHeap.h Structs/MixHeapMp.cpp Structs/MixHeapMp.h GDS/Algo.cpp GDS/Algo.h GDS/GFirmCore.cpp GDS/GFirmCore.h GDS/GDS.cpp GDS/GDS.h GDS/GDSMultiPrecision.cpp GDS/GDSMultiPrecision.h GDS/Stats.cpp GDS/Stats.h)

if (MPFR)
    target_link_libraries (mlgDensest gflags ${MPFR_LIBRARIES})
else()
    target_link_libraries (mlgDensest gflags)
endif()

