#pragma once
#include <arm/systick.hpp>
#include <bitfilled.hpp>
#include <st/stm32cmsis.h>

struct FLASH_t
{
    using mmr_ops = BITFILLED_FLASH_BB_OPS;
    struct ACR_t : BF_MMREG(uint32_t, rw, mmr_ops)
    {
        BF_COPY_SUPERCLASS(ACR_t);
        BF_MMREGBITS(uint32_t, rw, 0, 2) LATENCY;
        BF_MMREGBITS(uint32_t, rw, 3, 3) HLFCYA;
        BF_MMREGBITS(uint32_t, rw, 4, 4) PRFTBE;
        BF_MMREGBITS(uint32_t, r, 5, 5) PRFTBS;
    } ACR;
    struct KEYR_t : BF_MMREG(uint32_t, w, mmr_ops)
    {
        BF_COPY_SUPERCLASS(KEYR_t);
        BF_MMREGBITS(uint32_t, w, 0, 31) KEY;
    } KEYR;
    struct OPTKEYR_t : BF_MMREG(uint32_t, w, mmr_ops)
    {
        BF_COPY_SUPERCLASS(OPTKEYR_t);
        BF_MMREGBITS(uint32_t, w, 0, 31) OPTKEY;
    } OPTKEYR;
    struct SR_t : BF_MMREG(uint32_t, rw, mmr_ops)
    {
        BF_COPY_SUPERCLASS(SR_t);
        BF_MMREGBITS(uint32_t, rw, 5, 5) EOP;
        BF_MMREGBITS(uint32_t, rw, 4, 4) WRPRTERR;
        BF_MMREGBITS(uint32_t, rw, 2, 2) PGERR;
        BF_MMREGBITS(uint32_t, r, 0, 0) BSY;
    } SR;
    struct CR_t : BF_MMREG(uint32_t, rw, mmr_ops)
    {
        BF_COPY_SUPERCLASS(CR_t);
        BF_MMREGBITS(uint32_t, rw, 0, 0) PG;
        BF_MMREGBITS(uint32_t, rw, 1, 1) PER;
        BF_MMREGBITS(uint32_t, rw, 2, 2) MER;
        BF_MMREGBITS(uint32_t, rw, 4, 4) OPTPG;
        BF_MMREGBITS(uint32_t, rw, 5, 5) OPTER;
        BF_MMREGBITS(uint32_t, rw, 6, 6) STRT;
        BF_MMREGBITS(uint32_t, rw, 7, 7) LOCK;
        BF_MMREGBITS(uint32_t, rw, 9, 9) OPTWRE;
        BF_MMREGBITS(uint32_t, rw, 10, 10) ERRIE;
        BF_MMREGBITS(uint32_t, rw, 12, 12) EOPIE;
    } CR;
    struct AR_t : BF_MMREG(uint32_t, w, mmr_ops)
    {
        BF_COPY_SUPERCLASS(AR_t);
        BF_MMREGBITS(uint32_t, w, 0, 31) FAR;
    } AR;
    BF_MMREG_RESERVED(4, 1)
    struct OBR_t : BF_MMREG(uint32_t, r, mmr_ops)
    {
        BF_COPY_SUPERCLASS(OBR_t);
        BF_MMREGBITS(uint32_t, r, 0, 0) OPTERR;
        BF_MMREGBITS(uint32_t, r, 1, 1) RDPRT;
        BF_MMREGBITS(uint32_t, r, 2, 2) WDG_SW;
        BF_MMREGBITS(uint32_t, r, 3, 3) nRST_STOP;
        BF_MMREGBITS(uint32_t, r, 4, 4) nRST_STDBY;
        BF_MMREGBITS(uint32_t, r, 10, 17) Data0;
        BF_MMREGBITS(uint32_t, r, 18, 25) Data1;
    } OBR;
    struct WRPR_t : BF_MMREG(uint32_t, r, mmr_ops)
    {
        BF_COPY_SUPERCLASS(WRPR_t);
        BF_MMREGBITS(uint32_t, r, 0, 31) WRP;
    } WRPR;

    // single instance peripheral, optimize code size by using static functions
    static volatile FLASH_t& self() { return *reinterpret_cast<volatile FLASH_t*>(FLASH_R_BASE); }

    static void unlock()
    {
        if (self().CR.LOCK)
        {
            self().KEYR.KEY = FLASH_KEY1;
            self().KEYR.KEY = FLASH_KEY2;
        }
    }

    static void lock() { self().CR.LOCK = 1; }

    static uint32_t errors() { return self().SR & (FLASH_SR_PGERR | FLASH_SR_WRPRTERR); }
    static void clear_errors() { self().SR = FLASH_SR_EOP | FLASH_SR_PGERR | FLASH_SR_WRPRTERR; }

    static bool wait_for_completion(timeout timeout_ticks = timeout(50000))
    {
        while (self().SR.BSY)
        {
            if (timeout_ticks)
            {
                return false;
            }
        }
        return true;
    }

    static bool page_erase(uint32_t page_address, timeout timeout_ticks = timeout(50000))
    {
        self().CR.PER = 1;
        self().AR.FAR = page_address;
        self().CR.STRT = 1;
        bool ok = wait_for_completion(timeout_ticks);
        if (ok)
        {
            ok = self().SR.EOP;
            clear_errors();
        }
        self().CR.PER = 0;
        return ok;
    }

    static bool program(uint32_t address, const uint16_t* data, size_t size,
                        timeout timeout_ticks = timeout(50000))
    {
        self().CR.PG = 1;
        bool ok = true;
        for (size_t i = 0; i < (size + 1) / sizeof(uint16_t); ++i)
        {
            *reinterpret_cast<volatile uint16_t*>(address + i * 2) = data[i];
            ok = wait_for_completion(timeout_ticks);
            if (ok)
            {
                ok = self().SR.EOP;
                clear_errors();
            }
            if (!ok)
            {
                break;
            }
        }
        self().CR.PG = 0;
        return ok;
    }
};

#undef FLASH
inline auto& FLASH = *reinterpret_cast<volatile FLASH_t*>(FLASH_R_BASE);

inline const volatile uint16_t& FLASH_SIZE =
    *reinterpret_cast<const volatile uint16_t*>(0x1FFFF7E0);
