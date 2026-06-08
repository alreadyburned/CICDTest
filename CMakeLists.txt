cmake_minimum_required(VERSION 3.20)
project(cicd_cpp_sample LANGUAGES CXX)

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

include(CTest)
if(BUILD_TESTING)
    include(FetchContent)

    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )

    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    include(GoogleTest)

    add_executable(unit_tests
        tests/unit/pricing_service_test.cpp
        tests/unit/in_memory_inventory_repository_test.cpp
    )
    target_link_libraries(unit_tests PRIVATE cicd_core GTest::gtest_main)
    set_target_properties(unit_tests PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
    gtest_discover_tests(unit_tests)

    add_executable(integration_tests
        tests/integration/checkout_integration_test.cpp
    )
    target_link_libraries(integration_tests PRIVATE cicd_core GTest::gtest_main)
    set_target_properties(integration_tests PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
    gtest_discover_tests(integration_tests)
endif()
