cmake_minimum_required(VERSION 3.18)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(state-logger C CXX ASM)
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES ./src/main.c)
set(PIO_GPIO_HANDLER  ${CMAKE_CURRENT_LIST_DIR}/src/gpio_handler.pio)

pico_sdk_init()

add_executable(${PROJECT_NAME})

pico_generate_pio_header(${PROJECT_NAME} ${PIO_GPIO_HANDLER})
target_sources(${PROJECT_NAME} PRIVATE ${SOURCE_FILES})

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

# create map/bin/hex/uf2 files.
pico_add_extra_outputs(${PROJECT_NAME})
# Pull in our pico_stdlib which pulls in commonly used features (gpio, timer-delay etc)
target_link_libraries(${PROJECT_NAME} 
            pico_stdlib
            hardware_pio
)

pico_add_extra_outputs(${PROJECT_NAME})
