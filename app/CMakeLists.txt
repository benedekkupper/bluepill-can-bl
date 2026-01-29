cmake_minimum_required(VERSION 3.21)

# set this before including Zephyr
set_property(GLOBAL APPEND PROPERTY
    HEX_FILES_TO_MERGE
        "${CMAKE_BINARY_DIR}/../bootloader/bluepill-can-bl.hex"
        "${CMAKE_BINARY_DIR}/zephyr/zephyr.hex"
)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(bluepill-app
    # set VERSION in ./VERSION file
)

target_sources(app PRIVATE src/main.cpp)
target_link_libraries(app PRIVATE can_bl_interface)
