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

#include <nds.h>
#include <string.h>
#include "mSprite.h"


void MSpriteAllocator::init(OamState *oam) {
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
    for (int word = 0; word < (SPRITE_COUNT / 32); word++) {
        int bit = ffs(objAlloc[word]) - 1;
        int id = (word << 5) + bit;
        if (bit >= 0 && id >= range) {
            objAlloc[word] &= ~(1 << bit);
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

void MSprite::init(MSpriteAllocator *alloc) {
    this->alloc = alloc;
    objCount = 0;
    visible = true;
    angle = 0;
    sx = sy = isx = isy = 0x100;
    matrix = alloc->allocMatrix();
}

void MSprite::free() {
    hide();
    alloc->freeMatrix(matrix);
    for (unsigned int i = 0; i < objCount; i++) {
        alloc->freeOBJ(obj[i].entry);
    }
}

void MSprite::moveTo(int x, int y) {
    this->x = x;
    this->y = y;
    for (unsigned int i = 0; i < objCount; i++) {
        obj[i].entry->x = x + obj[i].xOffset;
        obj[i].entry->y = y + obj[i].yOffset;
    }
}

void MSprite::show(void) {
    /*
     * Show all OBJs, and update the rotation/scaling matrix.
     */

    unsigned int matrixIndex = alloc->getMatrixIndex(matrix);

    for (unsigned int i = 0; i < objCount; i++) {
        SpriteEntry *entry = obj[i].entry;

        entry->isRotateScale = true;
        entry->isSizeDouble = false;
        entry->rotationIndex = matrixIndex;
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

void MSprite::hide(void) {
    /*
     * Hide all OBJs.
     */

    for (unsigned int i = 0; i < objCount; i++) {
        SpriteEntry *entry = obj[i].entry;

        entry->isRotateScale = false;
        entry->isHidden = true;
    }
}

void MSprite::setAngle(int angle) {
    this->angle = angle;
}

void MSprite::setScale(int sx, int sy) {
    this->sx = sx;
    this->sy = sy;
}

void MSprite::setIntrinsicScale(int sx, int sy) {
    this->isx = sx;
    this->isy = sy;
}

MSpriteOBJ *MSprite::newOBJ(MSpriteRange range,
                            int xOffset,
                            int yOffset,
                            const void *gfx,
                            SpriteSize size,
                            SpriteColorFormat format) {

    sassert(objCount < MAX_OBJS, "Out of MSprite OBJs");
    MSpriteOBJ *o = &obj[objCount++];

    o->entry = alloc->allocOBJ(range);
    o->xOffset = xOffset;
    o->yOffset = yOffset;

    /*
     * XXX: Most of this is copied from the implementation of oamSet(). It'd be
     *      nice if this function was refactored to operate on a SpriteEntry
     *      instead of an OamState...
     */

    o->entry->shape = (ObjShape) SPRITE_SIZE_SHAPE(size);
    o->entry->size = (ObjSize) SPRITE_SIZE_SIZE(size);

    o->entry->palette = 0;
    o->entry->priority = OBJPRIORITY_0;
    o->entry->hFlip = false;
    o->entry->vFlip = false;
    o->entry->isMosaic = false;
    o->entry->isRotateScale = false;
    o->entry->isHidden = true;

    if (format != SpriteColorFormat_Bmp) {
        o->entry->gfxIndex = oamGfxPtrToOffset(gfx);
        o->entry->colorMode = (ObjColMode) format;
    } else {
        int sx = ((uint32_t)gfx >> 1) & ((1 << alloc->oam->gfxOffsetStep) - 1);
        int sy = (((uint32_t)gfx >> 1) >> alloc->oam->gfxOffsetStep) & 0xFF;
        o->entry->gfxIndex = (sx >> 3) | ((sy >> 3) << (alloc->oam->gfxOffsetStep - 3));
        o->entry->blendMode = (ObjBlendMode) format;
    }

    return o;
}
