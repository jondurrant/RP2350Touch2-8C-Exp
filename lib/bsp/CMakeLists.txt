# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_Config_SRCS 变量
aux_source_directory(. DIR_BSP_SRCS)

# 生成链接库
add_library(bsp ${DIR_BSP_SRCS})


# Add the standard include files to the build
target_include_directories(bsp PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}
)
# pico_generate_pio_header(bsp ${CMAKE_CURRENT_LIST_DIR}/bsp_i2s.pio)
# pico_generate_pio_header(bsp ${CMAKE_CURRENT_LIST_DIR}/st7701s_rgb.pio)
pico_generate_pio_header(bsp ${CMAKE_CURRENT_LIST_DIR}/pio_rgb.pio)

target_link_libraries(bsp PUBLIC 
    pico_stdlib 
    hardware_spi 
    hardware_i2c 
    hardware_pwm 
    hardware_adc 
    hardware_dma
    hardware_pio
    hardware_irq
    no-OS-FatFS-SD-SDIO-SPI-RPi-Pico)

