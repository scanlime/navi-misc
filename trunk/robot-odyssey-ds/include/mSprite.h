/* -*- Mode: C; c-basic-offset: 4 -*-
 *
 * The MSprite class is a medium-level abstraction for the NDS
 * hardware sprites. A single MSprite (multi-sprite) instance consists
 * of a plurality of hardware OBJs, and they may be moved as a fixed
 * unit. This module manages dynamic allocation of OBJs and matrices.
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

#ifndef _MSPRITE_H_
#define _MSPRITE_H_


/*
 * Allocation ranges. These are OAM indices where we start allocation.
 */
enum MSpriteRange {
    MSPRR_FRONT = 0x00,
    MSPRR_FRONT_BORDER = 0x20,
    MSPRR_UI = 0x40,
    MSPRR_BACK = 0x60,
};


/*
 * Sprite OBJ and matrix allocator. One of these must exist for each
 * graphics engine (main and sub) that is used with MSprite.
 */
class MSpriteAllocator
{
 public:
    MSpriteAllocator(OamState *oam) { init(oam); }
    void init(OamState *oam);

    SpriteRotation *allocMatrix();
    void freeMatrix(SpriteRotation *s);

    SpriteEntry *allocOBJ(MSpriteRange range);
    void freeOBJ(SpriteEntry *s);

    unsigned int getMatrixIndex(SpriteRotation *s);

    OamState *oam;

 private:
    uint32_t matrixAlloc;
    uint32_t objAlloc[SPRITE_COUNT / 32];
};


/*
 * One OBJ in an MSprite.
 */
class MSpriteOBJ
{
 public:
    void init(MSpriteAllocator *alloc,
              MSpriteRange range,
              int xOffset,
              int yOffset,
              const void *gfx,
              SpriteSize size,
              SpriteColorFormat format);

    void setGfx(const void *gfx);

    SpriteEntry *entry;
    int xOffset;
    int yOffset;

 private:
    SpriteSize size;
    SpriteColorFormat format;
    MSpriteAllocator *alloc;
};


/*
 * A Multi-sprite unit.
 */
class MSprite
{
 public:
    MSprite(MSpriteAllocator *alloc) { init(alloc); }
    void init(MSpriteAllocator *alloc);

    /*
     * Release all allocated OBJs and matrix.
     */
    void free();

    /*
     * Manage position and visibility.
     * This updates every OBJ in the sprite.
     */
    int getX() { return x; };
    int getY() { return y; };
    void moveTo(int x, int y);
    void show();
    void hide();

    /*
     * Transforms. Each MSprite has a settable angle and scale, plus
     * an 'intrinsic scale' which can be used for a scale factor
     * that's "built in" to the sprites and should be treated as the
     * object's native size.
     *
     * After any transform is updated, show() must be called to update
     * the OAM memory.
     */
    void setAngle(int angle);
    void setScale(int sx, int sy);
    void setIntrinsicScale(int sx, int sy);

    /*
     * Manage MSpriteOBJs.
     *
     * The MSprite position and visibility must be set
     * once after all OBJs have been created. Sprites start
     * out invisible.
     */
    MSpriteOBJ *newOBJ(MSpriteRange range,
                       int xOffset, int yOffset,
                       const void *gfx,
                       SpriteSize size,
                       SpriteColorFormat format);

    static const unsigned MAX_OBJS = 8;
    MSpriteOBJ obj[MAX_OBJS];
    unsigned int objCount;

 private:
    MSpriteAllocator *alloc;
    SpriteRotation *matrix;
    int x, y;
    bool visible;
    int angle;
    int sx, sy;
    int isx, isy;
};


#endif // _MSPRITE_H_
