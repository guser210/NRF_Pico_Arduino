#include "pico/stdlib.h"
#include "NRF24.h"
#include "HM11.H"
#include "stdio.h"

int main()
{
        gpio_init(25); // led

        gpio_set_dir(25,1);

        sleep_ms(100); // give time for things to settle.

        NRF24 nrf = NRF24(spi1,9,8);
        nrf.config((uint8_t*)"gyroc",2,10); // Name=gyroc, channel=2,messagSize=10

        nrf.modeTX(); // <---- Set as transmitter.

        char message[7];
        uint16_t counter = 0;

        while(1)  
        {
            sprintf(message,"%d",counter++);
            nrf.sendMessage((uint8_t*)message);
            sleep_ms(100);
        } 
        
    return 0;
}