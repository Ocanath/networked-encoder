#!/usr/bin/env bash
set -e

if [ $# -lt 1 ]; then
    echo "Usage: $0 <address>" >&2
    exit 1
fi

ADDR=$1

dartt_flash 255 ../network-encoder-firmware/encoder-rs485/ReleaseBootloader/encoder-rs485.bin
dartt_flash 255 --enable-autoboot
dartt_flash 255 --start
encoder-cli 0 --set-address "$ADDR" --current-bootloader-address 255

