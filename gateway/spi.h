#ifndef	_SPI_H
#define	_SPI_H
#include "bcm2835.h"
#include <pthread.h>
class Spi{
public:
    void transfernb(uint8_t *tbuf, uint8_t *rbuf, uint32_t len);
    void transfern(uint8_t *buf, uint32_t len);
    void writenb(char *buf, uint32_t len);
	void writeFIFO(uint8_t *buf, uint32_t len);
	void readFIFO(uint8_t *buf, uint32_t len);
    static void spi_init(void);
    Spi(uint8_t cs);
private:
    uint8_t m_cs;

};
Spi *getSpi0(void);
Spi *getSpi1(void);
//enum cmd_typeoi

#endif
