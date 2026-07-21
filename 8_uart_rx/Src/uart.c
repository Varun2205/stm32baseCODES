#include "uart.h"

#define GPIOAEN			(1U<<0)
#define UART2EN			(1U<<17)

#define CR1_TE			(1U<<3)
#define CR1_RE			(1U<<2)
#define CR1_UE			(1U<<13)

#define SR_TXE			(1U<<7)
#define SR_RXNE			(1U<<5)

#define SYS_FREQ		16000000
#define APB1_CLK		SYS_FREQ
#define UART_BAUDRATE	115200

static void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t PeriphClk, uint32_t BaudRate);
static uint16_t compute_uart_bd(uint32_t PeriphClk, uint32_t BaudRate);

void uart2_write(int ch);

int __io_putchar(int ch){
    uart2_write(ch);
    return ch;
}

void uart2_rxtx_init(void)
{
    /**************** Configure UART GPIO pins ***************/
    /* Enable clock access to GPIOA */
    RCC->AHB1ENR |= GPIOAEN;

    /* Set PA2 & PA3 mode to alternate function mode (10) */
    GPIOA->MODER &= ~((1U<<4) | (1U<<6));
    GPIOA->MODER |=  ((1U<<5) | (1U<<7));

    /* Set PA2 (TX) and PA3 (RX) alternate function type to AF07 */
    GPIOA->AFR[0] &= ~((0xFU << 8) | (0xFU << 12)); // Clear AF bits
    GPIOA->AFR[0] |=  ((0x7U << 8) | (0x7U << 12)); // Set AF07 (0111)

    /**************** Configure UART module ***************/
    /* Enable clock access to UART2 */
    RCC->APB1ENR |= UART2EN;

    /* Configure baudrate */
    uart_set_baudrate(USART2, APB1_CLK, UART_BAUDRATE);

    /* Configure transfer direction (Enable TX & RX) */
    USART2->CR1 = (CR1_RE | CR1_TE);

    /* Enable UART module */
    USART2->CR1 |= CR1_UE;
}

char uart2_read(void){
    /* Wait until Receive Data Register is NOT empty */
    while(!(USART2->SR & SR_RXNE)){}

    /* Read and return the data */
    return (char)(USART2->DR & 0xFF);
}

void uart2_write(int ch)
{
    /* Wait until Transmit Data Register is empty */
    while(!(USART2->SR & SR_TXE)){}

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
