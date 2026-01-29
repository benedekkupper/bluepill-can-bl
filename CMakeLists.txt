cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(bluepill-can-bl
    VERSION 1.0
    LANGUAGES C CXX ASM
)

add_executable(${PROJECT_NAME})

add_subdirectory(bluepill-can-bl)

target_sources(${PROJECT_NAME} PRIVATE
    bluepill-can-bl/arm/startup_cm3.s
    bluepill-can-bl/boot.cpp
    bluepill-can-bl/main.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${PROJECT_NAME}
)

include(cmake/get_cpm.cmake)
if(ZEPHYR_BITFILLED_MODULE_DIR)
    include(FetchContent)
    FetchContent_Declare(
        bitfilled
        SOURCE_DIR "${ZEPHYR_BITFILLED_MODULE_DIR}"
    )
    FetchContent_MakeAvailable(bitfilled)
else()
    CPMAddPackage("gh:IntergatedCircuits/bitfilled#a4aa48e0fa7568880a297825c9ce4bf691f641eb")
endif()

if(ZEPHYR_STM32HEADER_MODULE_DIR)
    target_include_directories(${PROJECT_NAME} PRIVATE ${ZEPHYR_STM32HEADER_MODULE_DIR})
else()
    CPMAddPackage("gh:IntergatedCircuits/stm32header#6c5151ac54ea0f61d9bef646a70209bd1cc5d5dc")
    target_include_directories(${PROJECT_NAME} PRIVATE ${CPM_PACKAGE_stm32header_SOURCE_DIR})
endif()

if (ZEPHYR_CMSIS_6_MODULE_DIR)
    target_include_directories(${PROJECT_NAME} PRIVATE ${ZEPHYR_CMSIS_6_MODULE_DIR}/CMSIS/Core/Include)
else()
    CPMAddPackage("gh:STMicroelectronics/cmsis-core@5.9.0")
    target_include_directories(${PROJECT_NAME} PRIVATE ${CPM_PACKAGE_cmsis-core_SOURCE_DIR}/Include)
endif()

if (ZEPHYR_HAL_STM32_MODULE_DIR)
    target_include_directories(${PROJECT_NAME} PRIVATE ${ZEPHYR_HAL_STM32_MODULE_DIR}/stm32cube/stm32f1xx/soc)
else()
    CPMAddPackage("gh:STMicroelectronics/cmsis-device-f1#c8e9a4a4f16b6d2cb2a2083cbe5161025280fb22")
    target_include_directories(${PROJECT_NAME} PRIVATE ${CPM_PACKAGE_cmsis-device-f1_SOURCE_DIR}/Include)
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE
	STM32F103xB
    $<$<CONFIG:Debug>:DEBUG>
    BITFILLED_BB_OPS=::bitfilled::bitband<PERIPH_BASE>
    CONFIG_CAN_BITRATE=${CONFIG_CAN_BITRATE}
    CONFIG_HSE_CLOCK_HZ=${CONFIG_HSE_CLOCK_HZ}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${TOOLCHAIN_LINK_LIBRARIES}
    bitfilled
    can_bl_interface
    -T "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/stm32f1/linker.ld"
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${PROJECT_NAME}> ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}> ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.bin
    COMMENT "Generating ${PROJECT_NAME}.bin from ELF"
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    ADDITIONAL_CLEAN_FILES "${PROJECT_NAME}.map;${PROJECT_NAME}.bin;${PROJECT_NAME}.hex"
)
