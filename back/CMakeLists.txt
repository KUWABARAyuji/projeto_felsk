cmake_minimum_required(VERSION 3.20)

project(projeto_felsk_backend LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(felsk_api
    src/main.cpp
)

if(WIN32)
    target_link_libraries(felsk_api PRIVATE ws2_32)
endif()
