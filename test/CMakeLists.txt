file(GLOB TEST_SRC
  *.cpp
)

add_executable(stars_tests ${TEST_SRC})
file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_BINARY_DIR})
target_link_libraries(stars_tests PUBLIC stars_lib GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(stars_tests)
