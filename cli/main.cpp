#include <iostream>
#include <string>
#include <cstring>
#include "args.h"
#include "encoder.h"
#include "dartt_map.h"
#include "dartt_flasher.h"
#include "serial.h"

int main(int argc, char ** argv)
{
    cli_args_t a;
    parse_args(argc, argv, a);

    if (strcmp(a.command, "read-angle")  == 0)
	{
		printf("Placeholder: TODO read angle until ctl+c\n");
	}
    if (strcmp(a.command, "read-adc")    == 0) 
	{
		printf("Placeholder: TODO read angle adc until ctl+c\n");
	}
    if (strcmp(a.command, "read-fds")    == 0)
	{
		printf("Placeholder: TODO read out fds angle\n");
	}
    if (strcmp(a.command, "set-zero")    == 0)
	{
		printf("Placeholder: zero out encoder\n");
	}
    if (strcmp(a.command, "calibrate")   == 0)
	{
		printf("Placeholder: calibrate encoder\n");
	}
    if (strcmp(a.command, "save-fds")    == 0)
	{
		printf("Placeholder: save filesystem\n");
	}
    if (strcmp(a.command, "restart")     == 0)
	{
		printf("Placeholder: restart\n");
	}
    if (strcmp(a.command, "bootload")    == 0) 
	{
		printf("Placeholder: enter bootlaoder\n");
	}
    if (strcmp(a.command, "set-address") == 0) 
	{
		printf("Placeholder: set regular address & save, enter bootloader and set bootloader address & save, then --start return\n");
	}
    return 1;
}
