#ifndef ENCODER_H
#define ENCODER_H

#include "dartt_map.h"
#include "dartt_sync.h"
#include "encoder_serial_callbacks.h"
#include "serial.h"

class Encoder
{
public:
	dartt_map_t dp_ctl;
	dartt_map_t dp_periph;
	dartt_sync_t ds;
	unsigned char * tx_buf_mem;
	unsigned char * rx_buf_mem;

	Encoder(unsigned char addr, Serial * ser);
	~Encoder();

	Encoder(const Encoder&) = delete;
	Encoder& operator=(const Encoder&) = delete;

	float theta;	//radians

	int read_angle(void);
	int read_adc_raw(void);
	int read_time_data(void);
	int write_fds(void);		//write full flash data store (calibration + address + baud + offset)
	int write_offset(void);		//write only the zero offset field

private:
	dartt_mem_t angle_slice;
	dartt_mem_t adc_slice;
	dartt_mem_t ms_slice;
	dartt_mem_t fds_slice;
	dartt_mem_t offset_slice;
};

#endif
