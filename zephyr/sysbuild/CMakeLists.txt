# https://docs.zephyrproject.org/latest/build/sysbuild/images.html#setting-image-configuration
function(${SYSBUILD_CURRENT_MODULE_NAME}_pre_cmake)
    cmake_parse_arguments(PRE_CMAKE "" "" "IMAGES" ${ARGN})
    if(SB_CONFIG_BOOTLOADER_BLUEPILL_CAN)
        foreach(image ${PRE_CMAKE_IMAGES})
            set_config_bool(${image} CONFIG_BUILD_OUTPUT_BIN y)
            set_config_bool(${image} CONFIG_BOOTLOADER_BLUEPILL_CAN y)

            set(image_default_dtc_overlay "${ZEPHYR_BLUEPILL_CAN_BL_MODULE_DIR}/zephyr/bluepill_partitions.overlay")
            if(NOT ${image_default_dtc_overlay} IN_LIST ${image}_EXTRA_DTC_OVERLAY_FILE)
                list(APPEND ${image}_EXTRA_DTC_OVERLAY_FILE ${image_default_dtc_overlay})
                set(${image}_EXTRA_DTC_OVERLAY_FILE
                    ${${image}_EXTRA_DTC_OVERLAY_FILE}
                    CACHE INTERNAL "Application extra DTC overlay file" FORCE
                )
            endif()
            message(STATUS "Configured Blue-pill CAN Bootloader settings for image: ${image}")
        endforeach()
    endif()
endfunction()

if(SB_CONFIG_BOOTLOADER_BLUEPILL_CAN)
    include(ExternalProject)
    ExternalProject_Add(
        bluepill-can-bl
        SOURCE_DIR "${SYSBUILD_CURRENT_MODULE_DIR}"
        BINARY_DIR ${CMAKE_BINARY_DIR}/bootloader
        CONFIGURE_COMMAND ${CMAKE_COMMAND} -S <SOURCE_DIR> -B <BINARY_DIR>
            --preset=default
            -DCONFIG_CAN_BITRATE=${SB_CONFIG_BLUEPILL_CAN_BUS_BITRATE}
            -DCONFIG_HSE_CLOCK_HZ=${SB_CONFIG_BLUEPILL_HSE_CLOCK_HZ}
            -DZEPHYR_BITFILLED_MODULE_DIR=${ZEPHYR_BITFILLED_MODULE_DIR}
            -DZEPHYR_STM32HEADER_MODULE_DIR=${ZEPHYR_STM32HEADER_MODULE_DIR}
            -DZEPHYR_CMSIS_6_MODULE_DIR=${ZEPHYR_CMSIS_6_MODULE_DIR}
            -DZEPHYR_HAL_STM32_MODULE_DIR=${ZEPHYR_HAL_STM32_MODULE_DIR}
        BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR>
        INSTALL_COMMAND ""
        BUILD_BYPRODUCTS
            <BINARY_DIR>/bluepill-can-bl.elf
            <BINARY_DIR>/bluepill-can-bl.bin
        BUILD_ALWAYS TRUE
    )

    add_dependencies(${DEFAULT_IMAGE} bluepill-can-bl)
    #sysbuild_add_dependencies(FLASH ${DEFAULT_IMAGE} bluepill-can-bl)

endif()
