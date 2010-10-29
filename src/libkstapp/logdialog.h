/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2010 C. Barth Netterfield                             *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "ui_logdialog.h"

#ifndef LOGDIALOG_H
#define LOGDIALOG_H

#include <QDialog>
#include <QProcess>

#include "kst_export.h"
#include "kst_i18n.h"

namespace Kst {

class MainWindow;

class LogDialog : public QDialog, Ui::LogDialog
{
  Q_OBJECT
  public:
    LogDialog(MainWindow *parent);
    virtual ~LogDialog();

  public slots:
    void changed();
    void enableApply();
    void apply();
    void enableWidthHeight();
    void runScript();
    void scriptStdErr();
    void scriptStdOut();
    void scriptStarted();
    void scriptFinished(int);
    void scriptError(QProcess::ProcessError);
  Q_SIGNALS:
    void exportLog(const QString &_logdir, time_t _logtime, const QString &_format, int x_size, int y_size,
              int size_option_index, const QString &message);

  private:
    time_t _logtime;
    QString _format;
    QString _logdir;
    QProcess *_proc;
    MainWindow *_parent;
    QString _imagename;
    QString _msgfilename;


};

}
#endif // LOGDIALOG_H
