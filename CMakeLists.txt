cmake_minimum_required(VERSION 3.20)
project(cicd_cpp_sample LANGUAGES CXX)

# CMP0135 경고 해결을 위한 정책 설정 추가
if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(cicd_core
    src/pricing_service.cpp
    src/in_memory_inventory_repository.cpp
    src/checkout_service.cpp
)

target_include_directories(cicd_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_executable(sample_app src/main.cpp)
target_link_libraries(sample_app PRIVATE cicd_core)

# CTest를 켜서 BUILD_TESTING 옵션(기본 ON)과 테스트 등록 기능을 활성화한다.
include(CTest)
if(BUILD_TESTING)
    # GoogleTest를 패키지 매니저 없이 CMake 단계에서 내려받기 위해 FetchContent를 사용한다.
    include(FetchContent)

    # 고정 버전(v1.14.0)으로 GoogleTest 소스를 가져와 재현 가능한 빌드를 만든다.
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )

    # Windows/MSVC 런타임 충돌을 피하기 위한 권장 옵션(특히 gtest와 앱 런타임 일치).
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    # 선언된 googletest 의존성을 실제로 다운로드/추가하고 GTest::gtest_main 타깃을 제공한다.
    FetchContent_MakeAvailable(googletest)

    # gtest_discover_tests() 함수를 사용하기 위해 GoogleTest CMake 모듈을 로드한다.
    include(GoogleTest)

    # 단위 테스트 실행 파일을 만들고, 앱 코드(cicd_core) + GoogleTest 메인에 링크한다.
    add_executable(unit_tests
        tests/unit/pricing_service_test.cpp
        tests/unit/in_memory_inventory_repository_test.cpp
    )
    target_link_libraries(unit_tests PRIVATE cicd_core GTest::gtest_main)
    # CI/Jenkins에서 경로를 고정하기 쉽도록 테스트 바이너리 출력 폴더를 통일한다.
    set_target_properties(unit_tests PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
    # 실행 시점에 테스트 목록을 자동 수집해 CTest에 등록한다.
    gtest_discover_tests(unit_tests)

    # 통합 테스트도 동일 패턴으로 구성한다.
    add_executable(integration_tests
        tests/integration/checkout_integration_test.cpp
    )
    target_link_libraries(integration_tests PRIVATE cicd_core GTest::gtest_main)
    set_target_properties(integration_tests PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
    gtest_discover_tests(integration_tests)
endif()
