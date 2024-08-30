# == DO NEVER EDIT THE NEXT LINES for Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.0.0)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
include(${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
# ====================================================================================
set(PICO_BOARD pico2 CACHE STRING "Board type")

cmake_minimum_required(VERSION 3.13)
set(PICO_EXTRAS_PATH "E:/pico-extras")
include(pico_sdk_import.cmake)
include(pico_extras_import.cmake)
project(pico2dexed)
pico_sdk_init()
add_executable(pico2dexed)

target_sources(pico2dexed PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/pico_perf.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mididevice.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/picodexed.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/serialmidi.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/sounddevice.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/usbmidi.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/usbtask.c
    ${CMAKE_CURRENT_LIST_DIR}/src/usb_descriptors.c
)

add_subdirectory(synth_dexed)
add_subdirectory("miditones")

# Make sure TinyUSB can find tusb_config.h
target_include_directories(pico2dexed PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src
    ${CMAKE_CURRENT_LIST_DIR}/synth_dexed/Synth_Dexed/src
    ${CMAKE_CURRENT_LIST_DIR}/synth_dexed
)

# In addition to pico_stdlib required for common PicoSDK functionality, add dependency on tinyusb_device
# for TinyUSB device support and tinyusb_board for the additional board support library used by the example
target_link_libraries(pico2dexed PUBLIC synth_dexed
    pico_stdlib
    pico_multicore
    tinyusb_device
    tinyusb_board
    pico_audio_i2s
    pico_audio_pwm
    miditones
 )

target_compile_definitions(pico2dexed PRIVATE
    PICO_AUDIO_I2S_MONO_OUTPUT=1
    PICO_AUDIO_I2S_MONO_INPUT=1
	USE_AUDIO_I2S=1
    USE_AUDIO_PWM=1
)

# Uncomment this line to enable fix for Errata RP2040-E5 (the fix requires use of GPIO 15)
#target_compile_definitions(picodexed PUBLIC PICO_RP2040_USB_DEVICE_ENUMERATION_FIX=1)

pico_add_extra_outputs(pico2dexed)
