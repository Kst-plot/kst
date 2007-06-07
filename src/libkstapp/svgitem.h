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

#ifndef SVGITEM_H
#define SVGITEM_H

#include "viewitem.h"

class QSvgRenderer;

namespace Kst {

class SvgItem : public ViewItem
{
  Q_OBJECT
  public:
    SvgItem(View *parent, const QString &file);
    ~SvgItem();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

  private:
    QSvgRenderer *_svg;
};


class KST_EXPORT CreateSvgCommand : public CreateCommand
{
  public:
    CreateSvgCommand() : CreateCommand(QObject::tr("Create Svg")) {}
    CreateSvgCommand(View *view) : CreateCommand(view, QObject::tr("Create Svg")) {}
    ~CreateSvgCommand() {}
    void createItem();
};

}

#endif

// vim: ts=2 sw=2 et
