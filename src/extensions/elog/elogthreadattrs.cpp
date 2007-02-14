/***************************************************************************
                                   elogthreadattrs.cpp
                             -------------------
    begin                : Feb 09 2004
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <qregexp.h>

#include <klocale.h>
#include <kmdcodec.h>

#include "elogthreadattrs.h"
#include <kst.h>

ElogThreadAttrs::ElogThreadAttrs(KstELOG* elog) : ElogThread(elog) {
}

ElogThreadAttrs::~ElogThreadAttrs() {
  if (_job) {
    _job->kill();
    _job = 0L;
  }
}

void ElogThreadAttrs::doTransmit( ) {
  KURL destination;
  QString strUserName;
  QString strUserPassword;
  QString strWritePassword;
  QString strLogbook;

  strUserName 					= _elog->configuration()->userName();
  strUserPassword 			= _elog->configuration()->userPassword();
  strWritePassword			= _elog->configuration()->writePassword();
  strLogbook  					= _elog->configuration()->name();

  destination.setProtocol("http");
  destination.setHost(_elog->configuration()->ipAddress());
  destination.setPort((short)_elog->configuration()->portNumber());
  destination.setQuery("cmd=new");
  if (!strLogbook.isEmpty()) {
    destination.setPath(QString("/%1/").arg(strLogbook));
  }

  _job = KIO::get(destination, false, false);
  if (_job) {
    QStringList cookies;

    if (!strWritePassword.isEmpty()) {
      QCString str = KCodecs::base64Encode(strWritePassword.ascii());
      cookies.append(QString("wpwd=%1").arg(str.data()));
    }

    if (!strUserName.isEmpty()) {
      cookies.append(QString("unm=%1").arg(strUserName));

      QCString str = KCodecs::base64Encode(strUserPassword.ascii());
      cookies.append(QString("upwd=%1").arg(str.data()));
    }

    if (cookies.count() > 0) {
      QString strCookie;

      strCookie = "Cookie: ";
      for (uint i=0; i<cookies.count(); i++) {
        strCookie += cookies[i];
        if (i != cookies.count()-1) {
          strCookie += "; ";
        }
      }
      _job->addMetaData("cookies", "manual");
      _job->addMetaData("setcookies", strCookie);
    }

    QObject::connect(_job, SIGNAL(result(KIO::Job*)), this, SLOT(result(KIO::Job*)));
    QObject::connect(_job, SIGNAL(dataReq(KIO::Job*, QByteArray&)), this, SLOT(dataReq(KIO::Job*, QByteArray&)));
    QObject::connect(_job, SIGNAL(data(KIO::Job*, const QByteArray&)), this, SLOT(data(KIO::Job*, const QByteArray&)));

    KIO::Scheduler::scheduleJob(_job);
  }
}

void ElogThreadAttrs::doResponse( char* response ) {
  QCustomEvent   eventAttrs(KstELOGAttrsEvent);
  KstELOGAttribStruct	attrib;
  ELOGAttribList attribs;
  QStringList		 strAttribs;
  QString        strResponse;
  QString        strAttrib;
  QString        strValues;
  QString        strValue;
  QString        strType;
  QString        strFont;
  static char    attribname[] = " class=\"attribname\">";
  static char    attribvalue[] = " class=\"attribvalue\">";
  static char    select[] = "<select ";
  static char    input[] = "<input ";
  static char    endlabel[] = "</td>";
  static char    fontstart[] = "<font ";
  static char    fontend[] = "</font>";
  static char    optionvalue[] = "<option value=\"";
  static char    type[] = " type=";
  static char    value[] = " value=\"";
  static char    valueNoQuote[] = " value=";
  char*				   p = response;
  char*					 pAttribNameStart;
  char*					 pAttribNameEnd = NULL;
  char*					 pAttribValueStart;
  char*					 pAttribValueEnd = NULL;
  bool           bAppend;
  int					   iFontStart;
  int						 iFontEnd;
  int            iOptionsStart = 0;
  int						 iOptionsEnd;
  int					   iValueStart;
  int						 iValueEnd;
  int						 iInput;
  int						 iSelect;
  
  //
  // find and categorise the attributes...
  //
  while( p != NULL ) {
    pAttribValueEnd = NULL;
    attrib.bMandatory = FALSE;
    attrib.values.clear();
    attrib.pWidget = NULL;
    
    //
    // retrieve the attribute label name...
    //
    pAttribNameStart = strstr( p, attribname );
    if( pAttribNameStart != NULL ) {
      pAttribNameStart += strlen( attribname );
      pAttribNameEnd = strstr( pAttribNameStart, endlabel );
      if( pAttribNameEnd != NULL ) {
        *pAttribNameEnd  = '\0';
        strAttrib = pAttribNameStart;
        pAttribNameEnd += strlen( endlabel );
        
        //
        // check for a mandatory field...
        //
        iFontStart = strAttrib.find( fontstart );
        if( iFontStart != -1 ) {
          iFontEnd = strAttrib.find( fontend );
          if( iFontEnd != -1 ) {
            strFont = strAttrib.mid( iFontStart, iFontEnd + strlen( fontend ) - iFontStart );
            strAttrib.remove( iFontStart, iFontEnd + strlen( fontend ) - iFontStart );
            if( strFont.find( '*' ) ) {
              attrib.bMandatory = TRUE;
            }
          }
        }
        
        //
        // remove the trailing : if present...
        //
        if( strAttrib.findRev( ':' ) == (int)strAttrib.length( ) - 1 ) {
          strAttrib.truncate( strAttrib.length( ) - 1 );
        }
        attrib.attribName = strAttrib;

        //
        // determine the input type...
        //
        pAttribValueStart = strstr( pAttribNameEnd, attribvalue );
        if( pAttribValueStart != NULL ) {
          pAttribValueStart += strlen( attribvalue );
          pAttribValueEnd   = strstr( pAttribValueStart, endlabel );
          if( pAttribValueEnd != NULL ) {
            iOptionsStart = 0;
            *pAttribValueEnd  = '\0';
            strValues = pAttribValueStart;
            pAttribValueEnd += strlen( endlabel );
            
            iInput = strValues.find( input );
            iSelect = strValues.find( select );
            if( ( iInput != -1 && iSelect == -1 ) || 
                ( iInput != -1 && iSelect != -1 && iInput < iSelect ) ) {     
              bAppend = FALSE;
              while( ( iOptionsStart = strValues.find( type, iOptionsStart ) ) != -1 ) {
                iOptionsStart += strlen( type );
                iOptionsEnd = strValues.find( " ", iOptionsStart );
                if( iOptionsEnd != -1 ) {
                  strType = strValues.mid( iOptionsStart, iOptionsEnd - iOptionsStart );
                  iOptionsStart += iOptionsEnd - iOptionsStart;
                  if( strType == "radio" ) {
                    attrib.type = AttribTypeRadio;
                    
                    iValueStart = strValues.find( value, iOptionsStart );
                    if( iValueStart != -1 ) {
                      iValueStart += strlen( value );
                      iValueEnd = strValues.find( "\"", iValueStart );
                      iOptionsStart = iValueEnd + strlen( "\"" );
                      strValue = strValues.mid( iValueStart, iValueEnd - iValueStart );
                      if( !strValue.isEmpty() ) {
                        attrib.values.append( strValue );
                        bAppend = TRUE;
                      }
                    }
                  }
                  else if( strType == "checkbox" ) {
                    attrib.type = AttribTypeCheck;

                    iValueStart = strValues.find( value, iOptionsStart );
                    if( iValueStart != -1 ) {
                      iValueStart += strlen( value );
                      iValueEnd = strValues.find( "\"", iValueStart );
                      if( iValueEnd != -1 ) {
                        iOptionsStart = iValueEnd + strlen( "\"" );
                        strValue = strValues.mid( iValueStart, iValueEnd - iValueStart );
                        if( !strValue.isEmpty() ) {
                          attrib.values.append( strValue );
                          bAppend = TRUE;
                        }
                      }
                    } else {
                      iValueStart = strValues.find( valueNoQuote, iOptionsStart );
                      if( iValueStart != -1 ) {
                        iValueStart += strlen( valueNoQuote );
                        iValueEnd = strValues.find( QRegExp(">| "), iValueStart );
                        if( iValueEnd != -1 ) {
                          iOptionsStart = iValueEnd + strlen( ">" );
                          strValue = strValues.mid( iValueStart, iValueEnd - iValueStart );
                          if( !strValue.isEmpty() ) {
                            attrib.values.append( strValue );
                            bAppend = TRUE;
                          }
                          if( strValue == "1" && attrib.values.count() == 1 ) {
                            attrib.type = AttribTypeBool;
                          }
                        }
                      }
                    }
                  }
                  else if( strType == "\"text\"" ) {
                    attrib.type = AttribTypeText;
                    bAppend = TRUE;
                    break;
                  }
                  else {
                    break;
                  }
                }
                else {
                  break;
                }
              }
              if( bAppend ) {
                attribs.append( attrib );               
              }
            }
            else if( ( iSelect != -1 && iInput == -1 ) || 
                     ( iSelect != -1 && iInput != -1 && iSelect < iInput ) ) {
              //
              // we have a combobox, so need to determine the options...
              //
              attrib.type = AttribTypeCombo;
             
              while( ( iOptionsStart = strValues.find( optionvalue, iOptionsStart ) ) != -1 ) {
                iOptionsStart += strlen( optionvalue );
                iOptionsEnd = strValues.find( "\">", iOptionsStart );
                if( iOptionsEnd != -1 ) {
                  strValue = strValues.mid( iOptionsStart, iOptionsEnd - iOptionsStart );
                  iOptionsStart = iOptionsEnd + strlen( "\">" );
                  if( !strValue.isEmpty() ) {
                    attrib.values.append( strValue );
                  }
                }
                else {
                  break;
                }
              }
              attribs.append( attrib );
            }
          }
        }
      }
    }
    p = pAttribValueEnd;   
  }
  
  eventAttrs.setData( &attribs );
  QApplication::sendEvent( (QObject*)_elog->entry(), (QEvent*)&eventAttrs );
  QApplication::sendEvent( (QObject*)_elog->eventEntry(), (QEvent*)&eventAttrs );
}

bool ElogThreadAttrs::doResponseError( const char* response, const QString& strDefault ) {
  QString strError;
  bool bRetVal = TRUE;
  
  if (strstr(response, "<title>ELOG error</title>")) {
    doError( i18n("Failed to access ELOG: no such logbook was found.") );
    bRetVal = FALSE;
  }
  else if (strstr(response, "<title>ELOG password</title>")) {
    doError( i18n("Failed to access ELOG: password was incorrect or missing.") );
    bRetVal = FALSE;
  } else {
    strError = i18n("Failed to access ELOG: error: %1").arg( strDefault );
    doError( strError );
  }
  
  return bRetVal;
}

void ElogThreadAttrs::dataReq(KIO::Job *job, QByteArray &array)
{
  Q_UNUSED(job)

  array.resize(0);

  return;
}

void ElogThreadAttrs::data(KIO::Job *job, const QByteArray &array)
{
  Q_UNUSED(job)

  if (array.count() > 0) {
    _textStreamResult << array.data();
  }
}

void ElogThreadAttrs::result(KIO::Job *job) {
  if (_job) {
    _job = 0L;

    if (job->error()) {
      _textStreamResult << '\0';
      doResponseError(_byteArrayResult.data(), job->errorText());
    } else {
      if (_byteArrayResult.count() > 0) {
        _textStreamResult << '\0';
        doResponse(_byteArrayResult.data());
      } else {
        doError( i18n("Failed to access ELOG: unable to receive response"), KstDebug::Notice );
      }
    }
  }

  delete this;
}

#include "elogthreadattrs.moc"

// vim: ts=2 sw=2 et
