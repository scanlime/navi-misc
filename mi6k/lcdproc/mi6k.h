#ifndef MI6K_H
#define MI6K_H

#include "lcd.h"

#define DEFAULT_CELL_WIDTH     5
#define DEFAULT_CELL_HEIGHT    7
#define DEFAULT_CONTRAST       560
#define DEFAULT_DEVICE         "/dev/usb/mi6k0"
#define DEFAULT_BRIGHTNESS     0
#define DEFAULT_OFFBRIGHTNESS  7
#define DEFAULT_SIZE           "20x2"


MODULE_EXPORT int  mi6k_init (Driver * drvthis, char *device);
MODULE_EXPORT void mi6k_close (Driver * drvthis);
MODULE_EXPORT int  mi6k_width (Driver * drvthis);
MODULE_EXPORT int  mi6k_height (Driver * drvthis);
MODULE_EXPORT void mi6k_clear (Driver * drvthis);
MODULE_EXPORT void mi6k_flush (Driver * drvthis);
MODULE_EXPORT void mi6k_string (Driver * drvthis, int x, int y, char
				   string[]);
MODULE_EXPORT void mi6k_chr (Driver * drvthis, int x, int y, char c);

MODULE_EXPORT void mi6k_vbar (Driver * drvthis, int x, int y, int
				 len, int promille, int options);
MODULE_EXPORT void mi6k_hbar (Driver * drvthis, int x, int y, int
				 len, int promille, int options);
MODULE_EXPORT int  mi6k_icon(Driver * drvthis, int x, int y, int
				icon);

MODULE_EXPORT void mi6k_set_char (Driver * drvthis, int n, char
				     *dat);

MODULE_EXPORT int  mi6k_get_contrast (Driver * drvthis);
MODULE_EXPORT void mi6k_set_contrast (Driver * drvthis, int
					 contrast);

MODULE_EXPORT void mi6k_backlight (Driver * drvthis, int on);

#endif
