#include "detail/normalizer.h"

#include <gtest/gtest.h>

TEST(Normalizer, AppliesUnicodeCaseFolding) {
  EXPECT_TRUE(yake::detail::normalize_token("").empty());
  EXPECT_EQ(yake::detail::normalize_token("YAKE"), "yake");
  EXPECT_EQ(yake::detail::normalize_token("CAF\xC3\x89"), "caf\xC3\xA9");
  EXPECT_EQ(yake::detail::normalize_token("Stra\xC3\x9F"
                                          "e"),
            "strasse");
}

TEST(Normalizer, AppliesCompatibilityComposition) {
  EXPECT_EQ(yake::detail::normalize_token("\xEF\xBC\xA1\xEF\xBC\xA2\xEF\xBC\xA3"), "abc");

  const std::string nfc{"\xED\x95\x9C"};
  const std::string nfd{"\xE1\x84\x92\xE1\x85\xA1\xE1\x86\xAB"};
  EXPECT_EQ(yake::detail::normalize_token(nfc), yake::detail::normalize_token(nfd));
}

TEST(Normalizer, CanonicalizesUnicodeWordConnectors) {
  EXPECT_EQ(yake::detail::normalize_token("DON\xE2\x80\x99T"), "don't");
  EXPECT_EQ(yake::detail::normalize_token("state\xE2\x80\x91of\xE2\x80\x91the\xE2\x80\x91"
                                          "art"),
            "state-of-the-art");
}

TEST(Normalizer, RejectsInvalidUtf8) {
  EXPECT_THROW((void)yake::detail::normalize_token("\xC3"), std::invalid_argument);
}
