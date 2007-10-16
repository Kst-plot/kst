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

#include "builtingraphics.h"
#include "boxitem.h"
#include "lineitem.h"
#include "ellipseitem.h"
#include "labelitem.h"
#include "layoutboxitem.h"
#include "pictureitem.h"
#include "plotitem.h"
#include "svgitem.h"

namespace Kst {
  namespace Builtins {
    void initGraphics() {
      new BoxItemFactory;
      new LineItemFactory;
      new EllipseItemFactory;
      new LabelItemFactory;
      new LayoutBoxItemFactory;
      new PictureItemFactory;
      new PlotItemFactory;
      new SvgItemFactory;
    }
  }
}

// vim: ts=2 sw=2 et
