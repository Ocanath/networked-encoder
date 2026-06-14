#ifndef ENCODER_SERIAL_CALLBACKS_H
#define ENCODER_SERIAL_CALLBACKS_H

#include "cobs.h"
#include "dartt.h"
#include "dartt_sync.h"

namespace dartt_encoder {
    int tx_blocking(unsigned char addr, dartt_buffer_t * b, void * user_context, uint32_t timeout);
    int rx_blocking(dartt_buffer_t * buf, void * user_context, uint32_t timeout);
}

#endif
