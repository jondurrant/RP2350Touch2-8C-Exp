#ifndef __BSP_TOUCH_H__
#define __BSP_TOUCH_H__

typedef struct
{
    uint8_t points; // Number of touch points
    struct
    {
        uint16_t x;        /*!< X coordinate */
        uint16_t y;        /*!< Y coordinate */
        uint16_t pressure; /*!< pressure */
    } coords[5];
}bsp_touch_data_t;


typedef struct
{
    uint16_t width;
    uint16_t height;
    uint16_t rotation;
    bsp_touch_data_t data;
}bsp_touch_info_t;

typedef struct bsp_touch_interface_t bsp_touch_interface_t;

struct bsp_touch_interface_t {
    void (*init)(void);
    void (*reset)(void);
    void (*set_rotation)(uint16_t rotation);
    void (*get_rotation)(uint16_t *rotation);
    void (*read)(void);
    bool (*get_data)(bsp_touch_data_t *data);
};

#endif