/*
 * loader_tool.c - A utility for downloading firmware and/or
 *                 FPGA configuration to the Unicone
 *
 * Universal Controller Emulator project
 * Copyright (C) 2004 Micah Dowty
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include <stdio.h>
#include <getopt.h>
#include "device.h"
#include "progress.h"

static int    config_force_bitstream = 0;
static int    config_force_firmware = 0;
static char*  config_bitstream_path = NULL;
static char*  config_firmware_path = "firmware.bin";

static int loader()
{
  struct unicone_device* dev;
  struct progress_reporter* progress = progress_reporter_console_new();
  int need_firmware_install, need_bitstream_install;
  int retval;

  unicone_usb_init();
  dev = unicone_device_new();
  if (!dev) {
    printf("Can't open unicone device\n");
    return 1;
  }

  /* Decide whether we need to install firmware */
  if (config_force_firmware)
    need_firmware_install = 1;
  else {
    retval = unicone_device_compare_firmware(dev, config_firmware_path, progress);
    if (retval < 0) {
      printf("Error checking firmware version\n");
      return 1;
    }
    need_firmware_install = (retval != 0);
  }

  if (need_firmware_install) {
    /* Firmware needs upgrading or needs initial installation */

    if (dev->firmware_installed) {
      printf("Firmware already loaded, rebooting to clear it.\n");
      unicone_device_remove_firmware(dev);
      if (unicone_device_reconnect(dev, progress) < 0)
	return 1;
    }

    if (unicone_device_upload_firmware(dev, config_firmware_path, progress) < 0)
      return 1;
    if (unicone_device_reconnect(dev, progress) < 0)
      return 1;
  }
  else {
    printf("Firmware is up to date.\n");
  }

  /* Decide whether we need to install a bitstream */
  if (config_force_bitstream)
    need_bitstream_install = 1;
  else {
    retval = unicone_device_compare_bitstream(dev, config_bitstream_path, progress);
    if (retval < 0) {
      printf("Error checking bitstream version\n");
      return 1;
    }
    need_bitstream_install = (retval != 0);
  }

  if (need_bitstream_install) {
    if (unicone_device_upload_bitstream(dev, config_bitstream_path, progress) < 0)
      return 1;
  }
  else {
    printf("FPGA configuration is up to date.\n");
  }

  unicone_device_delete(dev);
  return 0;
}

static void usage(char *progname)
{
  fprintf(stderr,
	  "Usage: %s [options] bitstream-file\n"
	  "\n"
	  "Configure an attached Unicone device with firmware and an FPGA\n"
	  "bitstream. By default, this uses SHA-1 digests to avoid reloading\n"
	  "the firmware or bitstream if an identical copy is already installed.\n"
	  "The provided bitstream must be in Xilinx .bit format.\n"
	  "\n"
	  "  -h, --help                     This text\n"
	  "  -f PATH, --firmware PATH       Set the raw binary file to load TUSB3410\n"
          "                                 firmware from [%s]\n"
	  "  -F, --force-firmware           Always load firmware, even if the device\n"
          "                                 already has firmware installed on it\n"
	  "  -B, --force-bitstream          Always load a bitstream, even if the\n"
	  "                                 FPGA has already been configured\n",
	  progname, config_firmware_path);
}


int main(int argc, char **argv)
{
  int c;

  while (1) {
    static struct option long_options[] = {
      {"help",            0, 0, 'h'},
      {"firmware",        1, 0, 'f'},
      {"bitstream",       1, 0, 'b'},
      {"force-firmware",  0, 0, 'F'},
      {"force-bitstream", 0, 0, 'B'},
      {0},
    };

    c = getopt_long(argc, argv, "hf:b:FB", long_options, NULL);
    if (c == -1)
      break;
    switch (c) {

    case 'f':
      config_firmware_path = optarg;
      break;

    case 'b':
      config_bitstream_path = optarg;
      break;

    case 'F':
      config_force_firmware = 1;
      break;

    case 'B':
      config_force_bitstream = 1;
      break;

    case 'h':
    default:
      usage(argv[0]);
      return 1;
    }
  }

  /* The bitstream filename should be on the command line */
  if (!argv[optind]) {
    usage(argv[0]);
    return 1;
  }
  config_bitstream_path = argv[optind++];

  /* Should be no more arguments left */
  if (argv[optind]) {
    usage(argv[0]);
    return 1;
  }

  return loader();
}

/* The End */
