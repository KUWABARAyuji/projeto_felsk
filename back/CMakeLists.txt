cmake_minimum_required(VERSION 3.20)

project(MaridoAluguelAPI)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(MaridoAluguelAPI
    src/main.cpp
)

target_include_directories(MaridoAluguelAPI PRIVATE
    src
    include
)