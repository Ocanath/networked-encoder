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
        printf("Placeholder: zero out encoder\n");
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
        printf("Placeholder: set regular address & save, enter bootloader and set bootloader address & save, then --bl-start\n");
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
    return 0;
}
