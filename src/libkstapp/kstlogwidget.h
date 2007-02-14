/***************************************************************************
                        kstlogwidget.h  -  view for debug log
                             -------------------
    begin                : Fri Apr 01 2005
    copyright            : (C) 2005 The University of Toronto
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

#ifndef KSTLOGWIDGET_H
#define KSTLOGWIDGET_H

#include "kstdebug.h"
#include <qmime.h>
#include <qtextbrowser.h>

class KstLogWidget : public QTextBrowser {
  Q_OBJECT
  public:
    KstLogWidget(QWidget *parent = 0, const char *name = 0);
    void setDebug(KstDebug *debug);

  public slots:
    void logAdded(const KstDebug::LogMessage&);
    void setShowDebug(bool show);
    void setShowNotice(bool show);
    void setShowWarning(bool show);
    void setShowError(bool show);
    void clear();

  private slots:
    void regenerate();

  private:            
    KstDebug *_debug;
    int _show;
    QMimeSourceFactory _msrc;

    void generateImages();
};

#endif
// vim: ts=2 sw=2 et
