#include "yake/keyword_extractor.h"

#include <iostream>

int main() {
  const yake::KeywordExtractor extractor{};
  const auto keywords{extractor.extract("Keyword extraction identifies important document terms.")};
  const yake::BasicKeywordExtractor<yake::LevenshteinPolicy> levenshtein_extractor{};
  const auto levenshtein_keywords{levenshtein_extractor.extract("Keyword extraction identifies important terms.")};

  if (keywords.empty() || levenshtein_keywords.empty()) return 1;
  std::cout << keywords.front().text << '\n';
}
