#pragma once
#include <bitfilled.hpp>
#include <st/stm32cmsis.h>

struct RCC_t
{
    using mmr_ops = BITFILLED_BB_OPS;
    struct CR_t : BF_MMREG(uint32_t, rw, mmr_ops)
    {
        BF_COPY_SUPERCLASS(CR_t);
        BF_MMREGBITS(uint32_t, rw, 0, 0) HSION;
        BF_MMREGBITS(uint32_t, r, 1, 1) HSIRDY;
        BF_MMREGBITS(uint32_t, rw, 3, 7) HSITRIM;
        BF_MMREGBITS(uint32_t, r, 8, 15) HSICAL;
        BF_MMREGBITS(uint32_t, rw, 16, 16) HSEON;
        BF_MMREGBITS(uint32_t, r, 17, 17) HSERDY;
        BF_MMREGBITS(uint32_t, rw, 18, 18) HSEBYP;
        BF_MMREGBITS(uint32_t, rw, 19, 19) CSSON;
        BF_MMREGBITS(uint32_t, rw, 24, 24) PLLON;
        BF_MMREGBITS(uint32_t, r, 25, 25) PLLRDY;
    } CR;
    struct CFGR_t : BF_MMREG(uint32_t, rw, mmr_ops)
    {
        BF_COPY_SUPERCLASS(CFGR_t);
        BF_MMREGBITS(uint32_t, rw, 0, 1) SW;
        BF_MMREGBITS(uint32_t, r, 2, 3) SWS;
        BF_MMREGBITS(uint32_t, rw, 4, 7) HPRE;
        BF_MMREGBITS(uint32_t, rw, 8, 10) PPRE1;
        BF_MMREGBITS(uint32_t, rw, 11, 13) PPRE2;
        BF_MMREGBITS(uint32_t, rw, 14, 15) ADCPRE;
        BF_MMREGBITS(uint32_t, rw, 16, 16) PLLSRC;
        BF_MMREGBITS(uint32_t, rw, 17, 17) PLLXTPRE;
        BF_MMREGBITS(uint32_t, rw, 18, 21) PLLMUL;
        BF_MMREGBITS(uint32_t, rw, 22, 22) OTGFSPRE;
        BF_MMREGBITS(uint32_t, rw, 24, 26) MCO;
    } CFGR;
    struct CIR_t : BF_MMREG(uint32_t, rw, mmr_ops)
    {
        BF_COPY_SUPERCLASS(CIR_t);
        BF_MMREGBITS(uint32_t, r, 0, 0) LSIRDYF;
        BF_MMREGBITS(uint32_t, r, 1, 1) LSERDYF;
        BF_MMREGBITS(uint32_t, r, 2, 2) HSIRDYF;
        BF_MMREGBITS(uint32_t, r, 3, 3) HSERDYF;
        BF_MMREGBITS(uint32_t, r, 4, 4) PLLRDYF;
        BF_MMREGBITS(uint32_t, r, 7, 7) CSSF;
        BF_MMREGBITS(uint32_t, rw, 8, 8) LSIRDYIE;
        BF_MMREGBITS(uint32_t, rw, 9, 9) LSERDYIE;
        BF_MMREGBITS(uint32_t, rw, 10, 10) HSIRDYIE;
        BF_MMREGBITS(uint32_t, rw, 11, 11) HSERDYIE;
        BF_MMREGBITS(uint32_t, rw, 12, 12) PLLRDYIE;
        BF_MMREGBITS(uint32_t, w, 16, 16) LSIRDYC;
        BF_MMREGBITS(uint32_t, w, 17, 17) LSERDYC;
        BF_MMREGBITS(uint32_t, w, 18, 18) HSIRDYC;
        BF_MMREGBITS(uint32_t, w, 19, 19) HSERDYC;
        BF_MMREGBITS(uint32_t, w, 20, 20) PLLRDYC;
        BF_MMREGBITS(uint32_t, w, 23, 23) CSSC;
    } CIR;
    struct APB2RSTR_t : BF_MMREG(uint32_t, rw, mmr_ops)
    {
        BF_COPY_SUPERCLASS(APB2RSTR_t);
        BF_MMREGBITS(uint32_t, rw, 0, 0) AFIORST;
        BF_MMREGBITS(uint32_t, rw, 2, 2) IOPARST;
        BF_MMREGBITS(uint32_t, rw, 3, 3) IOPBRST;
        BF_MMREGBITS(uint32_t, rw, 4, 4) IOPCRST;
        BF_MMREGBITS(uint32_t, rw, 5, 5) IOPDRST;
        BF_MMREGBITS(uint32_t, rw, 6, 6) IOPERST;
        BF_MMREGBITS(uint32_t, rw, 7, 7) IOPFRST;
        BF_MMREGBITS(uint32_t, rw, 8, 8) IOPGRST;
        BF_MMREGBITS(uint32_t, rw, 9, 9) ADC1RST;
        BF_MMREGBITS(uint32_t, rw, 10, 10) ADC2RST;
        BF_MMREGBITS(uint32_t, rw, 11, 11) TIM1RST;
        BF_MMREGBITS(uint32_t, rw, 12, 12) SPI1RST;
        BF_MMREGBITS(uint32_t, rw, 13, 13) TIM8RST;
        BF_MMREGBITS(uint32_t, rw, 14, 14) USART1RST;
        BF_MMREGBITS(uint32_t, rw, 15, 15) ADC3RST;
        BF_MMREGBITS(uint32_t, rw, 19, 19) TIM9RST;
        BF_MMREGBITS(uint32_t, rw, 20, 20) TIM10RST;
        BF_MMREGBITS(uint32_t, rw, 21, 21) TIM11RST;
    } APB2RSTR;
    struct APB1RSTR_t : BF_MMREG(uint32_t, rw, mmr_ops)
    {
        BF_COPY_SUPERCLASS(APB1RSTR_t);
        BF_MMREGBITS(uint32_t, rw, 0, 0) TIM2RST;
        BF_MMREGBITS(uint32_t, rw, 1, 1) TIM3RST;
        BF_MMREGBITS(uint32_t, rw, 2, 2) TIM4RST;
        BF_MMREGBITS(uint32_t, rw, 3, 3) TIM5RST;
        BF_MMREGBITS(uint32_t, rw, 4, 4) TIM6RST;
        BF_MMREGBITS(uint32_t, rw, 5, 5) TIM7RST;
        BF_MMREGBITS(uint32_t, rw, 6, 6) TIM12RST;
        BF_MMREGBITS(uint32_t, rw, 7, 7) TIM13RST;
        BF_MMREGBITS(uint32_t, rw, 8, 8) TIM14RST;
        BF_MMREGBITS(uint32_t, rw, 11, 11) WWDGRST;
        BF_MMREGBITS(uint32_t, rw, 14, 14) SPI2RST;
        BF_MMREGBITS(uint32_t, rw, 15, 15) SPI3RST;
        BF_MMREGBITS(uint32_t, rw, 17, 17) USART2RST;
        BF_MMREGBITS(uint32_t, rw, 18, 18) USART3RST;
        BF_MMREGBITS(uint32_t, rw, 19, 19) UART4RST;
        BF_MMREGBITS(uint32_t, rw, 20, 20) UART5RST;
        BF_MMREGBITS(uint32_t, rw, 21, 21) I2C1RST;
        BF_MMREGBITS(uint32_t, rw, 22, 22) I2C2RST;
        BF_MMREGBITS(uint32_t, rw, 23, 23) USBRST;
        BF_MMREGBITS(uint32_t, rw, 25, 25) CANRST;
        BF_MMREGBITS(uint32_t, rw, 27, 27) BKPRST;
        BF_MMREGBITS(uint32_t, rw, 28, 28) PWRRST;
        BF_MMREGBITS(uint32_t, rw, 29, 29) DACRST;
    } APB1RSTR;
    struct AHBENR_t : BF_MMREG(uint32_t, rw, mmr_ops)
    {
        BF_COPY_SUPERCLASS(AHBENR_t);
        BF_MMREGBITS(uint32_t, rw, 0, 0) DMA1EN;
        BF_MMREGBITS(uint32_t, rw, 1, 1) DMA2EN;
        BF_MMREGBITS(uint32_t, rw, 2, 2) SRAMEN;
        BF_MMREGBITS(uint32_t, rw, 4, 4) FLITFEN;
        BF_MMREGBITS(uint32_t, rw, 6, 6) CRCEN;
        BF_MMREGBITS(uint32_t, rw, 8, 8) FSMCEN;
        BF_MMREGBITS(uint32_t, rw, 10, 10) SDIOEN;
    } AHBENR;
    struct APB2ENR_t : BF_MMREG(uint32_t, rw, mmr_ops)
    {
        BF_COPY_SUPERCLASS(APB2ENR_t);
        BF_MMREGBITS(uint32_t, rw, 0, 0) AFIOEN;
        BF_MMREGBITS(uint32_t, rw, 2, 2) IOPAEN;
        BF_MMREGBITS(uint32_t, rw, 3, 3) IOPBEN;
        BF_MMREGBITS(uint32_t, rw, 4, 4) IOPCEN;
        BF_MMREGBITS(uint32_t, rw, 5, 5) IOPDEN;
        BF_MMREGBITS(uint32_t, rw, 6, 6) IOPEEN;
        BF_MMREGBITS(uint32_t, rw, 7, 7) IOPFEN;
        BF_MMREGBITS(uint32_t, rw, 8, 8) IOPGEN;
        BF_MMREGBITS(uint32_t, rw, 9, 9) ADC1EN;
        BF_MMREGBITS(uint32_t, rw, 10, 10) ADC2EN;
        BF_MMREGBITS(uint32_t, rw, 11, 11) TIM1EN;
        BF_MMREGBITS(uint32_t, rw, 12, 12) SPI1EN;
        BF_MMREGBITS(uint32_t, rw, 13, 13) TIM8EN;
        BF_MMREGBITS(uint32_t, rw, 14, 14) USART1EN;
        BF_MMREGBITS(uint32_t, rw, 15, 15) ADC3EN;
        BF_MMREGBITS(uint32_t, rw, 19, 19) TIM9EN;
        BF_MMREGBITS(uint32_t, rw, 20, 20) TIM10EN;
        BF_MMREGBITS(uint32_t, rw, 21, 21) TIM11EN;
    } APB2ENR;
    struct APB1ENR_t : BF_MMREG(uint32_t, rw, mmr_ops)
    {
        BF_COPY_SUPERCLASS(APB1ENR_t);
        BF_MMREGBITS(uint32_t, rw, 0, 0) TIM2EN;
        BF_MMREGBITS(uint32_t, rw, 1, 1) TIM3EN;
        BF_MMREGBITS(uint32_t, rw, 2, 2) TIM4EN;
        BF_MMREGBITS(uint32_t, rw, 3, 3) TIM5EN;
        BF_MMREGBITS(uint32_t, rw, 4, 4) TIM6EN;
        BF_MMREGBITS(uint32_t, rw, 5, 5) TIM7EN;
        BF_MMREGBITS(uint32_t, rw, 6, 6) TIM12EN;
        BF_MMREGBITS(uint32_t, rw, 7, 7) TIM13EN;
        BF_MMREGBITS(uint32_t, rw, 8, 8) TIM14EN;
        BF_MMREGBITS(uint32_t, rw, 11, 11) WWDGEN;
        BF_MMREGBITS(uint32_t, rw, 14, 14) SPI2EN;
        BF_MMREGBITS(uint32_t, rw, 15, 15) SPI3EN;
        BF_MMREGBITS(uint32_t, rw, 17, 17) USART2EN;
        BF_MMREGBITS(uint32_t, rw, 18, 18) USART3EN;
        BF_MMREGBITS(uint32_t, rw, 19, 19) UART4EN;
        BF_MMREGBITS(uint32_t, rw, 20, 20) UART5EN;
        BF_MMREGBITS(uint32_t, rw, 21, 21) I2C1EN;
        BF_MMREGBITS(uint32_t, rw, 22, 22) I2C2EN;
        BF_MMREGBITS(uint32_t, rw, 23, 23) USBEN;
        BF_MMREGBITS(uint32_t, rw, 25, 25) CANEN;
        BF_MMREGBITS(uint32_t, rw, 27, 27) BKPEN;
        BF_MMREGBITS(uint32_t, rw, 28, 28) PWREN;
        BF_MMREGBITS(uint32_t, rw, 29, 29) DACEN;
    } APB1ENR;
    struct BDCR_t : BF_MMREG(uint32_t, rw, mmr_ops)
    {
        BF_COPY_SUPERCLASS(BDCR_t);
        BF_MMREGBITS(uint32_t, rw, 0, 0) LSEON;
        BF_MMREGBITS(uint32_t, r, 1, 1) LSERDY;
        BF_MMREGBITS(uint32_t, rw, 2, 2) LSEBYP;
        BF_MMREGBITS(uint32_t, rw, 8, 9) RTCSEL;
        BF_MMREGBITS(uint32_t, rw, 15, 15) RTCEN;
        BF_MMREGBITS(uint32_t, rw, 16, 16) BDRST;
    } BDCR;
    struct CSR_t : BF_MMREG(uint32_t, rw, mmr_ops)
    {
        BF_COPY_SUPERCLASS(CSR_t);
        BF_MMREGBITS(uint32_t, rw, 0, 0) LSION;
        BF_MMREGBITS(uint32_t, r, 1, 1) LSIRDY;
        BF_MMREGBITS(uint32_t, rw, 24, 24) RMVF;
        BF_MMREGBITS(uint32_t, rw, 26, 26) PINRSTF;
        BF_MMREGBITS(uint32_t, rw, 27, 27) PORRSTF;
        BF_MMREGBITS(uint32_t, rw, 28, 28) SFTRSTF;
        BF_MMREGBITS(uint32_t, rw, 29, 29) IWDGRSTF;
        BF_MMREGBITS(uint32_t, rw, 30, 30) WWDGRSTF;
        BF_MMREGBITS(uint32_t, rw, 31, 31) LPWRRSTF;
    } CSR;
};

#undef RCC
inline auto& RCC = *reinterpret_cast<volatile RCC_t*>(RCC_BASE);
