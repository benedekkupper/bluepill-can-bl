#pragma once
#include <bitfilled.hpp>
#include <st/stm32cmsis.h>

struct GPIO_t
{
    using mmr_ops = BITFILLED_BB_OPS;
    struct CRL_t : BF_MMREG(uint32_t, rw, mmr_ops)
    {
        BF_COPY_SUPERCLASS(CRL_t);
        BF_MMREGBITS(uint32_t, rw, 0, 1) MODE0;
        BF_MMREGBITS(uint32_t, rw, 2, 3) CNF0;
        BF_MMREGBITS(uint32_t, rw, 4, 5) MODE1;
        BF_MMREGBITS(uint32_t, rw, 6, 7) CNF1;
        BF_MMREGBITS(uint32_t, rw, 8, 9) MODE2;
        BF_MMREGBITS(uint32_t, rw, 10, 11) CNF2;
        BF_MMREGBITS(uint32_t, rw, 12, 13) MODE3;
        BF_MMREGBITS(uint32_t, rw, 14, 15) CNF3;
        BF_MMREGBITS(uint32_t, rw, 16, 17) MODE4;
        BF_MMREGBITS(uint32_t, rw, 18, 19) CNF4;
        BF_MMREGBITS(uint32_t, rw, 20, 21) MODE5;
        BF_MMREGBITS(uint32_t, rw, 22, 23) CNF5;
        BF_MMREGBITS(uint32_t, rw, 24, 25) MODE6;
        BF_MMREGBITS(uint32_t, rw, 26, 27) CNF6;
        BF_MMREGBITS(uint32_t, rw, 28, 29) MODE7;
        BF_MMREGBITS(uint32_t, rw, 30, 31) CNF7;
    } CRL;
    struct CRH_t : BF_MMREG(uint32_t, rw, mmr_ops)
    {
        BF_COPY_SUPERCLASS(CRH_t);
        BF_MMREGBITS(uint32_t, rw, 0, 1) MODE8;
        BF_MMREGBITS(uint32_t, rw, 2, 3) CNF8;
        BF_MMREGBITS(uint32_t, rw, 4, 5) MODE9;
        BF_MMREGBITS(uint32_t, rw, 6, 7) CNF9;
        BF_MMREGBITS(uint32_t, rw, 8, 9) MODE10;
        BF_MMREGBITS(uint32_t, rw, 10, 11) CNF10;
        BF_MMREGBITS(uint32_t, rw, 12, 13) MODE11;
        BF_MMREGBITS(uint32_t, rw, 14, 15) CNF11;
        BF_MMREGBITS(uint32_t, rw, 16, 17) MODE12;
        BF_MMREGBITS(uint32_t, rw, 18, 19) CNF12;
        BF_MMREGBITS(uint32_t, rw, 20, 21) MODE13;
        BF_MMREGBITS(uint32_t, rw, 22, 23) CNF13;
        BF_MMREGBITS(uint32_t, rw, 24, 25) MODE14;
        BF_MMREGBITS(uint32_t, rw, 26, 27) CNF14;
        BF_MMREGBITS(uint32_t, rw, 28, 29) MODE15;
        BF_MMREGBITS(uint32_t, rw, 30, 31) CNF15;
    } CRH;
    struct IDR_t : BF_MMREG(uint32_t, r, mmr_ops)
    {
        BF_COPY_SUPERCLASS(IDR_t);
        BF_MMREGBITSET(std::uint32_t, r, 1, 16, 0) ID;
    } IDR;
    struct ODR_t : BF_MMREG(uint32_t, rw, mmr_ops)
    {
        BF_COPY_SUPERCLASS(ODR_t);
        BF_MMREGBITSET(std::uint32_t, rw, 1, 16, 0) OD;
    } ODR;
    struct BSRR_t : BF_MMREG(uint32_t, w, mmr_ops)
    {
        BF_COPY_SUPERCLASS(BSRR_t);
        BF_MMREGBITSET(std::uint32_t, rw, 1, 16, 0) BS;
        BF_MMREGBITSET(std::uint32_t, rw, 1, 16, 16) BR;
    } BSRR;
    struct BRR_t : BF_MMREG(uint32_t, w, mmr_ops)
    {
        BF_COPY_SUPERCLASS(BRR_t);
        BF_MMREGBITSET(std::uint32_t, rw, 1, 16, 0) BR;
    } BRR;
    struct LCKR_t : BF_MMREG(uint32_t, rw, mmr_ops)
    {
        BF_COPY_SUPERCLASS(LCKR_t);
        BF_MMREGBITSET(std::uint32_t, rw, 1, 16, 0) LCK;
        BF_MMREGBITS(uint32_t, rw, 16, 16) LCKK;
    } LCKR;
};

#undef GPIOA
#undef GPIOB
#undef GPIOC
#undef GPIOD
inline auto& GPIOA = *reinterpret_cast<volatile GPIO_t*>(GPIOA_BASE);
inline auto& GPIOB = *reinterpret_cast<volatile GPIO_t*>(GPIOB_BASE);
inline auto& GPIOC = *reinterpret_cast<volatile GPIO_t*>(GPIOC_BASE);
inline auto& GPIOD = *reinterpret_cast<volatile GPIO_t*>(GPIOD_BASE);

struct AFIO_t
{
    using mmr_ops = BITFILLED_BB_OPS;
    struct EVCR_t : BF_MMREG(uint32_t, rw, mmr_ops)
    {
        BF_COPY_SUPERCLASS(EVCR_t);
        BF_MMREGBITS(uint32_t, rw, 0, 3) PIN;
        BF_MMREGBITS(uint32_t, rw, 4, 6) PORT;
        BF_MMREGBITS(uint32_t, rw, 7, 7) EVOE;
    } EVCR;
    struct MAPR_t : BF_MMREG(uint32_t, rw, mmr_ops)
    {
        BF_COPY_SUPERCLASS(MAPR_t);
        BF_MMREGBITS(uint32_t, rw, 0, 0) SPI1_REMAP;
        BF_MMREGBITS(uint32_t, rw, 1, 1) I2C1_REMAP;
        BF_MMREGBITS(uint32_t, rw, 2, 2) USART1_REMAP;
        BF_MMREGBITS(uint32_t, rw, 3, 3) USART2_REMAP;
        BF_MMREGBITS(uint32_t, rw, 4, 5) USART3_REMAP;
        BF_MMREGBITS(uint32_t, rw, 6, 7) TIM1_REMAP;
        BF_MMREGBITS(uint32_t, rw, 8, 9) TIM2_REMAP;
        BF_MMREGBITS(uint32_t, rw, 10, 11) TIM3_REMAP;
        BF_MMREGBITS(uint32_t, rw, 12, 12) TIM4_REMAP;
        BF_MMREGBITS(uint32_t, rw, 13, 14) CAN_REMAP;
        BF_MMREGBITS(uint32_t, rw, 15, 15) PD01_REMAP;
        BF_MMREGBITS(uint32_t, rw, 16, 16) TIM5CH4_IREMAP;
        BF_MMREGBITS(uint32_t, rw, 17, 17) ADC1_ETRGINJ_REMAP;
        BF_MMREGBITS(uint32_t, rw, 18, 18) ADC1_ETRGREG_REMAP;
        BF_MMREGBITS(uint32_t, rw, 19, 19) ADC2_ETRGINJ_REMAP;
        BF_MMREGBITS(uint32_t, rw, 20, 20) ADC2_ETRGREG_REMAP;
        BF_MMREGBITS(uint32_t, w, 24, 26) SWJ_CFG;
    } MAPR;
    struct EXTICR1_t : BF_MMREG(uint32_t, rw, mmr_ops)
    {
        BF_COPY_SUPERCLASS(EXTICR1_t);
        BF_MMREGBITS(uint32_t, rw, 0, 3) EXTI0;
        BF_MMREGBITS(uint32_t, rw, 4, 7) EXTI1;
        BF_MMREGBITS(uint32_t, rw, 8, 11) EXTI2;
        BF_MMREGBITS(uint32_t, rw, 12, 15) EXTI3;
    } EXTICR1;
    struct EXTICR2_t : BF_MMREG(uint32_t, rw, mmr_ops)
    {
        BF_COPY_SUPERCLASS(EXTICR2_t);
        BF_MMREGBITS(uint32_t, rw, 0, 3) EXTI4;
        BF_MMREGBITS(uint32_t, rw, 4, 7) EXTI5;
        BF_MMREGBITS(uint32_t, rw, 8, 11) EXTI6;
        BF_MMREGBITS(uint32_t, rw, 12, 15) EXTI7;
    } EXTICR2;
    struct EXTICR3_t : BF_MMREG(uint32_t, rw, mmr_ops)
    {
        BF_COPY_SUPERCLASS(EXTICR3_t);
        BF_MMREGBITS(uint32_t, rw, 0, 3) EXTI8;
        BF_MMREGBITS(uint32_t, rw, 4, 7) EXTI9;
        BF_MMREGBITS(uint32_t, rw, 8, 11) EXTI10;
        BF_MMREGBITS(uint32_t, rw, 12, 15) EXTI11;
    } EXTICR3;
    struct EXTICR4_t : BF_MMREG(uint32_t, rw, mmr_ops)
    {
        BF_COPY_SUPERCLASS(EXTICR4_t);
        BF_MMREGBITS(uint32_t, rw, 0, 3) EXTI12;
        BF_MMREGBITS(uint32_t, rw, 4, 7) EXTI13;
        BF_MMREGBITS(uint32_t, rw, 8, 11) EXTI14;
        BF_MMREGBITS(uint32_t, rw, 12, 15) EXTI15;
    } EXTICR4;
    BF_MMREG_RESERVED(4, 1)
    struct MAPR2_t : BF_MMREG(uint32_t, rw, mmr_ops)
    {
        BF_COPY_SUPERCLASS(MAPR2_t);
        BF_MMREGBITS(uint32_t, rw, 5, 5) TIM9_REMAP;
        BF_MMREGBITS(uint32_t, rw, 6, 6) TIM10_REMAP;
        BF_MMREGBITS(uint32_t, rw, 7, 7) TIM11_REMAP;
        BF_MMREGBITS(uint32_t, rw, 8, 8) TIM13_REMAP;
        BF_MMREGBITS(uint32_t, rw, 9, 9) TIM14_REMAP;
        BF_MMREGBITS(uint32_t, rw, 10, 10) FSMC_NADV;
    } MAPR2;
};

#undef AFIO
inline auto& AFIO = *reinterpret_cast<volatile AFIO_t*>(AFIO_BASE);
