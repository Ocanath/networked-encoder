# Networked Encoder

A compact RS-485 magnetic encoder system powered by [DARTT](https://github.com/ocanath/dartt-protocol), designed for daisy-chaining multiple encoder nodes on a shared bus. The system consists of STM32-based firmware, a host-side C++ API, and a CLI tool for calibration, address assignment, and firmware flashing.

## What it does

Each encoder node sits on an RS-485 bus and responds to two address classes:

- **Motor address** — a minimal framing mode optimized for high-rate angle polling, intended for real-time motion control.
- **Misc address** — full DARTT register access for calibration (sin/cos bounds, angular offset, orthogonality error), device control (save to flash, restart, enter bootloader), and diagnostics.

Addresses are assigned per-device and persisted to flash. The bootloader (also DARTT-based) supports firmware updates over the same RS-485 bus with no additional hardware interface.

## Repository Structure

| Path | Contents |
|---|---|
| `embedded/encoder-rs485` | STM32G031 encoder application firmware |
| `embedded/encoder-bootloader` | DARTT-based RS-485 bootloader |
| `api/` | Host-side C++ encoder controller library |
| `cli/` | Command-line tool for calibration, addressing, and flashing |
| `scripts/` | Helper scripts for first-time device setup and firmware updates |

## Dependencies

The API and CLI pull in three submodules under `api/external/`:

| Submodule | Purpose |
|---|---|
| [`dartt-protocol`](https://github.com/ocanath/dartt-protocol) | DARTT framing, parsing, read/write helpers |
| [`byte-stuffing`](https://github.com/ocanath/byte-stuffing) | COBS encode/decode |
| [`serial-cross-platform`](https://github.com/ocanath/serial-cross-platform) | Cross-platform serial port |

Initialize all submodules after cloning with `git submodule update --init --recursive`.

## Building

Requires CMake 3.22+ and a C++20-capable compiler. The primary build target is `encoder-cli`. VSCode launch and build tasks are provided in `.vscode/`.

## CLI

The CLI (`encoder-cli`) takes a DARTT device address as its first argument followed by a port and baud rate. It supports streaming angle data, reading raw ADC values, running an interactive calibration session, reassigning device addresses, and flashing firmware over RS-485 via the bootloader. Run `encoder-cli --help` for the full command reference.

## First-time Device Setup

Use `scripts/init_flash.sh` to set up a brand-new device. The bootloader ships at address 255 and the encoder application defaults to address 0. The script flashes firmware, starts the application, then assigns a permanent address. For subsequent updates on an already-configured device, use `scripts/update_firmware.sh`.

## Using the API

`api/` is designed to be consumed as a CMake `add_subdirectory` dependency. Link against `encoder_controller` and construct an `Encoder` object with the device address and a pointer to an already-connected `Serial` object. The caller owns the `Serial` lifetime.
