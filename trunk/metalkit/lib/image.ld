/*
 * GNU Linker script for assembling a Metalkit binary image.
 *
 * Notable changes from ld's default behaviour:
 *
 *   - Load address is at the 1MB boundary.
 *
 *   - Our binary begins with a .boot section.
 *
 *   - The end of the data section is padded to a
 *     512-byte boundary, to make sure that our disk
 *     image ends on a sector boundary. (Required by QEMU)
 *
 *   - We calculate a few auxiliary values used by the
 *     bootloader, which depend on knowing the size of
 *     the entire binary.
 */

OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(_start)

/*
 * Stack starts at the top of the usable portion of the first 1MB, and
 * grows downward.
 */
_stack = 0x9fffc;

SECTIONS
{
   . = 0x100000;

   .text : {
      _file_origin = .;
      *(.boot);
      *(.text .text.*);
    }

   .data : {
      *(.rodata .rodata.* .data .data.*)
      _edata = .;

      _sector_padding = .;
      . = ALIGN(512);
      _sector_padding_end = .;
   }

   .bss : {
      __bss_start = .;
      *(.bss .bss.*);
   }

   _end = .;
}

_bss_size = _end - _edata;
_image_size = _edata - _file_origin;

/*
 * Disk geometry. CHS geometry is mostly irrelevant these days, so we
 * just pick something that will make fdisk happy. It tries to
 * autodetect the disk size by looking at the disk's existing
 * partitions, so the easiest way to keep it happy is to align the
 * partition to a cylinder boundary.
 *
 * We'd like to use a floppy-disk-compatible geometry for images that
 * are small enough to fit on a 1.44 MB disk, but for larger images we
 * need to use a bigger geometry so that our cylinder numbers can fit
 * in 10 bits. This larger geometry has 1 megabyte cylinders, so we
 * can address 1 GB without breaking the 10 bit boundary.
 */

_geom_large_disk = _image_size >= (2880 * 512);
_geom_sectors_per_head = _geom_large_disk ? 32 : 18;
_geom_heads_per_cylinder = _geom_large_disk ? 64 : 2;
_geom_sectors_per_cylinder = _geom_sectors_per_head * _geom_heads_per_cylinder;

/*
 * Partition is just big enough to hold our initialized data, rounded
 * up to the nearest cylinder. The "_partition_chs_cylinder" is the
 * number of the last cylinder in the partition. Also note that
 * sector numbers are 1-based.
 */
_image_sectors = (_image_size + 511) / 512;
_partition_chs_cylinder = _image_sectors / _geom_sectors_per_cylinder;
_partition_blocks = (_partition_chs_cylinder + 1) * _geom_sectors_per_cylinder;
_partition_chs_head = _geom_heads_per_cylinder - 1;
_partition_chs_sector = _geom_sectors_per_head;

/*
 * Encode the sector and cylinder bytes in the format expected by MBR
 * partition tables.
 */
_partition_chs_cylinder_byte = _partition_chs_cylinder & 0xff;
_partition_chs_sector_byte = _partition_chs_sector |
   ((_partition_chs_cylinder - _partition_chs_cylinder_byte) >> 2);
