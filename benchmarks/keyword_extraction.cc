#include "yake/keyword_extractor.h"

#include <chrono>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <string>
#include <string_view>

namespace {

constexpr std::size_t kIterations{100};
constexpr std::size_t kRepeats{20};
constexpr std::string_view kParagraph{
    "Keyword extraction identifies the most relevant expressions in a document. "
    "YAKE performs unsupervised keyword extraction using local statistical features. "
    "Graph algorithms process networks while statistical methods rank candidate phrases. "};

std::string make_document() {
  std::string text{};
  text.reserve(kParagraph.size() * kRepeats);
  for (std::size_t idx{0}; idx < kRepeats; ++idx) text.append(kParagraph);
  return text;
}

template <typename Policy>
void benchmark_extraction(std::string_view name, std::string_view text) {
  yake::Config config{};
  config.max_keywords = 20;
  const yake::BasicKeywordExtractor<Policy> extractor{config};

  std::size_t checksum{0};
  const auto begin{std::chrono::steady_clock::now()};
  for (std::size_t idx{0}; idx < kIterations; ++idx) {
    const auto keywords{extractor.extract(text)};
    checksum += keywords.size();
  }
  const auto end{std::chrono::steady_clock::now()};
  const std::chrono::duration<double, std::micro> elapsed{end - begin};

  std::cout << std::left << std::setw(18) << name << std::right << std::setw(12) << std::fixed << std::setprecision(2)
            << elapsed.count() / kIterations << " us/op  " << checksum << '\n';
}

template <typename Policy>
void benchmark_similarity(std::string_view name) {
  constexpr std::string_view lhs{"single document keyword extraction"};
  constexpr std::string_view rhs{"single-document keywords extraction"};
  constexpr std::size_t kSimilarityIterations{kIterations * 1000};
  const Policy policy{};

  double checksum{0.0};
  const auto begin{std::chrono::steady_clock::now()};
  for (std::size_t idx{0}; idx < kSimilarityIterations; ++idx) checksum += policy.similarity(lhs, rhs);
  const auto end{std::chrono::steady_clock::now()};
  const std::chrono::duration<double, std::nano> elapsed{end - begin};

  std::cout << std::left << std::setw(18) << name << std::right << std::setw(12) << std::fixed << std::setprecision(2)
            << elapsed.count() / kSimilarityIterations << " ns/op  " << checksum << '\n';
}

}  // namespace

int main() {
  const std::string text{make_document()};

  std::cout << "extraction\n";
  benchmark_extraction<yake::LevenshteinPolicy>("levenshtein", text);
  benchmark_extraction<yake::JaroWinklerPolicy>("jaro-winkler", text);
  benchmark_extraction<yake::SequenceMatcherPolicy>("sequence-matcher", text);

  std::cout << "\nsimilarity\n";
  benchmark_similarity<yake::LevenshteinPolicy>("levenshtein");
  benchmark_similarity<yake::JaroWinklerPolicy>("jaro-winkler");
  benchmark_similarity<yake::SequenceMatcherPolicy>("sequence-matcher");
}
