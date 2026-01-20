# Bluepill CAN Bootloader

A minimal CAN bootloader for Blue Pill STM32F103C8T6 boards. It occupies only the first 2 kB of flash, leaving ~62 kB free for the user application. The bootloader speaks a compact CAN protocol with segmentation identical to the Bluetooth ACS scheme and integrates cleanly with Zephyr via sysbuild; the `app/` folder shows a minimal Zephyr client that can request an update.

## Highlights
- 2 kB flash footprint; rest is available for the application.
- Simple CAN protocol with 1 kB page transfers and ACS-style segmentation.
- Default CAN IDs: prefix `0x7E5` (updater: `prefix+0`, loader: `prefix+1`); standard frames by default (IDE=0).
- App validity ensured by flashing MSP and reset vector **after** all payload pages are written.
- Zephyr module with sysbuild application example; shell command `image update` triggers bootloader mode.

## Flash layout
- Bootloader: 0x0800_0000 – 0x0800_07FF (2 kB)
- Application slot: 0x0800_0800 – 0x0800_FFFF (62 kB)

Defined in [zephyr/bluepill_partitions.overlay](zephyr/bluepill_partitions.overlay).

## CAN protocol (loader ↔ updater)
- IDs: `updater_id = CONFIG_BLUEPILL_CAN_ID_PREFIX + 0`, `loader_id = CONFIG_BLUEPILL_CAN_ID_PREFIX + 1`.
- Info frame (sent periodically by loader, DLC=8):
  - Byte0: error and app present flags.
  - Byte1: reserved.
  - Bytes2-3: app flash size in kB.
  - Bytes4-5: transfer chunk size (1 kB default).
  - Bytes6-7: current chunk index.
- Payload segmentation (ACS-like): each PDU prepends a 1-byte header: bit0 `first`, bit1 `last`, bits2-7 rolling `counter`. Data follows the header.
- Transfer rules:
  - Chunk size equals the flash page (1 kB). Chunk 0 begins with the application header (MSP + reset vector) but those two words are written **last** once all chunks arrive intact.
  - A zero-length CAN frame restarts the transfer (chunk index reset to 0).
  - Short final chunk terminates the transfer; the loader then programs the first two words and boots the new image if valid.
- Error flags: `0x01` invalid update (bad header/overflow), `0x02` flash failure, `0x04` RX timeout.

## Building the bootloader (standalone)
Requirements: CMake ≥3.22, `gcc-arm-none-eabi`, and a Ninja/Make generator. Provide the CAN bitrate via `CONFIG_CAN_BITRATE` (in bit/s).

```bash
cmake --preset=bluepill
cmake --build build/bluepill
# Output: build/bluepill/bluepill-can-bl.elf
```

## Zephyr integration (sysbuild sample)
The repository is a Zephyr module ([zephyr/module.yml](zephyr/module.yml)). The `app/` directory is a minimal client showing how to request an update via the shell. Sysbuild pulls in and builds the bootloader automatically.

Example inside a Zephyr workspace with this module present:

```bash
west build -b stm32_min_dev@blue --sysbuild -d app/build app
```

Notes:
- Sysbuild options live in [app/sysbuild.conf](app/sysbuild.conf) (e.g., `SB_CONFIG_BLUEPILL_CAN_BUS_BITRATE`).
- The sample enables shell and adds `image update` ([app/src/main.cpp](app/src/main.cpp)) which sets the update request flag and reboots.
- Partitioning comes from [zephyr/bluepill_partitions.overlay](zephyr/bluepill_partitions.overlay).

## Update flow
1. Application sets the update request flag (see `can_bl::set_update_request()` or run `image update` in the sample shell), then resets.
2. Bootloader stays if no valid app or an update was requested; otherwise it jumps to the app.
3. Loader advertises status via the periodic info frame; updater streams segmented CAN PDUs in 1 kB chunks.
4. Loader erases/writes each flash page; on the last (short) chunk it writes the MSP/reset vector and boots the new image if valid.

## Configuration knobs
- `CONFIG_CAN_BITRATE`: CAN bus bitrate (required when building standalone).
- `CONFIG_BLUEPILL_CAN_ID_PREFIX`: CAN ID base (default 0x7E5), defined in the interface target.
- `CONFIG_BLUEPILL_CAN_IDE`: 0 for standard, 1 for extended CAN IDs.

## Host-side updater
A host-side CAN sender is not included; any CAN tool that can emit the framed/segmented PDUs will work. Follow the protocol above to push 1 kB pages (chunk 0 starts with the app header, but those first two words are flashed last by the loader).

## How validity is enforced
The bootloader defers programming the first two words (MSP and reset vector) until **after** all other pages succeed. This prevents the boot vector from pointing to incomplete images.
