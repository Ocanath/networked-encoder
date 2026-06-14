#include "args.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void print_help(const char * prog)
{
    printf(
        "Usage: %s --addr <n> [--port <port>] [--baud <rate>] <command> [arg]\n"
        "\n"
        "Required:\n"
        "  --addr <n>          DARTT device address (0-255)\n"
        "\n"
        "Comms options:\n"
        "  --port <port>       Serial port (e.g. /dev/ttyUSB0, COM3); autoconnects if omitted\n"
        "  --baud <rate>       Baud rate (default 921600)\n"
        "\n"
        "Encoder commands:\n"
        "  read-angle          Print angle in radians\n"
        "  read-adc            Print raw sin/cos ADC values\n"
        "  read-fds            Print flash data store contents\n"
        "  set-zero            Set current position as zero\n"
        "  calibrate           Interactive sin/cos min/max calibration\n"
        "  save-fds            Commit current FDS to device flash\n"
        "  restart             Restart encoder firmware\n"
        "  bootload            Enter bootloader mode\n"
        "  set-address <n>     Change DARTT address (encoder + bootloader)\n"
        "\n"
        "Bootloader commands:\n"
        "  flash <file>        Flash firmware binary via bootloader\n"
        "  bl-version          Print bootloader firmware version\n"
        "  bl-start            Start application from bootloader\n"
        "\n"
        "  -h, --help          Print this help and exit\n",
        prog
    );
}

static void die(const char * msg)
{
    fprintf(stderr, "error: %s\n", msg);
    exit(1);
}

static unsigned long parse_ulong(const char * s, const char * name)
{
    char * end;
    unsigned long v = strtoul(s, &end, 0);
    if (*end != '\0')
    {
        fprintf(stderr, "error: invalid value for %s: '%s'\n", name, s);
        exit(1);
    }
    return v;
}

void parse_args(int argc, char ** argv, cli_args_t & args)
{
    if (argc < 2)
    {
        print_help(argv[0]);
        exit(1);
    }

    args.port        = nullptr;
    args.addr        = 0;
    args.baud        = 921600;
    args.command     = nullptr;
    args.command_arg = nullptr;

    bool has_addr = false;

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            print_help(argv[0]);
            exit(0);
        }
        else if (strcmp(argv[i], "--port") == 0)
        {
            if (i + 1 >= argc) die("'--port' requires a port argument");
            args.port = argv[++i];
        }
        else if (strcmp(argv[i], "--addr") == 0)
        {
            if (i + 1 >= argc) die("'--addr' requires an address argument");
            unsigned long v = parse_ulong(argv[++i], "--addr");
            if (v > 255)
            {
                fprintf(stderr, "error: address '%s' out of range (0-255)\n", argv[i]);
                exit(1);
            }
            args.addr = (unsigned char)v;
            has_addr = true;
        }
        else if (strcmp(argv[i], "--baud") == 0)
        {
            if (i + 1 >= argc) die("'--baud' requires a value argument");
            args.baud = parse_ulong(argv[++i], "--baud");
        }
        else if (argv[i][0] == '-')
        {
            fprintf(stderr, "error: unknown option '%s'\n", argv[i]);
            exit(1);
        }
        else if (!args.command)
        {
            args.command = argv[i];
        }
        else if (!args.command_arg)
        {
            args.command_arg = argv[i];
        }
        else
        {
            fprintf(stderr, "error: unexpected argument '%s'\n", argv[i]);
            exit(1);
        }
    }

    if (!has_addr) die("--addr is required");
    if (!args.command) die("a command is required");
}
