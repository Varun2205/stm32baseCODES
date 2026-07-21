#include "uart.h"

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

int __io_putchar(int ch) {
    uart2_write(ch);
    return ch;
}

void uart2_tx_init(void)
{
    /* 1. Enable clock access to GPIOA */
    RCC->AHB1ENR |= GPIOAEN;

    /* 2. Set PA2 mode to alternate function mode (MODER[5:4] = 10) */
    GPIOA->MODER &= ~(1U<<4);
    GPIOA->MODER |=  (1U<<5);

    /* 3. Set PA2 alternate function type to USART2_TX (AF7 = 0111) */
    GPIOA->AFR[0] |=  (1U<<8);
    GPIOA->AFR[0] |=  (1U<<9);
    GPIOA->AFR[0] |=  (1U<<10);
    GPIOA->AFR[0] &= ~(1U<<11);

    /* 4. Enable clock access to UART2 */
    RCC->APB1ENR |= UART2EN;

    /* 5. Configure baudrate */
    uart_set_baudrate(USART2, APB1_CLK, UART_BAUDRATE);

    /* 6. Configure transfer direction (Transmitter Enable) */
    USART2->CR1 = CR1_TE;

    /* 7. Enable UART module */
    USART2->CR1 |= CR1_UE;
}

void uart2_write(int ch)
{
    /* Wait until Transmit Data Register is empty */
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
