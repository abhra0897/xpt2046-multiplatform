// NOTE: Only to be included by platform_select.h. User should NOT include it

// ModusToolbox PSoC6 platform


#ifndef _PLATFORM_MTB_PSOC6_
#define _PLATFORM_MTB_PSOC6_

#include <stdint.h>
#include <string.h>
#include "cybsp.h"
#include "cyhal.h"

// Define mSPI in main application
extern cyhal_spi_t mSPI;

// SPI pins are shared between display and touch ic
#define SPI_IS_SHARED

#define TOUCH_CS  CYBSP_D6
#define DISP_CS   CYBSP_D7
#define SPI_MISO  CYBSP_SPI_MISO
#define SPI_MOSI  CYBSP_SPI_MOSI
#define SPI_CLK   CYBSP_SPI_CLK

// Optional Macros (default values are in xpt2046.h)
#define SPI_TOUCH_FREQ 2000000UL    /* 2MHz */
#define DISPLAY_WIDTH  320
#define DISPLAY_HEIGHT 240

// Mandatory Macros needed by xpt2046.c
#define PLATFORM_DISP_CS_HIGH()   {cyhal_gpio_write(DISP_CS, 1);}
#define PLATFORM_TOUCH_CS_HIGH()  {cyhal_gpio_write(TOUCH_CS, 1);}
#define PLATFORM_TOUCH_CS_LOW()   {cyhal_gpio_write(TOUCH_CS, 0);}
#define PLATFORM_DMA_BUSY_WAIT()  {/* wait while dma is busy */}


// Mandatory Functions needed by xpt2046.c
void     xpt2046_platform_spi_init(uint64_t spi_freq, uint8_t cpol, uint8_t cpha, uint8_t is_lsbfirst);
void     xpt2046_platform_spi_deinit(void);
uint8_t  xpt2046_platform_spi_transfer(uint8_t data);
uint16_t xpt2046_platform_spi_transfer16(uint16_t data);
void     xpt2046_platform_delay(uint64_t ms);
uint64_t xpt2046_platform_get_millis(void);
void     xpt2046_platform_print(const char *msg);


#endif /* _PLATFORM_MTB_PSOC6_ */
