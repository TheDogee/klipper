// SPI functions on STM32
//
// Copyright (C) 2019  Kevin O'Connor <kevin@koconnor.net>
//
// This file may be distributed under the terms of the GNU GPLv3 license.

#include "command.h" // shutdown
#include "gpio.h" // spi_setup
#include "internal.h" // gpio_peripheral
#include "sched.h" // sched_shutdown

struct spi_info {
    void *spi;
    uint8_t miso_pin, mosi_pin, sck_pin, function;
};

DECL_ENUMERATION("spi_bus", "spi2", 0);
DECL_CONSTANT_STR("BUS_PINS_spi2", "PB14,PB15,PB13");
DECL_ENUMERATION("spi_bus", "spi1", 1);
DECL_CONSTANT_STR("BUS_PINS_spi1", "PA6,PA7,PA5");
#ifdef SPI3
DECL_ENUMERATION("spi_bus", "spi3", 2);
DECL_CONSTANT_STR("BUS_PINS_spi3", "PB4,PB5,PB3");
#endif

static const struct spi_info spi_bus[] = {
    { SPI2, GPIO('B', 14), GPIO('B', 15), GPIO('B', 13), GPIO_FUNCTION(5) },
    { SPI1, GPIO('A', 6), GPIO('A', 7), GPIO('A', 5), GPIO_FUNCTION(5) },
#ifdef SPI3
    { SPI3, GPIO('B', 4), GPIO('B', 5), GPIO('B', 3), GPIO_FUNCTION(6) },
#endif
};

struct spi_config
spi_setup(uint32_t bus, uint8_t mode, uint32_t rate)
{
    if (bus >= ARRAY_SIZE(spi_bus))
        shutdown("Invalid spi bus");

    // Enable SPI
    SPI_TypeDef *spi = spi_bus[bus].spi;
    if (!is_enabled_pclock((uint32_t)spi)) {
        enable_pclock((uint32_t)spi);
        gpio_peripheral(spi_bus[bus].miso_pin, spi_bus[bus].function, 1);
        gpio_peripheral(spi_bus[bus].mosi_pin, spi_bus[bus].function, 0);
        gpio_peripheral(spi_bus[bus].sck_pin, spi_bus[bus].function, 0);
    }

    // Calculate CR1 register
    uint32_t pclk = get_pclock_frequency((uint32_t)spi);
    uint32_t div = 0;
    while ((pclk >> (div + 1)) > rate && div < 7)
        div++;
    uint32_t cr1 = ((mode << SPI_CR1_CPHA_Pos) | (div << SPI_CR1_BR_Pos)
                    | SPI_CR1_SPE | SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI);

    return (struct spi_config){ .spi = spi, .spi_cr1 = cr1 };
}

void
spi_prepare(struct spi_config config)
{
    SPI_TypeDef *spi = config.spi;
    spi->CR1 = config.spi_cr1;
}

void
spi_transfer(struct spi_config config, uint8_t receive_data,
             uint8_t len, uint8_t *data)
{
    SPI_TypeDef *spi = config.spi;
    while (len--) {
        spi->DR = *data;
        while (!(spi->SR & SPI_SR_RXNE))
            ;
        uint8_t rdata = spi->DR;
        if (receive_data)
            *data = rdata;
        data++;
    }
}
