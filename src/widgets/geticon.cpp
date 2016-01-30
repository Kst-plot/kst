/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2016 C. Barth Netterfield
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <QIcon>
#include <QFile>
#include <QDebug>

QIcon KstGetIcon(QString icon_name) {
  static bool has_svg = true;

  QString svg_name = ":svg_icons/"+icon_name+".svgz";
  if (QFile::exists(svg_name)) {
    //qDebug() << svg_name;
    return QIcon(svg_name);
  } else {
    QString png_name = ":"+icon_name+".png";
    qDebug() << png_name;
    return QIcon(png_name);
  }
}

