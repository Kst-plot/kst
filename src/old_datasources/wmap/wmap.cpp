/***************************************************************************
                    wmap.cpp  - WMAP FITS file data source
                             -------------------
    begin                : Feb 7 2007
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

#include "wmap.h"

#include <kdebug.h>
#include <qfile.h>
#include <ctype.h>
#include <stdlib.h>

#include "kststring.h"

#define TIME_FIELD  "TIME"

static char* keywords[] = {
    "STIME",
    "ETIME",
    "RELEASE",
    "NUMREC"
};

static char* metaDataHeaders[] = {
    "TIME",
    "POSITION",
    "VELOCITY",
    "GEOPOS",
    "GEOVEL",
    "QUATERN",
    "NSCI",
    "NDIHK"
};

static char* scienceDataHeaders[] = {
    "TIME",
    "K1#",
    "KA1#",
    "Q1#",
    "Q2#",
    "V1#",
    "V2#",
    "W1#",
    "W2#",
    "W3#",
    "W4#",
    "GenFlags",
    "DAFlags",
    "Error1",
    "Error2",
    "FrmInd",
    "SciInd"
};

static char* AIHKDataHeaders[] = {
    "TIME",
    "PDU",
    "AEU1",
    "AEU2",
    "AWIN1",
    "AWIN2",
    "Counters",
    "FrmInd",
    "Sweep"
};

static char* DIHKDataHeaders[] = {
    "TIME",
    "Data",
    "FrmInd",
    "DihkInd"
};

static char* LOSDataHeaders[] = {
    "K1A",
    "K1B",
    "KA1A",
    "KA1B",
    "Q1A",
    "Q1B",
    "Q2A",
    "Q2B",
    "V1A",
    "V1B",
    "V2A",
    "V2B",
    "W1A",
    "W1B",
    "W2A",
    "W2B",
    "W3A",
    "W3B",
    "W4A",
    "W4B"
};

static int iNumKeywords = sizeof(keywords)/sizeof(char*);
static int iNumMetaDataHeaders = sizeof(metaDataHeaders)/sizeof(char*);
static int iNumScienceDataHeaders = sizeof(scienceDataHeaders)/sizeof(char*);
static int iNumAIHKDataHeaders = sizeof(AIHKDataHeaders)/sizeof(char*);
static int iNumDIHKDataHeaders = sizeof(DIHKDataHeaders)/sizeof(char*);
static int iNumLOSDataHeaders = sizeof(LOSDataHeaders)/sizeof(char*);

WMAPSource::WMAPSource( KConfig *cfg, const QString& filename, const QString& type )
: KstDataSource( cfg, filename, type )
{
  _fields.setAutoDelete( TRUE );

  if( type.isEmpty( ) || type == "WMAP" )
  {
    if( initFile( ) )
    {
      _valid = true;
    }
  }
}


WMAPSource::~WMAPSource( )
{
}


bool WMAPSource::reset( )
{
  return true;
}


void WMAPSource::addToMetadata( fitsfile *ffits, int &iStatus )
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

void WMAPSource::addToMetadata( fitsfile *ffits, const int iNumCols, int &iStatus )
{
  QString str;
  char charTemplate[ FLEN_CARD ];
  char charName[ FLEN_CARD ];
  long lRepeat;
  long lWidth;
  int iTypeCode;
  int iColNumber;
  int iResult;
  int entry;
  int col;

  for( col=0; col<iNumCols; col++ )
  {
    iResult = fits_get_coltype( ffits, col+1, &iTypeCode, &lRepeat, &lWidth, &iStatus );
    if( iResult == 0 )
    {
      sprintf( charTemplate, "%d", col+1 );
      iResult = fits_get_colname( ffits, CASEINSEN, charTemplate, charName, &iColNumber, &iStatus );
      if( iResult == 0 )
      {
        if( lRepeat == 3 )
        {
          QString strValue;
          double dNan = strtod( "nan", NULL );
          double value;
          int iAnyNull;

          for( entry=0; entry<lRepeat; entry++ )
          {
            iResult = fits_read_col( ffits, TDOUBLE, iColNumber, 1, entry+1, 1, &dNan, &value, &iAnyNull, &iStatus );
            if( iResult == 0 )
            {
              KstString *metaString;
              QString keyname = QString("%1_%2").arg(charName).arg(QChar('X'+entry));
              KstObjectTag newTag( keyname, tag( ) );

              strValue = QString("%1").arg(value);
              metaString = new KstString( newTag, this, strValue );
              _metaData.insert( keyname, metaString );
            }
          }
        }
      }
    }
  }
}

void WMAPSource::addToFieldList( fitsfile *ffits, const int iNumCols, const long lNumRows, const long lNumBaseRows, int &iStatus )
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
  int entry;
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
          fld->entry = 1;
          fld->entries = lRepeat;
          fld->numSamplesPerFrame = lNumRows / lNumBaseRows;
          fld->numFrames = lNumBaseRows;

          str = charName;
          if( _fields.find( str ) != 0L )
          {
            str += QString("_%1").arg( iHDUNumber );
          }
          _fields.insert( str, fld );
          _fieldList.append( str );
        }
        else if( lRepeat == 3 )
        {
          for( entry=0; entry<lRepeat; entry++ )
          {
            field *fld = new field;

            fld->table = table;
            fld->column = iColNumber;
            fld->entry = entry+1;
            fld->entries = lRepeat;
            fld->numSamplesPerFrame = lNumRows / lNumBaseRows;
            fld->numFrames = lNumBaseRows;

            str = QString("%1_%2").arg(charName).arg(QChar('X'+entry));
            _fields.insert( str, fld );
            _fieldList.append( str );
          }
        }
        else if( strcmp( charName, "QUATERN" ) == 0 )
        {
          for( entry=0; entry<4; entry++ )
          {
            field *fld = new field;

            fld->table = table;
            fld->column = iColNumber;
            fld->entry = entry+1;
            fld->entries = 4;
            fld->numSamplesPerFrame = ( lRepeat / 4 ) - 3;
            fld->numFrames = lNumBaseRows;

            str = QString("%1_%2").arg(charName).arg(QChar('a'+entry));
            _fields.insert( str, fld );
            _fieldList.append( str );
          }
        }
        else
        {
          for( entry=0; entry<lRepeat; entry++ )
          {
            field *fld = new field;

            fld->table = table;
            fld->column = iColNumber;
            fld->entry = entry+1;
            fld->entries = lRepeat;
            fld->numSamplesPerFrame = lNumRows / lNumBaseRows;
            fld->numFrames = lNumBaseRows;

            str = QString("%1_%2").arg(charName).arg(entry);
            _fields.insert( str, fld );
            _fieldList.append( str );
          }
        }
      }
    }
  }
}

bool WMAPSource::initFile( )
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
        long lNumBaseRows = 0;
        long lNumRows;
        int iHDUType;
        int i;

        //
        // determine the number of frames...
        //
        for( i=0; i<iNumHeaderDataUnits-1; i++ )
        {
          if( iStatus == 0 )
          {
            fits_get_hdu_type( ffits, &iHDUType, &iStatus );
            if( iHDUType == BINARY_TBL || iHDUType == ASCII_TBL )
            {
              iResult = fits_get_num_rows( ffits, &lNumRows, &iStatus );
              if( iResult == 0 )
              {
                if( lNumBaseRows == 0 )
                {
                  lNumBaseRows = lNumRows;
                }
                else if( lNumRows != 1 )
                {
                  if( lNumRows < lNumBaseRows )
                  {
                    lNumBaseRows = lNumRows;
                  }
                }
              }
            }

            fits_movrel_hdu( ffits, 1, &iHDUType, &iStatus );
          }
        }

        fits_movabs_hdu( ffits, 1, &iHDUType, &iStatus);

        field *fld = new field;

        fld->table = 0;
        fld->column = 0;
        fld->entry = 0;
        fld->entries = 0;
        fld->numSamplesPerFrame = 1;
        fld->numFrames = lNumBaseRows;

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
                    if( lNumRows > 1 )
                    {
                      addToFieldList( ffits, iNumCols, lNumRows, lNumBaseRows, iStatus );
                    }
                    else if( lNumRows == 1 )
                    {
                      addToMetadata( ffits, iNumCols, iStatus );
                    }
                  }
                }
              }
            }

            fits_movrel_hdu( ffits, 1, &iHDUType, &iStatus);
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


KstObject::UpdateType WMAPSource::update( int u )
{
  if (KstObject::checkUpdateCounter(u)) {
    return lastUpdateResult();
  }

  KstObject::UpdateType updateType =  KstObject::NO_CHANGE;

  return setLastUpdateResult(updateType);
}


int WMAPSource::readField( double *v, const QString& fieldName, int s, int n )
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
    for( i = 0; i < n; i++ )
    {
      v[i] = (double)( s + i );
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
                if( fieldName.startsWith( "QUATERN" ) )
                {
                  iResult = fits_read_col( ffits, TDOUBLE, fld->column, s+1, fld->entry+4, 1, &dNan, v, &iAnyNull, &iStatus );
                }
                else
                {
                  iResult = fits_read_col( ffits, TDOUBLE, fld->column, (s*fld->numSamplesPerFrame)+1, fld->entry, 1, &dNan, v, &iAnyNull, &iStatus );
                }

                if( iResult == 0 )
                {
                  iRead = 1;
                }
              }
              else if( fld->entries == 1)
              {
                iResult = fits_read_col( ffits, TDOUBLE, fld->column, (s*fld->numSamplesPerFrame)+1, fld->entry, n*fld->numSamplesPerFrame, &dNan, v, &iAnyNull, &iStatus );
                if( iResult == 0 )
                {
                  iRead = n * fld->numSamplesPerFrame;
                }
              }
              else if( fieldName.startsWith( "QUATERN" ) )
              {
                int frame;

                for( frame=s; frame<s+n; ++frame )
                {
                  long naxes[] = { fld->entries, fld->numSamplesPerFrame };
                  long fpixels[] = { fld->entry + 4, frame + 1 };
                  long lpixels[] = { fld->entry + 4 + ( 4 * fld->numSamplesPerFrame ), frame + 1 };
                  long inc[] = { 4, 1 };

                  iResult = fits_read_subset_dbl( ffits, fld->column, 1, 
                          naxes, (long*)fpixels, (long*)lpixels, (long*)inc, dNan, v, &iAnyNull, &iStatus);
                  v += fld->numSamplesPerFrame;
                }

                if( iResult == 0 )
                {
                  iRead = n * fld->numSamplesPerFrame;
                }
              }
              else
              {
                long naxes[] = { fld->entries, fld->numFrames * fld->numSamplesPerFrame };
                long fpixels[] = { fld->entry, ( s * fld->numSamplesPerFrame ) + 1 };
                long lpixels[] = { fld->entry, ( ( s + n ) * fld->numSamplesPerFrame ) };
                long inc[] = { 1, 1 };

                iResult = fits_read_subset_dbl( ffits, fld->column, 1, 
                          naxes, (long*)fpixels, (long*)lpixels, (long*)inc, dNan, v, &iAnyNull, &iStatus);

                if( iResult == 0 )
                {
                  iRead = n * fld->numSamplesPerFrame;
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


bool WMAPSource::isValidField( const QString& field ) const
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


int WMAPSource::samplesPerFrame( const QString& fieldName )
{
  int rc = 1;
  field* fld = 0L;

  fld = _fields.find( fieldName );
  if( fld != 0L )
  {
    rc = fld->numSamplesPerFrame;
  }

  return rc;
}


int WMAPSource::frameCount( const QString& fieldName ) const
{
  int rc = 1;
  field* fld = 0L;

  if( fieldName.isEmpty() )
  {
    fld = _fields.find( "POSITION_X" );
    if( fld != 0L )
    {
      rc = fld->numFrames;
    }
  }
  else
  {
    fld = _fields.find( fieldName );
    if( fld != 0L )
    {
      rc = fld->numFrames;
    }
  }

  return rc;
}


bool WMAPSource::isEmpty( ) const {
  return _fields.isEmpty();
}


QString WMAPSource::fileType( ) const
{
  return "WMAP";
}


void WMAPSource::save( QTextStream& ts, const QString& indent )
{
  KstDataSource::save( ts, indent );
}


extern "C" {
  KstDataSource *create_wmap( KConfig *cfg, const QString& filename, const QString& type )
  {
    return new WMAPSource( cfg, filename, type );
  }

  QStringList provides_wmap( )
  {
    QStringList rc;

    rc += "WMAP";

    return rc;
  }

  int understands_wmap( KConfig*, const QString& filename )
  {
    fitsfile* ffits;
    int       iStatus = 0;
    int       iRetVal = 0;

    //
    // determine if it is a WMAP file...
    //
    if( fits_open_file( &ffits, filename.ascii( ), READONLY, &iStatus ) == 0 )
    {
      int iNumHeaderDataUnits;

      if( fits_get_num_hdus( ffits, &iNumHeaderDataUnits, &iStatus ) == 0 )
      {
        if( iNumHeaderDataUnits == 6 )
        {
          char  value[FLEN_VALUE];
          char  comment[FLEN_COMMENT];
          int   iHDUType;
          int   iColNum;
          int   i;
          int   j;

          for( i=0; i<iNumKeywords; ++i )
          {
            if( iStatus == 0 )
            {
              fits_read_keyword( ffits, keywords[i], value, comment, &iStatus );
            }
            else  
            {
              break;
            }
          }

          if( iStatus == 0 )
          {
            char **entries = metaDataHeaders;
            int iNumEntries = iNumMetaDataHeaders;

            for( i=0; i<iNumHeaderDataUnits-1; i++ )
            {
              switch (i)
              {
                case 0:
                  iNumEntries = iNumMetaDataHeaders;
                  entries = metaDataHeaders;
                  break;
                case 1:
                  iNumEntries = iNumScienceDataHeaders;
                  entries = scienceDataHeaders;
                  break;
                case 2:
                  iNumEntries = iNumAIHKDataHeaders;
                  entries = AIHKDataHeaders;
                  break;
                case 3:
                  iNumEntries = iNumDIHKDataHeaders;
                  entries = DIHKDataHeaders;
                  break;
                case 4:
                  iNumEntries = iNumLOSDataHeaders;
                  entries = LOSDataHeaders;
                  break;
                default:
                  iNumEntries = iNumMetaDataHeaders;
                  entries = metaDataHeaders;
                  break;
              }

              fits_movrel_hdu( ffits, 1, &iHDUType, &iStatus);
              if( iStatus == 0 && iHDUType == BINARY_TBL )
              {
                for( j=0; j<iNumEntries; ++j )
                {
                  if( iStatus == 0 )
                  {
                    fits_get_colnum( ffits, 0, entries[j], &iColNum, &iStatus );
                    if( strchr( entries[j], '#' ) != 0L &&  iStatus == COL_NOT_UNIQUE )
                    {
                      iStatus = 0;
                    }
                  }
                  else  
                  {
                    break;
                  }
                }
              }
            }
          }

          if( iStatus == 0 )
          {
            iRetVal = 99;
          }
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

KST_KEY_DATASOURCE_PLUGIN(wmap)

// vim: ts=2 sw=2 et
