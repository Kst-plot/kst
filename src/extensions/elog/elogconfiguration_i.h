/***************************************************************************
                       elogconfiguration_i.h  -  Part of KST
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

#ifndef ELOGCONFIGURATIONI_H
#define ELOGCONFIGURATIONI_H

#include "elogconfiguration.h"

class KstELOG;

class ElogConfigurationI : public ElogConfiguration {
  Q_OBJECT
  public:
    ElogConfigurationI( KstELOG* elog,
                        QWidget* parent = 0,
                        const char* name = 0,
                        bool modal = FALSE, WFlags fl = 0 );
    virtual ~ElogConfigurationI();
  
    void    initialize();
    QString ipAddress() { return _strIPAddress; }
    QString name() { return _strName; }
    QString userName() { return _strUserName; }
    QString userPassword() { return _strUserPassword; }
    QString writePassword() { return _strWritePassword; }
    int     portNumber() { return _iPortNumber; }
    bool    submitAsHTML() { return _bSubmitAsHTML; }
    bool    suppressEmail() { return _bSuppressEmail; }
    int     captureWidth() { return _iCaptureWidth; }
    int     captureHeight() { return _iCaptureHeight; }
        
  public slots:
  
  protected slots:
  
  private slots:
    virtual void apply();
    virtual void change();
    virtual void save();
    virtual void load();
    virtual void update();
    
  signals:
    
  protected:
    
  private:
    void fillConfigurations();
    void setSettings();
    void saveSettings();
    void loadSettings();
    
    KstELOG* _elog;
    QString  _strIPAddress;
    QString  _strName;
    QString  _strUserName;
    QString  _strUserPassword; 
    QString  _strWritePassword; 
    bool     _bSubmitAsHTML;
    bool     _bSuppressEmail;
    int      _iPortNumber; 
    int      _iCaptureWidth;
    int		   _iCaptureHeight;
};


#endif // ELOGCONFIGURATIONI_H
