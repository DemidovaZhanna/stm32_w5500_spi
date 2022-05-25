#include "stm32f10x.h"
#define VECT_TAB_OFFSET  0x0

#ifdef SYSCLK_FREQ_HSE
static void SetSysClockToHSE(void)
{
  __IO uint32_t StartUpCounter = 0, HSEStatus;

  RCC->CR |= RCC_CR_HSEON;

  /* Wait till HSE is ready and if Time out is reached exit */
  do
  {
    HSEStatus = RCC->CR & RCC_CR_HSERDY;
    StartUpCounter++;
  } while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

  if ((RCC->CR & RCC_CR_HSERDY) != RESET)
      HSEStatus = (uint32_t)0x01;
  else
      HSEStatus = (uint32_t)0x00;

  if (HSEStatus == (uint32_t)0x01)
  {
    /* Enable Prefetch Buffer */
    FLASH->ACR |= FLASH_ACR_PRFTBE;

    /* Flash 0 wait state */
    FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
    FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_0;

    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;      // HCLK = SYSCLK = PCLK2;
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV1;     // PCLK1 = HCLK; ТОЧНО ЛИ ШИНА APB1 НЕ ДОЛЖНА БЫТЬ МЕДЛЕННЕ APB2???

    /* Select HSE as system clock source */
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_HSE;

    /* Wait till HSE is used as system clock source */
    while ((RCC->CFGR & RCC_CFGR_SWS) != (uint32_t)0x04);
  }
  else
  { /* If HSE fails to start-up, the application will have wrong clock
         configuration. User can add here some code to deal with this error */
  }
}

#elif defined SYSCLK_FREQ_72MHz
static void SetSysClockTo72(void)
{
    RCC->CR |= ((uint32_t)RCC_CR_HSEON);

    while (!(RCC->CR & RCC_CR_HSERDY));     // Wait till HSE is ready and if Time out is reached exit

    FLASH->ACR |= FLASH_ACR_PRFTBE;         // Enable Prefetch Buffer

    /* Flash 2 wait state */
    FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
    FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_2;                        // задержка для частоты SYSCLK = 72MHz

    RCC->CFGR |= (uint32_t)(RCC_CFGR_HPRE_DIV1 | RCC_CFGR_PPRE1_DIV2);  // HCLK = SYSCLK = PCLK2; PCLK1 = HCLK/2

    /*  PLL configuration: PLLCLK = HSE * 9 = 72 MHz */
    RCC->CFGR &= (uint32_t)(~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE |
                                        RCC_CFGR_PLLMULL));
    RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLMULL9);

    RCC->CR |= RCC_CR_PLLON; // ДЕЛАЕТСЯ ПОСЛЕ УСТАНОВКИ PLLMUL и PLLSRC

    while(!(RCC->CR & RCC_CR_PLLRDY));      // Wait till PLL is ready

    /* Select PLL as system clock source */
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;

    /* Wait till PLL is used as system clock source */
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08);
}
#endif

void SystemInit (void) // есть ли принципиальная разница в какой последовательности сбрасывать биты?
{
    RCC->CR |= (uint32_t)RCC_CR_HSION;

    RCC->CFGR &= (uint32_t)0xF8FF0000;        // Reset SW, HPRE, PPRE1, PPRE2, ADCPRE and MCO bits
    RCC->CR &= (uint32_t)0xFEF2FFFF;          // Reset HSEON, CSSON, PLLON and HSEBYP bits
    RCC->CFGR &= (uint32_t)0xFF80FFFF;        // Reset PLLSRC, PLLXTPRE, PLLMUL and USBPRE/OTGFSPRE bits

    RCC->CIR = 0x009F0000;                    // Disable all interrupts and clear pending bits

#ifdef SYSCLK_FREQ_HSE
    SetSysClockToHSE();
#elif defined SYSCLK_FREQ_72MHz
    SetSysClockTo72();
#endif

    SCB->VTOR = SRAM_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal SRAM. */
}

