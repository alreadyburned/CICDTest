# C++ CI/CD Practice Sample (Jenkins + GoogleTest)

이 샘플은 Jenkins 파이프라인에서 **빌드 -> 유닛 테스트 -> 통합 테스트**를 자동화하는 학습용 예제입니다.

## 구성

- `src/`: 서비스 코드
- `tests/unit/`: 단위 테스트
- `tests/integration/`: 통합 테스트
- `Jenkinsfile`: Jenkins 파이프라인

## 로컬 실행

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel
./build/bin/unit_tests
./build/bin/integration_tests
```

Windows PowerShell:

```powershell
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel
.\build\bin\unit_tests.exe
.\build\bin\integration_tests.exe
```

## 자동 테스트 생성기 친화 포인트

- 함수 입력/출력이 명확하고 부작용이 제한된 API
- 인터페이스(`InventoryRepository`) 기반 설계로 Mock 생성 용이
- 실패 경로(재고 부족, 잘못된 SKU, 잘못된 수량)와 성공 경로를 모두 포함
- 금액 계산은 정수(센트) 사용으로 결과 검증이 안정적
