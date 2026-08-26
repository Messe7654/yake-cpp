# Examples

`yake-cpp`의 주요 사용 예제와 예상 결과를 제공합니다.

| 예제 | 의도 | 소스 |
| --- | --- | --- |
| 기본 키워드 추출 | 기본 문서 또는 명령줄로 전달한 문서에서 키워드 추출 | [`keyword_extraction.cc`](keyword_extraction.cc) |
| 설정 변경 | 최대 n-gram 크기, 키워드 개수, 문맥 범위를 지정하는 방법 | [`custom_configuration.cc`](custom_configuration.cc) |
| 중복 제거 정책 | `BasicKeywordExtractor<Policy>`에 정책을 적용하고 결과를 비교하는 방법 | [`deduplication_policies.cc`](deduplication_policies.cc) |

루트 디렉터리에서 다음과 같이 빌드할 수 있습니다.

```sh
cmake -S . -B build -DYAKE_BUILD_EXAMPLES=ON
cmake --build build
```
