/*
 * Copyright (c) 2023, CATIE
 * SPDX-License-Identifier: Apache-2.0
 */
#include "ili9163c.h"
#include "lvgl.h"
#include "mbed.h"
#include "tsc2003.h"

using namespace sixtron;

// Screen configuration
static const uint16_t screenWidth = 128;
static const uint16_t screenHeight = 160;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * 10];

// Peripherals
static SPI spi(SPI1_MOSI, SPI1_MISO, SPI1_SCK);
static ILI9163C display(&spi, SPI1_CS, DIO2, PWM1_OUT);
static TSC2003 touch(I2C1_SDA, I2C1_SCL, DIO3, TSC2003::ADDRESS1);
static DigitalOut led1(LED1);

EventQueue queue(32 * EVENTS_EVENT_SIZE);

void get_touch_coordinates()
{
    uint16_t x, y, z1, z2;
    touch.read_touch(&x, &y, &z1, &z2);
    printf("x: %d, y: %d, z1: %d, z2: %d\n", x, y, z1, z2);
}

void touch_callback()
{
    queue.call(get_touch_coordinates);
}

void lvgl_routine()
{
    lv_timer_handler();
    lv_tick_inc(5);
}

// Display flushing
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    display.setAddr(area->x1, area->y1, area->x2, area->y2);
    display.write_data_16((uint16_t *)&color_p->full, w * h);

    lv_disp_flush_ready(disp);
}

// Touchpad reading
void my_touchpad_read(lv_indev_drv_t *indev, lv_indev_data_t *data)
{
    uint16_t x, y, z1, z2;
    touch.read_touch(&x, &y, &z1, &z2);
    if (z1 > 0) { // Touch detected
        data->state = LV_INDEV_STATE_PRESSED;
        data->point.x = screenWidth - (int16_t)(((float)x / 4096) * screenWidth);
        data->point.y = screenHeight - (int16_t)(((float)y / 4096) * screenHeight);
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

static void anim_x_cb(void *var, int32_t v)
{
    lv_obj_set_x((lv_obj_t *)var, v);
}

static void sw_event_cb(lv_event_t *e)
{
    lv_obj_t *sw = (lv_obj_t *)lv_event_get_target(e);
    lv_obj_t *label = (lv_obj_t *)lv_event_get_user_data(e);

    if (lv_obj_has_state(sw, LV_STATE_CHECKED)) {
        led1 = 1;
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, label);
        lv_anim_set_values(&a, lv_obj_get_x(label), 20);
        lv_anim_set_time(&a, 500);
        lv_anim_set_exec_cb(&a, anim_x_cb);
        lv_anim_set_path_cb(&a, lv_anim_path_overshoot);
        lv_anim_start(&a);
    } else {
        led1 = 0;
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, label);
        lv_anim_set_values(&a, lv_obj_get_x(label), -lv_obj_get_width(label));
        lv_anim_set_time(&a, 500);
        lv_anim_set_exec_cb(&a, anim_x_cb);
        lv_anim_set_path_cb(&a, lv_anim_path_ease_in);
        lv_anim_start(&a);
    }
}

static void btn_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);
    if (code == LV_EVENT_CLICKED) {
        static uint8_t cnt = 0;
        cnt++;

        /*Get the first child of the button which is the label and change its text*/
        lv_obj_t *label = lv_obj_get_child(btn, 0);
        lv_label_set_text_fmt(label, "Button: %d", cnt);
    }
}

int main()
{
    printf("Zest Display LCD simple demo\n");

    // Setup touch screen
    touch.set_resolution(TSC2003::_12BITS);
    // touch.attach_touch_irq(touch_callback);

    // Init display
    display.init();

    // init LVGL
    lv_init();
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * 10);

    // Initialize the display
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);

    // Set up the functions to access to your display
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    // Initialize the touch input device driver
    static lv_indev_drv_t indev_drv; /*Descriptor of a input device driver*/
    lv_indev_drv_init(&indev_drv); /*Basic initialization*/
    indev_drv.type = LV_INDEV_TYPE_POINTER; /*Touch pad is a pointer-like device*/
    indev_drv.read_cb = my_touchpad_read; /*Set your driver function*/
    lv_indev_drv_register(&indev_drv); /*Finally register the driver*/

    // Setup a switch button with an animated label
    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "LED ON!");
    lv_obj_set_pos(label, -100, 10);

    lv_obj_t *sw = lv_switch_create(lv_scr_act());
    lv_obj_center(sw);
    lv_obj_add_state(sw, LV_STATE_DEFAULT);
    lv_obj_add_event_cb(sw, sw_event_cb, LV_EVENT_VALUE_CHANGED, label);

    queue.call_every(5ms, lvgl_routine);
    queue.dispatch_forever();
}
