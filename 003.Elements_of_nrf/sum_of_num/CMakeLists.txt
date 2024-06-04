cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(sum_of_num)

target_sources(app PRIVATE src/main.c)
target_sources(app PRIVATE src/myfunction.c)
