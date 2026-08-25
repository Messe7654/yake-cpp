#include "yake/keyword_extractor.h"

#include <iostream>
#include <string>
#include <string_view>

namespace {

constexpr std::string_view kExampleText{
    "Keyword extraction identifies the most relevant expressions in a document. "
    "YAKE performs unsupervised keyword extraction using local statistical features."};

}  // namespace

int main(int argc, char* argv[]) {
  std::string text{};
  for (int idx{1}; idx < argc; ++idx) {
    if (!text.empty()) text.push_back(' ');
    text.append(argv[idx]);
  }

  const yake::KeywordExtractor extractor{};
  const auto keywords{extractor.extract((text.empty()) ? kExampleText : std::string_view{text})};

  for (const yake::Keyword& keyword : keywords) std::cout << keyword.text << "\t" << keyword.score << '\n';
}
