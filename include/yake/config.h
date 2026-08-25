#ifndef YAKE_CPP_INCLUDE_YAKE_CONFIG_H_
#define YAKE_CPP_INCLUDE_YAKE_CONFIG_H_

#include <cstddef>
#include <string>

namespace yake {

struct Config {
  std::string language            = "en";
  std::size_t max_ngram_size      = 3;
  double deduplication_threshold  = 0.9;
  std::size_t context_window_size = 1;
  std::size_t max_keywords        = 20;
};

}  // namespace yake

#endif  // YAKE_CPP_INCLUDE_YAKE_CONFIG_H_
