// NOTE: Only to be included by platform_select.h. User should NOT include it

// To create your platform-specific implementation, follow this template

// This is the template platform file to use with xpt2046.c/h
// Mandatory macros and functions are declared here.
// Functions are defined in the respective .c file.


#ifndef _PLATFORM_GENERIC_
#define _PLATFORM_GENERIC_

#include <stdint.h>

// SPI pins are shared between XPT2046 touch IC and the display/other device(s)
// Comment out if xpt2046 is not sharing its SPI pins
#define SPI_IS_SHARED

// Optional Macros (default values are in xpt2046.h)
#define SPI_TOUCH_FREQ 2000000UL    /* 2MHz */
#define SPI_DISP_FREQ  20000000UL   /* 20MHz */
#define DISPLAY_WIDTH  320
#define DISPLAY_HEIGHT 240

// Mandatory Macros needed by xpt2046.c
// Sets display CS pin high. Called only if SPI_IS_SHARED defined. Else leave it empty
#define PLATFORM_DISP_CS_HIGH()   {/* set disp cs line high */}
// Sets touch CS pin low
#define PLATFORM_TOUCH_CS_HIGH()  {/* set touch cs line high */}
// Sets touch CS pin high
#define PLATFORM_TOUCH_CS_LOW()   {/* set touch cs line low */}
// Wait while DMA is busy. Needed only if using DMA. Else leave it empty
#define PLATFORM_DMA_BUSY_WAIT()  {/* wait while dma is busy */}


// Mandatory Functions needed by xpt2046.c

// Initialize or configure SPI bus with given settings.
void     xpt2046_platform_spi_init(uint64_t spi_freq, uint8_t cpol, uint8_t cpha, uint8_t is_lsbfirst);
// De-initialize SPI bus
void     xpt2046_platform_spi_deinit(void);
// Send 8-bit data and receive 8-bit data simultaneously
uint8_t  xpt2046_platform_spi_transfer(uint8_t data);
// Send 16-bit data and receive 16-bit data simultaneously
uint16_t xpt2046_platform_spi_transfer16(uint16_t data);
// Delay for specified milliseconds
void     xpt2046_platform_delay(uint64_t ms);
// Get platform time in milliseconds
uint64_t xpt2046_platform_get_millis(void);
// Print character array
void     xpt2046_platform_print(const char *msg);


#endif /* _PLATFORM_GENERIC_ */
