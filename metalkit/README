"Metalkit" is what I'm calling this set of software
modules for assembling simple programs that operate
on the bare metal: they run on IA32 PCs without any
operating system.

This software is released under the MIT license, so
you're free to copy, modify, and distribute it for
commercial or noncommercial purposes.


What is this good for?
----------------------

- Learning about PC architecture or systems programming.

- For hardware (or virtual machine) developers, writing
  test cases or example programs that demonstrate hardware
  features.

- Simple applications (even games) that can target common
  virtual machines or emulators (QEMU, VMware, Bochs) as
  their platform.

- Fun!


What can it do so far?
----------------------

- It has a nifty bootloader which can bootstrap a 32-bit
  protected mode program from either a raw floppy disk
  image (convenient for Virtual Machines), from GRUB
  (convenient for real PCs), or from a USB thumb drive
  (supports very large binary images).

- When running from a USB disk on a BIOS that supports
  LBA mode, it can load very large binary images. I tested
  loading a 128MB image on my IBM Thinkpad.

- It has simple PCI bus support. So far it's enough to
  enumerate the attached devices.

- VGA text mode, with a simple Console abstraction.

- Fault handlers, and a "red screen of death" for Panic
  and Unhandled Fault errors.

- Support for 16-bit BIOS calls (switching back to real mode
  temporarily, then back to protected mode)

- VESA BIOS graphics.

- Interrupt handlers, very simple thread switching.

- It has a simple PS/2 keyboard driver.

- It supports basic PIT timer configuration.

- Tested on VMware, Bochs, and a real PC.


How can I use it?
-----------------

It's easy. On any x86 system with a 32-bit GNU toolchain installed,
enter an example directory and type 'make'. You should end up with a
'.img' file. You can either:

 - Reboot your PC, and use grub to boot this file as a 'kernel'.

 - Point your virtual machine or emulator of choice to boot.img as a
   floppy disk image.

 - Use 'dd' to copy the image to a USB disk, then use your BIO's boot
   menu to select the USB disk. Make sure to copy the image to the
   entire disk, not a single partition (like /dev/sdX, not /dev/sdX1).


What features are planned?
--------------------------

- Higher-level libraries for threading.

- An optional standard C library (probably newlib), for apps that need one.

- Higher-level interfaces for video
- A simple single-tasking event driven runloop.

- Video, mouse, and keyboard drivers. (For generic
  PC hardware, and for VMware's mouse and video
  devices.)

- Quake port?


Contact
-------

Micah Dowty <micah@navi.cx>
