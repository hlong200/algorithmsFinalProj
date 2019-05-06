cmake_minimum_required(VERSION 3.13)
project(groupProject)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -lmingw32")
set(CMAKE_EXE_LNKER_FLAGS "-static-libgcc -static-libstdc++")

include_directories(${CMAKE_SOURCE_DIR}/SDL2-2.0.9/i686-w64-mingw32/include)
link_directories(${CMAKE_SOURCE_DIR}/SDL2-2.0.9/i686-w64-mingw32/lib)

set(SOURCE_FILES main.cpp SDL_Plotter.h SDL_Plotter.cpp)
add_executable(groupProject ${SOURCE_FILES})

target_link_libraries(groupProject mingw32 SDL2main SDL2 SDL2_mixer)