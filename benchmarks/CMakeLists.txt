add_executable(yake_benchmark keyword_extraction.cc)
yake_configure_target(yake_benchmark)

target_link_libraries(yake_benchmark
    PRIVATE
        yake::yake
)
