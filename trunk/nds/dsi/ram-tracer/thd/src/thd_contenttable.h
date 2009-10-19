/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
 *
 * thd_contenttable.h - A wxGrid-based widget which displays the contents of
 *                      memory at a particular time. Content decoding is handled
 *                      by separate Visualizer classes. The content table is configured
 *                      with a list of visualizers which map to columns in the table.
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

#ifndef __THD_CONTENTTABLE_H
#define __THD_CONTENTTABLE_H

#include <wx/grid.h>
#include <vector>
#include "thd_model.h"
#include "thd_visualizer.h"


/*
 * Information about an instance of a column. When AddColumns() is
 * called with count > 1, several THDContentColumns are created, but
 * they all share the same visualizerPtr_t.
 */

struct THDContentColumn {
    THDContentColumn(visualizerPtr_t _visualizer,
                     int _addrOffset=0)
        : visualizer(_visualizer),
          addrOffset(_addrOffset)
    {}

    visualizerPtr_t visualizer;
    int addrOffset;
};


class THDContentTable : public wxGridTableBase {
public:
    THDContentTable(THDModel *model);
    ~THDContentTable();

    virtual int GetNumberRows();
    virtual int GetNumberCols();

    virtual bool IsEmptyCell(int row, int col);
    virtual wxString GetValue(int row, int col);
    virtual void SetValue(int row, int col, const wxString &value);
    virtual wxGridCellAttr *GetAttr(int row, int col, wxGridCellAttr::wxAttrKind kind);

    THDVisBlock GetBlockForCell(int row, int col);

    std::vector<THDContentColumn> columns;
    int bytesPerRow;

private:
    THDModel *model;
};


class THDContentGrid : public wxGrid {
public:
    THDContentGrid(wxWindow *parent, THDModel *model);
    void Refresh();

    void ClearColumns();
    void AddColumns(visualizerPtr_t viz, int count=1);

private:
    void modelCursorChanged();

    THDModel *model;
    THDContentTable table;
    boost::signals::connection modelCursorChangeConn;
};


#endif /* __THD_TRANSFERTABLE_H */
