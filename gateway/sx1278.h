#ifndef	_SX1278_H
#define	_SX1278_H
#include "spi.h"
#include "gpio_ctrl.h"




//! MACROS //
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)  // read a bit
#define bitSet(value, bit) ((value) |= (1UL << (bit)))    // set bit to '1'
#define bitClear(value, bit) ((value) &= ~(1UL << (bit))) // set bit to '0'

//! REGISTERS //

#define        REG_FIFO        					0x00
#define        REG_OP_MODE        				0x01
#define        REG_BITRATE_MSB    				0x02
#define        REG_BITRATE_LSB    				0x03
#define        REG_FDEV_MSB   					0x04
#define        REG_FDEV_LSB    					0x05
#define        REG_FRF_MSB    					0x06
#define        REG_FRF_MID    					0x07
#define        REG_FRF_LSB    					0x08
#define        REG_PA_CONFIG    				0x09
#define        REG_PA_RAMP    					0x0A
#define        REG_OCP    						0x0B
#define        REG_LNA    						0x0C
#define        REG_RX_CONFIG    				0x0D
#define        REG_FIFO_ADDR_PTR  				0x0D
#define        REG_RSSI_CONFIG   				0x0E
#define        REG_FIFO_TX_BASE_ADDR 		    0x0E
#define        REG_RSSI_COLLISION    			0x0F
#define        REG_FIFO_RX_BASE_ADDR   			0x0F
#define        REG_RSSI_THRESH    				0x10
#define        REG_FIFO_RX_CURRENT_ADDR   		0x10
#define        REG_RSSI_VALUE_FSK	    		0x11
#define        REG_IRQ_FLAGS_MASK    			0x11
#define        REG_RX_BW		    			0x12
#define        REG_IRQ_FLAGS	    			0x12
#define        REG_AFC_BW		    			0x13
#define        REG_RX_NB_BYTES	    			0x13
#define        REG_OOK_PEAK	    				0x14
#define        REG_RX_HEADER_CNT_VALUE_MSB  	0x14
#define        REG_OOK_FIX	    				0x15
#define        REG_RX_HEADER_CNT_VALUE_LSB  	0x15
#define        REG_OOK_AVG	 					0x16
#define        REG_RX_PACKET_CNT_VALUE_MSB  	0x16
#define        REG_RX_PACKET_CNT_VALUE_LSB  	0x17
#define        REG_MODEM_STAT	  				0x18
#define        REG_PKT_SNR_VALUE	  			0x19
#define        REG_AFC_FEI	  					0x1A
#define        REG_PKT_RSSI_VALUE	  			0x1A
#define        REG_AFC_MSB	  					0x1B
#define        REG_RSSI_VALUE_LORA	  			0x1B
#define        REG_AFC_LSB	  					0x1C
#define        REG_HOP_CHANNEL	  				0x1C
#define        REG_FEI_MSB	  					0x1D
#define        REG_MODEM_CONFIG1	 		 	0x1D
#define        REG_FEI_LSB	  					0x1E
#define        REG_MODEM_CONFIG2	  			0x1E
#define        REG_PREAMBLE_DETECT  			0x1F
#define        REG_SYMB_TIMEOUT_LSB  			0x1F
#define        REG_RX_TIMEOUT1	  				0x20
#define        REG_PREAMBLE_MSB_LORA  			0x20
#define        REG_RX_TIMEOUT2	  				0x21
#define        REG_PREAMBLE_LSB_LORA  			0x21
#define        REG_RX_TIMEOUT3	 				0x22
#define        REG_PAYLOAD_LENGTH_LORA		 	0x22
#define        REG_RX_DELAY	 					0x23
#define        REG_MAX_PAYLOAD_LENGTH 			0x23
#define        REG_OSC		 					0x24
#define        REG_HOP_PERIOD	  				0x24
#define        REG_PREAMBLE_MSB_FSK 			0x25
#define        REG_FIFO_RX_BYTE_ADDR 			0x25
#define        REG_PREAMBLE_LSB_FSK 			0x26
// added by C. Pham
#define        REG_MODEM_CONFIG3	  			0x26
// end
#define        REG_SYNC_CONFIG	  				0x27
#define        REG_SYNC_VALUE1	 				0x28
#define        REG_SYNC_VALUE2	  				0x29
#define        REG_SYNC_VALUE3	  				0x2A
#define        REG_SYNC_VALUE4	  				0x2B
#define        REG_SYNC_VALUE5	  				0x2C
#define        REG_SYNC_VALUE6	  				0x2D
#define        REG_SYNC_VALUE7	  				0x2E
#define        REG_SYNC_VALUE8	  				0x2F
#define        REG_PACKET_CONFIG1	  			0x30
#define        REG_PACKET_CONFIG2	  			0x31
#define        REG_DETECT_OPTIMIZE              0x31
#define        REG_PAYLOAD_LENGTH_FSK			0x32
#define        REG_NODE_ADRS	  				0x33
#define        REG_BROADCAST_ADRS	 		 	0x34
#define        REG_FIFO_THRESH	  				0x35
#define        REG_SEQ_CONFIG1	  				0x36
#define        REG_SEQ_CONFIG2	  				0x37
#define        REG_DETECTION_THRESHOLD          0x37
#define        REG_TIMER_RESOL	  				0x38
#define        REG_TIMER1_COEF	  				0x39
// added by C. Pham
#define        REG_SYNC_WORD                    0x39
//end
#define        REG_TIMER2_COEF	  				0x3A
#define        REG_IMAGE_CAL	  				0x3B
#define        REG_TEMP		  					0x3C
#define        REG_LOW_BAT	  					0x3D
#define        REG_IRQ_FLAGS1	  				0x3E
#define        REG_IRQ_FLAGS2	  				0x3F
#define        REG_DIO_MAPPING1	  				0x40
#define        REG_DIO_MAPPING2	  				0x41
#define        REG_VERSION	  					0x42
#define        REG_AGC_REF	  					0x43
#define        REG_AGC_THRESH1	  				0x44
#define        REG_AGC_THRESH2	  				0x45
#define        REG_AGC_THRESH3	  				0x46
#define        REG_PLL_HOP	  					0x4B
#define        REG_PA_DAC                       0x4D
#define        REG_TCXO		  					0x58
//#define        REG_PA_DAC		  				0x5A
#define        REG_PLL		  					0x5C
#define        REG_PLL_LOW_PN	  				0x5E
#define        REG_FORMER_TEMP	  				0x6C
#define        REG_BIT_RATE_FRAC	  			0x70

// added by C. Pham
// copied from LoRaMAC-Node
/*!
 * RegImageCal
 */
#define RF_IMAGECAL_AUTOIMAGECAL_MASK               0x7F
#define RF_IMAGECAL_AUTOIMAGECAL_ON                 0x80
#define RF_IMAGECAL_AUTOIMAGECAL_OFF                0x00  // Default

#define RF_IMAGECAL_IMAGECAL_MASK                   0xBF
#define RF_IMAGECAL_IMAGECAL_START                  0x40

#define RF_IMAGECAL_IMAGECAL_RUNNING                0x20
#define RF_IMAGECAL_IMAGECAL_DONE                   0x00  // Default

#define RF_IMAGECAL_TEMPCHANGE_HIGHER               0x08
#define RF_IMAGECAL_TEMPCHANGE_LOWER                0x00

#define RF_IMAGECAL_TEMPTHRESHOLD_MASK              0xF9
#define RF_IMAGECAL_TEMPTHRESHOLD_05                0x00
#define RF_IMAGECAL_TEMPTHRESHOLD_10                0x02  // Default
#define RF_IMAGECAL_TEMPTHRESHOLD_15                0x04
#define RF_IMAGECAL_TEMPTHRESHOLD_20                0x06

#define RF_IMAGECAL_TEMPMONITOR_MASK                0xFE
#define RF_IMAGECAL_TEMPMONITOR_ON                  0x00 // Default
#define RF_IMAGECAL_TEMPMONITOR_OFF                 0x01

// added by C. Pham
// The crystal oscillator frequency of the module
#define RH_LORA_FXOSC 32000000.0

// The Frequency Synthesizer step = RH_LORA_FXOSC / 2^^19
#define RH_LORA_FCONVERT  (524288 / RH_LORA_FXOSC)

// Frf = frf(Hz)*2^19/RH_LORA_FXOSC

const uint8_t FreqTab[19][3] =
{
{0x6C, 0x80, 0x00}, //434MHz 0
{0x6D, 0x00, 0x00}, //436MHz 1
{0x6D, 0x80, 0x00}, //438MHz 2
{0x6E, 0x00, 0x00}, //440MHz 3
{0x6E, 0x80, 0x00}, //442MHz 4

{0x6F, 0x00, 0x00}, //444MHz 5
{0x6F, 0x80, 0x00}, //446MHz 6

{0x70, 0x00, 0x00}, //448MHz 7
{0x70, 0x80, 0x00}, //450MHz 8

{0x71, 0x00, 0x00}, //452MHz 9
{0x71, 0x80, 0x00}, //454MHz 10

{0x72, 0x00, 0x00}, //456MHz 11
{0x72, 0x80, 0x00}, //458MHz 12

{0x73, 0x00, 0x00}, //460MHz 13
{0x73, 0x80, 0x00}, //462MHz 14

{0x74, 0x00, 0x00}, //464MHz 15
{0x74, 0x80, 0x00}, //466MHz 16

{0x75, 0x00, 0x00}, //468MHz 17
{0x75, 0x80, 0x00}, //470MHz 18
};
const uint8_t PowerTab[4] =
{
  0xFF,                   //20dbm
  0xFC,                   //17dbm
  0xF9,                   //14dbm
  0xF6,                   //11dbm
};
const uint8_t BwTab[10] =
{
//7.8KHz,10.4KHz,15.6KHz,20.8KHz,31.2KHz,41.7KHz,62.5KHz,125KHz,250KHz,500KHz
  0,1,2,3,4,5,6,7,8,9
};

const double SignalBwLog[] =
{
    5.0969100130080564143587833158265,
    5.397940008672037609572522210551,
    5.6989700043360188047862611052755
};

//LORA CODING RATE:
const uint8_t CR_5 = 0x01;
const uint8_t CR_6 = 0x02;
const uint8_t CR_7 = 0x03;
const uint8_t CR_8 = 0x04;

//LORA SPREADING FACTOR:
const uint8_t SF_6 = 0x06;
const uint8_t SF_7 = 0x07;
const uint8_t SF_8 = 0x08;
const uint8_t SF_9 = 0x09;
const uint8_t SF_10 = 0x0A;
const uint8_t SF_11 = 0x0B;
const uint8_t SF_12 = 0x0C;

//LORA MODES:
const uint8_t LORA_SLEEP_MODE = 0x80;
const uint8_t LORA_STANDBY_MODE = 0x81;
const uint8_t LORA_TX_MODE = 0x83;
const uint8_t LORA_RX_SINGLE_MODE = 0x85;
const uint8_t LORA_RX_CONTINUS_MODE = 0X86;

// added by C. Pham
const uint8_t LORA_CAD_MODE = 0x87;
#define LNA_MAX_GAIN                0x23
#define LNA_OFF_GAIN                0x00
#define LNA_LOW_GAIN		    	0x20
//end

const uint8_t LORA_STANDBY_FSK_REGS_MODE = 0xC1;

//FSK MODES:
const uint8_t FSK_SLEEP_MODE = 0x00;
const uint8_t FSK_STANDBY_MODE = 0x01;
const uint8_t FSK_TX_MODE = 0x03;
const uint8_t FSK_RX_MODE = 0x05;

//OTHER CONSTANTS:

const uint8_t HEADER_ON = 0;
const uint8_t HEADER_OFF = 1;
const uint8_t CRC_ON = 1;
const uint8_t CRC_OFF = 0;
const uint8_t LORA = 1;
const uint8_t FSK = 0;
const uint8_t BROADCAST_0 = 0x00;
const uint8_t MAX_LENGTH = 255;
const uint8_t MAX_PAYLOAD = 251;
const uint8_t MAX_LENGTH_FSK = 64;
const uint8_t MAX_PAYLOAD_FSK = 60;
//modified by C. Pham, 7 instead of 5 because we added a type field which should be PKT_TYPE_ACK and the SNR
const uint8_t ACK_LENGTH = 7;
// added by C. Pham
#ifdef W_NET_KEY
const uint8_t NET_KEY_LENGTH=2;
const uint8_t OFFSET_PAYLOADLENGTH = 4+NET_KEY_LENGTH;
const uint8_t net_key_0 = 0x12;
const uint8_t net_key_1 = 0x34;
#else
// modified by C. Pham to remove the retry field
const uint8_t OFFSET_PAYLOADLENGTH = 4;
#endif
const uint8_t OFFSET_RSSI = 137;
const uint8_t NOISE_FIGURE = 6.0;
const uint8_t NOISE_ABSOLUTE_ZERO = 174.0;
const uint16_t MAX_TIMEOUT = 8000;		//8000 msec = 8.0 sec
const uint16_t MAX_WAIT = 12000;		//12000 msec = 12.0 sec
const uint8_t MAX_RETRIES = 5;
const uint8_t CORRECT_PACKET = 0;
const uint8_t INCORRECT_PACKET = 1;

// added by C. Pham
// Packet type definition

#define PKT_TYPE_MASK   0xF0
#define PKT_FLAG_MASK   0x0F

#define PKT_TYPE_DATA   0x10
#define PKT_TYPE_ACK    0x20

#define PKT_FLAG_ACK_REQ            0x08
#define PKT_FLAG_DATA_ENCRYPTED     0x04
#define PKT_FLAG_DATA_WAPPKEY       0x02
#define PKT_FLAG_DATA_ISBINARY      0x01

#define SX1272_ERROR_ACK        3
#define SX1272_ERROR_TOA        4
typedef unsigned char byte ;
typedef bool          boolean;
enum Lora_State{
		standby = 0,
		rx,
		tx,
		cad

	};
class sx1278_gpio{
public:
    sx1278_gpio(unsigned int r, unsigned int d0) : \
        reset(r), dio0(d0)
    {
        gpio_export(r);
        gpio_export(d0);
        gpio_set_dir(r, 1);//set out
        gpio_set_value(r, 1);//set high
        gpio_set_dir(d0, 0);//set in
        gpio_set_edge(d0, "rising");
        dio0_fd = gpio_fd_open(d0);
    }
    ~sx1278_gpio()
    {
        gpio_unexport(reset);
        gpio_unexport(dio0);
    }
    unsigned int reset;
    unsigned int dio0;
    int dio0_fd;
};
struct Gpio_Mask{
    volatile bool gpio_2 = false;
    volatile bool gpio_3 = false;
    volatile bool gpio_4 = false;
    volatile bool gpio_5 = false;
};
extern Gpio_Mask* p_gpio_mask;
void enable_isr(int gpio, bool e);
struct arg_event_setting{
	arg_event_setting(uint8_t cr = 0x01, uint8_t sf = 0x09, uint16_t bw = 0x08,\
		uint32_t ch = 0x6C5333) : m_cr(cr), m_sf(sf), m_bw(bw), m_ch(ch){}
    arg_event_setting(const arg_event_setting& a)
    {
        m_cr = a.m_cr;
        m_sf = a.m_sf;
        m_bw = a.m_bw;
        m_ch = a.m_ch;
    }
	uint8_t m_cr;
	uint8_t m_sf;
	uint16_t m_bw;
	uint32_t m_ch;
};

class SX1278{
public:
   // SX1278(arguement *arg);
    SX1278(Spi *spi, sx1278_gpio *gpio) : p_spi(spi), p_gpio(gpio){}

    int waitForInterrupt(int ms)
    {
        return gpio_wait_for_int(p_gpio->dio0_fd, ms);
    }

	uint8_t  readRegister(uint8_t address);
	void     writeRegister(uint8_t address,uint8_t data);
    void     RxChainCalibration(void);
	uint8_t  Init();
    void     setting(const arg_event_setting *a);
    void     setLORA();
    void     clearFlags();
    void     setFlagMask(uint8_t mask);
    void     setSF(uint8_t spr);
    void     setBW(uint8_t b);
    void     setCR(uint8_t cod);
    void     setHeaderON();
    void     setHeaderOFF();
    void     setCRC_ON();
    void     setPreambleLength(uint16_t l);
    void     setPower(uint8_t p);
    void     setChannel(const uint8_t * tab);
    void     sendSinglePayload(void *buf, uint8_t packet_size);
    void     entryStandby();
    void     entryCAD();
    void     entryRx();
    uint8_t  recSinglePackage(void *buf);

	Spi *p_spi;
    sx1278_gpio *p_gpio;
    uint8_t m_channel = 0;
    Lora_State lora_state;
//    char  txbuf[MAX_PAYLOAD]={'h','e','l','y','\0'};
//    char  rxbuf[MAX_PAYLOAD];

private:
	byte _modem;
	byte _bandwidth;
	byte _codingRate;
	byte _spreadingFactor;
	byte _header;
	byte _CRC;
	uint32_t _channel;
	byte _power;
	uint16_t _preamblelength;
	uint16_t _payloadlength;
	int8_t _SNR;
	int8_t _RSSI;
	int16_t _RSSIpacket;
	byte _maxCurrent;
	byte _reception;
    byte _diomap1;
    byte _diomap2;
};



#endif
