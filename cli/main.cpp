#include <iostream>
#include <string>
#include <signal.h>
#include "args.h"

static volatile sig_atomic_t g_stop = 0;
static void on_sigint(int) { g_stop = 1; }
#include "encoder.h"
#include "dartt_map.h"
#include "dartt_flasher.h"
#include "serial.h"

int main(int argc, char ** argv)
{
    cli_args_t a;
    parse_args(argc, argv, a);

    Serial ser;
    if (a.port) ser.connect(a.port, a.baud);
    else        ser.autoconnect(a.baud);

    Encoder enc(a.addr, &ser);

    if (a.read_angle)
    {
        while(1)
        {
            // int rc = enc.read_angle_misc();
			int rc = enc.read_angle();
            if(rc == 0)
            {
                printf("theta= %f\n", enc.theta);
            }
            else
            {
                printf("Error reading from encoder %d\n", rc);
            }
        }
    }
    if (a.read_angle_misc)
    {
        while(1)
        {
            int rc = enc.read_angle_misc();
            if(rc == 0)
            {
                printf("theta= %f\n", enc.theta);
            }
            else
            {
                printf("Error reading from encoder %d\n", rc);
            }
        }
    }
    if (a.read_adc)
    {
        while(1)
		{
			int rc = enc.read_adc_raw();
			if(rc == 0)
			{
				printf("[%d %d]\n", enc.dp_periph.dma_adc_raw[0], enc.dp_periph.dma_adc_raw[1]);
			}
			else
			{
				printf("Error reading %d\n", rc);
			}
		}
    }
    if (a.read_fds)
    {
        printf("Placeholder: TODO read out fds\n");
    }
    if (a.set_zero)
    {
        printf("Zero out encoder\n");
		float avg = 0;
		int nsamples = 0;
		int rc = 0;
		for(int ang_sample = 0; ang_sample < 1000; ang_sample++)
		{
			rc = enc.read_angle_misc();
			if(rc == 0)
			{
				avg += enc.dp_periph.angle;
				nsamples++;
			}
		}
		avg = avg / (float)nsamples;
		printf("avg = %f\n", avg);

		dartt_mem_t offset_slice ={.buf = (unsigned char *)&enc.dp_ctl.fds.offset,
			.size = sizeof(enc.dp_ctl.fds.offset)
		};
		rc = dartt_read_multi(&offset_slice, &enc.ds);
		if(rc != 0) 
		{
			printf("issue reading offset, abort\n");
			return -1;
		}
		printf("offset is equal to %d\n",enc.dp_periph.fds.offset);
		float offset = (float)enc.dp_periph.fds.offset;
		float raw = avg + offset;
		enc.dp_ctl.fds.offset = (int32_t)raw;

		rc = dartt_write_multi(&offset_slice, &enc.ds);
		printf("Wrote %d as offset\n", enc.dp_ctl.fds.offset);
		rc = enc.write_action_flag(FS_SAVE);
		if(rc != 0) {printf("FS_SAVE failed: %d\n", rc);}
		else{printf("Updated filesystem successfully\n");}


    }
    if (a.calibrate)
    {
		signal(SIGINT, on_sigint);
		int32_t sinmin = 1<<14;
		int32_t sinmax = 0;
		int32_t cosmin = 1 << 14;
		int32_t cosmax = 0;
		while(!g_stop)
		{
			int rc = enc.read_adc_raw();
			if(rc != 0)
			{
				printf("Error reading %d\n", rc);
			}
			int32_t c = enc.dp_periph.dma_adc_raw[0];
			int32_t s = enc.dp_periph.dma_adc_raw[1];
			if(c < cosmin)
			{
				cosmin = c;
			}
			if(c > cosmax)
			{
				cosmax = c;
			}
			if(s < sinmin)
			{
				sinmin = s;
			}
			if(s > sinmax)
			{
				sinmax = s;
			}
			printf("min = [%d, %d], max = [%d, %d]\n", cosmin, sinmin, cosmax, sinmax);
		}
		printf("\nFinal: cos=[%d, %d] sin=[%d, %d]\n", cosmin, cosmax, sinmin, sinmax);
		enc.dp_ctl.fds.cos_min = cosmin;
		enc.dp_ctl.fds.cos_max = cosmax;
		enc.dp_ctl.fds.sin_min = sinmin;
		enc.dp_ctl.fds.sin_max = sinmax;
		dartt_mem_t sincos_minmax = {
			.buf = (unsigned char *)(&enc.dp_ctl.fds.sin_min),
			.size = 4*sizeof(uint32_t)
		};
		int rc = dartt_write_multi(&sincos_minmax, &enc.ds);
		// int rc = enc.write_fds();
		if(rc != 0) printf("write_fds failed: %d\n", rc);
		rc = enc.write_action_flag(FS_SAVE);
		if(rc != 0) printf("FS_SAVE failed: %d\n", rc);
    }
    if (a.restart)
    {
		int	rc = enc.write_action_flag(RESTART);
		if(rc != 0) printf("RESTART failed: %d\n", rc);

    }
    if (a.bootload)
    {
		int	rc = enc.write_action_flag(BOOTLOAD);
		if(rc != 0) printf("BOOTLOAD failed: %d\n", rc);
    }
    if (a.set_address)
    {
        printf("Placeholder: set address to %d\n", a.new_address);

		enc.dp_ctl.fds.address = a.new_address;
		dartt_mem_t addr = {
			.buf = (unsigned char *)(&enc.dp_ctl.fds.address),
			.size = sizeof(enc.dp_ctl.fds.address)
		};
		int rc = dartt_write_multi(&addr, &enc.ds);
		if(rc != 0 ){printf("address update failed: %d\n", rc);}
		//TODO: add write action to update flash HERE but don't do it yet to avoid brickage
		rc = enc.write_action_flag(FS_SAVE);
		if(rc != 0 ){printf("settings save failed code: %d\n", rc);}

		//set long timeout and read back address to confirm
		enc.ds.timeout_ms = 1000;
		rc = dartt_read_multi(&addr, &enc.ds);
		if(rc != 0){printf("Readback post save failed: %d\n", rc);}
		if(enc.dp_periph.fds.address != a.new_address)
		{
			printf("Failed to write - readback mismatch. desired %d, actual %d\n", a.new_address, enc.dp_periph.fds.address);
		}
		else
		{
			printf("Readback match\n");
		}
		
		rc = enc.write_action_flag(BOOTLOAD);
		if(rc != 0 ){printf("bootload update failed: %d\n", rc);}

		DarttFlasher flasher(a.bootloader_addr, &ser);
		rc = flasher.poll_action_flags(500);
		if(rc < 0)
		{
			printf("Failed to get action flag: %d\n", rc);
			return rc;
		}
		else{printf("bootloader ready, writing address\n");}
		rc = flasher.update_target_address(a.new_address);
		if(rc != 0) {printf("Failed to update bootloader address. Code %d\n", rc);}
		rc = flasher.start_app();
		if(rc != 0) {printf("Failed to start app, code %d\n", rc);}
		printf("Done\n");
    }
    if (a.flash)
    {
        printf("Placeholder: flash %s via bootloader\n", a.filename);
    }
    if (a.bl_version)
    {
        printf("Placeholder: get bootloader version\n");
    }
    if (a.bl_start)
    {
        printf("Placeholder: start application\n");
    }
	if(a.scan)
	{
		for(uint8_t scan_addr = 0; scan_addr < 255; scan_addr++)
		{
			Encoder scan_enc(scan_addr, &ser);
			int s_rc = scan_enc.read_adc_raw();
			if(s_rc == 0)
			{
				printf("Found encoder %d\n", scan_addr);
			}
		}
	}
    return 0;
}
