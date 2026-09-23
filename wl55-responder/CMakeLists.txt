cmake_minimum_required(VERSION 3.20.0)

set(BOARD nucleo_wl55jc)
set(BOARD_FLASH_RUNNER openocd)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(wl55_responder)
target_sources(app PRIVATE src/main.c)