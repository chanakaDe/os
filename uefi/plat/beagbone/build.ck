/*++

Copyright (c) 2014 Minoca Corp.

    This file is licensed under the terms of the GNU General Public License
    version 3. Alternative licensing terms are available. Contact
    info@minocacorp.com for details. See the LICENSE file at the root of this
    project for complete licensing information.

Module Name:

    BeagleBone UEFI Firmware

Abstract:

    This module implements UEFI firmware for the TI BeagleBone Black.

Author:

    Evan Green 19-Dec-2014

Environment:

    Firmware

--*/

function build() {
    plat = "bbone";
    text_address = "0x82000000";
    sources = [
        "armv7/entry.S",
        "clock.c",
        "debug.c",
        "fwvol.c",
        "i2c.c",
        "intr.c",
        "main.c",
        "memmap.c",
        ":" + plat + "fwv.o",
        "ramdenum.c",
        "sd.c",
        "serial.c",
        "smbios.c",
        "timer.c",
        "video.c"
    ];

    includes = [
        "$//uefi/include"
    ];

    sources_config = {
        "CFLAGS": ["-fshort-wchar"]
    };

    link_ldflags = [
        "-nostdlib",
        "-Wl,--no-wchar-size-warning",
        "-static"
    ];

    link_config = {
        "LDFLAGS": link_ldflags
    };

    common_libs = [
        "//uefi/core:ueficore",
        "//kernel/kd:kdboot",
        "//uefi/core:ueficore",
        "//uefi/archlib:uefiarch",
        "//lib/fatlib:fat",
        "//lib/basevid:basevid",
        "//lib/rtl/base:basertlb",
        "//kernel/kd/kdusb:kdnousb",
        "//kernel:archboot",
        "//uefi/core:emptyrd",
    ];

    libs = common_libs + [
        "//uefi/dev/sd/core:sd",
        "//uefi/dev/omapuart:omapuart"
    ];

    platfw = plat + "fw";
    elf = {
        "label": platfw + ".elf",
        "inputs": sources + libs,
        "sources_config": sources_config,
        "includes": includes,
        "config": link_config
    };

    entries = executable(elf);

    //
    // Build the firmware volume.
    //

    ffs = [
        "//uefi/core/runtime:rtbase.ffs",
        "//uefi/plat/beagbone/runtime:" + plat + "rt.ffs",
        "//uefi/plat/beagbone/acpi:acpi.ffs"
    ];

    fw_volume = uefi_fwvol_o(plat, ffs);
    entries += fw_volume;

    //
    // Flatten the firmware image and convert to u-boot.
    //

    flattened = {
        "label": platfw + ".bin",
        "inputs": [":" + platfw + ".elf"]
    };

    flattened = flattened_binary(flattened);
    entries += flattened;
    uboot_config = {
        "TEXT_ADDRESS": text_address,
        "MKUBOOT_FLAGS": "-c -a arm -f legacy"
    };

    uboot = {
        "label": platfw,
        "inputs": [":" + platfw + ".bin"],
        "orderonly": ["//uefi/tools/mkuboot:mkuboot"],
        "tool": "mkuboot",
        "config": uboot_config,
        "nostrip": TRUE
    };

    entries += binplace(uboot);
    return entries;
}

return build();
