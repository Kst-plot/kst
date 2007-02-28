/***************************************************************************
                       ksteventmonitor_i.h  -  Part of KST
                             -------------------
    begin                :
    copyright            : (C) 2004 The University of British Columbia
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

#ifndef KSTEVENTMONITORI_H
#define KSTEVENTMONITORI_H

#include "kstdatadialog.h"
#include "ksteventmonitorentry.h"

class EventMonitorWidget;

class KstEventMonitorI : public KstDataDialog {
  Q_OBJECT
  public:
    KstEventMonitorI(QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WFlags fl = 0 );
    virtual ~KstEventMonitorI();
    static KstEventMonitorI *globalInstance();

  protected:
    QString objectName() { return tr("Event Monitor"); }
 
  public slots:
    void update();
    bool newObject();
    bool editObject();
    void enableELOG();
    void disableELOG();
    void populateEditMultiple();

  private:
    void fillEvent(EventMonitorEntryPtr& event);

    QString _vector;

    static QPointer<KstEventMonitorI> _inst;

    // the following are for the multiple edit mode
    bool _changed : 1;
    bool _setWidths : 1;
    bool _lineEditEquationDirty : 1;
    bool _lineEditDescriptionDirty : 1;
    bool _checkBoxDebugDirty : 1;
    bool _radioButtonLogNoticeDirty : 1;
    bool _radioButtonLogWarningDirty : 1;
    bool _radioButtonLogErrorDirty : 1;
    bool _checkBoxEMailNotifyDirty : 1;
    bool _lineEditEMailRecipientsDirty : 1;
    bool _checkBoxELOGNotifyDirty : 1;
    bool _scriptDirty : 1;
    bool editSingleObject(EventMonitorEntryPtr emPtr);
    
  private slots:
    void setcheckBoxDebugDirty();
    void setcheckBoxEMailNotifyDirty();
    void setcheckBoxELOGNotifyDirty();
    void setScriptDirty();

  private:
    void fillFieldsForEdit();
    void fillFieldsForNew();
    EventMonitorWidget *_w;
};


#endif
// vim: ts=2 sw=2 et
