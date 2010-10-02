/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SELECTIONRECT_H
#define SELECTIONRECT_H

#include <QPointF>
#include <QRectF>

class SelectionRect {
  public:
    SelectionRect();
    ~SelectionRect();

    bool isValid() const;
    void setFrom(const QPointF&);
    void setTo(const QPointF&);
    void reset();
    QRectF rect() const;

  private:
    QPointF _from, _to;
    bool _validFrom, _validTo;
};

#endif

// vim: ts=2 sw=2 et
