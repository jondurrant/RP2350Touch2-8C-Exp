#ifndef __BSP_I2C_H__
#define __BSP_I2C_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define BSP_I2C_NUM        i2c1
#define BSP_I2C_SDA_PIN    6
#define BSP_I2C_SCL_PIN    7

void bsp_i2c_write(uint8_t device_addr, uint8_t *buffer, size_t len);
void bsp_i2c_write_reg8(uint8_t device_addr, uint8_t reg_addr, uint8_t *buffer, size_t len);
void bsp_i2c_read_reg8(uint8_t device_addr, uint8_t reg_addr, uint8_t *buffer, size_t len);

void bsp_i2c_write_reg16(uint8_t device_addr, uint16_t reg_addr, uint8_t *buffer, size_t len);
void bsp_i2c_read_reg16(uint8_t device_addr, uint16_t reg_addr, uint8_t *buffer, size_t len);

void bsp_i2c_init(void);


#endif

