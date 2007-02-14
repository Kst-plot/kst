/***************************************************************************
                    planckIDEF.cpp  - Planck IDIS DMC Exchange Format file data source
                             -------------------
    begin                : Feb 13 2007
    copyright            : (C) 2007 The University of British Columbia
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

#include "planckIDEF.h"

#include <kdebug.h>
#include <qfile.h>
#include <ctype.h>
#include <stdlib.h>

#include "kststring.h"

#define TIME_FIELD  "TIME"

PLANCKIDEFSource::PLANCKIDEFSource( KConfig *cfg, const QString& filename, const QString& type )
: KstDataSource( cfg, filename, type )
{
  _fields.setAutoDelete( TRUE );

  if( type.isEmpty( ) || type == "PLANCKIDEF" )
  {
    if( initFile( ) )
    {
      _valid = true;
    }
  }
}


PLANCKIDEFSource::~PLANCKIDEFSource( )
{
}


bool PLANCKIDEFSource::reset( )
{
  return true;
}


void PLANCKIDEFSource::addToMetadata( fitsfile *ffits, int &iStatus )
{
  QString   str;
  int iResult;
  int keysexist;
  int morekeys;

  iResult = fits_get_hdrspace( ffits, &keysexist, &morekeys, &iStatus );
  if( iResult == 0 )
  {
    QString strTable;
    KstObjectTag tableTag( strTable, tag( ) );
    char keyname[FLEN_KEYWORD];
    char value[FLEN_VALUE];
    char comment[FLEN_COMMENT];
    int keynum;

    for( keynum=1; keynum<=keysexist; ++keynum )
    {
      iResult = fits_read_keyn( ffits, keynum, keyname, value, comment, &iStatus );
      if( iResult == 0 )
      {
        KstString *metaString;
        KstObjectTag newTag( keyname, tag( ) );

        str.sprintf( "%s %s", value, comment );
        metaString = new KstString( newTag, this, str );
        _metaData.insert( keyname, metaString );
      }
    }
  }
}


void PLANCKIDEFSource::addToFieldList( fitsfile *ffits, const int iNumCols, int &iStatus )
{
  QString str;
  char charTemplate[ FLEN_CARD ];
  char charName[ FLEN_CARD ];
  long lRepeat;
  long lWidth;
  int iHDUNumber;
  int iTypeCode;
  int iColNumber;
  int iResult;
  int table;
  int col;

  table = fits_get_hdu_num( ffits, &iHDUNumber );

  for( col=0; col<iNumCols; col++ )
  {
    iResult = fits_get_coltype( ffits, col+1, &iTypeCode, &lRepeat, &lWidth, &iStatus );
    if( iResult == 0 )
    {
      sprintf( charTemplate, "%d", col+1 );
      iResult = fits_get_colname( ffits, CASEINSEN, charTemplate, charName, &iColNumber, &iStatus );
      if( iResult == 0 )
      {
        if( lRepeat == 1 )
        {
          field *fld = new field;

          fld->table = table;
          fld->column = iColNumber;

          str = charName;
          if( _fields.find( str ) != 0L )
          {
            str += QString("_%1").arg( iHDUNumber );
          }
          _fields.insert( str, fld );
          _fieldList.append( str );
        }
      }
    }
  }
}

bool PLANCKIDEFSource::initFile( )
{
  bool bRetVal = true;
  int iResult = 0;

  _numFrames = 0;

  if( !_filename.isNull( ) && !_filename.isEmpty( ) )
  {
    QString   str;
    fitsfile* ffits;
    int       iStatus = 0;

    iResult = fits_open_file( &ffits, _filename.ascii( ), READONLY, &iStatus );
    if( iResult == 0 )
    {
      int iNumHeaderDataUnits;

      if( fits_get_num_hdus( ffits, &iNumHeaderDataUnits, &iStatus ) == 0 )
      {
        long lNumRows;
        int iHDUType;
        int i;

        //
        // determine the number of frames...
        //
        if( iNumHeaderDataUnits > 1 )
        {
          if( fits_movabs_hdu( ffits, 2, &iHDUType, &iStatus ) == 0 )
          {
            if( fits_get_hdu_type( ffits, &iHDUType, &iStatus ) == 0 )
            {
              if( iHDUType == BINARY_TBL )
              {
                iResult = fits_get_num_rows( ffits, &lNumRows, &iStatus );
                if( iResult == 0 )
                {
                  _numFrames = lNumRows;
                }
              }
            }
          }
        }

        if( _numFrames > 0 )
        {
          fits_movabs_hdu( ffits, 1, &iHDUType, &iStatus );

          field *fld = new field;

          fld->table = 0;
          fld->column = 0;

          _fields.insert( "INDEX", fld );
          _fieldList.append( "INDEX" );

          //
          // add the fields and metadata...
          //
          for( i=0; i<iNumHeaderDataUnits-1; i++ )
          {
            if( iStatus == 0 )
            {
              addToMetadata( ffits, iStatus );

              //
              // the first table never contains data...
              //
              if( i > 0 )
              {
                //
                // create the time entries if necessary...
                //
                if( _fields.find( "TIME_ABSOLUTE" ) == 0L &&
                    _fields.find( "TIME_RELATIVE" ) == 0L )
                {
                  char comment[FLEN_COMMENT];
                  int iStatusDelta = 0;
                  int iStatusZero = 0;

                  fits_read_key( ffits, TDOUBLE, "DELTA_T", &_dTimeDelta, comment, &iStatusDelta );
                  fits_read_key( ffits, TDOUBLE, "TIMEZERO", &_dTimeZero, comment, &iStatusZero );

                  if( iStatusDelta == 0 )
                  {
                    if( iStatusZero == 0 )
                    {
                      field *fld = new field;

                      fld->table = 0;
                      fld->column = 0;

                      _fields.insert( "TIME_ABSOLUTE", fld );
                      _fieldList.append( "TIME_ABSOLUTE" );
                    }
                    else
                    {
                      field *fld = new field;

                      fld->table = 0;
                      fld->column = 0;

                      _fields.insert( "TIME_RELATIVE", fld );
                      _fieldList.append( "TIME_RELATIVE" );
                    }
                  }
                }

                //
                // create the field entries...
                //
                fits_get_hdu_type( ffits, &iHDUType, &iStatus );
                if( iStatus == 0 )
                {
                  if( iHDUType == BINARY_TBL || iHDUType == ASCII_TBL )
                  {
                    int iNumCols;

                    iResult = fits_get_num_cols( ffits, &iNumCols, &iStatus );
                    if( iResult == 0 )
                    {
                      iResult = fits_get_num_rows( ffits, &lNumRows, &iStatus );
                      if( iResult == 0 )
                      {
                        addToFieldList( ffits, iNumCols, iStatus );
                      }
                    }
                  }
                }
              }

              fits_movrel_hdu( ffits, 1, &iHDUType, &iStatus);
            }
          }
        }
      }

      iStatus = 0;

      updateNumFramesScalar( );

      fits_close_file( ffits, &iStatus );
    }
  }

  return bRetVal;
}


KstObject::UpdateType PLANCKIDEFSource::update( int u )
{
  if (KstObject::checkUpdateCounter(u)) {
    return lastUpdateResult();
  }

  KstObject::UpdateType updateType =  KstObject::NO_CHANGE;

  return setLastUpdateResult(updateType);
}


int PLANCKIDEFSource::readField( double *v, const QString& fieldName, int s, int n )
{
  double    dNan = strtod( "nan", NULL );
  fitsfile* ffits;
  int       i;
  int       iRead = -1;
  int       iStatus = 0;
  int       iAnyNull;
  int       iResult = 0;

  if( fieldName == "INDEX" )
  {
    for( i = 0; i < n; ++i )
    {
      v[i] = (double)( s + i );
    }

    iRead =  n;
  }
  else if( fieldName == "TIME_ABSOLUTE" && _fields.find( fieldName ) != 0L )
  {
    for( i = 0; i < n; ++i )
    {
      v[i] = _dTimeZero + ( double( s + i ) * _dTimeDelta );
    }

    iRead =  n;
  }
  else if( fieldName == "TIME_RELATIVE" && _fields.find( fieldName ) != 0L )
  {
    for( i = 0; i < n; i++ )
    {
      v[i] = double( s + i ) * _dTimeDelta;
    }

    iRead =  n;
  }
  else
  {
    field *fld = 0L;

    fld = _fields.find( fieldName );
    if( fld != 0L ) 
    {
      _valid = false;

      if( !_filename.isNull( ) && !_filename.isEmpty( ) )
      {
        iResult = fits_open_file( &ffits, _filename.ascii( ), READONLY, &iStatus );
        if( iResult == 0 )
        {
          int iHDUType;

          if( fits_movabs_hdu( ffits, fld->table, &iHDUType, &iStatus ) == 0 )
          {
            if( iHDUType == BINARY_TBL )
            {
              _valid = true;

              if( n < 0 )
              {
                iResult = fits_read_col( ffits, TDOUBLE, fld->column, s+1, 1, 1, &dNan, v, &iAnyNull, &iStatus );
                if( iResult == 0 )
                {
                  iRead = 1;
                }
              }
              else
              {
                iResult = fits_read_col( ffits, TDOUBLE, fld->column, s+1, 1, n, &dNan, v, &iAnyNull, &iStatus );
                if( iResult == 0 )
                {
                  iRead = n;
                }
              }
 
              iStatus = 0;
            }
          }

          fits_close_file( ffits, &iStatus );
        }
      }
    }
  }

  return iRead;
}


bool PLANCKIDEFSource::isValidField( const QString& field ) const
{
  bool bRetVal = false;

  if( field == "INDEX" )
  {
    bRetVal = true;
  }
  else
  {
    if( _fields.find( field ) != 0L )
    {
      bRetVal = true;
    }
  }

  return bRetVal;
}


int PLANCKIDEFSource::samplesPerFrame( const QString& fieldName )
{
  Q_UNUSED( fieldName )

  int rc = 1;

  return rc;
}


int PLANCKIDEFSource::frameCount( const QString& fieldName ) const
{
  Q_UNUSED( fieldName )

  int rc = _numFrames;

  return rc;
}


bool PLANCKIDEFSource::isEmpty( ) const {
  return _fields.isEmpty();
}


QString PLANCKIDEFSource::fileType( ) const
{
  return "PLANCKIDEF";
}


void PLANCKIDEFSource::save( QTextStream& ts, const QString& indent )
{
  KstDataSource::save( ts, indent );
}


extern "C" {
  KstDataSource *create_planckIDEF( KConfig *cfg, const QString& filename, const QString& type )
  {
    return new PLANCKIDEFSource( cfg, filename, type );
  }

  QStringList provides_planckIDEF( )
  {
    QStringList rc;

    rc += "PLANCKIDEF";

    return rc;
  }

  int understands_planckIDEF( KConfig*, const QString& filename )
  {
    fitsfile* ffits;
    int       iStatus = 0;
    int       iRetVal = 0;

    //
    // determine if it is a Planck IDIS DMC Exchange Format file...
    //
    if( fits_open_file( &ffits, filename.ascii( ), READONLY, &iStatus ) == 0 )
    {
      int iNumHeaderDataUnits;

      if( fits_get_num_hdus( ffits, &iNumHeaderDataUnits, &iStatus ) == 0 )
      {
        char  value[FLEN_VALUE];
        char  comment[FLEN_COMMENT];
        bool  ok = false;
        int   iHDUType;
        int   iValue;
        int   i;

        //
        // the primary header should never have any data...
        //
        if( fits_get_hdu_type( ffits, &iHDUType, &iStatus ) == 0 )
        {
          if( iHDUType == IMAGE_HDU )
          {
            if( fits_read_key( ffits, TLOGICAL, "SIMPLE", &iValue, comment, &iStatus ) == 0 )
            {
              if( iValue != 0 )
              {
                if( fits_read_key( ffits, TLOGICAL, "EXTEND", &iValue, comment, &iStatus ) == 0 )
                {
                  if( iValue != 0 )
                  {
                    if( fits_read_key( ffits, TINT, "NAXIS", &iValue, comment, &iStatus ) == 0 )
                    {
                      if( iValue == 0 )
                      {
                        ok = true;
                      }
                    }
                  }
                }
              }
            }
          }
        }

        //
        // the name of each binary table must conform to aaa-bbbb[-ccc]
        //  where bbbb is a member of {OBTT, TOD., OBTH, HKP.}...
        //
        if( ok && iStatus == 0 )
        {
          if( iNumHeaderDataUnits > 1 )
          {
            long rowsCompare = 0;
            long rows;
            int cols;

            for( i=0; i<iNumHeaderDataUnits-1 && ok; i++ )
            {
              ok = false;

              fits_movrel_hdu( ffits, 1, &iHDUType, &iStatus);
              if( iStatus == 0 && iHDUType == BINARY_TBL )
              {
                if( fits_read_keyword( ffits, "EXTNAME", value, comment, &iStatus ) == 0 )
                {
                  QString section = QString( value ).section( '-', 1, 1 );

                  if( section.compare( "OBTT" ) == 0 ||
                      section.compare( "TOD." ) == 0 ||
                      section.compare( "OBTH" ) == 0 ||
                      section.compare( "HKP." ) == 0 )
                  {
                    bool okCols = false;

                    //
                    // all the columns should have a repeat value of 1...
                    //
                    if( fits_get_num_cols( ffits, &cols, &iStatus ) == 0 )
                    {
                      long lRepeat;
                      long lWidth;
                      int iTypeCode;
                      int col;

                      if( cols > 0 )
                      {
                        okCols = true;

                        for( col=0; col<cols; ++col )
                        {
                          if( fits_get_coltype( ffits, col+1, &iTypeCode, &lRepeat, &lWidth, &iStatus ) == 0 )
                          {
                            if( lRepeat != 1 )
                            {
                              okCols = false;

                              break;
                            }
                          }
                          else
                          {
                            okCols = false;

                            break;
                          }
                        }
                      }
                    }

                    if( okCols && fits_get_num_rows( ffits, &rows, &iStatus ) == 0 )
                    {
                      //
                      // all tables should have the same number of rows...
                      //
                      if( i == 0 )
                      {
                        rowsCompare = rows;

                        ok = true;
                      }
                      else if( rowsCompare == rows )
                      {
                        ok = true;
                      }
                    }
                  }
                }
              }
            }
          }
          else
          {
            ok = false;
          }
        }

        if( ok && iStatus == 0 )
        {
          iRetVal = 99;
        }
      }

      iStatus = 0;

      fits_close_file( ffits, &iStatus );
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

KST_KEY_DATASOURCE_PLUGIN(planckIDEF)

// vim: ts=2 sw=2 et
