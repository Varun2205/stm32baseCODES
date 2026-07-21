#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "uart.h"

#define GPIOAEN     (1U<<0)
#define GPIOA_5     (1U<<5)
#define LED_PIN     (GPIOA_5)

char key;

int main(void)
{
    /* Enable clock access to GPIOA */
    RCC->AHB1ENR |= GPIOAEN;

    /* SET PA5 as output PIN (MODER5[1:0] = 01) */
    GPIOA->MODER &= ~(1U<<11); // Clear bit 11
    GPIOA->MODER |=  (1U<<10); // Set bit 10

    uart2_rxtx_init();

    while(1){
        key = uart2_read();

        /* Compare with character '1' from terminal */
        if (key == '1'){
            GPIOA->ODR |= LED_PIN;  /* Turn LED ON */
        }
        else if (key == '0'){
            GPIOA->ODR &= ~LED_PIN; /* Turn LED OFF */
        }
    }
}
