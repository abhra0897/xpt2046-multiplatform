//Using SPI1
/*
    nRF52840		-	XPT2046
    MOSI (P0.30)	-	SDI
    MISO (P1.08)	-	SDO
    SCK (P0.31)		-	SCK
    DISP_CS (P1.07)	-	CS
    TOUCH_CS (P1.06)-	CS
*/

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>
#include <ncs_version.h>
#include "xpt2046.h"

#define ENABLE_TOUCH_CALIBRATION

// These structs are expected by driver `platform_ncs_zephyr_nrf52.h`
struct spi_config spi_cfg;
#define SPI1_NODE  DT_NODELABEL(spi1)
const struct device *spi_dev = DEVICE_DT_GET(SPI1_NODE);
#define MY_GPIO1 DT_NODELABEL(gpio1)	//CS pin(s)
const struct device *gpio1_dev = DEVICE_DT_GET(MY_GPIO1);

// GPIO0 port for LED1
#define LED1_PIN    13 // P0.13
const struct device *gpio0_dev = DEVICE_DT_GET(DT_NODELABEL(gpio0));

int main(void)
{
    // For LED1 pin
    gpio_pin_configure(gpio0_dev, LED1_PIN, GPIO_OUTPUT);
    gpio_pin_set(gpio0_dev, LED1_PIN, 1);   // LED1 is active low, turining it off

    // Four touch and display(optional) CS pin(s)
    gpio_pin_configure(gpio1_dev, TOUCH_CS, GPIO_OUTPUT);
#ifdef SPI_IS_SHARED
    gpio_pin_configure(gpio1_dev, DISP_CS, GPIO_OUTPUT);
#endif
    
    uint16_t cal_params[5] = {212, 3653, 349, 3578, 4};

#ifdef ENABLE_TOUCH_CALIBRATION
    xpt2046_calibrate_touch(cal_params);
    printk("uint16_t cal_params[5] = {%d", cal_params[0]);
    for (uint8_t i = 1; i < 5; i++)
    {
        printk(", %d", cal_params[i]);
    }
    printk("}\r\n");
#endif

    xpt2046_set_touch(cal_params);

    uint16_t x, y;
    uint64_t time = 0;
    for (;;)
    {
        bool pressed = xpt2046_get_touch(&x, &y);
        if (pressed)
        {
            printk("X: %d   Y: %d\r\n", x, y);
        }
        else
        {
            x = -1;
            y = -1;
        }

        uint64_t tmp_ms = k_uptime_get();
        if (tmp_ms >= time)
        {
            gpio_pin_toggle(gpio0_dev, LED1_PIN);
            time = tmp_ms + (pressed ? 50 : 500);
        }
    }

    return 0;

}