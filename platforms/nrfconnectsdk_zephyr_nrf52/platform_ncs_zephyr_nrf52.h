// NOTE: Only to be included by platform_select.h. User should NOT include it

// platform: nRF52 MCU with nRF Connect SDK and Zephyr RTOS


#ifndef _PLATFORM_NCS_ZEPHYR_NRF52_
#define _PLATFORM_NCS_ZEPHYR_NRF52_

#include <stdint.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>

// Define these in main application
extern struct spi_config spi_cfg;
extern const struct device *spi_dev;
extern const struct device *gpio1_dev;

// SPI pins are shared between display and touch ic
#define SPI_IS_SHARED

#define TOUCH_CS   6    // GPIO1 PIN 6 (P1.06)
#define DISP_CS    7    // GPIO1 PIN 7 (P1.07)
//Pins if using SPI1.
/*
    nRF52840     -   XPT2046 (touch)
    -------------------------
    MOSI (P0.30) -   SDI
    MISO (P1.08) -   SDO
    SCK (P0.31)  -   SCK
*/

// Optional Macros (default values are in xpt2046.h)
#define SPI_TOUCH_FREQ 2000000UL    /* 2MHz */
#define DISPLAY_WIDTH  320
#define DISPLAY_HEIGHT 240

// Mandatory Macros needed by xpt2046.c
#define PLATFORM_DISP_CS_HIGH()   {gpio_pin_set(gpio1_dev, DISP_CS, 1);}
#define PLATFORM_TOUCH_CS_HIGH()  {gpio_pin_set(gpio1_dev, TOUCH_CS, 1);}
#define PLATFORM_TOUCH_CS_LOW()   {gpio_pin_set(gpio1_dev, TOUCH_CS, 0);}
#define PLATFORM_DMA_BUSY_WAIT()  {/* wait while dma is busy */}


// Mandatory Functions needed by xpt2046.c
void     xpt2046_platform_spi_init(uint64_t spi_freq, uint8_t cpol, uint8_t cpha, uint8_t is_lsbfirst);
void     xpt2046_platform_spi_deinit(void);
uint8_t  xpt2046_platform_spi_transfer(uint8_t data);
uint16_t xpt2046_platform_spi_transfer16(uint16_t data);
void     xpt2046_platform_delay(uint64_t ms);
uint64_t xpt2046_platform_get_millis(void);
void     xpt2046_platform_print(const char *msg);


#endif /* _PLATFORM_NCS_ZEPHYR_NRF52_ */
