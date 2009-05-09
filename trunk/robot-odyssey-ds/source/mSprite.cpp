/* -*- Mode: C++; c-basic-offset: 4 -*-
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

#include <nds.h>
#include <string.h>
#include "mSprite.h"


MSpriteAllocator::MSpriteAllocator(OamState *oam) {
    this->oam = oam;
    memset(&matrixAlloc, 0xFF, sizeof matrixAlloc);
    memset(objAlloc, 0xFF, sizeof objAlloc);
 }

SpriteRotation *MSpriteAllocator::allocMatrix() {
    int i = ffs(matrixAlloc) - 1;
    sassert(i >= 0, "Out of MSprite matrices");
    matrixAlloc &= ~(1 << i);
    return oam->oamRotationMemory + i;
}

unsigned int MSpriteAllocator::getMatrixIndex(SpriteRotation *s) {
    int i = s - oam->oamRotationMemory;
    sassert(i >= 0 && i < MATRIX_COUNT, "Invalid matrix pointer");
    return i;
}

void MSpriteAllocator::freeMatrix(SpriteRotation *s) {
    int i = getMatrixIndex(s);
    sassert((matrixAlloc & (1 << i)) == 0, "Freeing freed matrix");
    matrixAlloc |= 1 << i;
}

SpriteEntry *MSpriteAllocator::allocOBJ(MSpriteRange range) {
    int id;

    /*
     * Start allocating at 'range', and keep going until we find an
     * empty slot.
     */
    for (id = range; id < SPRITE_COUNT; id++) {
        int word = id >> 5;
        int bit = id & 31;
        int mask = 1 << bit;

        if (objAlloc[word] & mask) {
            objAlloc[word] &= ~mask;
            return oam->oamMemory + id;
        }
    }

    sassert(false, "Out of MSprite OBJs");
    return NULL;
}

void MSpriteAllocator::freeOBJ(SpriteEntry *s) {
    unsigned int i = s - oam->oamMemory;
    sassert(i >= 0 && i < SPRITE_COUNT, "Freeing invalid OBJ pointer");
    unsigned int word = i >> 5;
    unsigned int bit = i & 31;
    sassert((objAlloc[word] & (1 << bit)) == 0, "Freeing freed OBJ");
    objAlloc[word] |= 1 << bit;
}

MSprite::MSprite(MSpriteAllocator *alloc) {
    this->alloc = alloc;
    objCount = 0;
    visible = true;
    angle = 0;
    sx = sy = isx = isy = 0x100;
    matrix = alloc->allocMatrix();
}

MSprite::~MSprite() {
    hide();
    alloc->freeMatrix(matrix);
}

void MSprite::moveTo(int x, int y) {
    this->x = x;
    this->y = y;
    update();
}

void MSprite::show(void) {
    for (unsigned int i = 0; i < objCount; i++) {
        obj[i].show();
    }
}

void MSprite::hide(void) {
    for (unsigned int i = 0; i < objCount; i++) {
        obj[i].hide();
    }
}

void MSprite::setDoubleSize(bool enable) {
    for (unsigned int i = 0; i < objCount; i++) {
        obj[i].enableDoubleSize = enable;
        if (obj[i].isVisible()) {
            obj[i].show();
        }
    }
    update();
}

void MSprite::update(void) {
    unsigned int matrixIndex = alloc->getMatrixIndex(matrix);

    for (unsigned int i = 0; i < objCount; i++) {
        obj[i].entry->rotationIndex = matrixIndex;

        int objX = x + obj[i].xOffset;
        int objY = y + obj[i].yOffset;

        int spriteW, spriteH;
        obj[i].getImageSize(spriteW, spriteH);

        /*
         * Calculate the hit box for this OBJ.
         */
        obj[i].setHitBox(objX, objY, spriteW, spriteH);

        /*
         * Compensate for double-size mode.
         */
        if (obj[i].enableDoubleSize) {
            objX -= spriteW >> 1;
            objY -= spriteH >> 1;
        }

        obj[i].entry->x = objX;
        obj[i].entry->y = objY;
    }

    int s = sinLerp(angle);
    int c = cosLerp(angle);

    int sxall = (sx * isx) >> 8;
    int syall = (sy * isy) >> 8;

    matrix->hdx =  c * sxall >> 12;
    matrix->hdy = -s * sxall >> 12;
    matrix->vdx =  s * syall >> 12;
    matrix->vdy =  c * syall >> 12;
}

bool MSprite::hitTest(int x, int y) {
    for (unsigned int i = 0; i < objCount; i++) {
        if (obj[i].hitTest(x, y)) {
            return true;
        }
    }
    return false;
}

void MSprite::setAngle(int angle) {
    this->angle = angle;
}

void MSprite::setScale(int sx, int sy) {
    this->sx = sx;
    this->sy = sy;
    update();
}

void MSprite::getScale(int &sx, int &sy) {
    sx = this->sx;
    sy = this->sy;
}

void MSprite::setIntrinsicScale(int sx, int sy) {
    this->isx = sx;
    this->isy = sy;
    update();
}

MSpriteOBJ *MSprite::newOBJ(MSpriteRange range,
                            int xOffset,
                            int yOffset,
                            const void *gfx,
                            SpriteSize size,
                            SpriteColorFormat format) {

    sassert(objCount < MAX_OBJS, "Out of MSprite OBJs");
    MSpriteOBJ *o = &obj[objCount++];
    o->init(alloc, range, xOffset, yOffset, gfx, size, format);
    update();
    return o;
}

MSpriteOBJ::MSpriteOBJ() {
    this->alloc = NULL;
    this->entry = NULL;
    this->enableDoubleSize = false;
}

void MSpriteOBJ::init(MSpriteAllocator *alloc,
                      MSpriteRange range,
                      int xOffset,
                      int yOffset,
                      const void *gfx,
                      SpriteSize size,
                      SpriteColorFormat format) {
    if (entry) {
        alloc->freeOBJ(entry);
        entry = NULL;
    }

    this->alloc = alloc;
    this->entry = alloc->allocOBJ(range);

    this->xOffset = xOffset;
    this->yOffset = yOffset;
    this->size = size;
    this->format = format;

    entry->shape = (ObjShape) SPRITE_SIZE_SHAPE(size);
    entry->size = (ObjSize) SPRITE_SIZE_SIZE(size);

    entry->palette = 0;
    entry->priority = OBJPRIORITY_0;
    entry->hFlip = false;
    entry->vFlip = false;
    entry->isMosaic = false;
    entry->isRotateScale = false;
    entry->isHidden = true;

    setGfx(gfx);
    hide();
}

MSpriteOBJ::~MSpriteOBJ() {
    if (entry) {
        alloc->freeOBJ(entry);
    }
}

void MSpriteOBJ::setGfx(const void *gfx) {
    /*
     * This is taken from ndslib's oamSet() implementation.
     */

    if (format != SpriteColorFormat_Bmp) {
        entry->gfxIndex = oamGfxPtrToOffset(gfx);
        entry->colorMode = (ObjColMode) format;
    } else {
        int sx = ((uint32_t)gfx >> 1) & ((1 << alloc->oam->gfxOffsetStep) - 1);
        int sy = (((uint32_t)gfx >> 1) >> alloc->oam->gfxOffsetStep) & 0xFF;
        entry->gfxIndex = (sx >> 3) | ((sy >> 3) << (alloc->oam->gfxOffsetStep - 3));
        entry->blendMode = (ObjBlendMode) format;
    }
}

bool MSpriteOBJ::hitTest(int x, int y) {
    /*
     * Perform a hit test: Does an (x,y) coordinate in screen space
     * touch an opaque region of this sprite?
     *
     * XXX: Currently this is not a pixel-accurate test, we just check
     *      whether (x, y) touches the sprite's bounding box. In the
     *      future this may change.
     */

    if (!isVisible()) {
        return false;
    }

    return (x >= hitBox.left && x < hitBox.right &&
            y >= hitBox.top && y < hitBox.bottom);
}

void MSpriteOBJ::setHitBox(int x, int y, int width, int height) {
    hitBox.left = x;
    hitBox.top = y;
    hitBox.right = x + width;
    hitBox.bottom = y + height;
}

void MSpriteOBJ::getImageSize(int &width, int &height) {
    /*
     * Get the size of the actual image data for this sprite.
     *
     * Square sprites are all nice powers of two, but
     * rectangular sprites are fairly irregular. Just use
     * a lookup table for them.
     */

    static const int major[] = { 16, 32, 32, 64 };
    static const int minor[] = { 8, 8, 16, 32 };
    int idx = SPRITE_SIZE_SIZE(size);

    if (SPRITE_SIZE_SHAPE(size) == OBJSHAPE_WIDE) {
        width = major[idx];
        height = minor[idx];
    } else if (SPRITE_SIZE_SHAPE(size) == OBJSHAPE_TALL) {
        width = minor[idx];
        height = major[idx];
    } else {
        width = height = 1 << (idx + TILE_SHIFT);
    }
}

void MSpriteOBJ::center() {
    int width, height;
    getImageSize(width, height);
    xOffset = -width/2;
    yOffset = -height/2;
}

void MSpriteOBJ::moveBy(int x, int y) {
    xOffset += x;
    yOffset += y;
}

void MSpriteOBJ::show() {
    entry->isRotateScale = true;
    entry->isSizeDouble = enableDoubleSize;
}

void MSpriteOBJ::hide() {
    entry->isRotateScale = false;
    entry->isHidden = true;
}

bool MSpriteOBJ::isVisible() {
    /* isHidden flag only valid when isRotateScale==false */
    return !entry->isHidden || entry->isRotateScale;
}

SpriteImages::SpriteImages(OamState *oam, SpriteSize size, SpriteColorFormat format,
                           uint16_t *buffer) {
    this->size = size;
    this->format = format;
    images = buffer;
    freeImages = false;
}

SpriteImages::SpriteImages(OamState *oam, SpriteSize size, SpriteColorFormat format,
                           int numImages) {
    allocate(oam, size, format, numImages);
}

SpriteImages::SpriteImages(OamState *oam, const void *data, DecompressType type,
                           SpriteSize size, SpriteColorFormat format, int numImages) {
    allocate(oam, size, format, numImages);
    decompress(data, images, type);
}

SpriteImages::~SpriteImages() {
    oamFreeGfx(oam, images);
}

uint32_t SpriteImages::getImageBytes() {
    uint32_t bytes= SPRITE_SIZE_PIXELS(size);

    if (format == SpriteColorFormat_16Color) {
        bytes >>= 1;
    }
    if (format == SpriteColorFormat_Bmp) {
        bytes <<= 1;
    }

    return bytes;
}

uint16_t *SpriteImages::getImage(int num) {
    return (uint16_t*) ((uint8_t*)images + getImageBytes() * num);
}

void SpriteImages::allocate(OamState *oam, SpriteSize size, SpriteColorFormat format,
                            int numImages) {
    this->size = size;
    this->format = format;

    /*
     * Make a SpriteSize value with a size that's a multiple of the
     * actual image size, so we can use the libnds OAM allocator to
     * grab enough VRAM for all images sequentially. Why sequential?
     * Just because it makes decompression easier.
     */
    SpriteSize stackedSize = (SpriteSize) ((size & ~0xFFF) | (size & 0xFFF) * numImages);
    images = oamAllocateGfx(oam, stackedSize, format);
    freeImages = true;
}
