#include "args.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void print_help(const char * prog)
{
    printf(
        "Usage: %s <addr> [--port <port>] [--baud <rate>] <command> [arg]\n"
        "\n"
        "  addr                DARTT device address (0-255)\n"
        "\n"
        "Comms options:\n"
        "  --port <port>       Serial port (e.g. /dev/ttyUSB0, COM3); autoconnects if omitted\n"
        "  --baud <rate>       Baud rate (default 921600)\n"
        "\n"
        "Encoder commands:\n"
        "  --read-angle        Print angle (motor address protocol, q13)\n"
        "  --read-angle-misc   Print angle (DARTT misc register, q14)\n"
        "  --read-adc          Print raw sin/cos ADC values\n"
        "  --read-fds          Print flash data store contents\n"
        "  --set-zero          Set current position as zero\n"
        "  --calibrate         Interactive sin/cos min/max calibration\n"
        "  --save-fds          Commit current FDS to device flash\n"
        "  --restart           Restart encoder firmware\n"
        "  --bootload          Enter bootloader mode\n"
        "  --set-address <n>   Change DARTT address (encoder + bootloader)\n"
        "  --current-bootloader-address <n>\n"
        "                      Current bootloader address if it differs from addr\n"
        "\n"
        "Bootloader commands:\n"
        "  --flash <file>      Flash firmware binary via bootloader\n"
        "  --bl-version        Print bootloader firmware version\n"
        "  --bl-start          Start application from bootloader\n"
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

    memset(&args, 0, sizeof(args));
    args.baud = 921600;

    bool has_addr = false;
    args.has_bootloader_addr = false;

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
        else if (strcmp(argv[i], "--baud") == 0)
        {
            if (i + 1 >= argc) die("'--baud' requires a value argument");
            args.baud = parse_ulong(argv[++i], "--baud");
        }
        else if (strcmp(argv[i], "--read-angle") == 0)       { args.read_angle      = true; }
        else if (strcmp(argv[i], "--read-angle-misc") == 0)  { args.read_angle_misc = true; }
        else if (strcmp(argv[i], "--read-adc")   == 0)  { args.read_adc   = true; }
        else if (strcmp(argv[i], "--read-fds")   == 0)  { args.read_fds   = true; }
        else if (strcmp(argv[i], "--set-zero")   == 0)  { args.set_zero   = true; }
        else if (strcmp(argv[i], "--calibrate")  == 0)  { args.calibrate  = true; }
        else if (strcmp(argv[i], "--save-fds")   == 0)  { args.save_fds   = true; }
        else if (strcmp(argv[i], "--restart")    == 0)  { args.restart    = true; }
        else if (strcmp(argv[i], "--bootload")   == 0)  { args.bootload   = true; }
		else if (strcmp(argv[i], "--scan") 		 == 0)	{ args.scan		  = true; }
        else if (strcmp(argv[i], "--bl-version") == 0)  { args.bl_version = true; }
        else if (strcmp(argv[i], "--bl-start")   == 0)  { args.bl_start   = true; }
        else if (strcmp(argv[i], "--set-address") == 0)
        {
            if (i + 1 >= argc) die("'--set-address' requires an address argument");
            unsigned long v = parse_ulong(argv[++i], "--set-address");
            if (v > 255) { fprintf(stderr, "error: address '%s' out of range (0-255)\n", argv[i]); exit(1); }
            args.new_address = (unsigned char)v;
            args.set_address = true;
        }
        else if (strcmp(argv[i], "--current-bootloader-address") == 0)
        {
            if (i + 1 >= argc) die("'--current-bootloader-address' requires an address argument");
            unsigned long v = parse_ulong(argv[++i], "--current-bootloader-address");
            if (v > 255) { fprintf(stderr, "error: address '%s' out of range (0-255)\n", argv[i]); exit(1); }
            args.bootloader_addr = (unsigned char)v;
            args.has_bootloader_addr = true;
        }
        else if (strcmp(argv[i], "--flash") == 0)
        {
            if (i + 1 >= argc) die("'--flash' requires a filename argument");
            args.filename = argv[++i];
            args.flash = true;
        }
        else if (argv[i][0] == '-')
        {
            fprintf(stderr, "error: unknown option '%s'\n", argv[i]);
            exit(1);
        }
        else if (!has_addr)
        {
            unsigned long v = parse_ulong(argv[i], "addr");
            if (v > 255)
            {
                fprintf(stderr, "error: address '%s' out of range (0-255)\n", argv[i]);
                exit(1);
            }
            args.addr = (unsigned char)v;
            has_addr = true;
        }
        else
        {
            fprintf(stderr, "error: unexpected argument '%s'\n", argv[i]);
            exit(1);
        }
    }

    if (!has_addr) die("address is required as the first argument");
    if (!args.has_bootloader_addr){ args.bootloader_addr = args.addr;}
}
