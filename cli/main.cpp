#include <iostream>
#include <string>
#include "args.h"
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
        // printf("Placeholder: calibrate encoder\n");
		int32_t sinmin = 1<<14;
		int32_t sinmax = 0;
		int32_t cosmin = 1 << 14;
		int32_t cosmax = 0;
		while(1)
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
    }
    if (a.save_fds)
    {
        printf("Placeholder: save filesystem\n");
    }
    if (a.restart)
    {
        printf("Placeholder: restart\n");
    }
    if (a.bootload)
    {
        printf("Placeholder: enter bootloader\n");
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
