#ifndef YAKE_CPP_INCLUDE_YAKE_VERSION_H_
#define YAKE_CPP_INCLUDE_YAKE_VERSION_H_

#include <string>

namespace yake {

constexpr int kVersionMajor{0};
constexpr int kVersionMinor{1};
constexpr int kVersionPatch{0};

std::string version();

}  // namespace yake

#endif  // YAKE_CPP_INCLUDE_YAKE_VERSION_H_
