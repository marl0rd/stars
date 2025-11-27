file(GLOB STAR_SRC
  main.cpp
)

add_executable(stars ${STAR_SRC})
target_include_directories(stars PRIVATE ${CMAKE_SOURCE_DIR}/include)
