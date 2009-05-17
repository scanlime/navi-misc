/* -*- Mode: C++; c-basic-offset: 4 -*-
 *
 * Utilities for text rendering, and drawing to our "text" background
 * layer, a bitmap layer that holds proportionally-spaced text and
 * backgrounds/borders for that text.
 *
 * Copyright (c) 2009 Micah Dowty <micah@navi.cx>
 *
 *    Permission is hereby granted, free of charge, to any person
 *    obtaining a copy of this software and associated documentation
 *    files (the "Software"), to deal in the Software without
 *    restriction, including without limitation the rights to use,
 *    copy, modify, merge, publish, distribute, sublicense, and/or sell
 *    copies of the Software, and to permit persons to whom the
 *    Software is furnished to do so, subject to the following
 *    conditions:
 *
 *    The above copyright notice and this permission notice shall be
 *    included in all copies or substantial portions of the Software.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *    OTHER DEALINGS IN THE SOFTWARE.
 */

#include <nds.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "uiText.h"
#include "font_table.h"


//********************************************************** Glyph


void Glyph::drawRow(int y, uint8_t *dest, TextColors colors) {
    /*
     * XXX: There's lots of room for optimization here...
     */

    uint32_t fgBits = getRow(y + 1) << 1;
    uint32_t borderBits = fgBits | ((getRow(y) | getRow(y + 2)) << 1);
    borderBits |= (borderBits << 1) | (borderBits >> 1);

    uint32_t mask = 1;
    for (int i = 0; i < getWidth(); i++) {
        if (mask & fgBits) {
            *dest = colors.fg;
        } else if (mask & borderBits) {
            *dest = colors.border;
        }
        dest++;
        mask <<= 1;
    }
}


//********************************************************** DefaultFont


int DefaultFont::getLineSpacing() {
    return 14;
}

Glyph DefaultFont::getGlyph(char c) {
    if (c < FONT_FIRST_CHAR || c > FONT_LAST_CHAR) {
        return Glyph(0, 0, NULL);
    } else {
        uint8_t *entry = &font_table[(c - FONT_FIRST_CHAR) << FONT_CHAR_SHIFT];
        return Glyph(entry[0], 15, entry + 1);
    }
}


//********************************************************** VScrollLayer


VScrollLayer::VScrollLayer() {
    /*
     * Initialize BG2.  Use 4 banks of memory in VRAM C, starting at
     * bank 3.  (We use the first 3 banks for background 3.)
     */
    bg = bgInitSub(2, BgType_Bmp8, BgSize_B8_256x256, 3, 0);

    yScroll = 0;
    resetClip();
    paint();

    bgWrapOn(bg);
}

VScrollLayer::~VScrollLayer() {
    clear();
    blit();
    bgWrapOff(bg);
    bgHide(bg);
}

void VScrollLayer::scrollTo(int y) {
    /*
     * Redraw the part of the screen that's coming into view.
     */

    if (y > yScroll) {
        /*
         * Viewport is moving down relative to the top of the image,
         * new data is showing up at the bottom of the screen.
         *
         * Note that we really do want SCREEN_HEIGHT here, not height
         * (the height of our buffer).  The bottom of the buffer wraps
         * to the top of the screen, so we would end up momentarily
         * displaying the new bottom-of-screen content at the top of
         * the screen, before the scrolling registers get updated.
         */
        clip.y1 = yScroll + SCREEN_HEIGHT;
        clip.y2 = y + SCREEN_HEIGHT;

    } else {
        /*
         * Other direction: Data is showing up at the top of the screen.
         */
        clip.y1 = y;
        clip.y2 = yScroll;
    }

    if (clip.y1 != clip.y2) {
        paint();
    }

    /*
     * Update the hardware scrolling.
     */

    yScroll = y;
    bgSetScroll(bg, 0, y);
}

void VScrollLayer::clear() {
    dirty.clear();

    if (clip.y1 != clip.y2) {
        Rect r;
        r.left = 0;
        r.right = width;
        r.top = clip.y1;
        r.bottom = clip.y2;
        drawRect(r, 0);
    }
}

void VScrollLayer::blit() {
    /*
     * Draw all dirty rectangles to the screen.
     */

    std::vector<Rect>::iterator i;

    for (i = dirty.rects.begin(); i != dirty.rects.end(); i++) {
        blitRect(*i);
    }
    dirty.rects.clear();
}

void VScrollLayer::paint() {
    /*
     * For subclasses to implement. By default, paint a blank screen.
     */
    clear();
    blit();
}

void VScrollLayer::drawRect(Rect r, uint8_t paletteIndex) {
    int y = r.top;
    int height = r.getHeight();
    int width = r.getWidth();
    uint32_t fillWord = (paletteIndex | (paletteIndex << 8) |
                         (paletteIndex << 16) | (paletteIndex << 24));

    while (height--) {
        if (inClip(y)) {
            uint32_t *line = linePtr32(y);

            if (width < 16) {
                /*
                 * Short fill: Just use one memset
                 */
                memset(r.left + (uint8_t*)line, paletteIndex, width);

            } else {
                /*
                 * Long fill: At least one full word and two partial
                 * words.  Break it up into pieces.
                 */

                if (r.left & 3) {
                    int leftRemainder = 4 - (r.left & 3);
                    memset(r.left + (uint8_t*)line, paletteIndex, leftRemainder);
                }

                int leftWord = (r.left + 3) >> 2;
                int rightWord = r.right >> 2;
                swiFastCopy(&fillWord, line + leftWord,
                            COPY_MODE_FILL | (rightWord - leftWord));

                if (r.right & 3) {
                    memset((r.right & ~3) + (uint8_t*)line, paletteIndex, r.right & 3);
                }
            }
        }
        y++;
    }

    addDirtyRect(r);
}

void VScrollLayer::addDirtyRect(Rect r) {
    dirty.add(r.align(4, 1));
}

void VScrollLayer::blitRect(Rect r) {
    sassert(r.isAligned(4, 1), "blitRect rectangle not aligned");
    sassert(!r.isEmpty(), "blitRect rectangle empty");

    int leftWord = r.left >> 2;
    int widthWords = r.getWidth() >> 2;
    uint32_t copyFlags = COPY_MODE_WORD | COPY_MODE_COPY | widthWords;

    for (int y = r.top; y < r.bottom; y++) {
        if (inClip(y)) {
            swiFastCopy(linePtr32(y) + leftWord,
                        destLinePtr32(y) + leftWord,
                        copyFlags);
        }
    }
}


//********************************************************** UITextLayer


UITextLayer::UITextLayer() {
    setWrapWidth(SCREEN_WIDTH);
    setAlignment(LEFT);
    setAutoclear(false);
    moveTo(0, 0);
}

void UITextLayer::drawFrame(Rect r, int thickness) {
    /*
     * Draw a frame around the specified rectangle. The rectangle
     * will be filled with the background color, and a frame will be
     * drawn around its edges.
     */

    drawRect(r, colors.bg);
    drawBorderRects(r, colors.frame, thickness);
    drawBorderRects(r.expand(thickness), colors.border, 1);
}

void UITextLayer::printf(const char *format, ...) {
    va_list v;

    va_start(v, format);
    vprintf(format, v);
    va_end(v);
}

void UITextLayer::vprintf(const char *format, va_list v) {
    static char buffer[1024];

    vsnprintf(buffer, sizeof buffer, format, v);
    draw(buffer);
}

void UITextLayer::draw(const char *text) {
    int offset;
    char c;

    if (align == CENTER) {
        offset = -measureWidth(text) / 2;
    } else if (align == RIGHT) {
        offset = -measureWidth(text);
    } else {
        offset = 0;
    }

    /*
     * To support autoClear, draw in two passes: First clear behind
     * each glyph, then draw each glyph.  This lets glyphs overlap
     * both horizontally and vertically, we just can't allow multiple
     * draw() calls to overlap each other.
     *
     * If autoClear is off, we only draw the text pass.
     */

    int pass;
    enum {
        BACKGROUND_PASS = 0,
        TEXT_PASS,
    };

    for (pass = autoClear ? BACKGROUND_PASS : TEXT_PASS; pass <= TEXT_PASS; pass++) {
        int lineX = 0;
        int lineY = 0;
        int lineClearedTo = 0;
        const char *cPtr = text;

        while ((c = *cPtr)) {

            /* We're done if we're past the bottom of the clip region. */
            if (y + lineY > clip.y2) {
                break;
            }

            if (c == ' ') {
                /* See if we want to wrap at this space. */
                if (lineX + measureNextWordWidth(cPtr + 1) > wrapWidth) {
                    c = '\n';
                }
            }

            if (c == '\v') {
                /* Vertical tab: Advance by less than a full line. */
                lineY += font.getLineSpacing() / 3;
                cPtr++;
                continue;
            }

            if (c == '\n') {
                lineX = 0;
                lineClearedTo = 0;
                lineY += font.getLineSpacing();
                cPtr++;
                continue;
            }

            Glyph glyph = font.getGlyph(c);

            if (glyph.getEscapement() == 0) {
                cPtr++;
                continue;
            }

            /*
             * Draw this pass, if it might intersect the clip region.
             */
            if (y + lineY + glyph.getHeight() >= clip.y1) {

                if (pass == BACKGROUND_PASS) {
                    /*
                     * Clear behind the next chunk of text.  Glyphs can overlap,
                     * so it's likely that the previous glyph needed to clear
                     * behind part of this glyph.  'lineClearedTo' keeps track of
                     * how far along this line we've cleared so far.
                     *
                     * Note that we don't need to add explicit dirty rects for the
                     * font glyphs, since the backing rectangle will add dirty
                     * rects.
                     */

                    int clearEnd = lineX + glyph.getWidth();

                    drawRect(Rect(x + lineClearedTo + offset,
                                  y + lineY,
                                  clearEnd - lineClearedTo,
                                  glyph.getHeight()),
                             colors.bg);

                    lineClearedTo = clearEnd;

                } else {
                    /*
                     * Render a normal glyph.
                     */

                    int i;
                    for (i = 0; i < glyph.getHeight(); i++) {
                        int gx = x + lineX + offset;
                        int gy = y + lineY + i;
                        if (inClip(gy)) {
                            glyph.drawRow(i, linePtr(gy) + gx, colors);
                        }
                    }
                }
            }
            lineX += glyph.getEscapement();
            cPtr++;
        }
    }
}

int UITextLayer::measureWidth(const char *text) {
    int lineWidth = 0;
    int maxWidth = 0;
    char c;

    /*
     * This may be a multi-line string. Measure the longest line.
     */
    while ((c = *text)) {
        if (c == '\n') {
            lineWidth = 0;
        } else {
            lineWidth += font.getGlyph(c).getEscapement();
            if (lineWidth > maxWidth) {
                maxWidth = lineWidth;
            }
        }
        text++;
    }

    /*
     * Each character has a pixel of padding built-in. Remove
     * the padding from the last character in the line.
     */
    return maxWidth - 1;
}

int UITextLayer::measureNextWordWidth(const char *text) {
    /*
     * Measure the width, in pixels, of the characters starting
     * at 'text' and ending at the next whitespace or at the end
     * of the string.
     */

    char c;
    int width = 0;

    while ((c = *text)) {
        if (c == ' ' || c == '\n') {
            break;
        }
        width += font.getGlyph(c).getEscapement();
        text++;
    }

    return width;
}

void UITextLayer::drawBorderRects(Rect r, uint8_t paletteIndex, int thickness) {
    // Sides
    drawRect(r.adjacentAbove(thickness), paletteIndex);
    drawRect(r.adjacentBelow(thickness), paletteIndex);
    drawRect(r.adjacentLeft(thickness), paletteIndex);
    drawRect(r.adjacentRight(thickness), paletteIndex);

    // Corners
    drawRect(r.adjacentAbove(thickness).adjacentLeft(thickness), paletteIndex);
    drawRect(r.adjacentAbove(thickness).adjacentRight(thickness), paletteIndex);
    drawRect(r.adjacentBelow(thickness).adjacentLeft(thickness), paletteIndex);
    drawRect(r.adjacentBelow(thickness).adjacentRight(thickness), paletteIndex);
}

