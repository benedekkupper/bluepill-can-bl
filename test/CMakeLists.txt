cmake_minimum_required(VERSION 3.22)

project(can_bl_interface_host_tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

if(NOT DEFINED BLUEPILL_CAN_BL_SOURCE_DIR)
    message(FATAL_ERROR "BLUEPILL_CAN_BL_SOURCE_DIR must point to the repository root")
endif()

include(CTest)
include(${BLUEPILL_CAN_BL_SOURCE_DIR}/cmake/get_cpm.cmake)

if(BITFILLED_SOURCE_DIR)
    include(FetchContent)
    FetchContent_Declare(
        bitfilled
        SOURCE_DIR "${BITFILLED_SOURCE_DIR}"
    )
    FetchContent_MakeAvailable(bitfilled)
else()
    CPMAddPackage("gh:IntergatedCircuits/bitfilled#a4aa48e0fa7568880a297825c9ce4bf691f641eb")
endif()

CPMAddPackage(
    NAME ut
    GITHUB_REPOSITORY boost-ext/ut
    VERSION 2.3.1
    OPTIONS
        "BOOST_UT_BUILD_BENCHMARKS OFF"
        "BOOST_UT_BUILD_EXAMPLES OFF"
        "BOOST_UT_BUILD_TESTS OFF"
        "BOOST_UT_ENABLE_INSTALL OFF"
)

add_subdirectory(${BLUEPILL_CAN_BL_SOURCE_DIR}/bluepill-can-bl ${CMAKE_CURRENT_BINARY_DIR}/bluepill-can-bl)

add_executable(can_bl_interface_tests can_bl_interface.test.cpp)
target_link_libraries(can_bl_interface_tests PRIVATE
    Boost::ut
    can_bl_interface
)

add_test(NAME can_bl_interface_tests COMMAND can_bl_interface_tests)
