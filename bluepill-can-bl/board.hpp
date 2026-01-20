#pragma once
#include "arm/systick.hpp"
#include "stm32f1/can.hpp"
#include "stm32f1/flash.hpp"
#include "stm32f1/gpio.hpp"
#include "stm32f1/pwr.hpp"
#include "stm32f1/rcc.hpp"

inline constexpr uint32_t system_core_clock = 72'000'000;
inline constexpr uint32_t can_peripheral_clock = system_core_clock / 2;

inline void sys_clocks_init()
{
    RCC.AHBENR.FLITFEN = 1; // enable flash interface clock

    RCC.CR.HSEON = 1; // enable HSE
    while (RCC.CR.HSERDY == 0)
        ; // wait until HSE ready
#if CONFIG_HSE_CLOCK_HZ == 16000000
    RCC.CFGR.PLLXTPRE = 1; // HSE / 2 = 8 MHz
#elif CONFIG_HSE_CLOCK_HZ == 8000000
#else
#error "Unsupported HSE clock frequency"
#endif
    RCC.CFGR.PLLSRC = 1; // HSE as PLL source
    RCC.CFGR.PLLMUL = 7; // 8 MHz * 9 = 72 MHz
    RCC.CR.PLLON = 1;    // enable
    while (RCC.CR.PLLRDY == 0)
        ; // wait until PLL ready

    FLASH.ACR.LATENCY = 2; // 2 wait states for 72 MHz
    FLASH.ACR.PRFTBE = 1;  // enable prefetch buffer

    RCC.CFGR.HPRE = 0;                                               // AHB = SYSCLK / 1
    RCC.CFGR.PPRE1 = RCC_CFGR_PPRE1_DIV2 >> RCC.CFGR.PPRE1.offset(); // APB1 = HCLK / 2
    RCC.CFGR.PPRE2 = 0;                                              // APB2 = HCLK / 1
    RCC.CFGR.SW = 2;                                                 // switch SYSCLK to PLL
    while (RCC.CFGR.SWS != 2)
        ; // wait until switch done

    SYSTICK.start(system_core_clock, 1000); // SysTick at 1ms interval
}

inline void can_pins_init()
{
    RCC.APB2ENR.IOPBEN = 1; // enable GPIOB clock
    RCC.APB2ENR.AFIOEN = 1; // enable AFIO clock
    __DSB();
    GPIOB.CRH.MODE8 = 0;    // PB8 as input (CAN RX)
    GPIOB.CRH.CNF8 = 0b01;  // PB8 as input floating
    GPIOB.CRH.MODE9 = 0b01; // PB9 as output (CAN TX)
    GPIOB.CRH.CNF9 = 0b10;  // PB9 as AF push-pull
    AFIO.MAPR.CAN_REMAP = 2;
}
