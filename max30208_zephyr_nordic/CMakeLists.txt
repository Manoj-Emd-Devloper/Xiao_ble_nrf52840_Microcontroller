cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(temp_max_nordic)

target_sources(app PRIVATE src/main.c)

zephyr_include_directories(driver)
target_sources(app PRIVATE driver/max30208.c)
