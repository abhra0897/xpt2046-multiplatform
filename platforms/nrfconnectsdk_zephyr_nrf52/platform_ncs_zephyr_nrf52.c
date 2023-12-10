#include <stdio.h>
#include "platform_ncs_zephyr_nrf52.h"

struct spi_buf spi_tx_buf;
const struct spi_buf_set tx = {
    .buffers = &spi_tx_buf,
    .count = 1
};
struct spi_buf spi_rx_buf;
const struct spi_buf_set rx = {
    .buffers = &spi_rx_buf,
    .count = 1
};

// Only called if SPI_IS_SHARED is defined
void xpt2046_platform_spi_init(uint64_t spi_freq, uint8_t cpol, uint8_t cpha, uint8_t is_lsbfirst)
{
    spi_cfg.operation = (SPI_OP_MODE_MASTER | SPI_WORD_SET(8) |
        (is_lsbfirst ? SPI_TRANSFER_LSB : SPI_TRANSFER_MSB) |
        (cpol ? SPI_MODE_CPOL : 0) |
        (cpha ? SPI_MODE_CPHA : 0));
    spi_cfg.frequency = spi_freq;
    spi_cfg.slave = 0;
}


// Only called if SPI_IS_SHARED is defined
void xpt2046_platform_spi_deinit(void)
{
    spi_release(spi_dev, &spi_cfg);
}

// Helper function for spi transfer
__attribute__((always_inline)) static inline uint16_t _spi_transfer(uint8_t *txdata, uint8_t *rxdata, uint8_t bytes)
{
    spi_tx_buf.buf = txdata;
    spi_tx_buf.len = bytes;

    spi_rx_buf.buf = rxdata;
    spi_rx_buf.len = bytes;

    spi_transceive(spi_dev, &spi_cfg, &tx, &rx);
}
uint8_t xpt2046_platform_spi_transfer(uint8_t data)
{
    uint8_t tx_data = data;
    uint8_t rx_data;
    _spi_transfer(&tx_data, &rx_data, 1);
    return rx_data;
}

// TODO: in xpt2046.c, remove transfer16 and use transfer instead
uint16_t xpt2046_platform_spi_transfer16(uint16_t data)
{
    uint8_t tx_data[2] = {(uint8_t)(data >> 8), (uint8_t)data};
    uint8_t rx_data[2] = {0};
    _spi_transfer(tx_data, rx_data, 2);
    return (((uint16_t)rx_data[0] << 8) | (uint16_t)rx_data[1]); // combine to a single uint16
}

void xpt2046_platform_delay(uint64_t ms)
{
    k_sleep(K_MSEC(ms));
}

uint32_t xpt2046_platform_get_millis(void)
{
    return k_uptime_get();
}

void xpt2046_platform_print(const char *msg)
{
    printk("%s", msg);
}


