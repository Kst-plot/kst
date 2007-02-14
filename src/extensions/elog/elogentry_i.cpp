/***************************************************************************
                    elogentry_i.cpp  -  Part of KST
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

#include <qlayout.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qtextedit.h>
#include <qradiobutton.h>
#include <qhbuttongroup.h>
#include <qpushbutton.h>
#include <kdebug.h>
#include <kconfig.h>
#include "elog.h"
#include "elogentry_i.h"

ElogEntryI::ElogEntryI(KstELOG* elog,
                       QWidget* parent,
                       const char* name,
                       bool modal,
                       WFlags fl) : ElogEntry(parent, name, modal, fl) {
  _elog = elog;
}

ElogEntryI::~ElogEntryI() {
}

void ElogEntryI::initialize() {
  QGridLayout *grid;
  
  connect( pushButtonConfiguration, SIGNAL( clicked() ), this, SLOT( configuration() ) );
  connect( pushButtonClose, SIGNAL( clicked() ), this, SLOT( close() ) );
  connect( pushButtonSubmit, SIGNAL( clicked() ), this, SLOT( submit() ) );
  
  grid = new QGridLayout(frameAttrs, 1, 1);
  grid->addWidget(_frameWidget = new QWidget(frameAttrs, "Frame Widget"), 0, 0);
  pushButtonSubmit->setDefault(true);
}

void ElogEntryI::configuration() {
  _elog->doShow();
}

void ElogEntryI::customEvent(QCustomEvent* event) {
  KstELOGAttribStruct	attrib;
  ELOGAttribList* pAttribs;
  QSizePolicy sizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
  QGridLayout* topGrid;
  QGridLayout* grid;
  QLabel* label;
  QLineEdit* lineedit;
  QCheckBox* checkbox;
  QComboBox* combobox;
  QRadioButton* radio;
  QHButtonGroup* buttonGroup;
  QString strLabel;
  unsigned int i;
  unsigned int j;
  
  if( event->type() == KstELOGAttrsEvent ) {
    pAttribs = (ELOGAttribList*)event->data();    
    _attribs = *pAttribs;
    
    delete _frameWidget;
    _frameWidget = 0L;
    
    topGrid = dynamic_cast<QGridLayout*>(frameAttrs->layout());
    if (topGrid) {
      topGrid->invalidate();
      _frameWidget = new QWidget(frameAttrs, "Frame Widget");
      if( _frameWidget != NULL ) {
        setEnabled( TRUE );
        
        topGrid->addWidget(_frameWidget, 0, 0);
        grid = new QGridLayout(_frameWidget, pAttribs->count(), 2);
        grid->setMargin(6);
        grid->setSpacing(5);

        for( i=0; i<_attribs.count(); i++ ) {
          attrib = _attribs[i];
          
          strLabel = attrib.attribName;
          if( attrib.bMandatory ) {
            strLabel += "*";
          }
          strLabel += ":";
          label = new QLabel( strLabel, _frameWidget );
          grid->addWidget( label, i, 0, Qt::AlignLeft);
          
          switch( attrib.type ) {
          case AttribTypeText:
            lineedit = new QLineEdit( _frameWidget );
            grid->addWidget( lineedit, i, 1);     
            _attribs[i].pWidget = lineedit;
            break;
          
          case AttribTypeBool:
            checkbox = new QCheckBox( _frameWidget );
            grid->addWidget( checkbox, i, 1);          
            _attribs[i].pWidget = checkbox;
            break;
          
          case AttribTypeCombo:
            combobox = new QComboBox( _frameWidget );
            grid->addWidget( combobox, i, 1);          
            for( j=0; j<attrib.values.count(); j++ ) {
              combobox->insertItem( attrib.values[j], -1 );
            }
            if( !attrib.bMandatory ) {
              combobox->insertItem( "", -1 );              
            }
            _attribs[i].pWidget = combobox;
            break;
          
          case AttribTypeRadio:
            buttonGroup = new QHButtonGroup( "", _frameWidget );
            buttonGroup->setSizePolicy( sizePolicy );
            buttonGroup->setRadioButtonExclusive( TRUE );
            buttonGroup->setAlignment( AlignLeft );
            buttonGroup->setInsideMargin( 0 );
            buttonGroup->setLineWidth( 0 );
            grid->addWidget( buttonGroup, i, 1 );                    
            for( j=0; j<attrib.values.count(); j++ ) {
              radio = new QRadioButton( attrib.values[j], buttonGroup );
              if( j == 0 ) {
                radio->setChecked( TRUE );
              }
            }
            _attribs[i].pWidget = buttonGroup;
            break;
          
          case AttribTypeCheck:
            buttonGroup = new QHButtonGroup( "", _frameWidget );
            buttonGroup->setSizePolicy( sizePolicy );
            buttonGroup->setAlignment( AlignLeft );             
            buttonGroup->setInsideMargin( 0 );
            buttonGroup->setLineWidth( 0 );
            grid->addWidget( buttonGroup, i, 1 );                    
            for( j=0; j<attrib.values.count(); j++ ) {
              checkbox = new QCheckBox( attrib.values[j], buttonGroup );
            }
            _attribs[i].pWidget = buttonGroup;            
            break;
          }
        }

        _frameWidget->show( );
        _frameWidget->resize(_frameWidget->sizeHint());
        frameAttrs->resize(frameAttrs->sizeHint());
        resize(sizeHint());
      
        loadSettings( );
        setSettings( );
      }
    }
  }
}

void ElogEntryI::submit() {
  saveSettings();
  _elog->submitEntry();
  close();
}

void ElogEntryI::setSettings() {
  //
  // if wanted we could set the old attribute values in here...
  //
  checkBoxIncludeCapture->setChecked( _bIncludeCapture );
  checkBoxIncludeConfiguration->setChecked( _bIncludeConfiguration );
  checkBoxIncludeDebugInfo->setChecked( _bIncludeDebugInfo );
}

void ElogEntryI::saveSettings() {
  KstELOGAttribStruct	attrib;
  KConfig cfg("kstrc", false, false);
  QButton* button;
  QString str;
  unsigned int i;
  int j;
  
  _strAttributes.truncate( 0 );
  for( i=0; i<_attribs.count(); i++ ) {
    attrib = _attribs[i];
    
    switch( attrib.type ) {
      case AttribTypeText:
      str.sprintf( "%s=%s\n", attrib.attribName.latin1(), ((QLineEdit*)(attrib.pWidget))->text().latin1() );
      _strAttributes += str;      
      break;
      
      case AttribTypeBool:
      if( ((QCheckBox*)(attrib.pWidget))->isChecked() ) {
        str.sprintf( "%s=1\n", attrib.attribName.latin1() );
        _strAttributes += str;      
      }
      break;
      
      case AttribTypeCombo:
        str.sprintf( "%s=%s\n", attrib.attribName.latin1(), ((QComboBox*)(attrib.pWidget))->currentText().latin1() );
        _strAttributes += str;      
      break;
      
      case AttribTypeRadio:
      button = ((QHButtonGroup*)(attrib.pWidget))->selected( );
      if( button != NULL ) {
        str.sprintf( "%s=%s\n", attrib.attribName.latin1(), button->text( ).latin1() );
        _strAttributes += str;      
      }
      break;
      
      case AttribTypeCheck:
      for( j=0; j<((QHButtonGroup*)(attrib.pWidget))->count(); j++ ) {
        button = ((QHButtonGroup*)(attrib.pWidget))->find( j );
        if( button != NULL ) {
          if( ((QRadioButton*)button)->isChecked() ) {
            str.sprintf( "%s#%d=%s\n", attrib.attribName.latin1(), j, button->text( ).latin1() );
            _strAttributes += str;      
          }
        }
      }
      break;
    }
  }
  
  _strText							  = textEditText->text();
  _bIncludeCapture 				= checkBoxIncludeCapture->isChecked();
  _bIncludeConfiguration 	= checkBoxIncludeConfiguration->isChecked();
  _bIncludeDebugInfo 	    = checkBoxIncludeDebugInfo->isChecked();
    
  cfg.setGroup("ELOG");
  str.sprintf( "Attributes:%s:%d:%s", 
               _elog->configuration()->ipAddress().ascii(),
               _elog->configuration()->portNumber(),
               _elog->configuration()->name().ascii() );  
  cfg.writeEntry(str, _strAttributes);
  cfg.writeEntry("IncludeCapture", _bIncludeCapture);
  cfg.writeEntry("IncludeConfiguration", _bIncludeConfiguration);
  cfg.writeEntry("IncludeDebugInfo", _bIncludeDebugInfo);
  cfg.sync();
}

void ElogEntryI::loadSettings() {
  KConfig cfg("kstrc");
  QString str;

  cfg.setGroup("ELOG");
  str.sprintf( "Attributes:%s:%d:%s", 
               _elog->configuration()->ipAddress().ascii(),
               _elog->configuration()->portNumber(),
               _elog->configuration()->name().ascii() );
  _strAttributes 					= cfg.readEntry(str, "");
  _bIncludeCapture 				= cfg.readBoolEntry("IncludeCapture", FALSE);
  _bIncludeConfiguration 	= cfg.readBoolEntry("IncludeConfiguration", FALSE);
  _bIncludeDebugInfo 	    = cfg.readBoolEntry("IncludeDebugInfo", FALSE);
}


void ElogEntryI::setEnabled(bool en) {
  ElogEntry::setEnabled(en);
  pushButtonClose->setEnabled(true);
}

#include "elogentry_i.moc"
// vim: ts=2 sw=2 et
