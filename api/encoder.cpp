#include "encoder.h"
#include "uart_config.h"

#define NUM_BYTES_COBS_OVERHEAD	2

Encoder::Encoder(unsigned char addr, Serial * ser)
{
	ds.address = addr;

	ds.ctl_base.buf = (unsigned char *)(&dp_ctl);
	ds.ctl_base.size = sizeof(dartt_map_t);
	ds.periph_base.buf = (unsigned char *)(&dp_periph);
	ds.periph_base.size = sizeof(dartt_map_t);
	for(int i = 0; i < (int)sizeof(dartt_map_t); i++)
	{
		ds.ctl_base.buf[i] = 0;
		ds.periph_base.buf[i] = 0;
	}

	tx_buf_mem = new unsigned char[UART_BUF_SIZE];
	rx_buf_mem = new unsigned char[UART_BUF_SIZE];

	ds.base_offset = 0;
	ds.msg_type = TYPE_SERIAL_MESSAGE;
	ds.tx_buf.buf = tx_buf_mem;
	ds.tx_buf.size = UART_BUF_SIZE - NUM_BYTES_COBS_OVERHEAD;
	ds.tx_buf.len = 0;
	ds.rx_buf.buf = rx_buf_mem;
	ds.rx_buf.size = UART_BUF_SIZE - NUM_BYTES_COBS_OVERHEAD;
	ds.rx_buf.len = 0;
	ds.blocking_tx_callback = &network_encoder::tx_blocking;
	ds.user_context_tx = (void*)(ser);
	ds.blocking_rx_callback = &network_encoder::rx_blocking;
	ds.user_context_rx = (void*)(ser);
	ds.timeout_ms = 10;

	theta = 0.f;

	angle_slice = {
		.buf = (unsigned char *)(&dp_ctl.angle),
		.size = sizeof(dp_ctl.angle),
	};
	adc_slice = {
		.buf = (unsigned char *)(&dp_ctl.dma_adc_raw[0]),
		.size = sizeof(dp_ctl.dma_adc_raw),
	};
	ms_slice = {
		.buf = (unsigned char *)(&dp_ctl.tick),
		.size = sizeof(dp_ctl.tick),
	};
	fds_slice = {
		.buf = (unsigned char *)(&dp_ctl.fds),
		.size = sizeof(dp_ctl.fds),
	};
	offset_slice = {
		.buf = (unsigned char *)(&dp_ctl.fds.offset),
		.size = sizeof(dp_ctl.fds.offset),
	};
}

Encoder::~Encoder()
{
	delete[] tx_buf_mem;
	delete[] rx_buf_mem;
}

int Encoder::read_angle(void)
{
	int rc = dartt_read_multi(&angle_slice, &ds);
	if(rc != DARTT_PROTOCOL_SUCCESS)
	{
		return rc;
	}
	theta = ((float)dp_periph.angle) / ((float)(1 << 14));
	return rc;
}

int Encoder::read_adc_raw(void)
{
	return dartt_read_multi(&adc_slice, &ds);
}

int Encoder::read_time_data(void)
{
	return dartt_read_multi(&ms_slice, &ds);
}

int Encoder::write_fds(void)
{
	return dartt_write_multi(&fds_slice, &ds);
}

int Encoder::write_offset(void)
{
	return dartt_write_multi(&offset_slice, &ds);
}
