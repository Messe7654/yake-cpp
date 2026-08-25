add_executable(yake_example keyword_extraction.cc)
yake_configure_target(yake_example)

target_link_libraries(yake_example
    PRIVATE
        yake::yake
)
