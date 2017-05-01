#include "spi.h"

pthread_mutex_t spi_lock = PTHREAD_MUTEX_INITIALIZER;

void Spi::transfernb(uint8_t *tbuf, uint8_t *rbuf, uint32_t len)
{
    pthread_mutex_lock(&spi_lock);
    bcm2835_spi_chipSelect(m_cs);
    bcm2835_spi_transfernb(tbuf, rbuf, len);
    pthread_mutex_unlock(&spi_lock);
}
void Spi::transfern(uint8_t *buf, uint32_t len)
{
    pthread_mutex_lock(&spi_lock);
    bcm2835_spi_chipSelect(m_cs);
    bcm2835_spi_transfern(buf, len);
    pthread_mutex_unlock(&spi_lock);

}
void Spi::writenb(char *buf, uint32_t len)
{
    pthread_mutex_lock(&spi_lock);
    bcm2835_spi_chipSelect(m_cs);
    bcm2835_spi_writenb(buf, len);
    pthread_mutex_unlock(&spi_lock);

}
void Spi::writeFIFO(uint8_t *buf, uint32_t len)
{
	pthread_mutex_lock(&spi_lock);
    bcm2835_spi_chipSelect(m_cs);
    uint8_t tmp[2];
    for(uint32_t i = 0; i < len; i++)
	{
        tmp[0] = 0x80;
        tmp[1] = buf[i];
        bcm2835_spi_transfern(tmp, 2);
	}
    pthread_mutex_unlock(&spi_lock);
}
void Spi::readFIFO(uint8_t *buf, uint32_t len)
{
	pthread_mutex_lock(&spi_lock);
    bcm2835_spi_chipSelect(m_cs);
    uint8_t tmp[2] = {0x00, 0x00};
    for(uint32_t i = 0; i < len; i++)
	{
        tmp[0] = 0x00;
        bcm2835_spi_transfern(tmp, 2);
		buf[i] = tmp[1];
	}
    pthread_mutex_unlock(&spi_lock);
}
Spi::Spi(uint8_t cs) : m_cs(cs)
{
    static bool init = false;
    if(!init)
    {
        spi_init();
        init = true;
    }
    bcm2835_spi_setChipSelectPolarity(cs, LOW_BCM);

}
void Spi::spi_init()
{
    bcm2835_init();
    bcm2835_spi_begin();
    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_64);
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);
}
Spi *getSpi0(void)
{
    static Spi spi0(BCM2835_SPI_CS0);
    return &spi0;
}
Spi *getSpi1(void)
{
    static Spi spi1(BCM2835_SPI_CS1);
    return &spi1;
}
struct cmd {

};
