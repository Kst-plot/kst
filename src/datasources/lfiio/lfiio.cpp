/***************************************************************************
                    lfiio.cpp  - FITS file data source
                             -------------------
    begin                : Fri Oct 17 2003
    copyright            : (C) 2003 The University of British Columbia
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

#include "lfiio.h"

#include <kdebug.h>
#include <qfile.h>
#include <ctype.h>
#include <fitsio.h>
#include <stdlib.h>

#include "kststring.h"

#define TIME_FIELD  "TIME"

LFIIOSource::LFIIOSource( KConfig *cfg, const QString& filename, const QString& type )
: KstDataSource( cfg, filename, type )
{
  _first = true;

  if( type.isEmpty( ) || type == "LFIIO" )
  {
    if( initFile( ) )
    {
      _valid = true;
    }
  }
}


LFIIOSource::~LFIIOSource( )
{
}


bool LFIIOSource::reset()
{
  _fieldList.clear();
  return _valid = initFile();
}


bool LFIIOSource::initFile( )
{
  KstObject::UpdateType updateType;
  bool                  bRetVal = false;
  int                   iResult = 0;

  _numFrames = 0;

  //
  // read the metadata
  //
  if( !_filename.isNull( ) && !_filename.isEmpty( ) )
  {
    QString   str;
    fitsfile* ffits;
    int       iStatus = 0;

    if( _first )
    {
      iResult = fits_open_table( &ffits, _filename.ascii( ), READONLY, &iStatus );
      if( iResult == 0 )
      {
        int keysexist;
        int morekeys;

        iResult = fits_get_hdrspace( ffits, &keysexist, &morekeys, &iStatus );
        if( iResult == 0 )
        {
          char  keyname[FLEN_KEYWORD];
          char  value[FLEN_VALUE];
          char  comment[FLEN_COMMENT];
          int   keynum;

          for( keynum=1; keynum <= keysexist; ++keynum )
          {
            iResult = fits_read_keyn( ffits, keynum, keyname, value, comment, &iStatus );
            if( iResult == 0 )
            {
              KstString *metaString;

              str.sprintf( "%s %s", value, comment );
              metaString = new KstString( KstObjectTag( keyname, tag() ), this, str );
              _metaData.insert( keyname, metaString );
            }
          }

          _first = false;
        }
      }
    }
  }

  updateType = update( );
  if( updateType == KstObject::UPDATE )
  {
    bRetVal = true;
  }

  return bRetVal;
}


KstObject::UpdateType LFIIOSource::update( int u )
{
  if (KstObject::checkUpdateCounter(u)) {
    return lastUpdateResult();
  }

  KstObject::UpdateType updateType =  KstObject::NO_CHANGE;
  QString               strTemplate;
  QString               strName;
  fitsfile*             ffits;
  char                  charTemplate[ FLEN_CARD ];
  char                  charName[ FLEN_CARD ];
  long                  lNumFrames;
  long                  lMaxRepeat = 1;
  long                  lRepeat;
  long                  lWidth;
  int                   iColNumber;
  int                   iNumCols;
  int                   iStatus = 0;
  int                   iResult = 0;
  int                   iTypeCode;
  int                   i;

  _valid  = false;

  if( !_filename.isNull( ) && !_filename.isEmpty( ) )
  {
    iResult = fits_open_table( &ffits, _filename.ascii( ), READONLY, &iStatus );
    if( iResult == 0 )
    {
      //
      // determine size of data...
      //
      iResult = fits_get_num_cols( ffits, &iNumCols, &iStatus );
      if( iResult == 0 )
      {
        iResult = fits_get_num_rows( ffits, &lNumFrames, &iStatus );
        if( iResult == 0 )
        {
          _fieldList.clear( );
          _fieldList.append( "INDEX" );

          _valid = true;
          _bHasTime = false;

          //
          // need to multiply lNumFrames by the maximum value of the vector repeat value...
          //
          for( i=0; i<iNumCols; i++ )
          {
            iStatus = 0;

            sprintf( charTemplate, "%d", i+1 );
            iResult = fits_get_colname( ffits, CASEINSEN, charTemplate, charName, &iColNumber, &iStatus );
            if( iResult == 0 )
            {
              int iOffset = i;

              strName = charName;
              //
              // ensure that we don't add duplicates to the _fieldList...
              //
              while( _fieldList.findIndex( strName ) != -1 )
              {
                strName = QString("%1[%2]").arg( charName ).arg( iOffset );
                iOffset++;
              }
            }
            else
            {
              strName.setNum( i );
            }
            _fieldList.append( strName );

            iStatus = 0;
            iResult = fits_get_coltype( ffits, i+1, &iTypeCode, &lRepeat, &lWidth, &iStatus );
            if( iResult == 0 )
            {
              if( lRepeat > lMaxRepeat )
              {
                lMaxRepeat = lRepeat;
              }
            }
          }

          //
          // check if we have a time field defined by the header keys TIMEZERO and DELTA_T.
          //  If so then we create a new field called $TIME_FIELD, unless such a field already
          //  exists, in which case we do nothing...
          //
          char charTimeZero[] = "TIMEZERO";

          iStatus = 0;
          iResult = fits_read_key( ffits, TDOUBLE, charTimeZero, &_dTimeZero, 0L, &iStatus );
          if( iResult == 0 )
          {
            char charTimeDelta[] = "DELTA_T";

            iResult = fits_read_key( ffits, TDOUBLE, charTimeDelta, &_dTimeDelta, 0L, &iStatus );
            if( iResult == 0 )
            {
              if( _fieldList.find( QString( TIME_FIELD ) ) == _fieldList.end( ) )
              {
                _bHasTime = true;
                _fieldList.append( TIME_FIELD );
              }
            }
          }

          if( lNumFrames * lMaxRepeat != _numFrames )
          {
            _numCols   = iNumCols;
            _numFrames = lNumFrames * lMaxRepeat;
            updateType = KstObject::UPDATE;
          }
        }
      }
      iStatus = 0;
      fits_close_file( ffits, &iStatus );
    }
  }

  updateNumFramesScalar();

  return setLastUpdateResult(updateType);
}


int LFIIOSource::readField( double *v, const QString& field, int s, int n )
{
  double    dNan = strtod( "nan", NULL );
  fitsfile* ffits;
  bool      bOk;
  int       i;
  int       iCol;
  int       iRead = -1;
  int       iStatus = 0;
  int       iAnyNull;
  int       iResult = 0;

  if( n < 0 )
  {
    n = 1; /* n < 0 means read one sample, not frame - irrelavent here */
  }

  if( field == "INDEX" )
  {
    for( i = 0; i < n; i++ )
    {
      v[i] = (double)( s + i );
    }
    iRead =  n;
  }
  else if( _bHasTime && field == TIME_FIELD )
  {
    for( i = 0; i < n; i++ )
    {
      v[i] = _dTimeZero + ( (double)( s + i ) * _dTimeDelta );
    }
    iRead =  n;    
  }
  else
  {
    memset( v, 0, n * sizeof( double ) );

    bOk = getColNumber( field, &iCol );
    if( bOk )
    {
      _valid = false;

      if( !_filename.isNull( ) && !_filename.isEmpty( ) )
      {
        iResult = fits_open_table( &ffits, _filename.ascii( ), READONLY, &iStatus );
        if( iResult == 0 )
        {
          _valid = true;

          //
          // copy the data...
          // N.B. fitsio column indices are 1 based, so we ask for iCol+1 instead of just iCol
          //
          iResult = fits_read_col( ffits, TDOUBLE, iCol+1, s+1, 1, n, &dNan, v, &iAnyNull, &iStatus );
          if( iResult == 0 )
          {
            iRead = n;
          }

          iStatus = 0;
          fits_close_file( ffits, &iStatus );
        }
      }
    }
  }

  return iRead;
}


bool LFIIOSource::getColNumber( const QString& field, int* piColNumber ) const
{
  QString strName;
  bool    bOk     = false;
  bool    bRetVal = false;
  int     iCount;
  int     iCol;
  int     i;

  iCol = field.toUInt( &bOk );
  if( bOk )
  {
    if( iCol >= 0 && iCol < _numCols )
    {
      *piColNumber = iCol;

      bRetVal = true;
    }
  }
  else
  {
    iCount = _fieldList.count( );

    //
    // start at 1, because the first entry is INDEX...
    //
    for( i=1; i<iCount; i++ )
    {
      strName = _fieldList[i].lower( );
      if( strName.compare( field.lower( ) ) == 0 )
      {
        bRetVal = true;

        //
        // subtract 1 to be consistent with the value returned from the previous branch...
        //
        *piColNumber = i-1;

        break;
      }
    }
  }

  return bRetVal;
}


bool LFIIOSource::isValidField( const QString& field ) const
{
  bool bRetVal = false;
  int  iCol;

  if( field == "INDEX" )
  {
    bRetVal = true;
  }
  else if( field == TIME_FIELD && _bHasTime )
  {
    bRetVal = true;
  }
  else
  {
    bRetVal = getColNumber( field, &iCol );
  }

  return bRetVal;
}


int LFIIOSource::samplesPerFrame( const QString& field )
{
  Q_UNUSED( field )

  return 1;
}


int LFIIOSource::frameCount(const QString& field) const
{
  Q_UNUSED( field )

  return  _numFrames;
}


bool LFIIOSource::isEmpty() const {
  return _numFrames < 1;
}


QString LFIIOSource::fileType( ) const
{
  return "LFIIO";
}


void LFIIOSource::save( QTextStream& ts, const QString& indent )
{
  KstDataSource::save( ts, indent );
}


extern "C" {
  KstDataSource *create_lfiio( KConfig *cfg, const QString& filename, const QString& type )
  {
    return new LFIIOSource( cfg, filename, type );
  }

  QStringList provides_lfiio( )
  {
    QStringList rc;

    rc += "LFIIO";

    return rc;
  }

  int understands_lfiio( KConfig*, const QString& filename )
  {
    fitsfile* ffits;
    int       iStatus = 0;
    int       iRetVal = 0;

    //
    // determine if it is a FITS file...
    //
    if( fits_open_table( &ffits, filename.ascii( ), READONLY, &iStatus ) == 0 )
    {
      fits_close_file( ffits, &iStatus );

      iRetVal = 90;
    }
    else
    {
      //
      // failed to open the file, so we can't understand it...
      //
    }
  
    return iRetVal;
  }
}

KST_KEY_DATASOURCE_PLUGIN(lfiio)

// vim: ts=2 sw=2 et
