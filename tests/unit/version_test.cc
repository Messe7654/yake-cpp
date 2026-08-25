#include "yake/version.h"

#include <gtest/gtest.h>

TEST(Version, ReturnsNonEmptyString) { EXPECT_FALSE(yake::version().empty()); }
