# SDK
add_library(stars_lib HistoryReader.cpp)
target_include_directories(stars_lib PUBLIC ${CMAKE_SOURCE_DIR}/src)

# Executable
add_executable(stars main.cpp)
target_link_libraries(stars PRIVATE stars_lib)
