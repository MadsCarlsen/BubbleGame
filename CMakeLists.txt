cmake_minimum_required (VERSION 3.21)

project (BubbleGame)

add_executable(BubbleGame BubbleGame.cpp)

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(BubbleGame PUBLIC OpenMP::OpenMP_CXX)
endif()