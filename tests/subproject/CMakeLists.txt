cmake_minimum_required(VERSION 3.16)

project(yake_subproject_consumer LANGUAGES CXX)

add_subdirectory(../.. yake-cpp)

if(TARGET yake_tests OR TARGET yake_example)
    message(FATAL_ERROR "yake-cpp tests and examples must be disabled when used as a subproject")
endif()

add_executable(yake_subproject_consumer ../package/main.cc)
target_link_libraries(yake_subproject_consumer PRIVATE yake::yake)
