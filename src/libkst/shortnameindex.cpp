/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 C. Barth Netterfield                             *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "namedobject.h"

namespace Kst 
{
  int _vectornum = 1; // vectors
  int _pluginnum = 1; // plugins
  int _csdnum = 1; // csd
  int _curvecnum = 1; // curves
  int _equationnum = 1; // equations
  int _histogramnum = 1; // histograms
  int _imagenum = 1; // images
  int _psdnum = 1; // psd
  int _scalarnum = 1; // scalars
  int _stringnum = 1; // text string
  int _matrixnum = 1; // matrix
  int _plotnum = 1; // plots
  int _legendnum = 1; // legend
  int _viewitemnum = 1; // view item
  int _datasourcenum = 1; // datasource

  int max_vectornum = 0; // vectors
  int max_pluginnum = 0; // plugins
  int max_csdnum = 0; // csd
  int max_curvenum = 0; // curves
  int max_equationnum = 0; // equations
  int max_histogramnum = 0; // histograms
  int max_imagenum = 0; // images
  int max_psdnum = 0; // psd
  int max_scalarnum = 0; // scalars
  int max_stringnum = 0; // text string
  int max_matrixnum = 0; // matrix
  int max_plotnum = 0; // plots
  int max_legendnum = 0; // legends
  int max_viewitemnum = 0; // view item
  int max_datasourcenum = 0; // datasource
}
