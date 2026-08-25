# yake-cpp

`yake-cpp`는 단일 문서에서 키워드를 추출하는 YAKE의 C++17 구현입니다. 현재 영어(`en`, `english`)를
지원합니다.

점수가 낮을수록 더 중요한 키워드입니다.

## 요구 사항

- CMake 3.16 이상
- C++17을 지원하는 컴파일러

## 빌드

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

최상위 프로젝트로 빌드할 때 테스트와 예제는 기본으로 활성화됩니다. 다른 프로젝트에서 하위 디렉터리로 포함하면
기본으로 비활성화됩니다.

| 옵션 | 기본값 | 설명 |
| --- | --- | --- |
| `YAKE_BUILD_TESTS` | 최상위 빌드에서 `ON` | 단위 테스트 빌드 |
| `YAKE_BUILD_EXAMPLES` | 최상위 빌드에서 `ON` | 예제 빌드 |
| `YAKE_BUILD_BENCHMARKS` | `OFF` | 벤치마크 빌드 |
| `YAKE_ENABLE_WARNINGS` | `ON` | 컴파일러 경고 활성화 |
| `YAKE_WARNINGS_AS_ERRORS` | `OFF` | 컴파일러 경고를 오류로 처리 |
| `YAKE_ENABLE_SANITIZERS` | `OFF` | GCC와 Clang에서 ASan·UBSan 활성화 |

clang-format을 찾으면 `yake_format`과 `yake_format_check` target도 생성합니다.

벤치마크는 테스트에 포함되지 않으며 별도로 빌드하고 실행합니다.

```sh
cmake -S . -B build -DYAKE_BUILD_BENCHMARKS=ON
cmake --build build --target yake_benchmark
./build/benchmarks/yake_benchmark
```

```sh
./build/examples/yake_example
./build/examples/yake_example "YAKE extracts keywords from a single document."
```

## 사용법

```cpp
#include "yake/keyword_extractor.h"

#include <iostream>

int main() {
  yake::Config config{};
  config.max_ngram_size = 3;
  config.max_keywords   = 10;

  const yake::KeywordExtractor extractor{config};
  const auto keywords{extractor.extract("YAKE extracts relevant keywords from a single document.")};

  for (const yake::Keyword& keyword : keywords)
    std::cout << keyword.text << '\t' << keyword.score << '\n';
}
```

### 설정

| 멤버 | 기본값 | 제약 조건 |
| --- | --- | --- |
| `language` | `"en"` | 현재 `"en"` 또는 `"english"` |
| `max_ngram_size` | `3` | 0보다 커야 함 |
| `deduplication_threshold` | `0.9` | `0.0` 이상 `1.0` 이하 |
| `context_window_size` | `1` | 0보다 커야 함 |
| `max_keywords` | `20` | 0이면 키워드를 반환하지 않음 |

### 중복 제거 Policy

`KeywordExtractor`는 공식 YAKE의 기본값인 `SequenceMatcherPolicy`를 사용합니다. 다른 방식은 template 인자로
선택할 수 있습니다.

```cpp
using Extractor = yake::BasicKeywordExtractor<yake::LevenshteinPolicy>;

const Extractor extractor{};
```

기본 제공 Policy는 `LevenshteinPolicy`, `JaroWinklerPolicy`, `SequenceMatcherPolicy`입니다. 사용자 정의 Policy는
다음 형태의 `similarity` 멤버 함수를 제공해야 합니다.

```cpp
struct CustomPolicy {
  [[nodiscard]] double similarity(std::string_view lhs, std::string_view rhs) const;
};
```

## 설치와 CMake 연동

```sh
cmake -S . -B build \
  -DYAKE_BUILD_TESTS=OFF \
  -DYAKE_BUILD_EXAMPLES=OFF \
  -DCMAKE_INSTALL_PREFIX=/path/to/prefix
cmake --build build
cmake --install build
```

설치한 패키지는 다음과 같이 사용할 수 있습니다.

```cmake
find_package(yake CONFIG REQUIRED)

target_link_libraries(your_target PRIVATE yake::yake)
```

표준 CMake 검색 경로가 아닌 곳에 설치했다면 설치 경로를 `CMAKE_PREFIX_PATH`로 지정해야 합니다.
