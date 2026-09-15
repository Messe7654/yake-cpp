add_executable(yake_benchmark keyword_extraction.cc)
yake_configure_target(yake_benchmark)

target_link_libraries(yake_benchmark
    PRIVATE
        yake::yake
)

add_executable(stop_words_benchmark stop_words_benchmark.cc)
yake_configure_target(stop_words_benchmark)

target_link_libraries(stop_words_benchmark
    PRIVATE
        yake::yake
)

target_include_directories(stop_words_benchmark
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_BINARY_DIR}/src/generated
)
