#ifndef ENCODER_CLI_ARGS_H
#define ENCODER_CLI_ARGS_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    /* required positional */
    unsigned char addr;

    /* comms options */
    const char *  port;         /* --port, NULL = autoconnect */
    unsigned long baud;         /* --baud, default 921600 */

    /* encoder commands */
    bool read_angle;
    bool read_angle_misc;
    bool read_adc;
    bool read_fds;
    bool set_zero;
    bool calibrate;
    bool save_fds;
    bool restart;
    bool bootload;
    bool set_address;
	bool has_bootloader_addr;
    unsigned char new_address;
    unsigned char bootloader_addr;      /* --current-bootloader-address, defaults to addr */

    /* bootloader commands */
    bool flash;
    const char * filename;
    bool bl_version;
    bool bl_start;
	bool scan;
} cli_args_t;

void parse_args(int argc, char ** argv, cli_args_t & args);

#endif
