#include <nds.h>
#include <nds/arm9/console.h>
#include <stdio.h>
#include <loader-exit.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "Vera_ttf.h"
#include "debug.h"

#define FPS_FREQUENCY  100
#define FPS_RESOLUTION 3

FT_Library ft_lib;
FT_Face ft_face;
unsigned int video_frame;
uint16 *video_buffer;
vuint32 video_fps_integer;
vuint32 video_fps_hundredths;

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

    err = FT_Set_Char_Size(ft_face, 0, 20 * 64, 0, 0);
    ASSERT_EQUAL(err, 0);

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
    video_fps_integer = fps / FPS_RESOLUTION;
    video_fps_hundredths = (fps * 100 / FPS_RESOLUTION) % 100;
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
    BG_PALETTE_SUB[255] = RGB15(31, 31, 31);
    consoleInitDefault((uint16*) SCREEN_BASE_BLOCK_SUB(31),
		       (uint16*) CHAR_BASE_BLOCK_SUB(0), 16);
}

void video_draw_rect(int x, int y, int w, int h, uint16 color)
{
    uint16 *p = video_buffer + x + SCREEN_WIDTH * y;
    int i;
    int gap = SCREEN_WIDTH - w;

    while (h > 0) {
	for (i=0; i<w; i++) {
	    *(p++) = color;
	}
	p += gap;
	h--;
    }
}

void video_draw_static(void)
{
    static unsigned int foo = 1;
    int i;
    for (i=0; i < (SCREEN_WIDTH * SCREEN_HEIGHT); i++) {
	int g = (3 & foo);
	g = (g << 3) | (g << 1) | (g >> 1);
	video_buffer[i] = RGB15(g,g,g);
	foo = (foo << 1) ^ (foo >> 15) ^ (1 & (foo >> 5)) ^ (1 & (foo >> 3));
    }
    foo++;
}

int main(void)
{
    irqInit();
    video_init();
    fonts_init();

    while (1) {
	video_draw_static();
	video_draw_rect(5,5,32,32, RGB15(0,0,0));

	LOG("Loading glyph\n");
	FT_Load_Char(ft_face, 'A', 0);
	LOG("Rendering glyph\n");
	FT_Render_Glyph(&ft_face->glyph, 0);

	swiWaitForVBlank();
	video_flip();

	scanKeys();
	DEBUG_ONLY({
	    if (keysHeld() == KEY_START) {
		Loader_Exit9();
		iprintf("No loader found!\n");
	    }
	});
    }

    return 0;
}
