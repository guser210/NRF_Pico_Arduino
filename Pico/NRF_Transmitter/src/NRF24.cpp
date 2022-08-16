#include "NRF24.h"
#include <string.h>


NRF24::NRF24(    spi_inst_t *spi, uint16_t csn, uint16_t ce  )
{
    this->spi = spi;
    this->csn = csn;
    this->ce = ce;
    
    // This init should be defined externally by the user.
    spi_init(this->spi, 8000000);
    gpio_set_function(10,GPIO_FUNC_SPI);
    gpio_set_function(11,GPIO_FUNC_SPI);
    gpio_set_function(12,GPIO_FUNC_SPI);
    // spi end.

    gpio_init(csn);
    gpio_init(ce);

    gpio_set_dir(csn,1);
    gpio_set_dir(ce,1);

    ceLow();
    csnHigh();
}

NRF24::~NRF24()
{
}

uint8_t NRF24::readReg(uint8_t reg)
{
    reg = 0b00011111 & reg;
    uint8_t result = 0;
    csnLow();
    spi_write_blocking(spi,&reg,1);
    spi_read_blocking(spi,0xff,&result,1);
    csnHigh();

    return result;
}
void NRF24::writeCommand(uint8_t cmd)
{
    csnLow();
    spi_write_blocking(spi, &cmd, 1);
    csnHigh();
}
void NRF24::writeReg(uint8_t reg, uint8_t data)
{
    writeReg(reg, &data,1);
}
void NRF24::writeReg(uint8_t reg, uint8_t *data, uint8_t size)
{
    //     Write bit|  Mask bits
    reg  = 0b00100000 | (0b00011111 & reg);
    csnLow();
    spi_write_blocking(spi, &reg,1);
    spi_write_blocking(spi,(uint8_t*)data,size);
    csnHigh();

}
void NRF24::enableAck(uint8_t ack)
{
    writeReg(1,ack);
}
void NRF24::config(uint8_t *address, uint8_t channel,uint8_t messageLen)
{
    //channel = 2;
    this->channel = channel;
    this->messageLen = messageLen;
    csnHigh();
    ceLow();
    sleep_ms(11);

    writeReg(0,0b00001110); // config CRC, 2Bytes CRD, Powerup.
    sleep_us(1500);

    writeReg(1,0b0); // disable ack.

    writeReg(3, 0b00000011); // 5 byte RX/TX address.

    writeReg(4, 0b00000000); // 250us AutoRetrans delay.

    writeReg(5, channel); // channel.

    writeReg(6, 0b00001110); // 2Mbs, 0dBm... max power.

    writeReg(0x0a,(uint8_t*)address,5);
    writeReg(0x10,(uint8_t*)address,5);
    
    writeReg(0x11, messageLen); // message Length.
}


void NRF24::modeTX()
{
    
    uint8_t config = readReg(0);
    config &= ~(1<<0); // clear PRIM_RX bit.
    writeReg(0,config);
    ceLow();

    sleep_us(130); // 130us settling time.
}

void NRF24::modeRX()
{
    uint8_t config = readReg(0);
    config |= (1<<0); // set PRIM_RX bit.
    writeReg(0,config);
    ceHigh();
    sleep_us(130); // 130us settling time.    
 }
uint8_t NRF24::newMessage()
{
    return ((readRegister(7) & 0b00001110) < 11);

}
void NRF24::sendMessage(uint8_t *data)
{
    uint8_t flush_tx = 0b11100001;
    uint8_t tx_payload = 0b10100000;
    uint8_t status = readReg(7);

    uint8_t buffer[32] = {0};
    memcpy(buffer,data,messageLen);

    if( status & 1)
    {
    	writeCommand(flush_tx);
    }
    if( status & 0b00110000)
        writeReg(7,0b00110000); // Clear Max_RT bit and dat send(TX_DS).


    csnLow();
    spi_write_blocking(spi,&tx_payload,1);
    spi_write_blocking(spi,buffer, messageLen);
    csnHigh();

    ceHigh();

    while( (readReg(7) & 0b00110000) == 0 ) 
    {
        // wait until data sent or max rt.
    }

    ceLow();
    writeReg(7,0b00110000);
    

    uint8_t observer = readReg(8);
    if( observer & 0b11110000)
    {
        packetsLost += (observer>>4); // keep track of packet lost.
        writeReg(5,channel); // clear PLOST_CNT by writing channel.
    }

}

void NRF24::getMessage(uint8_t *buffer)
{
    uint8_t rx_payload = 0b01100001;

    csnLow();
    spi_write_blocking(spi,&rx_payload,1);
    spi_read_blocking(spi,0xff,(uint8_t*)buffer,messageLen);
    csnHigh();
    
    writeReg(7, 0b01000000);

}

uint8_t NRF24::readRegister(uint8_t reg)
//uint8_t NRF24::readRegister(uint8_t reg)
{
    return readReg(reg);
}

