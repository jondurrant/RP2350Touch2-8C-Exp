
if (NOT DEFINED LVGL_DIR)
    set(LVGL_DIR "${CMAKE_CURRENT_LIST_DIR}/lib/lvgl")
endif()
if (NOT DEFINED LV_CONF_DIR)
    set(LV_CONF_DIR "${CMAKE_CURRENT_LIST_DIR}/port/lvgl/")
endif()


file(GLOB_RECURSE SOURCES ${LVGL_DIR}/src/*.c)
# file(GLOB_RECURSE EXAMPLE_SOURCES ${CMAKE_CURRENT_LIST_DIR}/examples/*.c)

file(GLOB_RECURSE PORT_SRC ${LV_CONF_DIR}/*.c)



add_library(lvgl STATIC)

target_sources(lvgl PUBLIC
	${SOURCES}
	${PORT_SRC}
	)


# Add include directory
target_include_directories(lvgl PUBLIC 
   ${LVGL_DIR}/
   ${LV_CONF_DIR}
)

# Add the standard library to the build
target_link_libraries(lvgl PUBLIC 
	pico_stdlib
	bsp
	)


#set(LV_CONF_H "${LV_CONF_DIR}/lv_conf.h")
target_compile_definitions(lvgl PUBLIC
#	LV_CONF_PATH=${LV_CONF_H}
	LV_CONF_INCLUDE_SIMPLE=1
)
