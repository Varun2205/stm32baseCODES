#include "stm32f4xx.h"


#define GPIOAEN          (1U<<0)
#define GPIOCEN          (1U<<2)


#define PIN5             (1U<<5)
#define PIN13            (1U<<13)

#define LED_PIN          PIN5
#define BTN_PIN          PIN13




int main (void){
	/*
	RCC->AHB1ENR |= GPIOAEN;
	GPIOA->MODER |= (1U<<10);
	GPIOA->MODER &=~ (1U<<11);
	while (1){
		GPIOA->BSRR = LED_PIN;
		for(int i=0;i<100000;i++){}

		GPIOA->BSRR = (1U<<21);
		for(int i=0;i<100000;i++){}
	}
	*/

	//Enable clock access to GPIO A and GPIO C
	RCC->AHB1ENR |= GPIOAEN;
	RCC->AHB1ENR |= GPIOCEN;

	//Set PA5 as O/P pin
	GPIOA->MODER |= (1U<<10);
	GPIOA->MODER &=~ (1U<<11);

	//Set PC13 as I/P pin
	GPIOC->MODER &=~ (1U<<26);
	GPIOC->MODER &=~ (1U<<27);


	while (1){
		//Check if button is pressed
		if (GPIOC->IDR & BTN_PIN){
			//Turn on the LED
			GPIOA->BSRR = LED_PIN;
		}
		else{
			//Turn it off
			GPIOA->BSRR = (1U<<21);
		}



		/*// Uncomment this to make sure you can toggle edge triggered LED
		static uint8_t last = 1;
		uint8_t current = (GPIOC->IDR & BTN_PIN) ? 1 : 0;

		if (last == 1 && current == 0)
		{
		    GPIOA->ODR ^= LED_PIN;
		}
		last = current;
		/*
	}



}
