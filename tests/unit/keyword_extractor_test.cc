#include "yake/keyword_extractor.h"

#include <algorithm>
#include <cmath>
#include <gtest/gtest.h>
#include <stdexcept>
#include <string>

namespace {

struct AlwaysDuplicatePolicy {
  [[nodiscard]] double similarity(std::string_view, std::string_view) const { return 1.0; }
};

}  // namespace

TEST(KeywordExtractor, UsesYakeDefaults) {
  const yake::KeywordExtractor extractor;

  EXPECT_EQ(extractor.config().language, "en");
  EXPECT_EQ(extractor.config().max_ngram_size, 3U);
  EXPECT_DOUBLE_EQ(extractor.config().deduplication_threshold, 0.9);
  EXPECT_EQ(extractor.config().context_window_size, 1U);
  EXPECT_EQ(extractor.config().max_keywords, 20U);
}

TEST(KeywordExtractor, RejectsInvalidConfiguration) {
  yake::Config config;
  config.max_ngram_size = 0;

  EXPECT_THROW((void)yake::KeywordExtractor{config}, std::invalid_argument);
}

TEST(KeywordExtractor, RejectsUnsupportedLanguageWithoutFallback) {
  yake::Config config;
  config.language = "eng";

  EXPECT_THROW((void)yake::KeywordExtractor{config}, std::invalid_argument);
}

TEST(KeywordExtractor, ReturnsNoKeywordsForEmptyInput) {
  const yake::KeywordExtractor extractor;

  EXPECT_TRUE(extractor.extract("").empty());
}

TEST(KeywordExtractor, RunsTheCompleteExtractionPipeline) {
  yake::Config config;
  config.max_ngram_size = 2;
  config.max_keywords   = 5;
  const yake::KeywordExtractor extractor{config};

  const auto keywords{extractor.extract(
      "Graph algorithms process graphs. Efficient graph algorithms solve network problems. Graph processing matters.")};

  ASSERT_FALSE(keywords.empty());
  EXPECT_LE(keywords.size(), config.max_keywords);
  EXPECT_TRUE(std::is_sorted(keywords.begin(), keywords.end(),
                             [](const auto& lhs, const auto& rhs) { return lhs.score < rhs.score; }));
  for (const auto& keyword : keywords) {
    EXPECT_FALSE(keyword.text.empty());
    EXPECT_TRUE(std::isfinite(keyword.score));
  }
}

TEST(KeywordExtractor, FiltersStopWordsFromCandidateEdges) {
  yake::Config config;
  config.max_ngram_size = 3;
  config.max_keywords   = 20;
  const yake::KeywordExtractor extractor{config};

  const auto keywords{extractor.extract("The theory of graphs explains the structure of networks.")};
  const auto contains{[&keywords](const std::string& text) {
    return std::any_of(keywords.begin(), keywords.end(), [&text](const auto& keyword) { return keyword.text == text; });
  }};

  EXPECT_FALSE(contains("The"));
  EXPECT_FALSE(contains("the structure"));
  EXPECT_TRUE(contains("theory"));
  EXPECT_TRUE(contains("theory of graphs"));
}

TEST(KeywordExtractor, RespectsZeroKeywordLimit) {
  yake::Config config;
  config.max_keywords = 0;
  const yake::KeywordExtractor extractor{config};

  EXPECT_TRUE(extractor.extract("graph algorithms").empty());
}

TEST(KeywordExtractor, AcceptsDeduplicationPolicy) {
  yake::Config config;
  config.max_keywords = 3;
  const yake::BasicKeywordExtractor<AlwaysDuplicatePolicy> extractor{config};

  EXPECT_EQ(extractor.extract("Graph graphs graph algorithms.").size(), 1U);
}
