#include <stdio.h>
#include "platform_generic.h"


void xpt2046_platform_spi_init(uint64_t spi_freq, uint8_t cpol, uint8_t cpha, uint8_t is_lsbfirst)
{
    //TODO: Add platform specific code here
}

void xpt2046_platform_spi_deinit(void)
{
    //TODO: Add platform specific code here
}

void xpt2046_platform_spi_set_freq(uint64_t spi_freq)
{
    //TODO: Add platform specific code here
}

uint8_t xpt2046_platform_spi_transfer(uint8_t data)
{
    //TODO: Add platform specific code here
    return 0;
}

uint16_t xpt2046_platform_spi_transfer16(uint16_t data)
{
    //TODO: Add platform specific code here
    return 0;
}

void xpt2046_platform_delay(uint64_t ms)
{
    //TODO: Add platform specific code here
}

uint32_t xpt2046_platform_get_millis(void)
{
    //TODO: Add platform specific code here
    return 0;
}

void xpt2046_platform_print(const char *msg)
{
    printf("%s", msg);
}


