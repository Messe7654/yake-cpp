#include "yake/keyword_extractor.h"

#include <iostream>
#include <string_view>

namespace {

template <typename Policy>
void print_keywords(std::string_view name, std::string_view text) {
  yake::Config config{};
  config.max_ngram_size          = 1;
  config.max_keywords            = 5;
  config.deduplication_threshold = 0.9;

  const yake::BasicKeywordExtractor<Policy> extractor{config};

  std::cout << name << ':' << '\n';
  for (const yake::Keyword& keyword : extractor.extract(text)) std::cout << "- " << keyword.text << '\n';
}

}  // namespace

int main() {
  constexpr std::string_view text{
      "Graph graphs describe networks. Graph algorithms analyze graphical structures and graphing techniques."};

  print_keywords<yake::LevenshteinPolicy>("Levenshtein", text);
  print_keywords<yake::JaroWinklerPolicy>("Jaro-Winkler", text);
  print_keywords<yake::SequenceMatcherPolicy>("Sequence matcher", text);
}

// Expected output:
// Levenshtein:
// - Graph
// - networks
// - graphs
// - describe
// - techniques
// Jaro-Winkler:
// - Graph
// - networks
// - describe
// - techniques
// - algorithms
// Sequence matcher:
// - Graph
// - networks
// - describe
// - techniques
// - algorithms
