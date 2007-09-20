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

#ifndef APPLICATION_H
#define APPLICATION_H

#include <QPointer>
#include <QApplication>

#include "kst_export.h"
#include "mainwindow.h"

#define kstApp \
  (static_cast<Kst::Application*>(QCoreApplication::instance()))

namespace Kst {

class KST_EXPORT Application : public QApplication
{
  Q_OBJECT
  public:
    Application(int &argc, char **argv);
    virtual ~Application();

    MainWindow *mainWindow() const;

  private:
    QPointer<MainWindow> _mainWindow;
};

}

#endif

// vim: ts=2 sw=2 et
