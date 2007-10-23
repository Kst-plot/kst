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

#ifndef EVENTMONITORDIALOG_H
#define EVENTMONITORDIALOG_H

#include "datadialog.h"
#include "datatab.h"

#include "debug.h"

#include "kst_export.h"

#include "ui_eventmonitortab.h"

namespace Kst {

class KST_EXPORT EventMonitorTab : public DataTab, Ui::EventMonitorTab {
  Q_OBJECT
  public:
    EventMonitorTab(QWidget *parent = 0);
    virtual ~EventMonitorTab();

    QString script() const;
    QString event() const;
    QString description() const;
    Debug::LogLevel logLevel() const;
    bool logKstDebug() const;
    bool logEMail() const;
    bool logELOG() const;
    QString emailRecipients() const;

  private Q_SLOTS:
  private:
};

class KST_EXPORT EventMonitorDialog : public DataDialog {
  Q_OBJECT
  public:
    EventMonitorDialog(ObjectPtr dataObject, QWidget *parent = 0);
    virtual ~EventMonitorDialog();

  protected:
    virtual QString tagName() const;
    virtual ObjectPtr createNewDataObject() const;
    virtual ObjectPtr editExistingDataObject() const;

  private:
    EventMonitorTab *_eventMonitorTab;
};

}

#endif

// vim: ts=2 sw=2 et
