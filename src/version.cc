#include "yake/version.h"

namespace yake {

std::string version() {
  return std::to_string(kVersionMajor) + "." + std::to_string(kVersionMinor) + "." + std::to_string(kVersionPatch);
}

}  // namespace yake
