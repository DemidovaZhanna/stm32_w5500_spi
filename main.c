#include "stm32f10x.h"
#include "inc/w5500.h"

void  dummy_loop(uint32_t count)
{
    while(--count);
}

uint8_t W5500_Write_ReadByte(uint8_t byte)
{
    while (!(SPI1->SR & SPI_SR_TXE) || (SPI1->SR & SPI_SR_BSY));
    SPI1->DR = byte;
    while (!(SPI1->SR & SPI_SR_RXNE) || (SPI1->SR & SPI_SR_BSY));
    byte = SPI1->DR;

    return byte;
}

void blink_led(uint8_t byte)
{
    GPIOC->BSRR = GPIO_BSRR_BS13;
    dummy_loop(4000000);
    for (int i = 0; i < 8; i++)
    {
        if (byte & 0x80)
        {
            GPIOC->BRR = GPIO_BSRR_BS13;
            dummy_loop(6000000);
            GPIOC->BSRR = GPIO_BSRR_BS13;
            dummy_loop(1000000);
        }
        else
        {
            GPIOC->BRR = GPIO_BSRR_BS13;
            dummy_loop(1000000);
            GPIOC->BSRR = GPIO_BSRR_BS13;
            dummy_loop(3000000);
        }
        byte <<= 1;
    }
}

int main()
{
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPAEN;

    GPIOB->CRL &= ~(GPIO_CRL_CNF0 | GPIO_CRL_MODE0);
    GPIOB->CRH &= ~(GPIO_CRH_CNF9 | GPIO_CRH_MODE9);

    GPIOB->CRH |= 0x3<<4;

    //reset
    GPIOB->BRR = GPIO_BSRR_BS9;
    dummy_loop(1);
    GPIOB->BSRR = GPIO_BSRR_BS9;

    GPIOB->CRL |= 0x3;
    GPIOB->BSRR |= GPIO_BSRR_BS0;

    //led
    GPIOC->CRH &= ~(uint32_t)(0xf<<20);
    GPIOC->CRH |= 0x02 << 20;

    // Setup PA4, PA5, PA7
    GPIOA->CRL &= ~(uint32_t)(0xf<<16);             // clear mode for PA4   (NSS)
    GPIOA->CRL &= ~(uint32_t)(0xf<<20);             // clear mode for PA5   (SCK)
    GPIOA->CRL &= ~(uint32_t)(0xf<<24);             // clear mode for PA6   (MISO)
    GPIOA->CRL &= ~(uint32_t)(0xf<<28);             // clear mode for PA7   (MOSI)

    GPIOA->CRL |= 0xB<<16;              // for PA4 set  PushPull mode, 50MHz
    GPIOA->CRL |= 0xB<<20;              // for PA5 set
    GPIOA->CRL |= 0x8<<24;              // for PA6 set  input with pull-up/pull-down, 50MHz     MODE=00 for __in__
    GPIOA->CRL |= 0xB<<28;              // for PA7 set, 50MHz     MODE>00 for __out__

    // --- SPI1 setup ----
    SPI1->CR1 = SPI_CR1_SPE | SPI_CR1_MSTR | SPI_CR1_BR_2;

    blink_led(0xf0);
    dummy_loop(6000000);

    GPIOB->BRR |= GPIO_BSRR_BS0;

    uint8_t i1 = W5500_Write_ReadByte(0);
    uint8_t i2 = W5500_Write_ReadByte(0x2E);
    uint8_t i3 = W5500_Write_ReadByte(0x01);
    uint8_t i = W5500_Write_ReadByte(0xff);

    GPIOB->BSRR |= GPIO_BSRR_BS0;

    blink_led(i1);
    blink_led(i2);
    blink_led(i3);
    blink_led(i);
}