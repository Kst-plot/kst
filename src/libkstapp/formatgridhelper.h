/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2011 C. Barth Netterfield
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef FORMATGRIDHELPER_H
#define FORMATGRIDHELPER_H

#include <QList>
#include <QVector>

namespace Kst {

class ViewItem;

struct AutoFormatEdges {
  int edge_number;
  qreal edge;
  bool left_or_top;
  ViewItem *item;
};

struct AutoFormatRC {
  int row;
  int row_span;
  int col;
  int col_span;
};


class FormatGridHelper
{
public:
  FormatGridHelper(const QList<ViewItem*> &viewItems, bool protectLayout=true);

  QList<AutoFormatRC> rcList;
  QVector< QVector <int> > a;
  int n_rows;
  int n_cols;
  int numHoles();
  void getHole(int &row, int &col);
  void condense();
};

}
#endif // FORMATGRIDHELPER_H
