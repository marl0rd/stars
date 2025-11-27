file(GLOB TEST_SRC
  *.cpp
)

add_executable(stars_tests ${TEST_SRC})
target_link_libraries(stars_tests PRIVATE GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(stars_tests)
