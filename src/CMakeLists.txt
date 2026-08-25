include(FetchContent)

FetchContent_Declare(
    utf8proc
    URL https://github.com/JuliaStrings/utf8proc/archive/refs/tags/v2.11.3.tar.gz
    URL_HASH SHA256=abfed50b6d4da51345713661370290f4f4747263ee73dc90356299dfc7990c78
)

set(UTF8PROC_INSTALL ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(utf8proc)
if(NOT TARGET utf8proc::utf8proc)
    add_library(utf8proc::utf8proc ALIAS utf8proc)
endif()

file(READ ${PROJECT_SOURCE_DIR}/resources/stopwords/en.txt YAKE_ENGLISH_STOP_WORDS)
configure_file(${PROJECT_SOURCE_DIR}/cmake/stop_words_data.h.in generated/detail/stop_words_data.h @ONLY)

add_library(yake
    detail/candidate_generator.cc
    detail/candidate_ranker.cc
    detail/normalizer.cc
    detail/similarity.cc
    detail/stop_words.cc
    detail/term_normalizer.cc
    detail/tokenizer.cc
    detail/word_features.cc
    detail/word_statistics.cc
    deduplication_policy.cc
    keyword_extractor.cc
    version.cc
)
add_library(yake::yake ALIAS yake)
yake_configure_target(yake)

target_include_directories(yake
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(yake PUBLIC cxx_std_17)

target_link_libraries(yake
    PRIVATE
        utf8proc::utf8proc
)

target_include_directories(yake
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}/generated
)
