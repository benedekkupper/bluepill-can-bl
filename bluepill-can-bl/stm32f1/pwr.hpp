#pragma once
#include <bitfilled.hpp>
#include <st/stm32cmsis.h>

struct PWR_t
{
    using mmr_ops = BITFILLED_BB_OPS;
    struct CR_t : BF_MMREG(uint32_t, rw, mmr_ops)
    {
        BF_COPY_SUPERCLASS(CR_t);
        BF_MMREGBITS(uint32_t, rw, 0, 0) LPDS;
        BF_MMREGBITS(uint32_t, rw, 1, 1) PDDS;
        BF_MMREGBITS(uint32_t, rw, 2, 2) CWUF;
        BF_MMREGBITS(uint32_t, rw, 3, 3) CSBF;
        BF_MMREGBITS(uint32_t, rw, 4, 4) PVDE;
        BF_MMREGBITS(uint32_t, rw, 5, 7) PLS;
        BF_MMREGBITS(uint32_t, rw, 8, 8) DBP;
    } CR;
    struct CSR_t : BF_MMREG(uint32_t, rw, mmr_ops)
    {
        BF_COPY_SUPERCLASS(CSR_t);
        BF_MMREGBITS(uint32_t, r, 0, 0) WUF;
        BF_MMREGBITS(uint32_t, r, 1, 1) SBF;
        BF_MMREGBITS(uint32_t, r, 2, 2) PVDO;
        BF_MMREGBITS(uint32_t, rw, 8, 8) EWUP;
    } CSR;
};

#undef PWR
inline auto& PWR = *reinterpret_cast<volatile PWR_t*>(PWR_BASE);
