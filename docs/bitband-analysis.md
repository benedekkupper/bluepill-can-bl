# Bit-banding in `bluepill-can-bl`: a cost-benefit analysis

**Target:** STM32F103 (Cortex-M3), GCC 14.3.1 (arm-none-eabi), `-Os -g3`

**Library:** [`bitfilled`][bitfilled] memory-mapped register framework

**Builds compared:**

| Build                 | `BITFILLED_BB_OPS`     | `BITFILLED_FLASH_BB_OPS` | `.text` size |
|---                    |---                     |---                       |---           |
| `bluepill`            | `bitband<PERIPH_BASE>` | `base`                   |       1920 B |
| `bluepill-allbitband` | `bitband<PERIPH_BASE>` | `bitband<PERIPH_BASE>`   |       1936 B |
| `bluepill-nobitband`  | `base`                 | `base`                   |       2028 B |

Enabling bit-banding for the selected peripherals (RCC, CAN, GPIO, PWR, BKP) saves 108 bytes
of overall code (§2). Extending the same technique to the FLASH controller made the binary 16 bytes
bigger (§3). Roughly a third of the firmware's `.text` (678–766 B, 36–38%) is spent inside `bitfilled`'s
inlined field-access templates themselves, that means the effect of bit-band access has improved
the code size of memory-mapped register manipulations by 15%.

## 1. How `bitfilled` implements bit-banding

Two policy classes implement the `mmr_ops` bitfield access strategy ([bitfilled/base_ops.hpp](https://github.com/IntergatedCircuits/bitfilled/blob/main/bitfilled/bitfilled/base_ops.hpp),
[bitfilled/bitband_ops.hpp](https://github.com/IntergatedCircuits/bitfilled/blob/main/bitfilled/bitfilled/bitband_ops.hpp)):

* **`bitfilled::base`** — every field access is a classic read-modify-write (RMW) on the full 32-bit register word:
  read the word, mask/shift to extract or insert the field, write the word back.
* **`bitfilled::bitband<BASE>`** — *only when the field is exactly one bit wide* (`FIRST_BIT == LAST_BIT`), the
  access is redirected to the Cortex-M3/M4 **bit-band alias region**:

  ```cpp
  auto address = BITBAND_BASE_ADDRESS                     // 0x42000000 | peripheral base
               | (((std::uintptr_t)&ptr & 0xfffff) << 5)  // word offset within region
               | (bit_index << 2);                        // bit offset
  return *(volatile uint32_t*)address;
  ```

  A single 32-bit load/store to this alias address reads or writes just that one bit — **no shift, no mask, no
  read-modify-write** is needed, because the CPU's bus matrix performs the bit extraction/insertion in hardware.
  For multi-bit fields, `bitband<BASE>::bitfield_ops` simply falls back to `base::bitfield_ops` (RMW) — there is
  no bit-band hardware support for fields wider than 1 bit.

This is the entire mechanism: **bit-banding only ever changes code generation for single-bit fields**; multi-bit
fields (`PLLMUL`, `HPRE`, `SW`, `ACR.LATENCY`, CAN filter banks, ...) are identical in both configurations.

## 2. Where bit-banding wins

`sys_clocks_init()` (in [board.hpp](../bluepill-can-bl/board.hpp)) is dominated by single-bit enable/ready-flag
idioms (`RCC.CR.HSEON = 1; while (RCC.CR.HSERDY == 0);`), which is exactly the pattern bit-banding targets.

### bit-band — enabling HSE oscillator and polling ready flag

```asm
08000220 <sys_clocks_init()>:
 8000220: movs    r3, #1
 8000222: ldr     r2, [pc, #148]     ; r2 = 0x42420000 (bit-band alias base)
 8000224: str.w   r3, [r2, #656]     ; RCC.AHBENR.FLITFEN = 1   (single store)
 8000228: str     r3, [r2, #64]      ; RCC.CR.HSEON = 1         (single store)
 800022a: ldr     r3, [r2, #68]      ; RCC.CR.HSERDY            (single load)
 800022c: cmp     r3, #0
 800022e: beq.n   800022a            ; poll
```

### regular RMW — same operations

```asm
08000224 <sys_clocks_init()>:
 8000224: ldr     r3, [pc, #160]     ; r3 = RCC_BASE (0x40021000)
 8000226: ldr     r2, [r3, #20]      ; load AHBENR
 8000228: orr.w   r2, r2, #16        ; set FLITFEN bit
 800022c: str     r2, [r3, #20]      ; store AHBENR back
 800022e: ldr     r2, [r3, #0]       ; load CR
 8000230: orr.w   r2, r2, #65536     ; set HSEON bit
 8000234: str     r2, [r3, #0]       ; store CR back
 8000236: ldr     r2, [r3, #0]       ; load CR
 8000238: lsls    r1, r2, #14        ; shift HSERDY (bit17) into sign position
 800023a: bpl.n   8000236            ; poll
```

Every independent single-bit **set** collapses from *load + orr + store* (3 instructions, 8 bytes) to a single
`str` (2–4 bytes) because the constant "1" only needs to be loaded once and reused, and no read of the old value is
required. Polling a single bit is roughly a wash (3 instructions either way — a plain load+compare vs. a
load+shift+branch), so the net saving in this function (168 B vs 176 B, **−8 bytes**) comes entirely from the
"set" side of RCC/CAN/GPIO single-bit fields (`HSEON`, `PLLON`, `IOPBEN`, `AFIOEN`, GPIO `MODEx`/`CNFx`, CAN `INRQ`,
etc.), repeated across `sys_clocks_init()`, `can_pins_init()` and `can_init()`.

A second, subtler effect: adjacent bits of the **same** register map to bit-band alias addresses only 4 bytes
apart (`RCC.CR.HSEON` at alias-offset `+0x40`, `RCC.CR.HSERDY` at `+0x44`). The compiler can therefore reuse one
base pointer (`r2 = 0x42420000`) across several unrelated bit accesses to the same register via small immediate
offsets, instead of re-materializing the peripheral base address for each field.

## 3. Where bit-banding loses

Bit-banding is not a universal win even when only single-bit fields are involved. `CAN_t::receive()` decodes a
received CAN frame by reading **several different single-bit/small fields out of the same hardware register**
(`RIR`, `RDTR`) in one function:

* **`bluepill-nobitband`**: loads `RIR` **once** into a register, then extracts `IDE`, `RTR`, `STID`/`EXID` from
  that one cached 32-bit value using shifts/masks (compiler-performed CSE). Function size: **126 B (0x7e)**.
* **`bluepill`**: each single-bit field (`IDE`, `RTR`, …) is bit-banded to a *different* alias address, so the
  compiler **cannot** reuse one load across fields — it must issue one distinct memory transaction per bit.
  Function size: **160 B (0xa0)**, i.e. **34 bytes larger**.

Another example are the FLASH register operations: making FLASH bit-banded is a regression relative
to the optimal configuration, even though it is still smaller than making *everything* regular RMW (1936 B vs 2028 B)
— i.e. bit-banding FLASH captures *some* of the available saving, but loses more than it gains, net negative.

The root cause is that FLASH registers are manipulated as multi-bit groups, not isolated flags.
Even where the FLASH access *is* inlined and the address *is* a compile-time constant (`sys_clocks_init()`'s
`FLASH.ACR.LATENCY`/`PRFTBE` setup), the surrounding code accesses **multiple fields of the same register block
together** (`LATENCY` is a 3-bit field — always RMW, bit-band cannot help — immediately followed by the 1-bit
`PRFTBE`). The single-bit `PRFTBE` store *does* fold to a one-instruction bit-band write here, but it needs its
**own, unrelated literal-pool constant** for the alias address (bit-band aliases bear no simple offset
relationship to the peripheral's normal base address), so it cannot reuse the `FLASH_R_BASE` pointer that is already
resident in a register from the immediately preceding `LATENCY` RMW:

```asm
; FLASH.ACR access, FLASH=base:  LATENCY (RMW) + PRFTBE (RMW), same r1 base pointer reused
 8000250: ldr r2, [r1, #0]        8000282: bic.w r2, r2, #7
 8000256: orr.w r2, r2, #2        800025a: str r2, [r1, #0]
 800025c: ldr r2, [r1, #0]        800025e: orr.w r2, r2, #16
 8000262: str r2, [r1, #0]                                 ; 14 bytes total for both fields

; FLASH.ACR access, FLASH=bit-band: LATENCY (RMW, unchanged) + PRFTBE (bit-band, needs a *new* constant)
 8000280: ldr r2, [r1, #0]        800028c: movs r1, #1
 8000282: bic.w r2, r2, #7        800028e: ldr r2, [pc, #100]   ; fresh literal: PRFTBE alias address
 8000286: orr.w r2, r2, #2        8000290: str r1, [r2, #16]
 800028a: str r2, [r1, #0]                                     ; 18 bytes total (+4)
```

Individually the `PRFTBE` bit-band store is a single instruction, but the extra literal pool entry costs more
than the `ldr+orr+str` RMW it replaces, because the neighboring `LATENCY` access can no longer make that
pointer-load "free" by sharing it.

This is the general failure mode of bit-banding: it helps when a single, isolated bit is set/read on its own
(control/enable/ready flags), but it hurts when **multiple bits of the same register are consumed together**,
because it defeats load-coalescing that a plain RMW/CSE approach gets for free.
`-Os` inlining decisions also differ slightly between the two builds (e.g. `can_pins_init()` and
`CAN_t::stop()` are inlined into `main()` in the bit-band build but remain standalone in the regular build), which
shifts a modest amount of size between individual functions without changing the overall picture.

## 4. Beyond code size: bit-banding makes single-bit RMW atomic

Every measurement above is about `.text` size, but bit-banding has a second, independent property worth weighing
even when the size trade-off is neutral or unfavorable: **a bit-band access is a single 32-bit bus transaction**,
so setting/clearing/reading one bit can never race with another context touching a different bit of the same
physical register.

* **`base` (RMW) is not atomic.** `CR.LOCK = 1` compiles to load CR, insert bit 7, store CR. If an interrupt or
  exception handler preempts the current context between the load and the store, and that handler modifies a
  different bit of the same register, the resumed RMW will blindly write back its stale cached copy — silently
  undoing the handler's change. This is a classic **lost-update** race on shared MMIO registers, and normally
  needs an explicit critical section (`__disable_irq()`/`__enable_irq()`, `PRIMASK`/`BASEPRI` save-restore, or
  `LDREX`/`STREX`) to avoid.
* **`bitband<BASE>` is atomic for that one bit, by construction.** The store to the alias address only ever
  touches the single targeted bit at the bus/peripheral level — no other bit of the real register is read or
  rewritten — so the operation is inherently race-free with respect to any concurrent access to other bits of
  the same register, with no locking code required at all.

Two caveats limit how far this goes:

1. **Only single-bit fields get the atomicity guarantee** — the same restriction as the code-size benefit (§1).
   Multi-bit fields always fall back to RMW and can still race.
2. **Only the bit write/read itself is atomic, not surrounding logic.** `unlock()`'s `if (CR.LOCK) { ... }` is
   still a separate read followed by separate writes; bit-banding makes each of those individual accesses
   race-free, but does not make the whole read-then-act sequence a single atomic transaction (a
   test-and-set/compare-and-swap would still need `LDREX`/`STREX` or a critical section).

## 5. When to use bit-banding: a rule of thumb, backed by the data above

| Situation | Recommendation | Evidence |
|---|---|---|
| A single-bit field (enable, ready-flag, request bit) is set/polled **in isolation**, from code that is (or can be) inlined at its unique call site | **Use bit-band** | §2: −108 B across RCC/CAN/GPIO |
| Several bits/fields of the **same register** are decoded or assembled together in one routine | **Avoid bit-band** — a single RMW load/store amortizes across all of them; bit-band forces one transaction per bit | §3: CAN `receive()` +34 B |
| Multi-bit fields (anything wider than 1 bit: `LATENCY`, `PLLMUL`, `HPRE`, …) | No bit-band access possible | §1 |
| A single bit can be concurrently modified by an interrupt/fault handler while mainline code is mid-RMW on the **same register** | **Use bit-band** — this is a correctness property, not a size/speed trade-off | §4 |

## 6. What fraction of the whole firmware is register accessing?

This section describes how to determine how much of the executable code is dealing with memory-mapped I/O,
i.e. how many bytes of the shipped `.text` are spent literally inside `bitfilled`'s templated get/set-field machinery
(register loads/stores, bit(field) insert/extract).

### 6.1 Method

Built with `-Os -g3`, the DWARF inline-subroutine info is rich enough that
`arm-none-eabi-objdump -dl -C --source-comment` prints a fresh pseudo-label every time execution crosses into a
different (possibly fully-inlined) function — including individual `bitfilled` template instantiations such as
`bitband<PERIPH_BASE>::bitfield_ops<...>::set_field<28,28,...>` — even when inlined deep inside `main()` or
`can_bl::pop_update_request()`. A short script walks this annotated disassembly, attributes every instruction's
byte length to the nearest such label, and buckets each label as **bitfilled** (label contains the `bitfilled::`
namespace, including un-demangled mangled names that still contain the literal substring) or **other**
(application/driver code, libc, CRT startup): [classify-dottext.py](./classify-dottext.py)

### 6.2 Result

| Build                 | `.text` (code, excl. `.isr_vector`) | attributed to `bitfilled::*` | attributed to other code |
|---                    |---                                  |---                           |---                       |
| `bluepill` (bit-band) |                              1892 B |            **678 B (35.8%)** |           1214 B (64.2%) |
| `bluepill-nobitband`  |                              2000 B |            **766 B (38.3%)** |           1234 B (61.7%) |
| Δ                     |                              108 B  |                     **88 B** |                     20 B |

Roughly one third of this firmware's entire code size is spent inside `bitfilled`'s field-access templates,
even though not a single one of those bytes is a standalone function call at runtime — it is all inlined
directly into `main()`, `sys_clocks_init()`, `CAN_t::receive()`, `pop_update_request()`, etc.
81% of the whole-program size delta between the two policies (88 of 108 bytes) comes from shrinking
the `bitfilled` access code itself by using bit-band access; the remaining 20 bytes (19%) is a secondary
knock-on effect on the surrounding business logic (the differing inlining/CSE decisions noted in §2–§3).
Bit-band is a self-contained win inside the accessor layer — it does not meaningfully change how the
application code around it compiles.

### 6.3 Caveat

This is an attribution of *instructions*, not a strict causal decomposition. When a single machine instruction
serves both a bitfilled access and adjacent logic (e.g. a literal-pool load reused across a field access and a
neighboring plain comparison), the DWARF line table assigns it to only one inline frame, so a small amount of
inherent bleed between the two buckets is unavoidable — the same caveat the compiler-inlining discussion in §3
already calls out. The ~0.1% gap between `total` above and the true `.text` size (2 B / 4 B) is unclassified
padding/alignment, not a methodology error.

## Appendix: reproduction steps

```bash
export GNUARMEMB_TOOLCHAIN_PATH=/path/to/arm-gnu-toolchain/bin/
cmake --preset bluepill              && cmake --build build/bluepill
cmake --preset bluepill-nobitband    && cmake --build build/bluepill-nobitband
arm-none-eabi-size   build/bluepill/bluepill-can-bl.elf build/bluepill-nobitband/bluepill-can-bl.elf
arm-none-eabi-objdump -d -C build/bluepill/bluepill-can-bl.elf         > bluepill.S
arm-none-eabi-objdump -d -C build/bluepill-nobitband/bluepill-can-bl.elf > nobitband.S
arm-none-eabi-nm -C --size-sort build/bluepill/bluepill-can-bl.elf | grep -v ' [bBdD] '
```

For the §6 whole-program attribution, generate source/inline-annotated disassembly and run it through the
`classify-dottext.py` script:

```bash
arm-none-eabi-objdump -dl -C --source-comment build/bluepill/bluepill-can-bl.elf         > bluepill_full.S
arm-none-eabi-objdump -dl -C --source-comment build/bluepill-nobitband/bluepill-can-bl.elf > nobitband_full.S
python3 docs/classify-dottext.py bluepill_full.S nobitband_full.S
```

[bitfilled]: https://github.com/IntergatedCircuits/bitfilled
