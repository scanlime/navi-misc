/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
 *
 * thd_visualizer.h - Visualizers are classes that take small discrete chunks of
 *                    raw memory data, and visualize them in some way. Visualizers
 *                    can be as simple as printing a word in hex, or as complex
 *                    as an interactive disassembler.
 *
 * Copyright (C) 2009 Micah Dowty
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef __THD_VISUALIZER_H
#define __THD_VISUALIZER_H

#include <wx/grid.h>
#include <wx/dc.h>
#include <boost/shared_ptr.hpp>
#include "thd_model.h"
#include "mem_transfer.h"

class THDVisBase;

typedef boost::shared_ptr<THDVisBase> visualizerPtr_t;


/*
 * The parameter to any visualizer is a block of memory, annotated
 * with that memory address. The size of this block is determined by
 * the visualizer's block size. A block size of zero is legal.
 */

struct THDVisBlock {
    typedef MemTransfer::AddressType AddressType;

    THDVisBlock(AddressType _address=0,
                AddressType _length=0,
                uint8_t *_data=NULL)
        : address(_address),
          length(_length),
          data(_data)
    {}

    AddressType address;
    AddressType length;
    uint8_t *data;
};


/*
 * Abstract base class for all visualizers.
 */

class THDVisBase {
public:
    THDVisBase();
    virtual ~THDVisBase();

    virtual THDVisBlock::AddressType GetBlockSize() = 0;
    virtual wxString GetStringValue(THDVisBlock &block) = 0;
    virtual int GetWidth(wxDC &dc);
    virtual wxGridCellAttr *GetAttr(THDVisBlock &block);

protected:
    wxGridCellAttr *defaultAttr;
};


/*
 * A very simple visualizer that displays the current address.
 */

class THDVisAddress : public THDVisBase {
public:
    virtual THDVisBlock::AddressType GetBlockSize();
    virtual wxString GetStringValue(THDVisBlock &block);
};


/*
 * A simple visualizer for displaying hex dumps
 */

class THDVisHex : public THDVisBase {
public:
    virtual THDVisBlock::AddressType GetBlockSize();
    virtual wxString GetStringValue(THDVisBlock &block);
};


#endif /* __THD_VISUALIZER_H */
