#pragma once
#include <bitfilled.hpp>
#include <st/stm32cmsis.h>

struct SYSTICK_t
{
    using mmr_ops = ::bitfilled::base;
    struct CTRL_t : BF_MMREG(uint32_t, rw, mmr_ops)
    {
        BF_COPY_SUPERCLASS(CTRL_t);
        BF_MMREGBITS(uint32_t, r, 16, 16) COUNTFLAG;
        BF_MMREGBITS(uint32_t, rw, 2, 2) CLKSOURCE;
        BF_MMREGBITS(uint32_t, rw, 1, 1) TICKINT;
        BF_MMREGBITS(uint32_t, rw, 0, 0) ENABLE;
    } CTRL;
    struct LOAD_t : BF_MMREG(uint32_t, rw, mmr_ops)
    {
        BF_COPY_SUPERCLASS(LOAD_t);
        BF_MMREGBITS(uint32_t, rw, 0, 23) RELOAD;
    } LOAD;
    struct VAL_t : BF_MMREG(uint32_t, rw, mmr_ops)
    {
        BF_COPY_SUPERCLASS(VAL_t);
        BF_MMREGBITS(uint32_t, rw, 0, 23) CURRENT;
    } VAL;
    struct CALIB_t : BF_MMREG(uint32_t, r, mmr_ops)
    {
        BF_COPY_SUPERCLASS(CALIB_t);
        BF_MMREGBITS(uint32_t, r, 31, 31) NOREF;
        BF_MMREGBITS(uint32_t, r, 30, 30) SKEW;
        BF_MMREGBITS(uint32_t, r, 24, 31) TENMS;
    } CALIB;

    void start(uint32_t core_hz, uint32_t tick_hz) volatile
    {
        LOAD.RELOAD = core_hz / tick_hz - 1;
        CTRL.CLKSOURCE = 1; // use core clock
        CTRL.ENABLE = 1;    // enable SysTick
    }
} volatile& SYSTICK = *reinterpret_cast<volatile SYSTICK_t*>(SysTick_BASE);

class timeout
{
    uint32_t duration;

  public:
    constexpr explicit timeout(uint32_t d) : duration(d) {}
    operator bool()
    {
        duration -= SYSTICK.CTRL.COUNTFLAG;
        return duration == 0;
    }
};
