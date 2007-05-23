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

#ifndef KSTAPPLICATION_H
#define KSTAPPLICATION_H

#include <QPointer>
#include <QApplication>

#include "kst_export.h"
#include "kstmainwindow.h"

#define kstApp \
  (static_cast<KstApplication*>(QCoreApplication::instance()))

class KST_EXPORT KstApplication : public QApplication
{
  Q_OBJECT
public:
  KstApplication(int &argc, char **argv);
  virtual ~KstApplication();

  KstMainWindow *mainWindow() const;

private:
  QPointer<KstMainWindow> _mainWindow;
};

#endif

// vim: ts=2 sw=2 et
