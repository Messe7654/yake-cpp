# yake-cpp

- YAKE 단일 문서 키워드 추출 알고리즘의 C++17 구현
- 지원 언어: 영어 (`en`, `english`)
- 비공식 구현: YAKE 연구진 및 INESC TEC와 무관

## 빌드

- 요구 사항
  - CMake 3.16 이상
  - C++17 지원 컴파일러
- 빌드 및 테스트

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

## 사용법

- 헤더 포함 및 키워드 추출

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

추가 사용 예시는 [`examples/README.md`](examples/README.md)에서 확인할 수 있습니다.

### CMake 연동

- 소스 트리를 서브디렉터리로 포함

```cmake
add_subdirectory(path/to/yake-cpp)
target_link_libraries(your_target PRIVATE yake::yake)
```

- 설치한 패키지를 검색하여 연결

```cmake
find_package(yake CONFIG REQUIRED)
target_link_libraries(your_target PRIVATE yake::yake)
```

## 인용

- 기반 논문

> Ricardo Campos, Vítor Mangaravite, Arian Pasquali, Alípio Jorge, Célia Nunes, and Adam Jatowt.
> “YAKE! Keyword extraction from single documents using multiple local features.” *Information Sciences*,
> 509:257–289, 2020. https://doi.org/10.1016/j.ins.2019.09.013

## 라이선스

- 프로젝트 라이선스: [`AGPL-3.0-or-later`](LICENSE)
- YAKE 원 저작물 및 제3자 구성 요소 고지
  - [`NOTICE`](NOTICE)
  - [`THIRD_PARTY_NOTICES.md`](THIRD_PARTY_NOTICES.md)
