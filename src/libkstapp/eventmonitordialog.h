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

    void setObjectStore(ObjectStore *store);

    QString script() const;
    void setScript(const QString script);

    QString event() const;
    void setEvent(const QString event);

    QString description() const;
    void setDescription(const QString description);

    Debug::LogLevel logLevel() const;
    void setLogLevel(const Debug::LogLevel level);

    bool logKstDebug() const;
    void setLogKstDebug(const bool logKstDebug);

    bool logEMail() const;
    void setLogEMail(const bool logEMail);

    bool logELOG() const;
    void setLogELOG(const bool logELOG);

    QString emailRecipients() const;
    void setEmailRecipients(const QString emailRecipients);

  private Q_SLOTS:
    void selectionChanged();

  Q_SIGNALS:
    void optionsChanged();
};

class KST_EXPORT EventMonitorDialog : public DataDialog {
  Q_OBJECT
  public:
    EventMonitorDialog(ObjectPtr dataObject, QWidget *parent = 0);
    virtual ~EventMonitorDialog();

  protected:
    virtual QString tagString() const;
    virtual ObjectPtr createNewDataObject() const;
    virtual ObjectPtr editExistingDataObject() const;

  private Q_SLOTS:
    void updateButtons();

  private:
    void configureTab(ObjectPtr object);

    EventMonitorTab *_eventMonitorTab;
};

}

#endif

// vim: ts=2 sw=2 et
