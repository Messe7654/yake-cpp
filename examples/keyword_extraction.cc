#include "yake/keyword_extractor.h"

#include <iostream>
#include <string>
#include <string_view>

int main(int argc, char* argv[]) {
  constexpr std::string_view example_text{
      "Keyword extraction identifies the most relevant expressions in a document. "
      "YAKE performs unsupervised keyword extraction using local statistical features."};

  std::string text{};
  for (int idx{1}; idx < argc; ++idx) {
    if (!text.empty()) text.push_back(' ');
    text.append(argv[idx]);
  }

  yake::Config config{};
  config.max_keywords = 5;
  const yake::KeywordExtractor extractor{config};
  const auto keywords{extractor.extract((text.empty()) ? example_text : std::string_view{text})};

  for (const yake::Keyword& keyword : keywords) std::cout << keyword.text << '\n';
}

// Expected output when run without arguments:
// Keyword extraction identifies
// relevant expressions
// Keyword extraction
// YAKE performs unsupervised
// extraction identifies
//
// The program also accepts the text to analyze as command-line arguments, so
// the output will differ when arguments are provided.
