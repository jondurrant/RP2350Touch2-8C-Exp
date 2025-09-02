if (DEFINED SENSOR_PATH)
	message("Using Given SENSOR_PATH '${SENSOR_PATH}')")
else ()
	set(SENSOR_PATH "${CMAKE_CURRENT_LIST_DIR}lib/sensors_PICO/")
    message("Using local SENSOR_PATH '${SENSOR_PATH}')")
endif ()

add_library(sensors STATIC)
target_sources(sensors PUBLIC
  ${SENSOR_PATH}/src/ahtxx/ahtxx.cpp 
  ${SENSOR_PATH}/src/bmp280/bmp280.cpp 
  ${SENSOR_PATH}/src/lm35/lm35.cpp
  ${SENSOR_PATH}/src/lm75/lm75.cpp 
)


# Add include directory
target_include_directories(sensors PUBLIC 
    ${SENSOR_PATH}/include
)


target_link_libraries(sensors PUBLIC 
    pico_stdlib 
    hardware_i2c 
    hardware_adc 
    hardware_spi 
    )
