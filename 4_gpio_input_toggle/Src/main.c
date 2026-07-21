#include "stm32f4xx.h"

#define GPIOAEN          (1U<<0)
#define GPIOCEN          (1U<<2)

#define PIN5             (1U<<5)
#define PIN13            (1U<<13)

#define LED_PIN          PIN5
#define BTN_PIN          PIN13

// Simple software delay function for debouncing
void delay_ms(uint32_t delay) {
    for (uint32_t i = 0; i < delay * 1000; i++) {
        __NOP(); // No operation
    }
}

int main(void) {
    // 1. Enable clock access to GPIOA and GPIOC
    RCC->AHB1ENR |= GPIOAEN;
    RCC->AHB1ENR |= GPIOCEN;

    // 2. Set PA5 as Output (01 in MODER5)
    GPIOA->MODER |=  (1U << 10);
    GPIOA->MODER &= ~(1U << 11);

    // 3. Set PC13 as Input (00 in MODER13)
    GPIOC->MODER &= ~(1U << 26);
    GPIOC->MODER &= ~(1U << 27);

    uint8_t led_state = 0;      // Tracks if LED is ON (1) or OFF (0)
    uint8_t button_last = 1;    // PC13 defaults to HIGH (1) due to hardware pull-up

    while (1) {
        // Read the current state of PC13 (0 = Pressed, 1 = Released)
        uint8_t button_current = (GPIOC->IDR & BTN_PIN) ? 1 : 0;

        // Detect falling edge: Button was 1 (released) and is now 0 (pressed)
        if (button_last == 1 && button_current == 0) {
            // Toggle LED state
            led_state ^= 1;

            if (led_state) {
                GPIOA->BSRR = LED_PIN;        // Turn LED ON (Set PA5)
            } else {
                GPIOA->BSRR = (LED_PIN << 16); // Turn LED OFF (Reset PA5)
            }

            // Debounce delay to prevent physical switch bounce from triggering multiple toggles
            delay_ms(50);
        }

        button_last = button_current;
    }
}
