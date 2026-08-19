cmake_minimum_required(VERSION 3.16)

set(COMPILER_FLAGS
  -Wall
  -Wextra
  -Wshadow
  -Werror=return-type
  -Wdouble-promotion
)

idf_component_register(
  SRCS
    "src/Displays/GC9A01/driver.cpp"
    "src/Displays/ST7701/driver.cpp"
    "src/Displays/ST77916/driver.cpp"

    "src/Expanders/TCA9554/driver.cpp"

    "src/Graphics/display_engine.cpp"

    "src/Sensors/QMI8658/driver.cpp"

  INCLUDE_DIRS
    "include"

  REQUIRES lvgl esp_lcd_gc9a01 esp_lcd_st77916 nvs_flash cjson
)

target_compile_options(${COMPONENT_LIB} PRIVATE ${COMPILER_FLAGS})