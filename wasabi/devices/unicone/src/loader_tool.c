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
static char*  config_bitstream_path = "fpga/test/test.bit";
static char*  config_firmware_path = "firmware.bin";

static int loader()
{
  struct unicone_device* dev;
  struct progress_reporter* progress = progress_reporter_console_new();

  unicone_usb_init();
  dev = unicone_device_new();
  if (!dev)
    return 1;

  if (dev->firmware_installed && config_force_firmware) {
    printf("Firmware already loaded, rebooting to clear it.\n");
    unicone_device_remove_firmware(dev);
    if (unicone_device_reconnect(dev, progress, 1000) < 0)
      return 1;
  }

  if (dev->firmware_installed) {
    printf("Firmware already installed.\n");
  }
  else {
    printf("Firmware not installed.\n");
    if (unicone_device_upload_firmware(dev, config_firmware_path, progress) < 0)
      return 1;
    if (unicone_device_reconnect(dev, progress, 1000) < 0)
      return 1;
  }

  if (dev->fpga_configured && !config_force_bitstream) {
    printf("FPGA already configured.\n");
  }
  else {
    if (unicone_device_upload_bitstream(dev, config_bitstream_path, progress, 1) < 0)
      return 1;
  }

  unicone_device_delete(dev);
  return 0;
}

static void usage(char *progname)
{
  fprintf(stderr,
	  "Usage: %s [options]\n"
	  "\n"
	  "Configure an attached Unicone device with firmware and an FPGA\n"
	  "bitstream. By default, any existing firmware or bitstream on the\n"
	  "device will not be disturbed, but this may be overridden\n"
	  "the indicated server. In addition to or instead of the explicit\n"
	  "devices given on the command line, this can automatically detect\n"
	  "categories of devices to send to the server using the --hotplug-*\n"
	  "command line options.\n"
	  "\n"
	  "  -h, --help                     This text\n"
	  "  -f PATH, --firmware PATH       Set the raw binary file to load TUSB3410\n"
          "                                 firmware from [%s]\n"
	  "  -b PATH, --bitstream PATH      Set the Xilinx .bit file to load FPGA\n"
	  "                                 configuration from [%s]\n"
	  "  -F, --force-firmware           Always load firmware, even if the device\n"
          "                                 already has firmware installed on it\n"
	  "  -B, --force-bitstream          Always load a bitstream, even if the\n"
	  "                                 FPGA has already been configured\n",
	  progname, config_firmware_path, config_bitstream_path);
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

  if (argv[optind]) {
    usage(argv[0]);
    return 1;
  }

  return loader();
}

/* The End */
