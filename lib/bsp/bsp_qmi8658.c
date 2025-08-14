#include "bsp_qmi8658.h"
#include "bsp_i2c.h"

// 读取QMI8658寄存器的值
static void bsp_qmi8658_reg_read(uint8_t reg_addr, uint8_t *data, size_t len)
{
    bsp_i2c_read_reg8(QMI8658_DEVICE_ADDR, reg_addr, data, len);
}

// 给QMI8658的寄存器写值
static void bsp_qmi8658_reg_write_byte(uint8_t reg_addr, uint8_t *data, size_t len)
{
    bsp_i2c_write_reg8(QMI8658_DEVICE_ADDR, reg_addr, data, len);
}

bool bsp_qmi8658_read_data(qmi8658_data_t *data)
{
    uint8_t status;
    float mask;
    uint16_t buf[6];
    bsp_qmi8658_reg_read(QMI8658_STATUS0, &status, 1); // 读状态寄存器
    if (status & 0x03)
    {
        bsp_qmi8658_reg_read(QMI8658_AX_L, (uint8_t *)buf, 12); // 读加速度和陀螺仪值
        data->acc_x = buf[0];
        data->acc_y = buf[1];
        data->acc_z = buf[2];
        data->gyr_x = buf[3];
        data->gyr_y = buf[4];
        data->gyr_z = buf[5];

        mask = (float)data->acc_x / sqrt(((float)data->acc_y * (float)data->acc_y + (float)data->acc_z * (float)data->acc_z));
        data->AngleX = atan(mask) * 57.29578f; // 180/π=57.29578
        mask = (float)data->acc_y / sqrt(((float)data->acc_x * (float)data->acc_x + (float)data->acc_z * (float)data->acc_z));
        data->AngleY = atan(mask) * 57.29578f; // 180/π=57.29578
        mask = sqrt(((float)data->acc_x * (float)data->acc_x + (float)data->acc_y * (float)data->acc_y)) / (float)data->acc_z;
        data->AngleZ = atan(mask) * 57.29578f; // 180/π=57.29578
        return true;
    }
    else
        printf("QMI8658 read data fail!\n");
    return false;
}

void bsp_qmi8658_init(void)
{
    uint8_t id = 0;

    bsp_qmi8658_reg_read(QMI8658_WHO_AM_I, &id, 1);
    if (0x05 != id)
    {
        printf("QMI8658 not found!\r\n");
        return;
    }
    printf("Find QMI8658!\r\n");
    bsp_qmi8658_reg_write_byte(QMI8658_RESET, (uint8_t[]){0xb0}, 1); // 复位
    sleep_ms(50);                                                    // 延时50ms
    bsp_qmi8658_reg_write_byte(QMI8658_CTRL1, (uint8_t[]){0x40}, 1); // CTRL1 设置地址自动增加
    bsp_qmi8658_reg_write_byte(QMI8658_CTRL7, (uint8_t[]){0x03}, 1); // CTRL7 允许加速度和陀螺仪
    bsp_qmi8658_reg_write_byte(QMI8658_CTRL2, (uint8_t[]){0x95}, 1); // CTRL2 设置ACC 4g 250Hz
    bsp_qmi8658_reg_write_byte(QMI8658_CTRL3, (uint8_t[]){0xd5}, 1); // CTRL3 设置GRY 512dps 250Hz
    sleep_ms(100); 
}

// static void bsp_qmi8658_task(void *arg)
// {
//     qmi8658_data_t data;
//     while (1)
//     {
//         bsp_qmi8658_read_data(&data);
//         printf("acc: %5d %5d %5d , gyr:%5d %5d %5d\r\n", data.acc_x, data.acc_y, data.acc_z, data.gyr_x, data.gyr_y, data.gyr_z);
//         vTaskDelay(pdMS_TO_TICKS(100));
//     }
// }

// void bsp_qmi8658_test(void)
// {
//     xTaskCreate(bsp_qmi8658_task, "bsp_qmi8658_task", 256, NULL, tskIDLE_PRIORITY + 1, NULL);
// }