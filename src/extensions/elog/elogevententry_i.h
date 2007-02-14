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

#ifndef ELOGEVENTENTRYI_H
#define ELOGEVENTENTRYI_H

#include "elogevententry.h"
#include <kstevents.h>

class KstELOG;

class ElogEventEntryI : public ElogEventEntry {
  Q_OBJECT
  public:
    ElogEventEntryI( KstELOG* elog,
                QWidget* parent = 0,
                const char* name = 0,
                bool modal = FALSE, WFlags fl = 0 );
    virtual ~ElogEventEntryI();
          
    void    initialize();
    QString attributes() { return _strAttributes; }
    bool    includeCapture() { return _bIncludeCapture; }
    bool    includeConfiguration() { return _bIncludeConfiguration; }
    bool    includeDebugInfo() { return _bIncludeDebugInfo; }
    
  public slots:
  
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
    bool    _bIncludeCapture;
    int     _bIncludeConfiguration;    
    int     _bIncludeDebugInfo;    
};


#endif // ELOGCONFIGURATIONI_H
