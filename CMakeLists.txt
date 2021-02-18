cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(lpcnet_benchmark CXX)

include(FetchContent)
include(buildtype.cmake)

set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY true)
set(FETCHCONTENT_QUIET FALSE)

FetchContent_Declare(lpcnet
                        GIT_REPOSITORY https://github.com/xnorpx/lpcnet.git
                        GIT_TAG origin/dev/cmakeopt
                        GIT_PROGRESS TRUE
                        USES_TERMINAL_DOWNLOAD
                        TRUE)

FetchContent_Declare(benchmark
                        GIT_REPOSITORY https://github.com/google/benchmark.git
                        GIT_PROGRESS TRUE
                        USES_TERMINAL_DOWNLOAD
                        TRUE)

FetchContent_GetProperties(lpcnet benchmark)
if(NOT benchmark_POPULATED)
  FetchContent_Populate(benchmark)

  set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)
  set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
  set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)

  add_subdirectory(${benchmark_SOURCE_DIR} EXCLUDE_FROM_ALL)
endif()

if(NOT lpcnet_POPULATED)
  FetchContent_Populate(lpcnet)
  add_subdirectory(${lpcnet_SOURCE_DIR} EXCLUDE_FROM_ALL)
endif()

enable_testing()

add_executable(lpcnet_benchmark 
               test_data_1_channel_16_khz_pcm.h
               test_data_1_channel_16_khz_pcm.cpp
               lpcnet_benchmark.cpp)
#target_compile_options(opus_encode_benchmark PRIVATE
#     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
#     -Werror -pedantic-errors -Wall -Wextra -Wconversion -Wsign-conversion>
#     $<$<CXX_COMPILER_ID:MSVC>: /W4 /WX>)
target_link_libraries(lpcnet_benchmark PRIVATE benchmark lpcnet_object)
add_test(lpcnet_benchmark lpcnet_benchmark)
