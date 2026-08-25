include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.tar.gz
    URL_HASH SHA256=7b42b4d6ed48810c5362c265a17faebe90dc2373c885e5216439d37927f02926
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(yake_tests
    unit/candidate_generator_test.cc
    unit/candidate_ranker_test.cc
    unit/deduplication_policy_test.cc
    unit/keyword_extractor_test.cc
    unit/normalizer_test.cc
    unit/stop_words_test.cc
    unit/tokenizer_test.cc
    unit/version_test.cc
    unit/word_features_test.cc
    unit/word_statistics_test.cc
)
yake_configure_target(yake_tests)

target_link_libraries(yake_tests
    PRIVATE
        yake::yake
        GTest::gtest_main
)

target_include_directories(yake_tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
)

include(GoogleTest)
if(YAKE_ENABLE_SANITIZERS AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set_property(
        TARGET yake_tests
        PROPERTY CROSSCOMPILING_EMULATOR ${CMAKE_COMMAND} -E env ASAN_OPTIONS=detect_leaks=0
    )
endif()
gtest_discover_tests(yake_tests)
