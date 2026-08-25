#ifndef YAKE_CPP_INCLUDE_YAKE_KEYWORD_H_
#define YAKE_CPP_INCLUDE_YAKE_KEYWORD_H_

#include <string>

namespace yake {

struct Keyword {
  std::string text = {};
  double score     = 0.0;
};

}  // namespace yake

#endif  // YAKE_CPP_INCLUDE_YAKE_KEYWORD_H_
