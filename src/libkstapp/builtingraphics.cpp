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

#include "builtingraphics.h"
#include "boxitem.h"
#include "lineitem.h"
#include "ellipseitem.h"
#include "labelitem.h"
#include "layoutboxitem.h"
#include "pictureitem.h"
#include "plotitem.h"
#include "svgitem.h"
#include "arrowitem.h"
#include "circleitem.h"
#include "sharedaxisboxitem.h"
#include "buttonitem.h"

namespace Kst {
  namespace Builtins {
    void initGraphics() {
      new ArrowItemFactory;
      new BoxItemFactory;
      new CircleItemFactory;
      new EllipseItemFactory;
      new LabelItemFactory;
      new ButtonItemFactory;
      new LayoutBoxItemFactory;
      new LineItemFactory;
      new PictureItemFactory;
      new PlotItemFactory;
#ifndef KST_NO_SVG
      new SvgItemFactory;
#endif
      new SharedAxisBoxItemFactory;
    }
  }
}

// vim: ts=2 sw=2 et
