#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx.h"

#define GPIOAEN         (1U<<0)
#define UART2EN         (1U<<17)

#define CR1_TE          (1U<<3)
#define CR1_UE          (1U<<13)
#define SR_TXE          (1U<<7)

#define SYS_FREQ        16000000
#define APB1_CLK        SYS_FREQ
#define UART_BAUDRATE   115200

static void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t PeriphClk, uint32_t BaudRate);
static uint16_t compute_uart_bd(uint32_t PeriphClk, uint32_t BaudRate);

void uar2_tx_init(void);
void uart2_write(int ch);

int __io_putchar(int ch) {
    uart2_write(ch);
    return ch;
}

int main(void)
{
    uar2_tx_init();

    while(1)
    {
        printf("Hello from stm32f4......\n\r");

        // Simple delay loop so you aren't spamming the terminal too fast
        for (volatile int i = 0; i < 500000; i++);
    }
}

void uar2_tx_init(void)
{
    /**************** Configure UART GPIO Pin ***************/
    /* Enable clock access to GPIOA */
    RCC->AHB1ENR |= GPIOAEN;

    /* Set PA2 mode to Alternate Function mode (0b10) */
    GPIOA->MODER &= ~(3U << (2 * 2)); // Clear bits 4 and 5
    GPIOA->MODER |=  (2U << (2 * 2)); // Set bit 5 (Alternate Function)

    /* Set PA2 alternate function type to AF7 (USART2_TX) */
    GPIOA->AFR[0] &= ~(0xFU << (4 * 2)); // Clear bits 8, 9, 10, 11
    GPIOA->AFR[0] |=  (7U   << (4 * 2)); // Set AF7 (0b0111)

    /**************** Configure UART Module ***************/
    /* Enable clock access to USART2 */
    RCC->APB1ENR |= UART2EN;

    /* Configure baudrate */
    uart_set_baudrate(USART2, APB1_CLK, UART_BAUDRATE);

    /* Configure transfer direction (Transmitter Enable) */
    USART2->CR1 = CR1_TE;

    /* Enable USART module */
    USART2->CR1 |= CR1_UE;
}

void uart2_write(int ch)
{
    /* MUST wait until Transmit Data Register is empty before writing */
    while (!(USART2->SR & SR_TXE)) {}

    /* Write to transmit data register */
    USART2->DR = (ch & 0xFF);
}

static void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t PeriphClk, uint32_t BaudRate)
{
    USARTx->BRR = compute_uart_bd(PeriphClk, BaudRate);
}

static uint16_t compute_uart_bd(uint32_t PeriphClk, uint32_t BaudRate)
{
    return ((PeriphClk + (BaudRate / 2U)) / BaudRate);
}
