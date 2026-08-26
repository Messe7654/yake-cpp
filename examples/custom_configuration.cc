#include "yake/keyword_extractor.h"

#include <iostream>

int main() {
  yake::Config config{};
  config.max_ngram_size      = 1;  // Return individual words only.
  config.max_keywords        = 3;
  config.context_window_size = 2;

  const yake::KeywordExtractor extractor{config};
  const auto keywords{
      extractor.extract("Graph algorithms process networks. Efficient graph algorithms find paths in large networks.")};

  for (const yake::Keyword& keyword : keywords) std::cout << keyword.text << '\n';
}

// Expected output:
// networks
// algorithms
// Graph
