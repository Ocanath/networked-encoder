#ifndef ENCODER_CLI_ARGS_H
#define ENCODER_CLI_ARGS_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct {
    /* comms */
    const char *  port;         /* --port, NULL = autoconnect */
    unsigned char addr;         /* --addr (required) */
    unsigned long baud;         /* --baud, default 921600 */

    /* subcommand */
    const char *  command;      /* first non-option positional */
    const char *  command_arg;  /* second non-option positional (flash <file>, set-address <n>) */
} cli_args_t;

void parse_args(int argc, char ** argv, cli_args_t & args);

#endif
