# Networked Encoder

A compact RS-485 enabled magnetic encoder powered by DARTT, designed to be daisy-chained with other DARTT devices on a shared bus. The system consists of STM32-based firmware, a host-side C++ API, and a CLI tool for calibration, address assignment, and firmware flashing.

## What it does

Each encoder node sits on an RS-485 bus and responds to two address classes:

- **Motor address** — a lightweight protocol that returns a 16-bit fixed-point angle reading with minimal overhead, intended for high-rate polling from a motion controller.
- **Misc address** — full DARTT register access for reading/writing calibration data (sin/cos min/max, offset), controlling action flags (save to flash, restart, enter bootloader), and reading diagnostics.

Addresses are assigned per-device and persisted to flash. The bootloader (also DARTT-based) allows firmware updates over the same RS-485 bus without any additional hardware interface.

Refer to the [DARTT codebase](https://github.com/ocanath/dartt-protocol) for more information.

## Dependencies

The API and CLI pull in three submodules under `api/external/`:

| Submodule | Purpose |
|---|---|
| `dartt-protocol` | DARTT framing, parsing, read/write helpers |
| `byte-stuffing` | COBS encode/decode |
| `serial-cross-platform` | Cross-platform serial port |

After cloning, initialize all submodules:

```bash
git submodule update --init --recursive
```

## Building the CLI

Requires CMake 3.22+ and a C++20-capable compiler.

```bash
cmake -S . -B build
cmake --build build --target encoder-cli
```

The binary lands at `build/cli/encoder-cli`.

For debug builds (enables stepping through with gdb):

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --target encoder-cli
```

VSCode launch and build tasks are provided in `.vscode/` and target `encoder-cli` automatically.

## CLI usage

```
encoder-cli <addr> [--port <port>] [--baud <rate>] <command>
```

`addr` is the DARTT address of the target device (0–255) and is always the first positional argument.

Key commands:

| Command | Description |
|---|---|
| `--read-angle` | Stream angle via motor address protocol (q13 fixed-point) |
| `--read-angle-misc` | Stream angle via DARTT misc register (q14 fixed-point) |
| `--read-adc` | Stream raw sin/cos ADC values |
| `--calibrate` | Interactive min/max calibration (Ctrl+C to finish and save) |
| `--set-address <n>` | Reassign DARTT address (updates encoder flash and bootloader) |
| `--current-bootloader-address <n>` | Override bootloader address when it differs from device address |
| `--restart` | Restart encoder firmware |
| `--bootload` | Enter bootloader mode |
| `--flash <file>` | Flash firmware binary via bootloader |

Run `encoder-cli --help` for the full list.

## First-time device setup

Use `scripts/init_flash.sh` to flash a brand-new device. The bootloader ships at address 255; the encoder application defaults to address 0. The script flashes the firmware, starts the application, then assigns a permanent address:

```bash
./scripts/init_flash.sh <desired-address>
```

For subsequent firmware updates on an already-addressed device:

```bash
./scripts/update_firmware.sh <device-address>
```

## Using the API

`api/` is designed to be consumed as a CMake `add_subdirectory` dependency. Add it to your parent project and link against `encoder_controller`:

```cmake
add_subdirectory(path/to/networked-encoder/api)
target_link_libraries(your_target PRIVATE encoder_controller)
```

Construct an `Encoder` with the device address and a pointer to an already-connected `Serial` object. The caller owns the `Serial` lifetime.
