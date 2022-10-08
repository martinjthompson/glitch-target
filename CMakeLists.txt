cmake_minimum_required(VERSION 3.12)
# if (TARGET tinyusb_device)

    include (pico_sdk_import.cmake)

    project(glitch-target-prj)

    pico_sdk_init()

    add_executable(glitch_target 
        glitch_target.c
    )

    target_link_libraries(glitch_target pico_stdlib hardware_pwm)


    # pico_enable_stdio_usb(glitch_target 1)
    # pico_enable_stdio_uart(glitch_target 0)




    pico_add_extra_outputs(glitch_target)

# elseif(PICO_ON_DEVICE)
#     message(FATAL_ERROR  "not building glitch_target because TinyUSB submodule is not initialized in the SDK")
# endif()