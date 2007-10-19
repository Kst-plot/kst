/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _PALETTE_H
#define _PALETTE_H

#include <QColor>
#include <QHash>
#include "kst_export.h"

namespace Kst {

class Palette {
  public:
    static QStringList getPaletteList();
    Palette();
    Palette(const QString &paletteName);
    virtual ~Palette();

    void addColor(const QColor & color);

    QString paletteName() const;
    int colorCount() const;
    QColor color(const int colorId) const ;

  private:
    void createPalette();

    QHash<int, QColor> _palette;
    QString _paletteName;
    int _count;
};

}
#endif

// vim: ts=2 sw=2 et
