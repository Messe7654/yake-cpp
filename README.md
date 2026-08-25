# yake-cpp

`yake-cpp`는 단일 문서에서 키워드를 추출하는 YAKE 알고리즘의 C++17 구현입니다. 현재 영어
(`en`, `english`)를 지원합니다.

이 프로젝트는 YAKE 연구진이나 INESC TEC가 관리하는 공식 구현이 아닙니다.

## 빌드

요구 사항은 CMake 3.16 이상과 C++17 컴파일러입니다.

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

## 사용법

```cpp
#include "yake/keyword_extractor.h"

#include <iostream>

int main() {
  yake::Config config{};
  config.max_keywords = 10;

  const yake::KeywordExtractor extractor{config};
  const auto keywords{
      extractor.extract("YAKE extracts relevant keywords from a single document.")};

  for (const yake::Keyword& keyword : keywords)
    std::cout << keyword.text << '\t' << keyword.score << '\n';
}
```

설치한 패키지는 CMake에서 다음과 같이 연결할 수 있습니다.

```cmake
find_package(yake CONFIG REQUIRED)
target_link_libraries(your_target PRIVATE yake::yake)
```

## 인용

이 구현의 기반이 된 YAKE 논문은 다음과 같습니다.

> Ricardo Campos, Vítor Mangaravite, Arian Pasquali, Alípio Jorge, Célia Nunes, and Adam Jatowt.
> “YAKE! Keyword extraction from single documents using multiple local features.” *Information Sciences*,
> 509:257–289, 2020. https://doi.org/10.1016/j.ins.2019.09.013

## 라이선스

이 프로젝트는 [`AGPL-3.0-or-later`](LICENSE)로 배포됩니다.
YAKE 원 저작물과 제3자 구성 요소의 저작권 및 라이선스는 [`NOTICE`](NOTICE)와
[`THIRD_PARTY_NOTICES.md`](THIRD_PARTY_NOTICES.md)를 참고하세요.
