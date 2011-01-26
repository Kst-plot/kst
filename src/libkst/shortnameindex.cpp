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
  int _vnum = 1; // vectors
  int _pnum = 1; // plugins
  int _csdnum = 1; // csd
  int _cnum = 1; // curves
  int _enum = 1; // equations
  int _hnum = 1; // histograms
  int _inum = 1; // images
  int _psdnum = 1; // psd
  int _xnum = 1; // scalars
  int _tnum = 1; // text string
  int _mnum = 1; // matrix
  int _plotnum = 1; // plots
  int _lnum = 1; // legend
  int _dnum = 1; // view item
  int _dsnum = 1; // datasource

  int max_vnum = 0; // vectors
  int max_pnum = 0; // plugins
  int max_csdnum = 0; // csd
  int max_cnum = 0; // curves
  int max_enum = 0; // equations
  int max_hnum = 0; // histograms
  int max_inum = 0; // images
  int max_psdnum = 0; // psd
  int max_xnum = 0; // scalars
  int max_tnum = 0; // text string
  int max_mnum = 0; // matrix
  int max_plotnum = 0; // plots
  int max_lnum = 0; // legends
  int max_dnum = 0; // view item
  int max_dsnum = 0; // datasource
}
