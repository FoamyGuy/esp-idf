#include <stdio.h>
#include <string.h>
#include "unity.h"
#include "test_utils.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "soc/soc_caps.h"
#include "driver/gpio.h"
#include "test_i80_board.h"

#if SOC_LCDCAM_SUPPORTED
TEST_CASE("lcd i80 device swap color bytes", "[lcd]")
{
    esp_lcd_i80_bus_handle_t i80_bus = NULL;
    esp_lcd_i80_bus_config_t bus_config = {
        .dc_gpio_num = TEST_LCD_DC_GPIO,
        .wr_gpio_num = TEST_LCD_PCLK_GPIO,
        .data_gpio_nums = {
            TEST_LCD_DATA0_GPIO,
            TEST_LCD_DATA1_GPIO,
            TEST_LCD_DATA2_GPIO,
            TEST_LCD_DATA3_GPIO,
            TEST_LCD_DATA4_GPIO,
            TEST_LCD_DATA5_GPIO,
            TEST_LCD_DATA6_GPIO,
            TEST_LCD_DATA7_GPIO,
        },
        .bus_width = 8,
        .max_transfer_bytes = 20,
    };
    TEST_ESP_OK(esp_lcd_new_i80_bus(&bus_config, &i80_bus));

    esp_lcd_panel_io_handle_t io_handles[4] = {};
    esp_lcd_panel_io_i80_config_t io_config = {
        .cs_gpio_num = TEST_LCD_CS_GPIO,
        .pclk_hz = 5000000,
        .trans_queue_depth = 10,
        .dc_levels = {
            .dc_idle_level = 0,
            .dc_cmd_level = 0,
            .dc_dummy_level = 0,
            .dc_data_level = 1,
        },
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
    };

    io_config.flags.reverse_color_bits = 0;
    io_config.flags.swap_color_bytes = 0;
    TEST_ESP_OK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &io_handles[0]));
    io_config.flags.reverse_color_bits = 0;
    io_config.flags.swap_color_bytes = 1;
    TEST_ESP_OK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &io_handles[1]));
    io_config.flags.reverse_color_bits = 1;
    io_config.flags.swap_color_bytes = 0;
    TEST_ESP_OK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &io_handles[2]));
    io_config.flags.reverse_color_bits = 1;
    io_config.flags.swap_color_bytes = 1;
    TEST_ESP_OK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &io_handles[3]));

    for (int i = 0; i < 4; i++) {
        esp_lcd_panel_io_tx_param(io_handles[i], 0xA5, (uint8_t[]) {
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06
        }, 6);
        esp_lcd_panel_io_tx_color(io_handles[i], 0x5A, (uint8_t[]) {
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06
        }, 6);
        TEST_ESP_OK(esp_lcd_panel_io_del(io_handles[i]));
    }

    TEST_ESP_OK(esp_lcd_del_i80_bus(i80_bus));
}

TEST_CASE("lcd i80 device clock mode", "[lcd]")
{
    esp_lcd_i80_bus_handle_t i80_bus = NULL;
    esp_lcd_i80_bus_config_t bus_config = {
        .dc_gpio_num = TEST_LCD_DC_GPIO,
        .wr_gpio_num = TEST_LCD_PCLK_GPIO,
        .data_gpio_nums = {
            TEST_LCD_DATA0_GPIO,
            TEST_LCD_DATA1_GPIO,
            TEST_LCD_DATA2_GPIO,
            TEST_LCD_DATA3_GPIO,
            TEST_LCD_DATA4_GPIO,
            TEST_LCD_DATA5_GPIO,
            TEST_LCD_DATA6_GPIO,
            TEST_LCD_DATA7_GPIO,
        },
        .bus_width = 8,
        .max_transfer_bytes = 20,
    };
    TEST_ESP_OK(esp_lcd_new_i80_bus(&bus_config, &i80_bus));

    esp_lcd_panel_io_handle_t io_handles[4] = {};
    esp_lcd_panel_io_i80_config_t io_config = {
        .cs_gpio_num = TEST_LCD_CS_GPIO,
        .pclk_hz = 5000000,
        .trans_queue_depth = 10,
        .dc_levels = {
            .dc_idle_level = 0,
            .dc_cmd_level = 0,
            .dc_dummy_level = 0,
            .dc_data_level = 1,
        },
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
    };

    io_config.flags.pclk_idle_low = 0;
    io_config.flags.pclk_active_neg = 0;
    TEST_ESP_OK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &io_handles[0]));
    io_config.flags.pclk_idle_low = 0;
    io_config.flags.pclk_active_neg = 1;
    TEST_ESP_OK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &io_handles[1]));
    io_config.flags.pclk_idle_low = 1;
    io_config.flags.pclk_active_neg = 0;
    TEST_ESP_OK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &io_handles[2]));
    io_config.flags.pclk_idle_low = 1;
    io_config.flags.pclk_active_neg = 1;
    TEST_ESP_OK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &io_handles[3]));

    for (int i = 0; i < 4; i++) {
        esp_lcd_panel_io_tx_param(io_handles[i], 0xA5, (uint8_t[]) {
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06
        }, 6);
        TEST_ESP_OK(esp_lcd_panel_io_del(io_handles[i]));
    }
    TEST_ESP_OK(esp_lcd_del_i80_bus(i80_bus));
}
#endif // SOC_LCDCAM_SUPPORTED

#if SOC_LCD_I80_SUPPORTED
TEST_CASE("lcd i80 bus and device allocation", "[lcd]")
{
    esp_lcd_i80_bus_handle_t i80_buses[SOC_LCD_I80_BUSES] = {};
    esp_lcd_i80_bus_config_t bus_config = {
        .dc_gpio_num = TEST_LCD_DC_GPIO,
        .wr_gpio_num = TEST_LCD_PCLK_GPIO,
        .data_gpio_nums = {
            TEST_LCD_DATA0_GPIO,
            TEST_LCD_DATA1_GPIO,
            TEST_LCD_DATA2_GPIO,
            TEST_LCD_DATA3_GPIO,
            TEST_LCD_DATA4_GPIO,
            TEST_LCD_DATA5_GPIO,
            TEST_LCD_DATA6_GPIO,
            TEST_LCD_DATA7_GPIO,
        },
        .bus_width = 8,
        .max_transfer_bytes = TEST_LCD_H_RES * 40 * sizeof(uint16_t)
    };
    for (int i = 0; i < SOC_LCD_I80_BUSES; i++) {
        TEST_ESP_OK(esp_lcd_new_i80_bus(&bus_config, &i80_buses[i]));
    }
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_FOUND, esp_lcd_new_i80_bus(&bus_config, &i80_buses[0]));
    esp_lcd_panel_io_handle_t io_handles[10] = {};
    esp_lcd_panel_io_i80_config_t io_config = {
        .cs_gpio_num = TEST_LCD_CS_GPIO,
        .pclk_hz = 5000000,
        .trans_queue_depth = 4,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
    };
    for (int i = 0; i < 10; i++) {
        TEST_ESP_OK(esp_lcd_new_panel_io_i80(i80_buses[0], &io_config, &io_handles[i]));
    }
    // can't delete bus handle before we delete all devices
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, esp_lcd_del_i80_bus(i80_buses[0]));
    for (int i = 0; i < 10; i++) {
        TEST_ESP_OK(esp_lcd_panel_io_del(io_handles[i]));
    }
    for (int i = 0; i < SOC_LCD_I80_BUSES; i++) {
        TEST_ESP_OK(esp_lcd_del_i80_bus(i80_buses[i]));
    }
}

TEST_CASE("lcd panel i80 io test", "[lcd]")
{
    esp_lcd_i80_bus_handle_t i80_bus = NULL;
    esp_lcd_i80_bus_config_t bus_config = {
        .dc_gpio_num = TEST_LCD_DC_GPIO,
        .wr_gpio_num = TEST_LCD_PCLK_GPIO,
        .data_gpio_nums = {
            TEST_LCD_DATA0_GPIO,
            TEST_LCD_DATA1_GPIO,
            TEST_LCD_DATA2_GPIO,
            TEST_LCD_DATA3_GPIO,
            TEST_LCD_DATA4_GPIO,
            TEST_LCD_DATA5_GPIO,
            TEST_LCD_DATA6_GPIO,
            TEST_LCD_DATA7_GPIO,
            TEST_LCD_DATA8_GPIO,
            TEST_LCD_DATA9_GPIO,
            TEST_LCD_DATA10_GPIO,
            TEST_LCD_DATA11_GPIO,
            TEST_LCD_DATA12_GPIO,
            TEST_LCD_DATA13_GPIO,
            TEST_LCD_DATA14_GPIO,
            TEST_LCD_DATA15_GPIO,
        },
        .bus_width = 16,
        .max_transfer_bytes = 100,
    };
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_i80_config_t io_config = {
        .cs_gpio_num = TEST_LCD_CS_GPIO,
        .pclk_hz = 8000000, // 8MHz
        .trans_queue_depth = 10,
        .dc_levels = {
            .dc_idle_level = 0,
            .dc_cmd_level = 0,
            .dc_dummy_level = 0,
            .dc_data_level = 1,
        },
    };
    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = TEST_LCD_RST_GPIO,
        .color_space = ESP_LCD_COLOR_SPACE_RGB,
        .bits_per_pixel = 16,
    };

// On esp32, GPIO16 and GPIO17 are connected to PSRAM, and we don't have other spare GPIOs can be used in the test
// so we skip the 16bit test on esp32 when PSRAM is enabled
#if !CONFIG_ESP32_SPIRAM_SUPPORT
    printf("testing bus-width=16bit, cmd/param bit-width=8bit\r\n");
    bus_config.bus_width = 16;
    TEST_ESP_OK(esp_lcd_new_i80_bus(&bus_config, &i80_bus));
    io_config.lcd_cmd_bits = 8;
    io_config.lcd_param_bits = 8;
    TEST_ESP_OK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &io_handle));
    TEST_ESP_OK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));

    esp_lcd_panel_io_tx_param(io_handle, 0x1A, NULL, 0);
    esp_lcd_panel_io_tx_param(io_handle, 0x1B, (uint8_t[]) {
        0x11, 0x22, 0x33
    }, 3);
    esp_lcd_panel_io_tx_param(io_handle, 0x1C, NULL, 0);
    TEST_ESP_OK(esp_lcd_panel_del(panel_handle));
    TEST_ESP_OK(esp_lcd_panel_io_del(io_handle));
    TEST_ESP_OK(esp_lcd_del_i80_bus(i80_bus));

    printf("testing bus-width=16bit, cmd/param bit-width=16bit\r\n");
    bus_config.bus_width = 16;
    TEST_ESP_OK(esp_lcd_new_i80_bus(&bus_config, &i80_bus));
    io_config.lcd_cmd_bits = 16;
    io_config.lcd_param_bits = 16;
    TEST_ESP_OK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &io_handle));
    TEST_ESP_OK(esp_lcd_new_panel_nt35510(io_handle, &panel_config, &panel_handle));
    esp_lcd_panel_io_tx_param(io_handle, 0x1A01, NULL, 0);
    esp_lcd_panel_io_tx_param(io_handle, 0x1B02, (uint16_t[]) {
        0x11, 0x22, 0x33
    }, 6);
    esp_lcd_panel_io_tx_param(io_handle, 0x1C03, NULL, 0);
    TEST_ESP_OK(esp_lcd_panel_del(panel_handle));
    TEST_ESP_OK(esp_lcd_panel_io_del(io_handle));
    TEST_ESP_OK(esp_lcd_del_i80_bus(i80_bus));
#endif

    printf("testing bus-width=8bit, cmd/param bit-width=8bit\r\n");
    bus_config.bus_width = 8;
    TEST_ESP_OK(esp_lcd_new_i80_bus(&bus_config, &i80_bus));
    io_config.lcd_cmd_bits = 8;
    io_config.lcd_param_bits = 8;
    TEST_ESP_OK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &io_handle));
    TEST_ESP_OK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));
    esp_lcd_panel_io_tx_param(io_handle, 0x1A, NULL, 0);
    esp_lcd_panel_io_tx_param(io_handle, 0x1B, (uint8_t[]) {
        0x11, 0x22, 0x33
    }, 3);
    esp_lcd_panel_io_tx_param(io_handle, 0x1C, NULL, 0);
    TEST_ESP_OK(esp_lcd_panel_del(panel_handle));
    TEST_ESP_OK(esp_lcd_panel_io_del(io_handle));
    TEST_ESP_OK(esp_lcd_del_i80_bus(i80_bus));

    printf("testing bus-width=8bit, cmd/param bit-width=16bit\r\n");
    bus_config.bus_width = 8;
    TEST_ESP_OK(esp_lcd_new_i80_bus(&bus_config, &i80_bus));
    io_config.lcd_cmd_bits = 16;
    io_config.lcd_param_bits = 16;
    TEST_ESP_OK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &io_handle));
    TEST_ESP_OK(esp_lcd_new_panel_nt35510(io_handle, &panel_config, &panel_handle));
    esp_lcd_panel_io_tx_param(io_handle, 0x1A01, NULL, 0);
    esp_lcd_panel_io_tx_param(io_handle, 0x1B02, (uint16_t[]) {
        0x11, 0x22, 0x33
    }, 6);
    esp_lcd_panel_io_tx_param(io_handle, 0x1C03, NULL, 0);
    TEST_ESP_OK(esp_lcd_panel_del(panel_handle));
    TEST_ESP_OK(esp_lcd_panel_io_del(io_handle));
    TEST_ESP_OK(esp_lcd_del_i80_bus(i80_bus));
}

TEST_CASE("lcd panel with i80 interface (st7789, 8bits)", "[lcd]")
{
#define TEST_IMG_SIZE (100 * 100 * sizeof(uint16_t))
    uint8_t *img = heap_caps_malloc(TEST_IMG_SIZE, MALLOC_CAP_DMA);
    TEST_ASSERT_NOT_NULL(img);

    gpio_config_t bk_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << TEST_LCD_BK_LIGHT_GPIO
    };
    TEST_ESP_OK(gpio_config(&bk_gpio_config));

    esp_lcd_i80_bus_handle_t i80_bus = NULL;
    esp_lcd_i80_bus_config_t bus_config = {
        .dc_gpio_num = TEST_LCD_DC_GPIO,
        .wr_gpio_num = TEST_LCD_PCLK_GPIO,
        .data_gpio_nums = {
            TEST_LCD_DATA0_GPIO,
            TEST_LCD_DATA1_GPIO,
            TEST_LCD_DATA2_GPIO,
            TEST_LCD_DATA3_GPIO,
            TEST_LCD_DATA4_GPIO,
            TEST_LCD_DATA5_GPIO,
            TEST_LCD_DATA6_GPIO,
            TEST_LCD_DATA7_GPIO,
        },
        .bus_width = 8,
        .max_transfer_bytes = TEST_IMG_SIZE + 10,
    };
    TEST_ESP_OK(esp_lcd_new_i80_bus(&bus_config, &i80_bus));
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_i80_config_t io_config = {
        .cs_gpio_num = TEST_LCD_CS_GPIO,
        .pclk_hz = 8000000, // 8MHz
        .trans_queue_depth = 10,
        .dc_levels = {
            .dc_idle_level = 0,
            .dc_cmd_level = 0,
            .dc_dummy_level = 0,
            .dc_data_level = 1,
        },
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
    };
    TEST_ESP_OK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &io_handle));

    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = TEST_LCD_RST_GPIO,
        .color_space = ESP_LCD_COLOR_SPACE_RGB,
        .bits_per_pixel = 16,
    };
    TEST_ESP_OK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));

    // turn off backlight
    gpio_set_level(TEST_LCD_BK_LIGHT_GPIO, 0);
    esp_lcd_panel_reset(panel_handle);
    esp_lcd_panel_init(panel_handle);
    esp_lcd_panel_invert_color(panel_handle, true);
    // the gap is LCD panel specific, even panels with the same driver IC, can have different gap value
    esp_lcd_panel_set_gap(panel_handle, 0, 20);
    // turn on backlight
    gpio_set_level(TEST_LCD_BK_LIGHT_GPIO, 1);

    for (int i = 0; i < 200; i++) {
        uint8_t color_byte = esp_random() & 0xFF;
        int x_start = esp_random() % (TEST_LCD_H_RES - 100);
        int y_start = esp_random() % (TEST_LCD_V_RES - 100);
        memset(img, color_byte, TEST_IMG_SIZE);
        esp_lcd_panel_draw_bitmap(panel_handle, x_start, y_start, x_start + 100, y_start + 100, img);
    }
    esp_lcd_panel_disp_off(panel_handle, true); // turn off screen

    TEST_ESP_OK(esp_lcd_panel_del(panel_handle));
    TEST_ESP_OK(esp_lcd_panel_io_del(io_handle));
    TEST_ESP_OK(esp_lcd_del_i80_bus(i80_bus));
    TEST_ESP_OK(gpio_reset_pin(TEST_LCD_BK_LIGHT_GPIO));
    free(img);
#undef TEST_IMG_SIZE
}

// The following test shows a porting example of LVGL GUI library
// To run the LVGL tests, you need to clone the LVGL library into components directory firstly
#if CONFIG_LV_USE_USER_DATA
#include "test_lvgl_port.h"

static bool notify_lvgl_ready_to_flush(esp_lcd_panel_io_handle_t panel_io, void *user_data, void *event_data)
{
    lv_disp_t *disp = *(lv_disp_t **)user_data;
    lv_disp_flush_ready(&disp->driver);
    return false;
}

TEST_CASE("lvgl gui with i80 interface (st7789, 8bits)", "[lcd][lvgl][ignore]")
{
    // initialize LVGL graphics library
    lv_disp_t *disp = NULL;
    lv_init();

    gpio_config_t bk_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << TEST_LCD_BK_LIGHT_GPIO
    };
    TEST_ESP_OK(gpio_config(&bk_gpio_config));

    esp_lcd_i80_bus_handle_t i80_bus = NULL;
    esp_lcd_i80_bus_config_t bus_config = {
        .dc_gpio_num = TEST_LCD_DC_GPIO,
        .wr_gpio_num = TEST_LCD_PCLK_GPIO,
        .data_gpio_nums = {
            TEST_LCD_DATA0_GPIO,
            TEST_LCD_DATA1_GPIO,
            TEST_LCD_DATA2_GPIO,
            TEST_LCD_DATA3_GPIO,
            TEST_LCD_DATA4_GPIO,
            TEST_LCD_DATA5_GPIO,
            TEST_LCD_DATA6_GPIO,
            TEST_LCD_DATA7_GPIO,
        },
        .bus_width = 8,
        .max_transfer_bytes = TEST_LCD_H_RES * 40 * sizeof(uint16_t)
    };
    TEST_ESP_OK(esp_lcd_new_i80_bus(&bus_config, &i80_bus));
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_i80_config_t io_config = {
        .cs_gpio_num = TEST_LCD_CS_GPIO,
        .pclk_hz = 10000000, // 10MHz
        .trans_queue_depth = 10,
        .dc_levels = {
            .dc_idle_level = 0,
            .dc_cmd_level = 0,
            .dc_dummy_level = 0,
            .dc_data_level = 1,
        },
        .flags = {
            .swap_color_bytes = 1,
        },
        .on_color_trans_done = notify_lvgl_ready_to_flush,
        .user_data = &disp,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
    };
    TEST_ESP_OK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &io_handle));

    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = TEST_LCD_RST_GPIO,
        .color_space = ESP_LCD_COLOR_SPACE_RGB,
        .bits_per_pixel = 16,
    };
    TEST_ESP_OK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));

    // turn off backlight
    gpio_set_level(TEST_LCD_BK_LIGHT_GPIO, 0);
    esp_lcd_panel_reset(panel_handle);
    esp_lcd_panel_init(panel_handle);
    esp_lcd_panel_invert_color(panel_handle, true);
    // the gap is LCD panel specific, even panels with the same driver IC, can have different gap value
    esp_lcd_panel_set_gap(panel_handle, 0, 20);
    // turn on backlight
    gpio_set_level(TEST_LCD_BK_LIGHT_GPIO, 1);

    test_lvgl_task_loop(panel_handle, TEST_LCD_H_RES, TEST_LCD_V_RES, &disp);
}

#define TEST_NT35510_DATA_WIDTH (8) // change this to 16 when NT35510 is configured to 16bit in length
TEST_CASE("lvgl gui with i80 interface (nt35510, 8/16bits)", "[lcd][lvgl][ignore]")
{
    // initialize LVGL graphics library
    lv_disp_t *disp = NULL;
    lv_init();

    esp_lcd_i80_bus_handle_t i80_bus = NULL;
    esp_lcd_i80_bus_config_t bus_config = {
        .dc_gpio_num = TEST_LCD_DC_GPIO,
        .wr_gpio_num = TEST_LCD_PCLK_GPIO,
        .data_gpio_nums = {
            TEST_LCD_DATA0_GPIO,
            TEST_LCD_DATA1_GPIO,
            TEST_LCD_DATA2_GPIO,
            TEST_LCD_DATA3_GPIO,
            TEST_LCD_DATA4_GPIO,
            TEST_LCD_DATA5_GPIO,
            TEST_LCD_DATA6_GPIO,
            TEST_LCD_DATA7_GPIO,
            TEST_LCD_DATA8_GPIO,
            TEST_LCD_DATA9_GPIO,
            TEST_LCD_DATA10_GPIO,
            TEST_LCD_DATA11_GPIO,
            TEST_LCD_DATA12_GPIO,
            TEST_LCD_DATA13_GPIO,
            TEST_LCD_DATA14_GPIO,
            TEST_LCD_DATA15_GPIO,
        },
        .bus_width = TEST_NT35510_DATA_WIDTH,
        .max_transfer_bytes = TEST_LCD_H_RES * 40 * sizeof(uint16_t)
    };
    TEST_ESP_OK(esp_lcd_new_i80_bus(&bus_config, &i80_bus));
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_i80_config_t io_config = {
        .cs_gpio_num = TEST_LCD_CS_GPIO,
        .pclk_hz = 10000000, // 10MHz
        .trans_queue_depth = 4,
        .dc_levels = {
            .dc_idle_level = 0,
            .dc_cmd_level = 0,
            .dc_dummy_level = 0,
            .dc_data_level = 1,
        },
        .on_color_trans_done = notify_lvgl_ready_to_flush,
        .user_data = &disp,
        .lcd_cmd_bits = 16,
        .lcd_param_bits = 16,
    };
    TEST_ESP_OK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &io_handle));

    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = -1,
        .color_space = ESP_LCD_COLOR_SPACE_RGB,
        .bits_per_pixel = 16,
    };
    TEST_ESP_OK(esp_lcd_new_panel_nt35510(io_handle, &panel_config, &panel_handle));

    esp_lcd_panel_reset(panel_handle);
    esp_lcd_panel_init(panel_handle);
    esp_lcd_panel_swap_xy(panel_handle, true);
    esp_lcd_panel_mirror(panel_handle, true, false);

    test_lvgl_task_loop(panel_handle, TEST_LCD_H_RES, TEST_LCD_V_RES, &disp);
}
#endif // CONFIG_LV_USE_USER_DATA
#endif // SOC_LCD_I80_SUPPORTED

#if SOC_I2S_LCD_I80_VARIANT
#include "driver/i2s.h"

TEST_CASE("i80 and i2s driver coexistance", "[lcd][i2s]")
{
    esp_lcd_i80_bus_handle_t i80_bus = NULL;
    esp_lcd_i80_bus_config_t bus_config = {
        .dc_gpio_num = TEST_LCD_DC_GPIO,
        .wr_gpio_num = TEST_LCD_PCLK_GPIO,
        .data_gpio_nums = {
            TEST_LCD_DATA0_GPIO,
            TEST_LCD_DATA1_GPIO,
            TEST_LCD_DATA2_GPIO,
            TEST_LCD_DATA3_GPIO,
            TEST_LCD_DATA4_GPIO,
            TEST_LCD_DATA5_GPIO,
            TEST_LCD_DATA6_GPIO,
            TEST_LCD_DATA7_GPIO,
        },
        .bus_width = 8,
        .max_transfer_bytes = 20,
    };
    TEST_ESP_OK(esp_lcd_new_i80_bus(&bus_config, &i80_bus));

    i2s_config_t i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_TX,
        .sample_rate = 36000,
        .bits_per_sample = 16,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .dma_buf_count = 6,
        .dma_buf_len = 60,
    };
    // I2S driver won't be installed as the same I2S port has been used by LCD
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, i2s_driver_install(0, &i2s_config, 0, NULL));
    TEST_ESP_OK(esp_lcd_del_i80_bus(i80_bus));
}
#endif // SOC_I2S_LCD_I80_VARIANT
