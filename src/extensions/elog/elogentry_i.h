/***************************************************************************
                       elogentry_i.h  -  Part of KST
                             -------------------
    begin                :
    copyright            : (C) 2004 The University of British Columbia
                           (C) 2004 Andrew R. Walker
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

#ifndef ELOGENTRYI_H
#define ELOGENTRYI_H

#include "elogentry.h"

class KstELOG;

class ElogEntryI : public ElogEntry {
  Q_OBJECT
  public:
    ElogEntryI( KstELOG* elog,
                QWidget* parent = 0,
                const char* name = 0,
                bool modal = FALSE, WFlags fl = 0 );
    virtual ~ElogEntryI();
          
    void    initialize();
    QString text() { return _strText; }
    QString attributes() { return _strAttributes; }
    bool    includeCapture() { return _bIncludeCapture; }
    bool    includeConfiguration() { return _bIncludeConfiguration; }
    bool    includeDebugInfo() { return _bIncludeDebugInfo; }
    
  public slots:
    virtual void setEnabled(bool);
  
  protected slots:
  
  private slots:
    virtual void configuration();
    virtual void submit();
  signals:
  
  protected:
    virtual void customEvent(QCustomEvent* event);
    
  private:
    void setSettings();
    void saveSettings();
    void loadSettings();
    
    ELOGAttribList _attribs;
    KstELOG* _elog;
    QWidget* _frameWidget;
    QString _strAttributes;
    QString _strText; 
    bool    _bIncludeCapture;
    int     _bIncludeConfiguration;    
    int     _bIncludeDebugInfo;    
};


#endif // ELOGCONFIGURATIONI_H
// vim: ts=2 sw=2 et
