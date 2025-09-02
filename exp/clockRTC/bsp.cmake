if (DEFINED BSP_PATH)
	message("Using Given BSP_PATH '${BSP_PATH}')")
else ()
	set(BSP_PATH "${CMAKE_CURRENT_LIST_DIR}lib/bsp/")
    message("Using local BSP_PATH '${BSP_PATH}')")
endif ()

add_library(bsp STATIC)
target_sources(bsp PUBLIC
  ${BSP_PATH}/bsp_battery.c
  ${BSP_PATH}/bsp_buzzer.c
  ${BSP_PATH}/bsp_dma_channel_irq.c
  ${BSP_PATH}/bsp_gt911.c
  ${BSP_PATH}/bsp_i2c.c
  ${BSP_PATH}/bsp_pcf85063.c
  ${BSP_PATH}/bsp_qmi8658.c
  #${BSP_PATH}/bsp_sd_card.c
  ${BSP_PATH}/bsp_st7701s.c
  ${BSP_PATH}/pio_rgb.c
)

pico_generate_pio_header(bsp ${BSP_PATH}/pio_rgb.pio)

# Add include directory
target_include_directories(bsp PUBLIC 
    ${BSP_PATH}
)


target_link_libraries(bsp PUBLIC 
    pico_stdlib 
    hardware_spi 
    hardware_i2c 
    hardware_pwm 
    hardware_adc 
    hardware_dma
    hardware_pio
    hardware_irq
    )
    
    
target_compile_definitions(bsp PUBLIC
    PICO_EMBED_XIP_SETUP=1
)