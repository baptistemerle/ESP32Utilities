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
    "src/Displays/GC9A01_driver.cpp"
    "src/Displays/ST7701_driver.cpp"
    "src/Displays/ST77916_driver.cpp"

    "src/Expanders/TCA9554_driver.cpp"

    "src/Graphics/display_engine.cpp"

    "src/Sensors/QMI8658_driver.cpp"

  INCLUDE_DIRS
    "src"

  REQUIRES lvgl esp_lcd_gc9a01 esp_lcd_st77916 nvs_flash cjson
)

target_compile_options(${COMPONENT_LIB} PRIVATE ${COMPILER_FLAGS})