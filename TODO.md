# Plan for Networked Encoder

## Primary Firmware 
1. Solder hardware. Confirm DMA UART wiring & dartt
2. Plug in a second conditional for address match. Something basic - probably just a 16bit angle transmission
3. Wire gl_dp.tick directly into GetTick()/IncTick(), same as mctl
4. Deferred action handler for reset (normal) and reset (magic RAM word) based on same pattern in foc code

## Bootloader
1. Use this project as boilerplate for a STM32G031 DARTT bootloader (generic) within the dartt bootloader project
1. Add new linker script and build configurations to eclipse project
1. Test bootloader

We'll need a USB-RS485 dongle for this. Guess we have to find one since mine are getting fabricated. Pretty sure I've got one here.


## Viz

- Submodule this repo into a 'arms' project
- CAD some arms
- fork/copy opengl boilerplate
- use the arms to control it

It's crucial that there is no exterior wiring.
