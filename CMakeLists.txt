# To build on Linux/macOS:
#   cmake -H. -Bbuild -DCMAKE_BUILD_TYPE=Release [Debug]
#   cmake --build build

cmake_minimum_required(VERSION 3.08)

# set the project name and version
project(cg_cvrp VERSION 0.0.11 LANGUAGES CXX)

configure_file("./cmake/cmake_config.hpp.in" "../include/utils/cmake_config.hpp")

option(CXX "enable C++ compilation" ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif(NOT CMAKE_BUILD_TYPE)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

include_directories(${PROJECT_SOURCE_DIR}/headers)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wno-deprecated-copy -Wno-missing-field-initializers -pedantic-errors")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -fopenmp")

# determine the Gurobi's necessary include and linker commands
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
find_package(GUROBI REQUIRED)

# find boost paths
find_package(Boost REQUIRED COMPONENTS program_options)

# list all source files here
set(SOURCES
    src/main.cpp
    src/base_lp.cpp
    src/config_parameters.cpp
    src/instance.cpp
    # src/vrp_lp.cpp
    src/column_generation/callback_sec.cpp
    src/column_generation/column.cpp
    src/column_generation/cg.cpp
    src/column_generation/init_grb_model.cpp
    src/column_generation/init_pool.cpp
    src/column_generation/set_covering_lp.cpp
    src/column_generation/pctsp_ilp.cpp
    src/utils/helper.cpp
    src/utils/stopwatch.cpp
    src/utils/tools.cpp
    src/ext/cvrpsep/basegrph.cpp
    src/ext/cvrpsep/blocks.cpp
    src/ext/cvrpsep/capsep.cpp
    src/ext/cvrpsep/cnstrmgr.cpp
    src/ext/cvrpsep/compcuts.cpp
    src/ext/cvrpsep/compress.cpp
    src/ext/cvrpsep/cutbase.cpp
    src/ext/cvrpsep/fcapfix.cpp
    src/ext/cvrpsep/grsearch.cpp
    src/ext/cvrpsep/memmod.cpp
    src/ext/cvrpsep/mxf.cpp
    src/ext/cvrpsep/sort.cpp
    src/ext/cvrpsep/strngcmp.cpp
    src/ext/lkh-2.0.9/Activate.cpp
    src/ext/lkh-2.0.9/AddCandidate.cpp
    src/ext/lkh-2.0.9/AddTourCandidates.cpp
    src/ext/lkh-2.0.9/AdjustCandidateSet.cpp
    src/ext/lkh-2.0.9/AdjustClusters.cpp
    src/ext/lkh-2.0.9/AllocateStructures.cpp
    src/ext/lkh-2.0.9/Ascent.cpp
    src/ext/lkh-2.0.9/Best2OptMove.cpp
    src/ext/lkh-2.0.9/Best3OptMove.cpp
    src/ext/lkh-2.0.9/Best4OptMove.cpp
    src/ext/lkh-2.0.9/Best5OptMove.cpp
    src/ext/lkh-2.0.9/BestKOptMove.cpp
    src/ext/lkh-2.0.9/Between.cpp
    src/ext/lkh-2.0.9/Between_SL.cpp
    src/ext/lkh-2.0.9/Between_SSL.cpp
    src/ext/lkh-2.0.9/BridgeGain.cpp
    src/ext/lkh-2.0.9/BuildKDTree.cpp
    src/ext/lkh-2.0.9/C.cpp
    src/ext/lkh-2.0.9/CandidateReport.cpp
    src/ext/lkh-2.0.9/ChooseInitialTour.cpp
    src/ext/lkh-2.0.9/Connect.cpp
    src/ext/lkh-2.0.9/CreateCandidateSet.cpp
    src/ext/lkh-2.0.9/Distance.cpp
    src/ext/lkh-2.0.9/Distance_SPECIAL.cpp
    src/ext/lkh-2.0.9/eprintf.cpp
    src/ext/lkh-2.0.9/ERXT.cpp
    src/ext/lkh-2.0.9/Excludable.cpp
    src/ext/lkh-2.0.9/Exclude.cpp
    src/ext/lkh-2.0.9/FindTour.cpp
    src/ext/lkh-2.0.9/FixedOrCommonCandidates.cpp
    src/ext/lkh-2.0.9/Flip.cpp
    src/ext/lkh-2.0.9/Flip_SL.cpp
    src/ext/lkh-2.0.9/Flip_SSL.cpp
    src/ext/lkh-2.0.9/Forbidden.cpp
    src/ext/lkh-2.0.9/FreeStructures.cpp
    src/ext/lkh-2.0.9/fscanint.cpp
    src/ext/lkh-2.0.9/Gain23.cpp
    src/ext/lkh-2.0.9/GenerateCandidates.cpp
    src/ext/lkh-2.0.9/Genetic.cpp
    src/ext/lkh-2.0.9/GetTime.cpp
    src/ext/lkh-2.0.9/gpx.cpp
    src/ext/lkh-2.0.9/GreedyTour.cpp
    src/ext/lkh-2.0.9/Hashing.cpp
    src/ext/lkh-2.0.9/Heap.cpp
    src/ext/lkh-2.0.9/IsBackboneCandidate.cpp
    src/ext/lkh-2.0.9/IsCandidate.cpp
    src/ext/lkh-2.0.9/IsCommonEdge.cpp
    src/ext/lkh-2.0.9/IsPossibleCandidate.cpp
    src/ext/lkh-2.0.9/KSwapKick.cpp
    src/ext/lkh-2.0.9/LinKernighan.cpp
    src/ext/lkh-2.0.9/LKH.cpp
    src/ext/lkh-2.0.9/LKHmain.cpp
    src/ext/lkh-2.0.9/Make2OptMove.cpp
    src/ext/lkh-2.0.9/Make3OptMove.cpp
    src/ext/lkh-2.0.9/Make4OptMove.cpp
    src/ext/lkh-2.0.9/Make5OptMove.cpp
    src/ext/lkh-2.0.9/MakeKOptMove.cpp
    src/ext/lkh-2.0.9/MergeTourWithBestTour.cpp
    src/ext/lkh-2.0.9/MergeWithTourGPX2.cpp
    src/ext/lkh-2.0.9/MergeWithTourIPT.cpp
    src/ext/lkh-2.0.9/Minimum1TreeCost.cpp
    src/ext/lkh-2.0.9/MinimumSpanningTree.cpp
    src/ext/lkh-2.0.9/NormalizeNodeList.cpp
    src/ext/lkh-2.0.9/NormalizeSegmentList.cpp
    src/ext/lkh-2.0.9/OrderCandidateSet.cpp
    src/ext/lkh-2.0.9/PatchCycles.cpp
    src/ext/lkh-2.0.9/printff.cpp
    src/ext/lkh-2.0.9/PrintParameters.cpp
    src/ext/lkh-2.0.9/Random.cpp
    src/ext/lkh-2.0.9/ReadCandidates.cpp
    src/ext/lkh-2.0.9/ReadEdges.cpp
    src/ext/lkh-2.0.9/ReadLine.cpp
    src/ext/lkh-2.0.9/ReadParameters.cpp
    src/ext/lkh-2.0.9/ReadPenalties.cpp
    src/ext/lkh-2.0.9/ReadProblem.cpp
    src/ext/lkh-2.0.9/RecordBestTour.cpp
    src/ext/lkh-2.0.9/RecordBetterTour.cpp
    src/ext/lkh-2.0.9/RemoveFirstActive.cpp
    src/ext/lkh-2.0.9/ResetCandidateSet.cpp
    src/ext/lkh-2.0.9/RestoreTour.cpp
    src/ext/lkh-2.0.9/SegmentSize.cpp
    src/ext/lkh-2.0.9/Sequence.cpp
    src/ext/lkh-2.0.9/SFCTour.cpp
    src/ext/lkh-2.0.9/SolveCompressedSubproblem.cpp
    src/ext/lkh-2.0.9/SolveSubproblem.cpp
    src/ext/lkh-2.0.9/Statistics.cpp
    src/ext/lkh-2.0.9/StoreTour.cpp
    src/ext/lkh-2.0.9/SymmetrizeCandidateSet.cpp
    src/ext/lkh-2.0.9/TrimCandidateSet.cpp
    src/ext/loguru/loguru.cpp)

# list all header files here
set(HEADERS
    include/base_lp.hpp
    include/config_parameters.hpp
    include/instance.hpp
    # include/vrp_lp.hpp
    include/column_generation/callback_sec.hpp
    include/column_generation/column.hpp
    include/column_generation/cg.hpp
    include/column_generation/init_grb_model.hpp
    include/column_generation/init_pool.hpp
    include/column_generation/set_covering_lp.hpp
    include/column_generation/pctsp_ilp.hpp
    include/utils/helper.hpp
    include/utils/multi_vector.hpp
    include/utils/stopwatch.hpp
    include/utils/tools.hpp
    include/ext/cvrpsep/basegrph.h
    include/ext/cvrpsep/blocks.h
    include/ext/cvrpsep/capsep.h
    include/ext/cvrpsep/cnstrmgr.h
    include/ext/cvrpsep/compcuts.h
    include/ext/cvrpsep/compress.h
    include/ext/cvrpsep/cutbase.h
    include/ext/cvrpsep/fcapfix.h
    include/ext/cvrpsep/grsearch.h
    include/ext/cvrpsep/memmod.h
    include/ext/cvrpsep/mxf.h
    include/ext/cvrpsep/sort.h
    include/ext/cvrpsep/strngcmp.h
    include/ext/lkh-2.0.9/lkh_tsp.hpp
    include/ext/lkh-2.0.9/GainType.h
    include/ext/lkh-2.0.9/Genetic.h
    include/ext/lkh-2.0.9/gpx.h
    include/ext/lkh-2.0.9/Hashing.h
    include/ext/lkh-2.0.9/Heap.h
    include/ext/lkh-2.0.9/LKH.h
    include/ext/lkh-2.0.9/Segment.h
    include/ext/lkh-2.0.9/Sequence.h
    include/ext/loguru/loguru.hpp)

# add the executable
if(Boost_FOUND)
    include_directories(${GUROBI_INCLUDE_DIRS})
    include_directories(${Boost_INCLUDE_DIRS}) 
    add_executable(${CMAKE_PROJECT_NAME} ${SOURCES} ${HEADERS})
    target_link_libraries(${CMAKE_PROJECT_NAME} optimized ${GUROBI_CXX_LIBRARY} debug ${GUROBI_CXX_DEBUG_LIBRARY})
    target_link_libraries(${CMAKE_PROJECT_NAME} ${GUROBI_LIBRARY})
    target_link_libraries(${CMAKE_PROJECT_NAME} ${Boost_LIBRARIES})
    target_link_libraries(${CMAKE_PROJECT_NAME} dl) # for fix -ldl linker error
endif()
