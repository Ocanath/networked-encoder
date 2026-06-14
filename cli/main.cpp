#include <iostream>
#include <string>
#include <cstring>
#include "args.h"
#include "encoder.h"
#include "dartt_map.h"
#include "dartt_flasher.h"
#include "serial.h"

static bool open_serial(Serial & ser, const cli_args_t & a)
{
    if (a.port) return ser.connect(a.port, a.baud);
    return ser.autoconnect(a.baud);
}

static int flasher_init(DarttFlasher & flasher, const cli_args_t & a)
{
    if (a.port) flasher.ser.connect(a.port, a.baud);
    else        flasher.ser.autoconnect(a.baud);
    return flasher.init();
}

// --- encoder commands ---

static int cmd_read_angle(const cli_args_t & a)
{
    Serial ser;
    if (!open_serial(ser, a)) { std::cerr << "connect failed\n"; return -1; }
    Encoder enc(a.addr, &ser);
    int rc = enc.read_angle();
    if (rc) { std::cerr << "read_angle failed: " << rc << "\n"; return rc; }
    std::cout << enc.theta << "\n";
    return 0;
}

static int cmd_read_adc(const cli_args_t & a)
{
    Serial ser;
    if (!open_serial(ser, a)) { std::cerr << "connect failed\n"; return -1; }
    Encoder enc(a.addr, &ser);
    int rc = enc.read_adc_raw();
    if (rc) { std::cerr << "read_adc_raw failed: " << rc << "\n"; return rc; }
    // ADC_SIN_CHAN = 1, ADC_COS_CHAN = 0 (from tle_encoder.h)
    std::cout << "sin = " << enc.dp_periph.dma_adc_raw[1] << "\n";
    std::cout << "cos = " << enc.dp_periph.dma_adc_raw[0] << "\n";
    return 0;
}

static int cmd_read_fds(const cli_args_t & a)
{
    // TODO: add a read_fds slice to Encoder and read fds fields from dp_periph
    Serial ser;
    if (!open_serial(ser, a)) { std::cerr << "connect failed\n"; return -1; }
    Encoder enc(a.addr, &ser);
    (void)enc;
    std::cerr << "TODO: read_fds\n";
    return 0;
}

static int cmd_set_zero(const cli_args_t & a)
{
    // TODO: read periph.angle and periph.fds.offset, compute
    //       new_offset = periph.angle + periph.fds.offset,
    //       write to dp_ctl.fds.offset, call write_offset(), then save-fds
    Serial ser;
    if (!open_serial(ser, a)) { std::cerr << "connect failed\n"; return -1; }
    Encoder enc(a.addr, &ser);
    (void)enc;
    std::cerr << "TODO: set_zero\n";
    return 0;
}

static int cmd_calibrate(const cli_args_t & a)
{
    // TODO: stream ADC values continuously, capture min/max on keypress,
    //       write sin_min/sin_max/cos_min/cos_max to dp_ctl.fds, call write_fds()
    Serial ser;
    if (!open_serial(ser, a)) { std::cerr << "connect failed\n"; return -1; }
    Encoder enc(a.addr, &ser);
    (void)enc;
    std::cerr << "TODO: calibrate\n";
    return 0;
}

static int cmd_save_fds(const cli_args_t & a)
{
    Serial ser;
    if (!open_serial(ser, a)) { std::cerr << "connect failed\n"; return -1; }
    Encoder enc(a.addr, &ser);
    int rc = enc.write_fds();
    if (rc) { std::cerr << "write_fds failed: " << rc << "\n"; return rc; }
    rc = enc.write_action_flag(FS_SAVE);
    if (rc) { std::cerr << "FS_SAVE failed: " << rc << "\n"; return rc; }
    return 0;
}

static int cmd_restart(const cli_args_t & a)
{
    Serial ser;
    if (!open_serial(ser, a)) { std::cerr << "connect failed\n"; return -1; }
    Encoder enc(a.addr, &ser);
    int rc = enc.write_action_flag(RESTART);
    if (rc) std::cerr << "restart failed: " << rc << "\n";
    return rc;
}

static int cmd_bootload(const cli_args_t & a)
{
    Serial ser;
    if (!open_serial(ser, a)) { std::cerr << "connect failed\n"; return -1; }
    Encoder enc(a.addr, &ser);
    int rc = enc.write_action_flag(BOOTLOAD);
    if (rc) std::cerr << "bootload failed: " << rc << "\n";
    return rc;
}

static int cmd_set_address(const cli_args_t & a)
{
    if (!a.command_arg) { std::cerr << "set-address requires a new address\n"; return -1; }
    unsigned char new_addr = (unsigned char)atoi(a.command_arg);

    // Update encoder FDS and commit to flash
    {
        Serial ser;
        if (!open_serial(ser, a)) { std::cerr << "connect failed\n"; return -1; }
        Encoder enc(a.addr, &ser);
        enc.dp_ctl.fds.address = new_addr;
        int rc = enc.write_fds();
        if (rc) { std::cerr << "write_fds failed: " << rc << "\n"; return rc; }
        rc = enc.write_action_flag(FS_SAVE);
        if (rc) { std::cerr << "FS_SAVE failed: " << rc << "\n"; return rc; }
    }

    // Update bootloader address (device must already be in bootloader mode)
    // TODO: optionally trigger BOOTLOAD via encoder first, wait, then connect flasher
    {
        DarttFlasher flasher(a.addr);
        int rc = flasher_init(flasher, a);
        if (rc) { std::cerr << "flasher init failed: " << rc << "\n"; return rc; }
        rc = flasher.update_target_address(new_addr);
        if (rc) { std::cerr << "update_target_address failed: " << rc << "\n"; return rc; }
    }
    return 0;
}

// --- bootloader commands ---

static int cmd_flash(const cli_args_t & a)
{
    if (!a.command_arg) { std::cerr << "flash requires a filename\n"; return -1; }
    // TODO: optionally trigger BOOTLOAD via encoder, wait for device to re-enumerate
    DarttFlasher flasher(a.addr);
    int rc = flasher_init(flasher, a);
    if (rc) { std::cerr << "flasher init failed: " << rc << "\n"; return rc; }
    rc = flasher.write_bin(a.command_arg, /*verify=*/true);
    if (rc) { std::cerr << "write_bin failed: " << rc << "\n"; return rc; }
    return flasher.start_app();
}

static int cmd_bl_version(const cli_args_t & a)
{
    DarttFlasher flasher(a.addr);
    int rc = flasher_init(flasher, a);
    if (rc) { std::cerr << "flasher init failed: " << rc << "\n"; return rc; }
    std::string ver;
    rc = flasher.get_version(ver);
    if (rc) { std::cerr << "get_version failed: " << rc << "\n"; return rc; }
    std::cout << ver << "\n";
    return 0;
}

static int cmd_bl_start(const cli_args_t & a)
{
    DarttFlasher flasher(a.addr);
    int rc = flasher_init(flasher, a);
    if (rc) { std::cerr << "flasher init failed: " << rc << "\n"; return rc; }
    return flasher.start_app();
}

// --- dispatch ---

int main(int argc, char ** argv)
{
    cli_args_t a;
    parse_args(argc, argv, a);

    if (strcmp(a.command, "read-angle")  == 0) return cmd_read_angle(a);
    if (strcmp(a.command, "read-adc")    == 0) return cmd_read_adc(a);
    if (strcmp(a.command, "read-fds")    == 0) return cmd_read_fds(a);
    if (strcmp(a.command, "set-zero")    == 0) return cmd_set_zero(a);
    if (strcmp(a.command, "calibrate")   == 0) return cmd_calibrate(a);
    if (strcmp(a.command, "save-fds")    == 0) return cmd_save_fds(a);
    if (strcmp(a.command, "restart")     == 0) return cmd_restart(a);
    if (strcmp(a.command, "bootload")    == 0) return cmd_bootload(a);
    if (strcmp(a.command, "set-address") == 0) return cmd_set_address(a);
    if (strcmp(a.command, "flash")       == 0) return cmd_flash(a);
    if (strcmp(a.command, "bl-version")  == 0) return cmd_bl_version(a);
    if (strcmp(a.command, "bl-start")    == 0) return cmd_bl_start(a);

    fprintf(stderr, "error: unknown command '%s'\n", a.command);
    return 1;
}
