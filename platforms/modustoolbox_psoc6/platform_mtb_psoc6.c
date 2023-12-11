#include <stdio.h>
#include "platform_mtb_psoc6.h"

/**
 * Sharing SPI bus with display.
 * Display driver must re-initialize spi with proper mode after each touch sampling.
 * Comment out SPI_IS_SHARED if touch and display use seperate SPI buses. In that case,
 * no need to re-init spi bus for display since `xpt2046_platform_spi_init()` will not be used.
 */

// Only called if SPI_IS_SHARED is defined
void xpt2046_platform_spi_init(uint64_t spi_freq, uint8_t cpol, uint8_t cpha, uint8_t is_lsbfirst)
{
    cyhal_spi_init(&mSPI, SPI_MOSI, SPI_MISO, SPI_CLK, NC, NULL,
                          8, CYHAL_SPI_MODE(cpol, cpha, is_lsbfirst), false);
    cyhal_spi_set_frequency(&mSPI, spi_freq);
}

// Only called if SPI_IS_SHARED is defined
void xpt2046_platform_spi_deinit(void)
{
    cyhal_spi_free(&mSPI);
}

uint8_t xpt2046_platform_spi_transfer(uint8_t data)
{
    const uint8_t tx_buff = data;
    uint8_t rx_buff;
    cyhal_spi_transfer(&mSPI, &tx_buff, 1u, &rx_buff, 1u, 0);
    return rx_buff;
}

// TODO: in xpt2046.c, remove transfer16 and use transfer instead
uint16_t xpt2046_platform_spi_transfer16(uint16_t data)
{
    const uint8_t tx_buff[2] = {(uint8_t)(data >> 8), (uint8_t)data};
    uint8_t rx_buff[2];
    cyhal_spi_transfer(&mSPI, tx_buff, 2u, rx_buff, 2u, 0);
    return (((uint16_t)rx_buff[0] << 8) | (uint16_t)rx_buff[1]); // combine to a single uint16
}

void xpt2046_platform_delay(uint64_t ms)
{
    cyhal_system_delay_ms(ms);
}

uint64_t xpt2046_platform_get_millis(void)
{
    // Define this somewhere in main application
    // Use CysTick or relevant function from FreeRTOS
    return (uint64_t)get_millis();
}

void xpt2046_platform_print(const char *msg)
{
    printf("%s", msg);
}
