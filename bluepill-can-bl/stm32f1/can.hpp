#pragma once
#include <optional>
#include "arm/systick.hpp"
#include "can/frame.hpp"
#include <bitfilled.hpp>
#include <st/stm32cmsis.h>

struct CAN_t
{
    using mmr_ops = BITFILLED_BB_OPS;
    struct MCR_t : BF_MMREG(uint32_t, rw, mmr_ops)
    {
        BF_COPY_SUPERCLASS(MCR_t);
        BF_MMREGBITS(uint32_t, rw, 16, 16) DBF;
        BF_MMREGBITS(uint32_t, rw, 15, 15) RESET;
        BF_MMREGBITS(uint32_t, rw, 7, 7) TTCM;
        BF_MMREGBITS(uint32_t, rw, 6, 6) ABOM;
        BF_MMREGBITS(uint32_t, rw, 5, 5) AWUM;
        BF_MMREGBITS(uint32_t, rw, 4, 4) NART;
        BF_MMREGBITS(uint32_t, rw, 3, 3) RFLM;
        BF_MMREGBITS(uint32_t, rw, 2, 2) TXFP;
        BF_MMREGBITS(uint32_t, rw, 1, 1) SLEEP;
        BF_MMREGBITS(uint32_t, rw, 0, 0) INRQ;
    } MCR;
    struct MSR_t : BF_MMREG(uint32_t, rw, mmr_ops)
    {
        BF_COPY_SUPERCLASS(MSR_t);
        BF_MMREGBITS(uint32_t, r, 11, 11) RX;
        BF_MMREGBITS(uint32_t, r, 10, 10) SAMP;
        BF_MMREGBITS(uint32_t, r, 9, 9) RXM;
        BF_MMREGBITS(uint32_t, r, 8, 8) TXM;
        BF_MMREGBITS(uint32_t, rw, 4, 4) SLAKI;
        BF_MMREGBITS(uint32_t, rw, 3, 3) WKUI;
        BF_MMREGBITS(uint32_t, rw, 2, 2) ERRI;
        BF_MMREGBITS(uint32_t, r, 1, 1) SLAK;
        BF_MMREGBITS(uint32_t, r, 0, 0) INAK;
    } MSR;
    struct TSR_t : BF_MMREG(uint32_t, rw, mmr_ops)
    {
        BF_COPY_SUPERCLASS(TSR_t);
        BF_MMREGBITSET(uint32_t, r, 1, 3, 29) LOW;
        BF_MMREGBITSET(uint32_t, r, 1, 3, 26) TME;
        BF_MMREGBITS(uint32_t, r, 24, 25) CODE;
        BF_MMREGBITS(uint32_t, rw, 23, 23) ABRQ2;
        BF_MMREGBITS(uint32_t, rw, 19, 19) TERR2;
        BF_MMREGBITS(uint32_t, rw, 18, 18) ALST2;
        BF_MMREGBITS(uint32_t, rw, 17, 17) TXOK2;
        BF_MMREGBITS(uint32_t, rw, 16, 16) RQCP2;
        BF_MMREGBITS(uint32_t, rw, 15, 15) ABRQ1;
        BF_MMREGBITS(uint32_t, rw, 11, 11) TERR1;
        BF_MMREGBITS(uint32_t, rw, 10, 10) ALST1;
        BF_MMREGBITS(uint32_t, rw, 9, 9) TXOK1;
        BF_MMREGBITS(uint32_t, rw, 8, 8) RQCP1;
        BF_MMREGBITS(uint32_t, rw, 7, 7) ABRQ0;
        BF_MMREGBITS(uint32_t, rw, 3, 3) TERR0;
        BF_MMREGBITS(uint32_t, rw, 2, 2) ALST0;
        BF_MMREGBITS(uint32_t, rw, 1, 1) TXOK0;
        BF_MMREGBITS(uint32_t, rw, 0, 0) RQCP0;
    } TSR;
    struct RFR_t : BF_MMREG(uint32_t, rw, mmr_ops)
    {
        BF_COPY_SUPERCLASS(RFR_t);
        BF_MMREGBITS(uint32_t, rw, 5, 5) RFOM;
        BF_MMREGBITS(uint32_t, rw, 4, 4) FOVR;
        BF_MMREGBITS(uint32_t, rw, 3, 3) FULL;
        BF_MMREGBITS(uint32_t, r, 0, 1) FMP;
    } RFR[2];
    struct IER_t : BF_MMREG(uint32_t, rw, mmr_ops)
    {
        BF_COPY_SUPERCLASS(IER_t);
        BF_MMREGBITS(uint32_t, rw, 17, 17) SLKIE;
        BF_MMREGBITS(uint32_t, rw, 16, 16) WKUIE;
        BF_MMREGBITS(uint32_t, rw, 15, 15) ERRIE;
        BF_MMREGBITS(uint32_t, rw, 11, 11) LECIE;
        BF_MMREGBITS(uint32_t, rw, 10, 10) BOFIE;
        BF_MMREGBITS(uint32_t, rw, 9, 9) EPVIE;
        BF_MMREGBITS(uint32_t, rw, 8, 8) EWGIE;
        BF_MMREGBITS(uint32_t, rw, 6, 6) FOVIE1;
        BF_MMREGBITS(uint32_t, rw, 5, 5) FFIE1;
        BF_MMREGBITS(uint32_t, rw, 4, 4) FMPIE1;
        BF_MMREGBITS(uint32_t, rw, 3, 3) FOVIE0;
        BF_MMREGBITS(uint32_t, rw, 2, 2) FFIE0;
        BF_MMREGBITS(uint32_t, rw, 1, 1) FMPIE0;
        BF_MMREGBITS(uint32_t, rw, 0, 0) TMEIE;
    } IER;
    struct ESR_t : BF_MMREG(uint32_t, rw, mmr_ops)
    {
        BF_COPY_SUPERCLASS(ESR_t);
        BF_MMREGBITS(uint32_t, r, 24, 31) REC;
        BF_MMREGBITS(uint32_t, r, 16, 23) TEC;
        BF_MMREGBITS(uint32_t, rw, 4, 6) LEC;
        BF_MMREGBITS(uint32_t, r, 2, 2) BOFF;
        BF_MMREGBITS(uint32_t, r, 1, 1) EPVF;
        BF_MMREGBITS(uint32_t, r, 0, 0) EWGF;
    } ESR;
    struct BTR_t : BF_MMREG(uint32_t, rw, mmr_ops)
    {
        BF_COPY_SUPERCLASS(BTR_t);
        BF_MMREGBITS(uint32_t, rw, 31, 31) SILM;
        BF_MMREGBITS(uint32_t, rw, 30, 30) LBKM;
        BF_MMREGBITS(uint32_t, rw, 24, 25) SJW;
        BF_MMREGBITS(uint32_t, rw, 20, 22) TS2;
        BF_MMREGBITS(uint32_t, rw, 16, 19) TS1;
        BF_MMREGBITS(uint32_t, rw, 0, 9) BRP;
    } BTR;
    BF_MMREG_RESERVED(4, 88)
    struct TXMB_t
    {
        struct TIR_t : BF_MMREG(uint32_t, rw, mmr_ops)
        {
            BF_COPY_SUPERCLASS(TIR_t);
            BF_MMREGBITS(uint32_t, rw, 21, 31) STID;
            BF_MMREGBITS(uint32_t, rw, 3, 31) EXID;
            BF_MMREGBITS(uint32_t, rw, 2, 2) IDE;
            BF_MMREGBITS(uint32_t, rw, 1, 1) RTR;
            BF_MMREGBITS(uint32_t, rw, 0, 0) TXRQ;
        } TIR;
        struct TDTR_t : BF_MMREG(uint32_t, rw, mmr_ops)
        {
            BF_COPY_SUPERCLASS(TDTR_t);
            BF_MMREGBITS(uint32_t, rw, 16, 31) TIME;
            BF_MMREGBITS(uint32_t, rw, 8, 8) TGT;
            BF_MMREGBITS(uint32_t, rw, 0, 3) DLC;
        } TDTR;
        struct TDR_t : BF_MMREG(uint32_t, rw, mmr_ops)
        {
            BF_COPY_SUPERCLASS(TDR_t);
            BF_MMREGBITSET(uint32_t, rw, 8, 4, 0) DATA;
        } TDR[2];
    } TXMB[3];
    struct RXFIFO_t
    {
        struct RIR_t : BF_MMREG(uint32_t, r, mmr_ops)
        {
            BF_COPY_SUPERCLASS(RIR_t);
            BF_MMREGBITS(uint32_t, r, 21, 31) STID;
            BF_MMREGBITS(uint32_t, r, 3, 31) EXID;
            BF_MMREGBITS(uint32_t, r, 2, 2) IDE;
            BF_MMREGBITS(uint32_t, r, 1, 1) RTR;
        } RIR;
        struct RDTR_t : BF_MMREG(uint32_t, r, mmr_ops)
        {
            BF_COPY_SUPERCLASS(RDTR_t);
            BF_MMREGBITS(uint32_t, r, 16, 31) TIME;
            BF_MMREGBITS(uint32_t, r, 8, 15) FMI;
            BF_MMREGBITS(uint32_t, r, 0, 3) DLC;
        } RDTR;
        struct RDR_t : BF_MMREG(uint32_t, r, mmr_ops)
        {
            BF_COPY_SUPERCLASS(RDR_t);
            BF_MMREGBITSET(uint32_t, rw, 8, 4, 0) DATA;
        } RDR[2];
    } RXFIFO[2];
    BF_MMREG_RESERVED(4, 12)
    struct FMR_t : BF_MMREG(uint32_t, rw, mmr_ops)
    {
        BF_COPY_SUPERCLASS(FMR_t);
        BF_MMREGBITS(uint32_t, rw, 0, 0) FINIT;
    } FMR;
    struct FM1R_t : BF_MMREG(uint32_t, rw, mmr_ops)
    {
        BF_COPY_SUPERCLASS(FM1R_t);
        BF_MMREGBITSET(uint32_t, rw, 1, 14, 0) FBM;
    } FM1R;
    BF_MMREG_RESERVED(4, 1)
    struct FS1R_t : BF_MMREG(uint32_t, rw, mmr_ops)
    {
        BF_COPY_SUPERCLASS(FS1R_t);
        BF_MMREGBITSET(uint32_t, rw, 1, 14, 0) FSC;
    } FS1R;
    BF_MMREG_RESERVED(4, 1)
    struct FFA1R_t : BF_MMREG(uint32_t, rw, mmr_ops)
    {
        BF_COPY_SUPERCLASS(FFA1R_t);
        BF_MMREGBITSET(uint32_t, rw, 1, 14, 0) FFA;
    } FFA1R;
    BF_MMREG_RESERVED(4, 1)
    struct FA1R_t : BF_MMREG(uint32_t, rw, mmr_ops)
    {
        BF_COPY_SUPERCLASS(FA1R_t);
        BF_MMREGBITSET(uint32_t, rw, 1, 14, 0) FACT;
    } FA1R;
    BF_MMREG_RESERVED(4, 8)
    struct FILT_t
    {
        struct FR_t : BF_MMREG(uint32_t, rw, mmr_ops)
        {
            BF_COPY_SUPERCLASS(FR_t);
        } FR[2];
        void operator=(const FILT_t& f) volatile
        {
            FR[0] = f.FR[0];
            FR[1] = f.FR[1];
        }
    } FILT[14];

    static constexpr std::optional<uint32_t> calc_bit_timing(uint32_t pclk_Hz, uint32_t bitrate)
    {
        // 1bit: 4TQ <= 1TQ sync + 2-16TQ BS1 + 1-8TQ BS2 <= 25TQ
        // using 75% sample point the highest TQ is 23
        unsigned oversampling = pclk_Hz / bitrate;
        unsigned timequantas = 23u;
        do
        {
            unsigned prescaler = oversampling / timequantas;

            // Division without remainder?
            if (0u == (oversampling - (timequantas * prescaler)))
            {
                timequantas--; // remove one TQ for sync seg

                // Sample point is set at 75% of the bit
                unsigned phase_seg1 = (timequantas * 3u) / 4u;
                unsigned phase_seg2 = timequantas - phase_seg1;

                // Calculate default sjw as phase_seg2 / 2
                unsigned sjw = std::clamp(phase_seg2 / 2u, 1u, 4u);

                uint32_t btr = 0u;
                btr |= (prescaler - 1u) << decltype(BTR_t().BRP)::offset();
                btr |= (phase_seg1 - 1u) << decltype(BTR_t().TS1)::offset();
                btr |= (phase_seg2 - 1u) << decltype(BTR_t().TS2)::offset();
                btr |= (sjw - 1u) << decltype(BTR_t().SJW)::offset();
                return std::make_optional(btr);
            }
            timequantas--;
        } while (timequantas >= 4u);

        return std::nullopt;
    }

    void stop() volatile
    {
        MCR.INRQ = 1; // enter initialization mode
        while (MSR.INAK == 0)
        { // wait until acknowledged
        }
    }

    void start(BTR_t bit_timing) volatile
    {
        stop();

        // exit sleep mode
        MCR.SLEEP = 0;
        while (MSR.SLAK != 0)
        { // wait until acknowledged
        }

        // Feature configuration
        MCR.ABOM = 1;
#if 0
        MCR.NART = 0; // no automatic retransmission
        MCR.RFLM = 0; // FIFO locked mode off
        MCR.TXFP = 0; // transmit FIFO priority by identifier
#endif
        // Bit timing configuration
        BTR = bit_timing;

        MCR.INRQ = 0; // leave initialization mode
        while (MSR.INAK != 0)
        { // wait until acknowledged
        }
    }

    struct filter_32b_t
    {
        unsigned : 1;
        bool rtr : 1;
        bool ide : 1;
        uint32_t id : 29;
    };
    struct filter_bank_mask_32b_t
    {
        filter_32b_t match;
        filter_32b_t mask;
    };
    using filter_bank_list_32b_t = std::array<filter_32b_t, 2>;

    struct filter_16b_t
    {
        unsigned : 3;
        bool ide : 1;
        bool rtr : 1;
        uint32_t stdid : 11;
    };
    struct filter_mask_16b_t
    {
        filter_16b_t match;
        filter_16b_t mask;
    };
    using filter_bank_mask_16b_t = std::array<filter_mask_16b_t, 2>;
    using filter_bank_list_16b_t = std::array<filter_16b_t, 4>;

    template <typename T>
    void configure_filter_bank(unsigned bank_index, const T& filter_bank, uint8_t fifo = 0) volatile
    {
        FMR.FINIT = 1;
        FM1R.FBM[bank_index] =
            std::is_same_v<T, filter_bank_list_32b_t> or std::is_same_v<T, filter_bank_list_16b_t>;
        FS1R.FSC[bank_index] =
            std::is_same_v<T, filter_bank_list_32b_t> or std::is_same_v<T, filter_bank_mask_32b_t>;
        FFA1R.FFA[bank_index] = fifo;
        FILT[bank_index] = *reinterpret_cast<const FILT_t*>(&filter_bank);
        FA1R.FACT[bank_index] = 1;
        FMR.FINIT = 0;
    }

    bool send(const can::frame& frame, unsigned retries = 2) volatile
    {
        const unsigned mb_index = 0;
#if 0 // won't happen realistically
        if (TSR.TME[mb_index] == 0)
        {
            return false;
        }
#endif

        // prepare message
        auto& mb = TXMB[mb_index];
        mb.TIR.IDE = frame.ide;
        if (frame.ide)
        {
            mb.TIR.EXID = frame.id;
        }
        else
        {
            mb.TIR.STID = frame.id;
        }
        mb.TIR.RTR = frame.rtr;
        mb.TDTR.DLC = frame.dlc;
        *reinterpret_cast<volatile uint32_t*>(&mb.TDR[0]) =
            *reinterpret_cast<const uint32_t*>(&frame.data[0]);
        *reinterpret_cast<volatile uint32_t*>(&mb.TDR[1]) =
            *reinterpret_cast<const uint32_t*>(&frame.data[4]);

        // request transmission
        mb.TIR.TXRQ = 1;

        while (TSR.TXOK0 == 0)
        {
            if (TSR.TERR0) // transmission error
            {
                TSR.TERR0 = 1; // clear error flag
                if (retries-- == 0)
                {
                    TSR.ABRQ0 = 1; // abort transmission
                    return false;
                }
            }
        }
        // transmission successful
        return true;
    }

    void flush_rx(unsigned fifo_index = 0) volatile
    {
        while (RFR[fifo_index].FMP != 0)
        {
            RFR[fifo_index].RFOM = 1;
        }
    }

    std::optional<can::frame> receive(timeout timeout_ticks, unsigned fifo_index = 0) volatile
    {
        std::optional<can::frame> frame_opt{};
        while (!timeout_ticks)
        {
            if (RFR[fifo_index].FMP != 0)
            {
                auto& fifo = RXFIFO[fifo_index];
                bool ide = fifo.RIR.IDE;
                frame_opt = can::frame{can::frame_header{.id = ide ? fifo.RIR.EXID : fifo.RIR.STID,
                                                         .ide = ide,
                                                         .rtr = bool(fifo.RIR.RTR),
                                                         .dlc = uint8_t(fifo.RDTR.DLC)}};
                *reinterpret_cast<uint32_t*>(&frame_opt->data[0]) =
                    *reinterpret_cast<volatile uint32_t*>(&fifo.RDR[0]);
                *reinterpret_cast<uint32_t*>(&frame_opt->data[4]) =
                    *reinterpret_cast<volatile uint32_t*>(&fifo.RDR[1]);

                // release FIFO output mailbox
                RFR[fifo_index].RFOM = 1;
                break;
            }
        }
        return frame_opt;
    }
};

#undef CAN1
inline auto& CAN1 = *reinterpret_cast<volatile CAN_t*>(CAN1_BASE);
#undef CAN
inline auto& CAN = *reinterpret_cast<volatile CAN_t*>(CAN1_BASE);
