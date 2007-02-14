/***************************************************************************
                    elogconfiguration_i.cpp  -  Part of KST
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

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qpushbutton.h>

#include <kdebug.h>
#include <kconfig.h>

#include "elogthreadattrs.h"
#include "elogconfiguration_i.h"

ElogConfigurationI::ElogConfigurationI(KstELOG* elog,
                                       QWidget* parent,
                                       const char* name,
                                       bool modal,
                                       WFlags fl) : ElogConfiguration(parent, name, modal, fl) {
  _elog = elog;
}

ElogConfigurationI::~ElogConfigurationI() {
}

void ElogConfigurationI::initialize() {
  fillConfigurations( );
  loadSettings( );
  setSettings( );

  connect( pushButtonSave, SIGNAL( clicked() ), this, SLOT( save() ) );
  connect( pushButtonLoad, SIGNAL( clicked() ), this, SLOT( load() ) );
  connect( pushButtonClose, SIGNAL( clicked() ), this, SLOT( close() ) );  connect( pushButtonUpdate, SIGNAL( clicked() ), this, SLOT( update() ) );
  connect( pushButtonApply, SIGNAL( clicked() ), this, SLOT( apply() ) );
  connect( lineEditIPAddress, SIGNAL( textChanged(const QString&) ), this, SLOT( change() ) );
  connect( spinBoxPortNumber, SIGNAL( valueChanged(const QString&) ), this, SLOT( change() ) );
  connect( lineEditLogbook, SIGNAL( textChanged(const QString&) ), this, SLOT( change() ) );
  connect( lineEditUserName, SIGNAL( textChanged(const QString&) ), this, SLOT( change() ) );
  connect( lineEditUserPassword, SIGNAL( textChanged(const QString&) ), this, SLOT( change() ) );
  connect( lineEditWritePassword, SIGNAL( textChanged(const QString&) ), this, SLOT( change() ) );
  connect( checkBoxSubmitAsHTML, SIGNAL( clicked() ), this, SLOT( change() ) );
  connect( checkBoxSuppressEmail, SIGNAL( clicked() ), this, SLOT( change() ) );
  connect( comboBoxCaptureSize, SIGNAL( activated(int) ), this, SLOT( change() ) );
  
  pushButtonApply->setEnabled(false);
    
  if( !_strIPAddress.isEmpty() ) {
    ElogThreadAttrs* thread = new ElogThreadAttrs(_elog);

    _elog->entry()->setEnabled(FALSE);
    thread->doTransmit();
  }
}

void ElogConfigurationI::fillConfigurations() {
  QString strIPAddress;
  QString strName;
  QString strGroup;
  QString strEntry;
  KConfig cfg("kstrc", false, false);
  int     iPortNumber;
  int     i;
  
  for( i=0; i<10; i++ ) {
    strGroup.sprintf("ELOG%d", i );
    cfg.setGroup(strGroup);
    
    strIPAddress  = cfg.readEntry("IPAddress", "");
    iPortNumber   = cfg.readNumEntry("Port", 8080);
    strName       = cfg.readEntry("Name", "");
    
    strIPAddress.stripWhiteSpace();
    strName.stripWhiteSpace();
    
    if( !strIPAddress.isEmpty() ) {
      strEntry.sprintf( "%d [%s:%d:%s]", i, strIPAddress.ascii(), iPortNumber, strName.ascii() );
    } else {
      strEntry.sprintf( "%d", i );
    }
    comboBoxConfiguration->insertItem( strEntry );
  }
}

void ElogConfigurationI::apply() {
  ElogThreadAttrs* thread = new ElogThreadAttrs(_elog);
  
  _elog->entry()->setEnabled(FALSE);
  saveSettings();
  pushButtonApply->setEnabled(FALSE);
  thread->doTransmit();
}

void ElogConfigurationI::update() {
  ElogThreadAttrs* thread = new ElogThreadAttrs(_elog);
  
  _elog->entry()->setEnabled(FALSE);
  thread->doTransmit();
}

void ElogConfigurationI::change() {
  pushButtonApply->setEnabled(TRUE);
}

void ElogConfigurationI::save() {
  QString strConfiguration;
  QString strGroup;
  QString strIPAddress;
  QString strName;
  QString strUserName;
  QString strUserPassword;
  QString strWritePassword;
  QString strEntry;
  KConfig cfg("kstrc", false, false);
  int			iPortNumber;
  int			iIndex;
  
  strConfiguration = comboBoxConfiguration->currentText( );
  iIndex = strConfiguration.find( ' ' );
  if( iIndex != -1 ) {
    strConfiguration = strConfiguration.left( iIndex );
  }
  iIndex = strConfiguration.toInt();
  
  strGroup.sprintf("ELOG%d", iIndex);
  strIPAddress   	 = lineEditIPAddress->text();
  iPortNumber    	 = spinBoxPortNumber->value();
  strName        	 = lineEditLogbook->text();
  strUserName    	 = lineEditUserName->text();
  strUserPassword  = lineEditUserPassword->text();
  strWritePassword = lineEditWritePassword->text();

  strIPAddress.stripWhiteSpace();
  strName.stripWhiteSpace();
  
  cfg.setGroup(strGroup);
  cfg.writeEntry("IPAddress", strIPAddress);
  cfg.writeEntry("Port", iPortNumber);
  cfg.writeEntry("Name", strName);
  cfg.writeEntry("UserName", strUserName);
  cfg.writeEntry("UserPassword", strUserPassword);
  cfg.writeEntry("WritePassword", strWritePassword);
  cfg.sync();  
  
  if( !strIPAddress.isEmpty() ) {
    strEntry.sprintf( "%d [%s:%d:%s]", iIndex, strIPAddress.ascii(), iPortNumber, strName.ascii() );
  } else {
    strEntry.sprintf( "%d", iIndex );
  }
  comboBoxConfiguration->changeItem( strEntry, iIndex );  
}

void ElogConfigurationI::load() {  
  QString strConfiguration;
  QString strGroup;
  KConfig cfg("kstrc", false, false);
  int			iIndex;
  
  strConfiguration = comboBoxConfiguration->currentText( );
  iIndex = strConfiguration.find( ' ' );
  if( iIndex != -1 ) {
    strConfiguration = strConfiguration.left( iIndex );
  }
  iIndex = strConfiguration.toInt();
  strGroup.sprintf("ELOG%d", iIndex);

  cfg.setGroup(strGroup);
  _strIPAddress   	= cfg.readEntry("IPAddress", "");
  _iPortNumber    	= cfg.readNumEntry("Port", 8080);
  _strName        	= cfg.readEntry("Name", "");
  _strUserName    	= cfg.readEntry("UserName", "");
  _strUserPassword 	= cfg.readEntry("UserPassword", "");
  _strWritePassword = cfg.readEntry("WritePassword", "");
    
  setSettings();
  apply();
}

void ElogConfigurationI::setSettings() {
  QString str;
  
  str.sprintf( "%d x %d", _iCaptureWidth, _iCaptureHeight );
  
  lineEditIPAddress->setText( _strIPAddress );
  spinBoxPortNumber->setValue( _iPortNumber );
  lineEditLogbook->setText( _strName );
  lineEditUserName->setText( _strUserName );
  lineEditUserPassword->setText( _strUserPassword );
  comboBoxCaptureSize->setCurrentText( str );
  lineEditWritePassword->setText( _strWritePassword );
  checkBoxSubmitAsHTML->setChecked( _bSubmitAsHTML );
  checkBoxSuppressEmail->setChecked( _bSuppressEmail );
  comboBoxCaptureSize->setCurrentText( str );
}

void ElogConfigurationI::saveSettings() {
  KConfig cfg("kstrc", false, false);
  QString strCaptureSize;
  int 		iIndex;
  
  _strIPAddress   	= lineEditIPAddress->text();
  _iPortNumber    	= spinBoxPortNumber->value();
  _strName        	= lineEditLogbook->text();
  _strUserName    	= lineEditUserName->text();
  _strUserPassword  = lineEditUserPassword->text();
  _strWritePassword	= lineEditWritePassword->text();
  _bSubmitAsHTML  	= checkBoxSubmitAsHTML->isChecked();
  _bSuppressEmail 	= checkBoxSuppressEmail->isChecked();

  strCaptureSize = comboBoxCaptureSize->currentText();
  iIndex = strCaptureSize.find( 'x' );
  if( iIndex == -1 ) {
    _iCaptureWidth  = 800;
    _iCaptureHeight = 600;
  } else {
    _iCaptureWidth  = strCaptureSize.left( iIndex ).toInt();
    _iCaptureHeight = strCaptureSize.right( strCaptureSize.length() - iIndex - 1 ).toInt();
  }

  cfg.setGroup("ELOG");
  cfg.writeEntry("IPAddress", _strIPAddress);
  cfg.writeEntry("Port", _iPortNumber);
  cfg.writeEntry("Name", _strName);
  cfg.writeEntry("UserName", _strUserName);
  cfg.writeEntry("UserPassword", _strUserPassword);
  cfg.writeEntry("WritePassword", _strWritePassword);
  cfg.writeEntry("SubmitAsHTML", _bSubmitAsHTML);
  cfg.writeEntry("SuppressEmail", _bSuppressEmail);
  cfg.writeEntry("CaptureWidth", _iCaptureWidth);
  cfg.writeEntry("CaptureHeight", _iCaptureHeight);
  cfg.sync();
}

void ElogConfigurationI::loadSettings() {
  KConfig cfg("kstrc", false, false);

  cfg.setGroup("ELOG");
  _strIPAddress   	= cfg.readEntry("IPAddress", "");
  _iPortNumber    	= cfg.readNumEntry("Port", 8080);
  _strName        	= cfg.readEntry("Name", "");
  _strUserName    	= cfg.readEntry("UserName", "");
  _strUserPassword 	= cfg.readEntry("UserPassword", "");
  _strWritePassword = cfg.readEntry("WritePassword", "");
  _bSubmitAsHTML  	= cfg.readBoolEntry("SubmitAsHTML", FALSE);
  _bSuppressEmail 	= cfg.readBoolEntry("SuppressEmail", FALSE);
  _iCaptureWidth  	= cfg.readNumEntry("CaptureWidth", 640);
  _iCaptureHeight 	= cfg.readNumEntry("CaptureHeight", 480);
}

#include "elogconfiguration_i.moc"
// vim: ts=2 sw=2 et
