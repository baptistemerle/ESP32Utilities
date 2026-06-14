cmake_minimum_required(VERSION 3.16)

set(COMPILER_FLAGS
  -Wall
  -Wextra
  -Wshadow
  -Werror=return-type
  -Wdouble-promotion
)

idf_component_register(
  SRC_DIRS "src/Displays"
           "src/Graphics"
           "src/Sensors"
  INCLUDE_DIRS "src"
  REQUIRES lvgl lovyangfx
)

target_compile_options(${COMPONENT_LIB} PRIVATE ${COMPILER_FLAGS})