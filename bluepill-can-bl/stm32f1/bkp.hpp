#pragma once
#include "pwr.hpp"
#include "rcc.hpp"

struct BKP_t
{
    using mmr_ops = BITFILLED_BB_OPS;
    BF_MMREG_RESERVED(4, 1)
#if 0
    struct DR_t : BF_MMREG(uint32_t, rw, mmr_ops)
    {
        BF_COPY_SUPERCLASS(DR_t);
        BF_MMREGBITS(uint16_t, rw, 0, 15) D;
    } DR[10];
#else
    uint32_t DR[10];
#endif
    struct RTCCR_t : BF_MMREG(uint32_t, rw, mmr_ops)
    {
        BF_COPY_SUPERCLASS(RTCCR_t);
        BF_MMREGBITS(uint32_t, rw, 0, 6) CAL;
        BF_MMREGBITS(uint32_t, rw, 7, 7) CCO;
        BF_MMREGBITS(uint32_t, rw, 8, 8) ASOE;
        BF_MMREGBITS(uint32_t, rw, 9, 9) ASOS;
    } RTCCR;
    struct CR_t : BF_MMREG(uint32_t, rw, mmr_ops)
    {
        BF_COPY_SUPERCLASS(CR_t);
        BF_MMREGBITS(uint32_t, rw, 0, 0) TPE;
        BF_MMREGBITS(uint32_t, rw, 1, 1) TPAL;
    } CR;
    struct CSR_t : BF_MMREG(uint32_t, rw, mmr_ops)
    {
        BF_COPY_SUPERCLASS(CSR_t);
        BF_MMREGBITS(uint32_t, w, 0, 0) CTE;
        BF_MMREGBITS(uint32_t, w, 1, 1) CTI;
        BF_MMREGBITS(uint32_t, rw, 2, 2) TPIE;
        BF_MMREGBITS(uint32_t, r, 8, 8) TEF;
        BF_MMREGBITS(uint32_t, r, 9, 9) TIF;
    } CSR;

    static void enable_access()
    {
        RCC.APB1ENR.PWREN = 1; // enable PWR clock
        RCC.APB1ENR.BKPEN = 1; // enable BKP clock
        __DSB();
        PWR.CR.DBP = 1; // enable access to BKP domain
    }
    static void disable_access()
    {
        PWR.CR.DBP = 0; // disable access to BKP domain
    }
};

#undef BKP
inline auto& BKP = *reinterpret_cast<volatile BKP_t*>(BKP_BASE);
