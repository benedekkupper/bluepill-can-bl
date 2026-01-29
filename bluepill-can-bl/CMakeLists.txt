if(CONFIG_BOOTLOADER_BLUEPILL_CAN)
    zephyr_library_named(bluepill-can-bl)
    zephyr_library_sources(boot.cpp)
    target_link_libraries(bluepill-can-bl PRIVATE bitfilled)
    zephyr_library_compile_definitions(
        BITFILLED_BB_OPS=::bitfilled::bitband<PERIPH_BASE>
    )
endif()

add_library(can_bl_interface INTERFACE)
target_include_directories(can_bl_interface INTERFACE interface)
target_compile_definitions(can_bl_interface INTERFACE
    CONFIG_BLUEPILL_CAN_ID_PREFIX=0x7E5 # this and +1 IDs are used
    CONFIG_BLUEPILL_CAN_IDE=0
)
