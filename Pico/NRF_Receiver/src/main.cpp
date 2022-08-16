#include "pico/stdlib.h"
#include "NRF24.h"
#include "HM11.H"
#include "stdio.h"

int main()
{
        gpio_init(15); // led
        gpio_set_dir(15,1);
        sleep_ms(100);

        HM11 phone(uart0,9600,16,17);

        NRF24 nrf = NRF24(spi1,9,8);
        nrf.config((uint8_t*)"gyroc",2,10); // Name=gyroc, channel=2,messagSize=10

        nrf.modeRX(); // <------ Set as Receiver.

        char message[7];
        char phoneMessage[1024];
        uint16_t counter = 0;

        while(1)  
        {

            if(nrf.newMessage() )
            {
                gpio_put(15,1); // Turn led on pin 20/gpio15 ON when message received.
                
                nrf.getMessage((uint8_t*)&message);
                sprintf(phoneMessage,"%s\r",message);
                phone.sendMessage(phoneMessage);
                sleep_ms(50);
            }
            else
            {
                gpio_put(15,0); // Turn led on pin 20/gpio15 OFF when message received.
            }
        } 
        
    return 0;
}