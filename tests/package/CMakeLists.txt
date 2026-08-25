cmake_minimum_required(VERSION 3.16)

project(yake_package_consumer LANGUAGES CXX)

find_package(yake CONFIG REQUIRED)

add_executable(yake_package_consumer main.cc)
target_link_libraries(yake_package_consumer PRIVATE yake::yake)
