#!/usr/bin/env bash
set -e

if [ $# -lt 1 ]; then
    echo "Usage: $0 <address>" >&2
    exit 1
fi

ADDR=$1

encoder-cli "$ADDR" --bootload
dartt_flash "$ADDR" ../embedded/encoder-rs485/ReleaseBootloader/encoder-rs485.bin
dartt_flash "$ADDR" --start

