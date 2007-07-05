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

#include "kstwidgets.h"

KstWidgets::KstWidgets(QObject *parent)
    : QObject(parent) {
  _plugins.append(new ColorButtonPlugin(this));
  _plugins.append(new FillAndStrokePlugin(this));
  _plugins.append(new GradientEditorPlugin(this));
}

Q_EXPORT_PLUGIN2(kstwidgets, KstWidgets)
