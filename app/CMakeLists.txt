cmake_minimum_required(VERSION 3.22)

add_library(app STATIC
    src/app.cpp
)

target_include_directories(app PUBLIC
    include
    ${CMAKE_SOURCE_DIR}/gn10_can
)
target_compile_features(app PUBLIC cxx_std_17)

target_link_libraries(app PUBLIC gn10_can)
target_link_libraries(app PRIVATE stm32cubemx)
