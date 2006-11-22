#include <nds.h>
#include <nds/arm9/console.h>
#include <nds/arm9/trig_lut.h>
#include <stdio.h>
#include <loader-exit.h>
#include "Vera_ttf.h"
#include "debug.h"
#include <ft2build.h>
#include FT_FREETYPE_H

#define FT_FLOOR(x)     (((x) & -64) / 64)
#define FT_CEIL(x)      ((((x) + 63) & -64) / 64)
#define FT_FIXED(x)     ((x) * 64)

#define FPS_FREQUENCY  10
#define FPS_RESOLUTION 3

FT_Library ft_lib;
FT_Face ft_face;
unsigned int video_frame;
uint16 *video_buffer;
char video_fps[32];

void fonts_init(void)
{
    FT_Error err;

    LOG("Initializing Freetype");

    err = FT_Init_FreeType(&ft_lib);
    ASSERT_EQUAL(err, 0);

    FT_Open_Args openArgs = {
	.flags = FT_OPEN_MEMORY,
	.memory_size = Vera_ttf_size,
	.memory_base = Vera_ttf,
    };
    err = FT_Open_Face(ft_lib, &openArgs, 0, &ft_face);
    ASSERT_EQUAL(err, 0);
    ASSERT(ft_face != NULL);

    LOG("Done initializing freetype");
}

void video_flip(void)
{
    video_frame++;

    if (video_frame & 1) {
	videoSetMode(MODE_FB0);
	video_buffer = VRAM_B;
    } else {
	videoSetMode(MODE_FB1);
	video_buffer = VRAM_A;
    }
}

void fps_timer_irq(void)
{
    static int fps_ring_buffer[FPS_FREQUENCY * FPS_RESOLUTION];
    static int fps_head;

    unsigned int prev_video_frame = fps_ring_buffer[fps_head];
    fps_ring_buffer[fps_head] = video_frame;
    fps_head = (fps_head + 1) % (sizeof fps_ring_buffer / sizeof fps_ring_buffer[0]);

    unsigned int fps = video_frame - prev_video_frame;
    sprintf(video_fps, "%d.%02d FPS",
	    fps / FPS_RESOLUTION,
	    (fps * 100 / FPS_RESOLUTION) % 100);
}

void video_init(void)
{
    irqEnable(IRQ_VBLANK);
    powerSET(POWER_ALL_2D);
    lcdMainOnTop();

    /* Set up a timer to count FPS */
    TIMER3_DATA = TIMER_FREQ_1024(FPS_FREQUENCY);
    TIMER3_CR = TIMER_DIV_1024 | TIMER_IRQ_REQ;
    irqSet(IRQ_TIMER3, fps_timer_irq);
    irqEnable(IRQ_TIMER3);

    /*
     * Set up the main display for a 16-bit 256x256 double-buffered framebuffer,
     * and set up the secondary display for console mode (Using BG0 and VRAM bank C).
     */
    video_flip();
    videoSetModeSub(MODE_0_2D | DISPLAY_BG0_ACTIVE);
    vramSetMainBanks(VRAM_A_LCD, VRAM_B_LCD,
		     VRAM_C_SUB_BG, VRAM_D_LCD);

    SUB_BG0_CR = BG_MAP_BASE(31);
    BG_PALETTE_SUB[0] = RGB15(0, 0, 0);
    BG_PALETTE_SUB[255] = RGB15(31, 31, 31);

    consoleInitDefault((uint16*) SCREEN_BASE_BLOCK_SUB(31),
		       (uint16*) CHAR_BASE_BLOCK_SUB(0), 16);
}

void video_draw_ftbitmap(int x, int y, FT_Bitmap *bitmap)
{
    uint8 *src = bitmap->buffer;
    uint16 *dest = video_buffer + x + SCREEN_WIDTH * y;
    int width = bitmap->width / 3;
    int height = bitmap->rows;
    int dest_gap = SCREEN_WIDTH - width;
    int src_gap = bitmap->pitch - bitmap->width;
    int i;

    while (height > 0) {

	for (i=width; i; i--) {
	    /*
	     * Black text on a white background, BGR subpixel rendering
	     */
	    uint8 r = *(src++);
	    uint8 g = *(src++);
	    uint8 b = *(src++);

	    if (r || g || b) {
		*dest = RGB15((0xFF - b) >> 3,
			      (0xFF - g) >> 3,
			      (0xFF - r) >> 3);
	    }
	    dest++;
	}

	src += src_gap;
	dest += dest_gap;
	height--;
    }

}

void video_draw_string(int x, int y, FT_Face face, const char *string)
{
    int ascent = FT_CEIL(FT_MulFix(face->bbox.yMax, face->size->metrics.y_scale));
    y += ascent;

    while (*string) {
	FT_Load_Char(face, *string, FT_LOAD_RENDER | FT_LOAD_TARGET_LCD);
	string++;

	video_draw_ftbitmap(x + FT_FLOOR(face->glyph->metrics.horiBearingX),
			    y - FT_FLOOR(face->glyph->metrics.horiBearingY),
			    &face->glyph->bitmap);

	x += FT_CEIL(face->glyph->metrics.horiAdvance);
    }
}


int main(void)
{
    int hello_size = FT_FIXED(20);

    DEBUG_ONLY(setExceptionHandler(exception_handler));
    irqInit();
    video_init();
    fonts_init();

    iprintf("\n[UP] / [DOWN] scales the font\n"
	    "[X] disables VBL synch\n"
	    "[Y] hilights background\n"
	    "[A] disables background\n"
	    "[Start] exits to loader\n");

    while (1) {
	uint16 background = (keysHeld() & KEY_Y) ? 
	    RGB15(15, 15, 15) : RGB15(31, 31, 31);
	if (!(keysHeld() & KEY_A)) {
	    swiCopy(&background, video_buffer, 
		    COPY_MODE_HWORD | COPY_MODE_FILL | (SCREEN_WIDTH * SCREEN_HEIGHT));
	}

	FT_Set_Char_Size(ft_face, 0, hello_size, 0, 0);
	video_draw_string(10, 10, ft_face, "Hello World!");

	FT_Set_Char_Size(ft_face, 0, FT_FIXED(12), 0, 0);	
	video_draw_string(10, SCREEN_HEIGHT - 25, ft_face, video_fps);

	if (!(keysHeld() & KEY_X)) {
	    swiWaitForVBlank();
	}
	video_flip();

	scanKeys();
	if (keysHeld() == KEY_START) {
	    Loader_Exit9();
	    iprintf("No loader found!\n");
	}
	if (keysHeld() & KEY_UP) {
	    hello_size += FT_FIXED(1) / 4;
	}
	if (keysHeld() & KEY_DOWN) {
	    hello_size -= FT_FIXED(1) / 4;
	}
    }

    return 0;
}
