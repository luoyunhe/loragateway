#include "sx1278.h"
#include <iostream>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <cstring>

Gpio_Mask* p_gpio_mask = NULL;


uint8_t SX1278::readRegister(uint8_t address)
{
	uint8_t buf[2] = {address, 0};
	p_spi->transfern(buf, 2);
	return buf[1];
}


void SX1278::writeRegister(uint8_t address,uint8_t data)
{
    address |= 0x80;
    uint8_t buf[2]={address, data};
	p_spi->transfern(buf, 2);
}


void SX1278::RxChainCalibration()
{
	// Cut the PA just in case, RFO output, power = -1 dBm
	writeRegister( REG_PA_CONFIG, 0x00 );
	// Launch Rx chain calibration for LF band
	writeRegister( REG_IMAGE_CAL, ( readRegister( REG_IMAGE_CAL ) & RF_IMAGECAL_IMAGECAL_MASK ) | RF_IMAGECAL_IMAGECAL_START );
	while( ( readRegister( REG_IMAGE_CAL ) & RF_IMAGECAL_IMAGECAL_RUNNING ) == RF_IMAGECAL_IMAGECAL_RUNNING )
	{
	}
    setChannel(FreqTab[18]);
	// Launch Rx chain calibration for HF band
	writeRegister( REG_IMAGE_CAL, ( readRegister( REG_IMAGE_CAL ) & RF_IMAGECAL_IMAGECAL_MASK ) | RF_IMAGECAL_IMAGECAL_START );
	while( ( readRegister( REG_IMAGE_CAL ) & RF_IMAGECAL_IMAGECAL_RUNNING ) == RF_IMAGECAL_IMAGECAL_RUNNING )
	{
	}
}
uint8_t SX1278::Init()
{
	uint8_t state = 2;
#if (debug_mode > 1)
    std::cout << "Init..." << std::endl;
#endif
    gpio_set_value(p_gpio->reset, 0);
    delay(500);
    gpio_set_value(p_gpio->reset, 1);
#if (debug_mode > 1)
    std::cout << "reset sx1278." << std::endl;
#endif
    uint8_t version = readRegister(REG_VERSION);
    if(version == 0x12)
    {
#if (debug_mode > 1)
        std::cout << "sx1278 detected." << std::endl;
#endif
    }
    else
        return state;

#if (debug_mode > 1)
	//printf("## Setting ON with maximum current supply ##\n");
	//printf("\n");
#endif
    RxChainCalibration();
	setLORA();
	setChannel(FreqTab[0]);
    setPower(PowerTab[2]);
    writeRegister(REG_OCP, 0x0b);  //RegOcp,Close Ocp
    writeRegister(REG_LNA, 0x23);  //RegLNA,High & LNA Enable
	setBW(BwTab[6]);
    setCR(CR_5);
    setHeaderON();
    setSF(SF_10);
    setCRC_ON();
    writeRegister(REG_SYMB_TIMEOUT_LSB, 0xff);
    setPreambleLength(7);
	return state;
}
void SX1278::setting(const arg_event_setting *a)
{
    setBW(a->m_bw);
    setCR(a->m_cr);
    setChannel(FreqTab[a->m_ch]);
    setSF(a->m_sf);
}

void SX1278::setLORA()
{

    writeRegister(REG_OP_MODE, LORA_SLEEP_MODE);    // LoRa sleep mode
    delay(50);
    writeRegister(REG_OP_MODE, LORA_STANDBY_MODE);
    delay(50);
	clearFlags();
	setFlagMask(0xff);
    lora_state = standby;
    return ;
}


void  SX1278::clearFlags()
{
    writeRegister(REG_OP_MODE, LORA_STANDBY_MODE);	  // Stdby mode to write in registers
    writeRegister(REG_IRQ_FLAGS, 0xFF);	              // LoRa mode flags register
}


void  SX1278::setFlagMask(uint8_t mask)
{
    writeRegister(REG_OP_MODE, LORA_STANDBY_MODE);	  // Stdby mode to write in registers
    writeRegister(REG_IRQ_FLAGS_MASK, mask);	      // LoRa mode flags register
}
void SX1278::setSF(uint8_t spr)
{
    byte config1;
    byte config2;
    byte config3;
    writeRegister(REG_OP_MODE, LORA_STANDBY_MODE);	// LoRa standby mode
    config1 = (readRegister(REG_MODEM_CONFIG1));	// Save config1 to modify only the LowDataRateOptimize
    config2 = (readRegister(REG_MODEM_CONFIG2));	// Save config2 to modify SF value (bits 7-4)
    switch(spr)
    {
    case SF_6: 	config2 = config2 & 0B01101111;		// clears bits 7 & 4 from REG_MODEM_CONFIG2
        config2 = config2 | 0B01100000;				// sets bits 6 & 5 from REG_MODEM_CONFIG2
        setHeaderOFF();
        break;
    case SF_7: 	config2 = config2 & 0B01111111;		// clears bits 7 from REG_MODEM_CONFIG2
        config2 = config2 | 0B01110000;				// sets bits 6, 5 & 4
        break;
    case SF_8: 	config2 = config2 & 0B10001111;		// clears bits 6, 5 & 4 from REG_MODEM_CONFIG2
        config2 = config2 | 0B10000000;				// sets bit 7 from REG_MODEM_CONFIG2
        break;
    case SF_9: 	config2 = config2 & 0B10011111;		// clears bits 6, 5 & 4 from REG_MODEM_CONFIG2
        config2 = config2 | 0B10010000;				// sets bits 7 & 4 from REG_MODEM_CONFIG2
        break;
    case SF_10:	config2 = config2 & 0B10101111;		// clears bits 6 & 4 from REG_MODEM_CONFIG2
        config2 = config2 | 0B10100000;				// sets bits 7 & 5 from REG_MODEM_CONFIG2
        break;
    case SF_11:	config2 = config2 & 0B10111111;		// clears bit 6 from REG_MODEM_CONFIG2
        config2 = config2 | 0B10110000;				// sets bits 7, 5 & 4 from REG_MODEM_CONFIG2
        break;
    case SF_12: config2 = config2 & 0B11001111;	// clears bits 5 & 4 from REG_MODEM_CONFIG2
        config2 = config2 | 0B11000000;	// sets bits 7 & 6 from REG_MODEM_CONFIG2
        break;
    }
    if(spr == SF_11 || spr == SF_12)
    {
        //printf("youhua\n");
        config3 = readRegister(REG_MODEM_CONFIG3);
        config3=config3 | 0b00001000;
        writeRegister(REG_MODEM_CONFIG3, config3);
    }
    else
    {
        //printf("buyouha")
        config3 = readRegister(REG_MODEM_CONFIG3);
        config3=config3 & 0b11110111;
        writeRegister(REG_MODEM_CONFIG3, config3);
    }
    writeRegister(REG_MODEM_CONFIG2, config2);

    return ;
}

void SX1278::setBW(uint8_t b)
{
	byte config1;
    writeRegister(REG_OP_MODE, LORA_STANDBY_MODE);		// Set Standby mode to write in register
    config1 = readRegister(REG_MODEM_CONFIG1);	// Save config1 to modify only the CR
	config1 = config1 & 0B00001111;
    config1 = config1 | (b << 4);
    writeRegister(REG_MODEM_CONFIG1, config1);		// Update config1
}



void SX1278::setCR(uint8_t cod)
{
    byte config1;
    writeRegister(REG_OP_MODE, LORA_STANDBY_MODE);		// Set Standby mode to write in register
    config1 = readRegister(REG_MODEM_CONFIG1);	// Save config1 to modify only the CR
	config1 = config1 & 0B11110001;	// clears bits 3 - 1 from REG_MODEM_CONFIG1
	switch(cod)
	{
	case CR_5:
		config1 = config1 | 0B00000010;   //4/5
		break;
	case CR_6:
		config1 = config1 | 0B00000100;	  //4/6
		break;
	case CR_7:
		config1 = config1 | 0B00000110;   //4/7
		break;
	case CR_8:
		config1 = config1 | 0B00001000;   //4/8
		break;
	}
    writeRegister(REG_MODEM_CONFIG1, config1);		// Update config1
    return ;
}


void SX1278::setHeaderON()
{
    byte config1;
	config1 = readRegister(REG_MODEM_CONFIG1);
	config1 = config1 & 0xfe;              // clears bit 0 from config1 = headerON
	writeRegister(REG_MODEM_CONFIG1,config1);	 // Update config1
}



void SX1278::setHeaderOFF()
{
    byte config1;
    config1 = readRegister(REG_MODEM_CONFIG1);	// Save config1 to modify only the header bit
    config1 = config1 | 0x01;                   // sets bit 0 from REG_MODEM_CONFIG1 = headerOFF
    writeRegister(REG_MODEM_CONFIG1,config1);	// Update config1
}



void SX1278::setCRC_ON()
{
    int8_t config1 = readRegister(REG_MODEM_CONFIG2);	// Save config1 to modify only the CRC bit
    config1 = config1 | 0B00000100;         // sets bit 2 from REG_MODEM_CONFIG2 = CRC_ON
    writeRegister(REG_MODEM_CONFIG2,config1);
    return ;
}


void SX1278::setPreambleLength(uint16_t l)
{

	uint8_t p_length;
	writeRegister(REG_OP_MODE, LORA_STANDBY_MODE);    // Set Standby mode to write in registers
	p_length = ((l >> 8) & 0x0FF);
	// Storing MSB preamble length in LoRa mode
	writeRegister(REG_PREAMBLE_MSB_LORA, p_length);
	p_length = (l & 0x0FF);
	// Storing LSB preamble length in LoRa mode
	writeRegister(REG_PREAMBLE_LSB_LORA, p_length);
}


void SX1278::setPower(uint8_t p)
{
	writeRegister(REG_OP_MODE, LORA_STANDBY_MODE);    // Set Standby mode to write in registers
	writeRegister(REG_PA_CONFIG, p);
}



void SX1278::setChannel(const uint8_t * tab)
{
    writeRegister(REG_FRF_MSB, tab[0]);
    writeRegister(REG_FRF_MID, tab[1]);
    writeRegister(REG_FRF_LSB, tab[2]);
    return ;
}



void SX1278::sendSinglePayload(void *vbuf, uint8_t packet_size)
{
	uint8_t value;
    uint8_t* buf = static_cast<uint8_t*>(vbuf);
	writeRegister(REG_OP_MODE,LORA_STANDBY_MODE);
	writeRegister(REG_HOP_PERIOD, 0);
    writeRegister(REG_DIO_MAPPING1, 0x01 << 6);
	enable_isr(p_gpio->dio0, true);
	setFlagMask(0xf7);            //txdone
	clearFlags();

	writeRegister(REG_FIFO_TX_BASE_ADDR, 0);
	value = readRegister(REG_FIFO_TX_BASE_ADDR);
	writeRegister(REG_FIFO_ADDR_PTR, value);

	writeRegister(REG_PAYLOAD_LENGTH_LORA, packet_size);
	p_spi->writeFIFO(buf, packet_size);

	lora_state = tx;
	writeRegister(REG_OP_MODE, LORA_TX_MODE);        //进入发送模式开始发送

}



void SX1278::entryStandby()
{
	writeRegister(REG_OP_MODE, LORA_STANDBY_MODE);
}
void SX1278::entryCAD()
{
#if (debug_mode >1)
    //printf("entry cad.\n");
#endif
	writeRegister(REG_OP_MODE,LORA_STANDBY_MODE);
	setFlagMask(0xfa);            //caddetected   caddone
	clearFlags();	// Initializing flags
	writeRegister(REG_DIO_MAPPING1, 0x2 << 6);
	enable_isr(p_gpio->dio0, true);
	lora_state = cad;
	writeRegister(REG_OP_MODE, LORA_CAD_MODE);  // LORA mode - Cad

	return ;
}
void SX1278::entryRx()
{
	writeRegister(REG_SYMB_TIMEOUT_LSB,0xff);
	setFlagMask(0x1f);//(0x3f);   // rxtimeout   rxdone
	clearFlags();
	writeRegister(REG_DIO_MAPPING1, 0x0 << 6);
	enable_isr(p_gpio->dio0, true);
	writeRegister(REG_FIFO_RX_BASE_ADDR,0);
	writeRegister(REG_FIFO_ADDR_PTR, 0);         //SPI接口地址对齐RX基地址
	lora_state = rx;
	writeRegister(REG_OP_MODE,LORA_RX_SINGLE_MODE);
	return ;
}
uint8_t SX1278::recSinglePackage(void *vbuf)
{
    uint8_t* buf = static_cast<uint8_t*>(vbuf);
	clearFlags();
	setFlagMask(0xff);
    uint8_t addr = readRegister(REG_FIFO_RX_CURRENT_ADDR);
    writeRegister(REG_FIFO_ADDR_PTR, addr);
	uint8_t packet_size = readRegister(REG_RX_NB_BYTES);
	p_spi->readFIFO(buf, packet_size);

	return packet_size;
}

void enable_isr(int gpio, bool e)
{
    switch(gpio)
    {
        case 2:
            p_gpio_mask->gpio_2 = e;
        break;
        case 3:
            p_gpio_mask->gpio_3 = e;
        break;
        case 4:
            p_gpio_mask->gpio_4 = e;
        break;
        case 5:
            p_gpio_mask->gpio_5 = e;
        break;
    }
}
