/***************************************************************************
                             dialogdefaults.h
                             -------------------
    begin                : November 13, 2007
    copyright            : (C) 2007 C. Barth Netterfield
    email                :
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KSTDIALOGDEFAULTS
#define KSTDIALOGDEFAULTS

#include "datavector.h"
class QSettings;

namespace Kst {
  extern QSettings *dialogDefaults;

  void setDataVectorDefaults(DataVectorPtr);
  void setGenVectorDefaults(GeneratedVectorPtr V);

}

#endif

// Dialog defaults are stored and retreived from the Kst::dialogDefaults.
// As you add defaults, add them here!
// Name                   Type    Where used
// vector/datasource      QString vectordialog
// vector/range           int     vectordialog
// vector/start           int     vectordialog
// vector/countFromEnd    bool    vectordialog
// vector/readToEnd       bool    vectordialog
// vector/skip            int     vectordialog
// vector/doSkip          bool    vectordialog
// vector/doAve           bool    vectordialog
//
// genVector/min"         int     vectordialog
// genVector/max"         int     vectordialog
// genVector/length       int     vectordialog

