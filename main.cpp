/*
 * Copyright (c) 2023, CATIE
 * SPDX-License-Identifier: Apache-2.0
 */
#include "ili9163c.h"
#include "lvgl.h"
#include "mbed.h"

using namespace sixtron;

// Screen configuration
static const uint16_t screenWidth = 128;
static const uint16_t screenHeight = 160;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * 10];

// Peripherals
static SPI spi(P1_SPI_MOSI, P1_SPI_MISO, P1_SPI_SCK);
ILI9163C display(&spi, P1_SPI_CS, P1_DIO2, P1_PWM1);

static DigitalOut led1(LED1);

// Display flushing
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    display.setAddr(area->x1, area->y1, area->x2, area->y2);
    display.write_data_16((uint16_t *)&color_p->full, w * h);

    lv_disp_flush_ready(disp);
}

int main()
{
    printf("Zest Display LCD simple demo\n");

    // Init display
    display.init();

    // init LVGL
    lv_init();
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * 10);

    // Initialize the display
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);

    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    // Add label
    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "Hello from 6TRON!");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    while (true) {
        lv_timer_handler();
        ThisThread::sleep_for(10ms);
    }
}
