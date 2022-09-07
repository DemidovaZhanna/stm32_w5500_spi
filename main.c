// Работающий вариант программы для SPI1 STM32F103C8T6 и W5500 на скорости 18MГц??? у PCLK2 максимальная скорость 72MHz (для SPI1) следовательно нужен делитель 4
// заставила работать NSS

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
    // Используемые полярность и фаза тактового сигнала; должны быть одинаковыми и для ведущего, и для ведомого устройства.
    const uint32_t CPOL = SPI_CR1_CPOL*0;
    const uint32_t CPHA = SPI_CR1_CPHA*1;

    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPAEN;

    //led
    GPIOC->CRH &= ~(uint32_t)(0xf<<20);
    GPIOC->CRH |= 0x02 << 20;

    // PA4 (NSS), SPI1_NSS: alt. out, push-pull, high speed
    // PA5 (SCK), SPI1_SCK: alt. out, push-pull, high speed
    // PA6 (MISO), SPI1_MISO: input, pull up/down
    // PA7 (MOSI), SPI1_MOSI: alt. out, push-pull, high speed
    GPIOA->CRL = GPIOA->CRL & ~0xFFFF0000 | 0xB8BB0000;

    // Настраиваем подтяжку входа PA6 (SPI1_MISO) - к высокому уровню
    // (если вход окажется не подключён, SPI будет получать все единичные биты; вообще использование подтяжки необязательно).
    GPIOA->BSRR = GPIO_BSRR_BS6;

    // --- SPI1 setup ----
    SPI1->CR1 = SPI_CR1_MSTR | SPI_CR1_BR_2 | CPOL | CPHA;

    // С помощью регистра CR2 настраиваем генерацию запросов на прерывание и DMA (если нужно)
    SPI1->CR2 &= ~0xE7;           // Сбрасываем все значимые биты регистра.
    SPI1->CR2 |= SPI_CR2_SSOE;    // NSS будет выходом.

    // Включаем SPI1.
    // Передача не начнётся, пока не запишем что-то в регистр данных, но установится состояние выходов SPI (выходы переходят из Z-состояния в состояние формирования выходного сигнала).
    // После этого можно будет включить ведомое устройство без опасения, что оно получит некоторое количество мусорных данных в процессе включения ведущего.
    SPI1->CR1 |= SPI_CR1_SPE;

    blink_led(0xf0);
    dummy_loop(6000000);

    uint8_t i1 = W5500_Write_ReadByte(0);
    uint8_t i2 = W5500_Write_ReadByte(0x2E);
    uint8_t i3 = W5500_Write_ReadByte(0x01);
    uint8_t i = W5500_Write_ReadByte(0xff);

    blink_led(i1);
    blink_led(i2);
    blink_led(i3);
    blink_led(i);
}