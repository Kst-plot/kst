/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "planckIDEF.h"

#include <assert.h>
#include <QXmlStreamWriter>
#include <math.h>
#include <QFileInfo>
#include <QDir>

#include "ui_planckIDEFconfig.h"

#include "kst_i18n.h"

static const QString PlanckIDEFTypeString = I18N_NOOP("PlanckIDEF image");

class PlanckIDEFSource::Config {
  public:
    Config() {
      _checkFilename = true;
    }

    bool _checkFilename;

    void read(QSettings *cfg, const QString& fileName = QString::null) {
      Q_UNUSED(fileName);
      cfg->beginGroup(PlanckIDEFTypeString);
      _checkFilename = cfg->readBoolEntry("Check Filename", true);
      cfg->endGroup();
    }

    void save(QXmlStreamWriter& s) {
      s.writeStartElement("properties");
      s.writeAttribute("checkfilename", QVariant(_checkFilename).toString());
      s.writeEndElement();
    }

    void parseProperties(QXmlStreamAttributes &properties) {
      _checkFilename = QVariant(properties.value("checkfilename").toString()).toBool();
    }


    void load(const QDomElement& e) {
      _checkFilename = false;

      QDomNode n = e.firstChild();
      while (!n.isNull()) {
        QDomElement e = n.toElement();
        if (!e.isNull()) {
          if (e.tagName() == "checkfilename") {
            _checkFilename = true;
          }
        }
        n = n.nextSibling();
      }
    }
};


PlanckIDEFSource::PlanckIDEFSource(Kst::ObjectStore *store, QSettings *cfg, const QString& filename, const QString& type, const QDomElement& e)
: Kst::DataSource(store, cfg, filename, type, None), _config(0L) {
  _valid = false;

  if (!type.isEmpty() && type != "PlanckIDEF Source") {
    return;
  }

  _config = new PlanckIDEFSource::Config;
  _config->read(cfg, filename);
  if (!e.isNull()) {
    _config->load(e);
  }

  if (init()) {
    _valid = true;
  }

  update();
}


PlanckIDEFSource::~PlanckIDEFSource() {
  delete _config;
  _config = 0L;
}


bool PlanckIDEFSource::isValidFilename(const QString& filename, Config *config) {
  bool ok = false;

  if (!config || config->_checkFilename) {
    unsigned gzLength;

    if (filename.right(3).lower() == ".gz") {
      gzLength = 3;
    } else {
      gzLength = 0;
    }

    if (!ok) {
      //
      // check for a valid science timeline file...
      //
      // Accc-xxxx-T-yyyymmdd.fits
      //  A = one character coding the instrument identifier
      //  ccc = three character coding the obervation frequency
      //  xxxx = four digits identifying the operational day to which the data belong
      //  T = {C,R} one letter identifying if the timeline is converted or reduced
      //  yyyy = four digits coding the start timeline year
      //  mm = two digits coding the start timeline month
      //  dd = two digits coding the start timeline day

      int checkLength = gzLength + 25;
      if (filename.length() >= checkLength) {
        QString tail = filename.right(checkLength);
        char instrument;
        char frequency[3];
        char timeline;
        int operationalDay;
        int year;
        int month;
        int day;

        if (sscanf(tail.latin1(), "%c%c%c%c-%4d-%c-%4d%2d%2d.fits",
          &instrument, &frequency[0], &frequency[1], &frequency[2],
          &operationalDay, &timeline, &year, &month, &day) == 9) {
          if (year > 0      &&
              month >= 1    &&
              month <= 12   &&
              day >= 1      &&
              day <= 31    ) {
            ok = true;
          }
        }
      }
    }

    if (!ok) {
      //
      // check for a valid HK timeline file...
      //
      // A-namefile-xxxx-yyyymmdd.fits
      //  A = one character coding the instrument identifier
      //  xxxx = four digits identifying the operational day to which the data belong
      //  yyyy = four digits coding the start timeline year
      //  mm = two digits coding the start timeline month
      //  dd = two digits coding the start timeline day

      int checkLength = gzLength + 19;
      if (filename.length() >= checkLength) {
        QString tail = filename.right(checkLength);
        int operationalDay;
        int year;
        int month;
        int day;

        if (sscanf(tail.latin1(), "-%4d-%4d%2d%2d.fits",
          &operationalDay, &year, &month, &day) == 4) {
          if (year > 0      &&
              month >= 1    &&
              month <= 12   &&
              day >= 1      &&
              day <= 31    ) {
            ok = true;
          }
        }
      }
    }

    if (!ok) {
      //
      // check for a previosuly valid definition of the filename...
      //
      // *_yyyymmddhhmm_vv.fits
      //  yyyy = four digits coding the start timeline year
      //  mm = two digits coding the start timeline month
      //  dd = two digits coding the start timeline day
      //  hh = two digits coding the start timeline hour
      //  mm = two digits coding the start timeline minute
      //  vv = version number (to be used in case of regeneration of HK timelines, starting from 00)

      int checkLength = gzLength + 21;
      if (filename.length() >= checkLength) {
        QString tail = filename.right(checkLength);
        int year;
        int month;
        int day;
        int hour;
        int minute;
        int version;

        if (sscanf(tail.latin1(), "_%4d%2d%2d%2d%2d_%2d.fits", 
            &year, &month, &day, &hour, &minute, &version) == 6) {
          if (year > 0      &&
              month >= 1    &&
              month <= 12   &&
              day >= 1      &&
              day <= 31    ) {
            ok = true;
          }
        }
      }
    }
  } else {
    ok = true;
  }

  return ok;
}


QString PlanckIDEFSource::baseFilename(const QString& filename) {
  QString base;
  int minLength = 14;

  if (filename.right(3).lower() == ".gz") {
    minLength += 3;
  }

  // -yyyymmdd.fits
  //  yyyy = four digits coding the start timeline year
  //  mm = two digits coding the start timeline month
  //  dd = two digits coding the start timeline day
  if (filename.length() > minLength) {
    base = filename.left(filename.length() - minLength);
  }

  return base;
}


bool PlanckIDEFSource::reset() {
  return true;
}


bool PlanckIDEFSource::init() {
  bool bRetVal = true;

  _numFrames = 0;

  if (!_filename.isNull() && !_filename.isEmpty()) {
    QFileInfo fileInfo(_filename);

    if (fileInfo.isFile()) {
      bRetVal = initFile();
      if (bRetVal) {
        _isSingleFile = true;
      }
    }
    else if (fileInfo.isDir()) {
      bRetVal = initFolder();
      if (bRetVal) {
        _isSingleFile = false;
      }
    }
  }

  return bRetVal;
}


bool PlanckIDEFSource::initFile() {
  bool bRetVal = false;

  if (initFile(_filename)) {
    field *fld = new field;

    fld->table = 0;
    fld->column = 0;

    _fields.insert(QString("INDEX"), fld);
    _fieldList.prepend("INDEX");

    bRetVal = true;
  }

  return bRetVal;
}


bool PlanckIDEFSource::initFile(const QString& filename) {
  QString   prefixNew;
  QString   str;
  fitsfile* ffits;
  bool      bRetVal = false;
  int       iResult = 0;
  int       iStatus = 0;

  iResult = fits_open_file(&ffits, filename.ascii(), READONLY, &iStatus);
  if (iResult == 0) {
    int iNumHeaderDataUnits;

    if (fits_get_num_hdus(ffits, &iNumHeaderDataUnits, &iStatus) == 0) {
      long lNumRows;
      int iHDUType;
      int i;

      _numFrames = getNumFrames(ffits, iNumHeaderDataUnits);

      if (_numFrames > 0) {
        fits_movabs_hdu(ffits, 1, &iHDUType, &iStatus);

        // add the fields and metadata...
        for (i=0; i<iNumHeaderDataUnits; i++) {
          if (iStatus == 0) {
            addToMetadata(ffits, iStatus);

            // the first table never contains data...
            if (i > 0) {
              // create the field entries...
              fits_get_hdu_type(ffits, &iHDUType, &iStatus);
              if (iStatus == 0) {
                if (iHDUType == BINARY_TBL) {
                  int iNumCols;

                  iResult = fits_get_num_cols(ffits, &iNumCols, &iStatus);
                  if (iResult == 0) {
                    iResult = fits_get_num_rows(ffits, &lNumRows, &iStatus);
                    if (iResult == 0) {
                      if (iResult == 0) {
                        addToFieldList(ffits, iNumCols, iStatus);
                      }
                    }
                  }
                }
              }
            }
            fits_movrel_hdu(ffits, 1, &iHDUType, &iStatus);
          }
        }
        bRetVal = true;
      }
    }
    iStatus = 0;
    fits_close_file(ffits, &iStatus);
  }
  return bRetVal;
}


bool PlanckIDEFSource::initFolderFile(const QString& filename, const QString& prefix, const QString& baseName) {
  QString   prefixNew;
  QString   str;
  fitsfile* ffits;
  bool      bRetVal = false;
  int       iResult = 0;
  int       iStatus = 0;

  iResult = fits_open_file(&ffits, filename.ascii(), READONLY, &iStatus);
  if (iResult == 0) {
    int iNumHeaderDataUnits;

    if (fits_get_num_hdus(ffits, &iNumHeaderDataUnits, &iStatus) == 0) {
      long lNumRows;
      int iHDUType;
      int i;

      _numFrames = getNumFrames(ffits, iNumHeaderDataUnits);
      if (_numFrames > 0) {
        fits_movabs_hdu(ffits, 1, &iHDUType, &iStatus);

        // add the fields and metadata...
        char charExtName[] = "EXTNAME";
        for (i = 0; i < iNumHeaderDataUnits; i++) {
          if (iStatus == 0) {
            addToMetadata(ffits, iStatus);

            // the first table never contains data...
            if (i > 0) {
              // create the field entries...
              fits_get_hdu_type(ffits, &iHDUType, &iStatus);
              if (iStatus == 0) {
                if (iHDUType == BINARY_TBL) {
                  int iNumCols;

                  iResult = fits_get_num_cols(ffits, &iNumCols, &iStatus);
                  if (iResult == 0) {
                    iResult = fits_get_num_rows(ffits, &lNumRows, &iStatus);
                    if (iResult == 0) {
                      if (!prefix.isEmpty()) {
                        char value[FLEN_VALUE];
                        char comment[FLEN_COMMENT];

                        prefixNew.truncate(0);
                        iResult = fits_read_keyword(ffits, charExtName, value, comment, &iStatus);
                        if (iResult == 0) {
                          prefixNew = prefix + QDir::separator() + QString(value).remove(QChar('\''));
                        }

                        iResult = 0;
                        iStatus = 0;
                      }

                      if (iResult == 0) {
                        addToFieldList(ffits, prefixNew, baseName, iNumCols, iStatus);
                      }
                    }
                  }
                }
              }
            }

            fits_movrel_hdu(ffits, 1, &iHDUType, &iStatus);
          }
        }
        bRetVal = true;
      }
    }
    iStatus = 0;
    fits_close_file(ffits, &iStatus);
  }
  return bRetVal;
}


bool PlanckIDEFSource::initFolder() {
  QDir        folder(_filename, "*.fits *.fits.gz", QDir::Name | QDir::IgnoreCase, QDir::Files | QDir::Readable);
  QStringList files;
  QStringList filesBase;
  bool        bRetVal = true;

  files = folder.entryList();
  if (files.size() > 0) {
    for (QStringList::ConstIterator it = files.begin(); it != files.end(); ++it) {
      if (isValidFilename(*it, 0L)) {
        fileList*   folderFields;
        folderField folderField;
        QString     baseName = baseFilename(*it);
        QString     pathname = folder.path() + QDir::separator() + *it;
        int         numFrames;

        folderFields = *_basefiles.find(baseName);
        if (folderFields == 0L) {
          fileList* folderFieldsNew = new fileList;

          if (initFolderFile(pathname, baseName, baseName)) {
            // first add the INDEX field...
            QString strIndex;
            field *fld = new field;

            fld->basefile = baseName;
            fld->table = 0;
            fld->column = 0;

            strIndex = baseName + QDir::separator() + "INDEX";
            _fields.insert(strIndex, fld);
            _fieldList.append(strIndex);

            //
            // now add everything else...
            //
            folderField.frameLo = 0;
            folderField.frames  = _numFrames;
            folderField.file    = pathname;

            folderFieldsNew->append(folderField);

            _basefiles.replace(baseName, folderFieldsNew);
          }
        } else {
          numFrames = getNumFrames(pathname);
          if (numFrames > 0) {
            folderField.frameLo  = folderFields->back().frameLo + folderFields->back().frames;
            folderField.frames   = numFrames;
            folderField.file     = pathname;

            folderFields->append(folderField);
          }
        }
      }
    }
  }
  return bRetVal;
}


QStringList PlanckIDEFSource::fieldList(const QString& filename) {
  QStringList fields;
  if (!filename.isNull() && !filename.isEmpty()) {
    QFileInfo fileInfo(filename);

    if (fileInfo.isFile()) {
      fields = fieldListFromFile(filename);
    }
    else if (fileInfo.isDir()) {
    }
  }
  return fields;
}


QStringList PlanckIDEFSource::fieldListFromFile(const QString& filename) {
  QStringList fields;
  QString   prefixNew;
  QString   str;
  fitsfile* ffits;
  int       iResult = 0;
  int       iStatus = 0;

  fields.append("INDEX");

  iResult = fits_open_file(&ffits, filename.ascii(), READONLY, &iStatus);
  if (iResult == 0) {
    int iNumHeaderDataUnits;

    if (fits_get_num_hdus(ffits, &iNumHeaderDataUnits, &iStatus) == 0) {
      long lNumRows;
      int iHDUType;
      int i;

      int numFrames = getNumFrames(ffits, iNumHeaderDataUnits);

      if (numFrames > 0) {
        fits_movabs_hdu(ffits, 1, &iHDUType, &iStatus);

        // add the fields and metadata...
        for (i=0; i<iNumHeaderDataUnits; i++) {
          if (iStatus == 0) {
            // the first table never contains data...
            if (i > 0) {
              // create the field entries...
              fits_get_hdu_type(ffits, &iHDUType, &iStatus);
              if (iStatus == 0) {
                if (iHDUType == BINARY_TBL) {
                  int iNumCols;

                  iResult = fits_get_num_cols(ffits, &iNumCols, &iStatus);
                  if (iResult == 0) {
                    iResult = fits_get_num_rows(ffits, &lNumRows, &iStatus);
                    if (iResult == 0) {
                      if (iResult == 0) {
                        QString str;
                        char charTemplate[FLEN_CARD];
                        char charName[FLEN_CARD];
                        long lRepeat;
                        long lWidth;
                        int iHDUNumber;
                        int iTypeCode;
                        int iColNumber;
                        int iResult;
                        int table;
                        int col;

                        table = fits_get_hdu_num(ffits, &iHDUNumber);

                        for (col=0; col<iNumCols; col++) {
                          iResult = fits_get_coltype(ffits, col+1, &iTypeCode, &lRepeat, &lWidth, &iStatus);
                          if (iResult == 0) {
                            sprintf(charTemplate, "%d", col+1);

                            if (fits_get_colname(ffits, CASEINSEN, charTemplate, charName, &iColNumber, &iStatus) == 0) {
                              if (lRepeat == 1) {
                                str = QString("%1_%2").arg(charName).arg(iHDUNumber-1);
                                fields.append(str);
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
            fits_movrel_hdu(ffits, 1, &iHDUType, &iStatus);
          }
        }
      }
    }
    iStatus = 0;
    fits_close_file(ffits, &iStatus);
  }
  return fields;
}


QStringList PlanckIDEFSource::fieldListFromFolder(const QString& filename) {
  QStringList fields;
  QDir        folder(filename, "*.fits *.fits.gz", QDir::Name | QDir::IgnoreCase, QDir::Files | QDir::Readable);
  QStringList files;
  QStringList filesBase;

  files = folder.entryList();
  if (files.size() > 0) {
    for (QStringList::ConstIterator it = files.begin(); it != files.end(); ++it) {
      if (isValidFilename(*it, 0L)) {
        QString     baseName = baseFilename(*it);
        QString     pathname = folder.path() + QDir::separator() + *it;

        QString   prefixNew;
        QString   str;
        fitsfile* ffits;
        bool      bRetVal = false;
        int       iResult = 0;
        int       iStatus = 0;

        iResult = fits_open_file(&ffits, filename.ascii(), READONLY, &iStatus);
        if (iResult == 0) {
          int iNumHeaderDataUnits;

          if (fits_get_num_hdus(ffits, &iNumHeaderDataUnits, &iStatus) == 0) {
            long lNumRows;
            int iHDUType;
            int i;

            int numFrames = getNumFrames(ffits, iNumHeaderDataUnits);
            if (numFrames > 0) {
              fits_movabs_hdu(ffits, 1, &iHDUType, &iStatus);

              // add the fields and metadata...
              char charExtName[] = "EXTNAME";
              for (i = 0; i < iNumHeaderDataUnits; i++) {
                if (iStatus == 0) {
                  // the first table never contains data...
                  if (i > 0) {
                    // create the field entries...
                    fits_get_hdu_type(ffits, &iHDUType, &iStatus);
                    if (iStatus == 0) {
                      if (iHDUType == BINARY_TBL) {
                        int iNumCols;

                        iResult = fits_get_num_cols(ffits, &iNumCols, &iStatus);
                        if (iResult == 0) {
                          iResult = fits_get_num_rows(ffits, &lNumRows, &iStatus);
                          if (iResult == 0) {
                            if (!baseName.isEmpty()) {
                              char value[FLEN_VALUE];
                              char comment[FLEN_COMMENT];

                              prefixNew.truncate(0);
                              iResult = fits_read_keyword(ffits, charExtName, value, comment, &iStatus);
                              if (iResult == 0) {
                                prefixNew = baseName + QDir::separator() + QString(value).remove(QChar('\''));
                              }

                              iResult = 0;
                              iStatus = 0;
                            }

                            if (iResult == 0) {
                              QString str;
                              char charTemplate[FLEN_CARD];
                              char charName[FLEN_CARD];
                              long lRepeat;
                              long lWidth;
                              int iHDUNumber;
                              int iTypeCode;
                              int iColNumber;
                              int iResult;
                              int table;
                              int col;

                              table = fits_get_hdu_num(ffits, &iHDUNumber);

                              for (col=0; col<iNumCols; col++) {
                                iResult = fits_get_coltype(ffits, col+1, &iTypeCode, &lRepeat, &lWidth, &iStatus);
                                if (iResult == 0) {
                                  sprintf(charTemplate, "%d", col+1);

                                  if (fits_get_colname(ffits, CASEINSEN, charTemplate, charName, &iColNumber, &iStatus) == 0) {
                                    if (lRepeat == 1) {
                                      str = QString("%1_%2").arg(charName).arg(iHDUNumber-1);
                                      fields.append(str);
                                    }
                                  }
                                }
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                  fits_movrel_hdu(ffits, 1, &iHDUType, &iStatus);
                }
              }
              bRetVal = true;
            }
          }
          iStatus = 0;
          fits_close_file(ffits, &iStatus);
        }

        if (bRetVal) {
          // first add the INDEX field...
          QString strIndex;

          strIndex = baseName + QDir::separator() + "INDEX";
          fields.append(strIndex);
        }
      }
    }
  }
  return fields;
}


QStringList PlanckIDEFSource::stringList(const QString& filename) {
  QStringList strings;
  if (!filename.isNull() && !filename.isEmpty()) {
    QFileInfo fileInfo(filename);

    if (fileInfo.isFile()) {
      strings = stringListFromFile(filename);
    }
    else if (fileInfo.isDir()) {
      strings = stringListFromFolder(filename);
    }
  }
  return strings;
}


QStringList PlanckIDEFSource::stringListFromFile(const QString& filename) {
  QStringList strings;
  fitsfile* ffits;
  int       iResult = 0;
  int       iStatus = 0;

  strings.append("FILENAME");

  iResult = fits_open_file(&ffits, filename.ascii(), READONLY, &iStatus);
  if (iResult == 0) {
    int iNumHeaderDataUnits;

    if (fits_get_num_hdus(ffits, &iNumHeaderDataUnits, &iStatus) == 0) {
      int iHDUType;
      int i;

      int numFrames = getNumFrames(ffits, iNumHeaderDataUnits);
      if (numFrames > 0) {
        fits_movabs_hdu(ffits, 1, &iHDUType, &iStatus);

        // add the fields and metadata...
        for (i=0; i<iNumHeaderDataUnits; i++) {
          if (iStatus == 0) {
            int keysexist;
            int morekeys;

            iResult = fits_get_hdrspace(ffits, &keysexist, &morekeys, &iStatus);
            if (iResult == 0) {
              QString strKey;
              char keyname[FLEN_KEYWORD];
              char value[FLEN_VALUE];
              char comment[FLEN_COMMENT];
              int keynum;
              int hdu;

              fits_get_hdu_num(ffits, &hdu);

              for (keynum=1; keynum<=keysexist; ++keynum) {
                iResult = fits_read_keyn(ffits, keynum, keyname, value, comment, &iStatus);
                if (iResult == 0) {
                  strKey.sprintf("%02d_%03d %s", hdu, keynum, keyname);
                  strings.append(strKey);
                }
              }
            }
            fits_movrel_hdu(ffits, 1, &iHDUType, &iStatus);
          }
        }
      }
    }
    iStatus = 0;
    fits_close_file(ffits, &iStatus);
  }
  return strings;
}


QStringList PlanckIDEFSource::stringListFromFolder(const QString& filename) {
  QStringList strings;
  QDir        folder(filename, "*.fits *.fits.gz", QDir::Name | QDir::IgnoreCase, QDir::Files | QDir::Readable);
  QStringList files;
  QStringList filesBase;

  files = folder.entryList();
  if (files.size() > 0) {
    for (QStringList::ConstIterator it = files.begin(); it != files.end(); ++it) {
      if (isValidFilename(*it, 0L)) {
        QString     baseName = baseFilename(*it);
        QString     pathname = folder.path() + QDir::separator() + *it;

        QString   prefixNew;
        QString   str;
        fitsfile* ffits;
        int       iResult = 0;
        int       iStatus = 0;

        iResult = fits_open_file(&ffits, filename.ascii(), READONLY, &iStatus);
        if (iResult == 0) {
          int iNumHeaderDataUnits;

          if (fits_get_num_hdus(ffits, &iNumHeaderDataUnits, &iStatus) == 0) {
            int iHDUType;
            int i;

            int numFrames = getNumFrames(ffits, iNumHeaderDataUnits);
            if (numFrames > 0) {
              fits_movabs_hdu(ffits, 1, &iHDUType, &iStatus);

              // add the fields and metadata...
              for (i = 0; i < iNumHeaderDataUnits; i++) {
                if (iStatus == 0) {
                  int keysexist;
                  int morekeys;

                  iResult = fits_get_hdrspace(ffits, &keysexist, &morekeys, &iStatus);
                  if (iResult == 0) {
                    QString strKey;
                    char keyname[FLEN_KEYWORD];
                    char value[FLEN_VALUE];
                    char comment[FLEN_COMMENT];
                    int keynum;
                    int hdu;

                    fits_get_hdu_num(ffits, &hdu);

                    for (keynum=1; keynum<=keysexist; ++keynum) {
                      iResult = fits_read_keyn(ffits, keynum, keyname, value, comment, &iStatus);
                      if (iResult == 0) {
                        strKey.sprintf("%02d_%03d %s", hdu, keynum, keyname);
                        strings.append(strKey);
                      }
                    }
                  }
                  fits_movrel_hdu(ffits, 1, &iHDUType, &iStatus);
                }
              }
            }
          }
          iStatus = 0;
          fits_close_file(ffits, &iStatus);
        }
      }
    }
  }
  return strings;
}


long PlanckIDEFSource::getNumFrames(fitsfile* ffits, int iNumHeaderDataUnits) {
  long lNumRows = 0;

  if (iNumHeaderDataUnits > 1) {
    int iHDUType;
    int iResult = 0;
    int iStatus = 0;

    if (fits_movabs_hdu(ffits, 2, &iHDUType, &iStatus) == 0) {
      if (fits_get_hdu_type(ffits, &iHDUType, &iStatus) == 0) {
        if (iHDUType == BINARY_TBL) {
          iResult = fits_get_num_rows(ffits, &lNumRows, &iStatus);
        }
      }
    }
  }
  return lNumRows;
}


long PlanckIDEFSource::getNumFrames(const QString& filename) {
  fitsfile* ffits;
  int numFrames = 0;
  int iResult = 0;
  int iStatus = 0;

  iResult = fits_open_file(&ffits, filename.ascii(), READONLY, &iStatus);
  if (iResult == 0) {
    int iNumHeaderDataUnits;

    if (fits_get_num_hdus(ffits, &iNumHeaderDataUnits, &iStatus) == 0) {
      numFrames = getNumFrames(ffits, iNumHeaderDataUnits);
    }

    iStatus = 0;
    fits_close_file(ffits, &iStatus);
  }
  return numFrames;
}



void PlanckIDEFSource::addToMetadata(fitsfile *ffits, int &iStatus) {
  int iResult;
  int keysexist;
  int morekeys;

  iResult = fits_get_hdrspace(ffits, &keysexist, &morekeys, &iStatus);
  if (iResult == 0) {
    QString strKey;
    char keyname[FLEN_KEYWORD];
    char value[FLEN_VALUE];
    char comment[FLEN_COMMENT];
    int keynum;
    int hdu;

    fits_get_hdu_num(ffits, &hdu);

    for (keynum=1; keynum<=keysexist; ++keynum) {
      iResult = fits_read_keyn(ffits, keynum, keyname, value, comment, &iStatus);
      if (iResult == 0) {
        strKey.sprintf("%02d_%03d %s", hdu, keynum, keyname);

        QString str;
        if (strlen(comment) > 0) {
          if (strlen(value) > 0) {
            str.sprintf("%s / %s", value, comment);
          } else {
            str.sprintf("%s", comment);
          }
        } else if (strlen(value) > 0) {
          str.sprintf("%s", value);
        }

        _metaData.insert(keyname, str);
      }
    }
  }
}


void PlanckIDEFSource::addToFieldList(fitsfile *ffits, const int iNumCols, int &iStatus) {
  QString str;
  char charTemplate[FLEN_CARD];
  char charName[FLEN_CARD];
  long lRepeat;
  long lWidth;
  int iHDUNumber;
  int iTypeCode;
  int iColNumber;
  int iResult;
  int table;
  int col;

  table = fits_get_hdu_num(ffits, &iHDUNumber);

  for (col=0; col<iNumCols; col++) {
    iResult = fits_get_coltype(ffits, col+1, &iTypeCode, &lRepeat, &lWidth, &iStatus);
    if (iResult == 0) {
      sprintf(charTemplate, "%d", col+1);

      if (fits_get_colname(ffits, CASEINSEN, charTemplate, charName, &iColNumber, &iStatus) == 0) {
        if (lRepeat == 1) {
          field *fld = new field;

          str = QString("%1_%2").arg(charName).arg(iHDUNumber-1);

          fld->basefile = QString("");
          fld->table = table;
          fld->column = iColNumber;

          _fields.insert(str, fld);
          _fieldList.append(str);
        }
      }
    }
  }
}


void PlanckIDEFSource::addToFieldList(fitsfile *ffits, const QString& prefix, const QString& baseName, const int iNumCols, int &iStatus) {
  QString str;
  char charTemplate[FLEN_CARD];
  char charName[FLEN_CARD];
  long lRepeat;
  long lWidth;
  int iHDUNumber;
  int iTypeCode;
  int iColNumber;
  int iResult;
  int table;
  int col;

  table = fits_get_hdu_num(ffits, &iHDUNumber);

  for (col=0; col<iNumCols; col++) {
    iResult = fits_get_coltype(ffits, col+1, &iTypeCode, &lRepeat, &lWidth, &iStatus);
    if (iResult == 0) {
      sprintf(charTemplate, "%d", col+1);

      if (fits_get_colname(ffits, CASEINSEN, charTemplate, charName, &iColNumber, &iStatus) == 0) {
        if (lRepeat == 1) {
          field *fld = new field;

          if (prefix.isEmpty()) {
            str = QString("%1_%2").arg(charName).arg(iHDUNumber-1);
          } else {
            str = QString("%1/%2").arg(prefix).arg(charName);
            if (*_fields.find(str) != 0) {
              str = QString("%1/%2_%3").arg(prefix).arg(charName).arg(iHDUNumber-1);
            }
          }

          fld->basefile = baseName;
          fld->table = table;
          fld->column = iColNumber;

          _fields.insert(str, fld);
          _fieldList.append(str);
        }
      }
    }
  }
}


Kst::Object::UpdateType PlanckIDEFSource::update() {
  // updates not supported yet
  // we should check to see if the FITS file has changed on disk
  return Kst::Object::NO_CHANGE;
}


int PlanckIDEFSource::readField(double *v, const QString& fieldName, int s, int n) {
  int       i;
  int       iRead = -1;

  if (fieldName == "INDEX") {
    for (i = 0; i < n; ++i) {
      v[i] = (double)(s + i);
    }

    iRead =  n;
  } else {
    field *fld = 0L;

    fld = *_fields.find(fieldName);
    if (fld != 0L) {
      if (fieldName == fld->basefile + QDir::separator() + QString("INDEX")) {
        for (i = 0; i < n; ++i) {
          v[i] = (double)(s + i);
        }
        iRead =  n;
      } else {
        _valid = false;

        if (!_filename.isNull() && !_filename.isEmpty()) {
          if (_isSingleFile) {
            iRead = readFileFrames(_filename, fld, v, s, n);
          } else {
            iRead = readFolderFrames(fld, v, s, n);
          }
        }
      }
    }
  }
  return iRead;
}


int PlanckIDEFSource::readFileFrames(const QString& filename, field *fld, double *v, int s, int n) {
  double    dNan = strtod("nan", NULL);
  fitsfile* ffits;
  int       iRead = -1;
  int       iStatus = 0;
  int       iAnyNull;
  int       iResult = 0;

  iResult = fits_open_file(&ffits, filename.ascii(), READONLY, &iStatus);
  if (iResult == 0) {
    int iHDUType;

    if (fits_movabs_hdu(ffits, fld->table, &iHDUType, &iStatus) == 0) {
      if (iHDUType == BINARY_TBL) {
        _valid = true;

        if (n < 0) {
          iResult = fits_read_col(ffits, TDOUBLE, fld->column, s+1, 1, 1, &dNan, v, &iAnyNull, &iStatus);
          if (iResult == 0) {
            iRead = 1;
          }
        } else {
          iResult = fits_read_col(ffits, TDOUBLE, fld->column, s+1, 1, n, &dNan, v, &iAnyNull, &iStatus);
          if (iResult == 0) {
            iRead = n;
          }
        }
        iStatus = 0;
      }
    }
    fits_close_file(ffits, &iStatus);
  }
  return iRead;
}


int PlanckIDEFSource::readFolderFrames(field *fld, double *v, int s, int n) {
  int iRead = -1;

  if (!fld->basefile.isEmpty()) {
    fileList*   folderFields;
    double*     vNew = v;
    int         sNew = s;
    int         nNew = n;
    int         iReadSub = 0;

    folderFields = *_basefiles.find(fld->basefile);
    if (folderFields != 0L) {
      for (fileList::ConstIterator it = folderFields->begin(); it != folderFields->end(); ++it) {
        // check if we need to read any frames from the current file...
        if (n < 0 && (*it).frameLo + (*it).frames > s) {
          sNew = s - (*it).frameLo;
          if (sNew < 0) {
            sNew = 0;
          }

          nNew = -1;
          vNew = v + (*it).frameLo + sNew;

          iReadSub = readFileFrames((*it).file, fld, vNew, sNew, nNew);
          if (iReadSub > 0) {
            if (iRead < 0) {
              iRead = iReadSub;
            } else {
              iRead += iReadSub;
            }
          }
        } else if ((*it).frameLo < s + n && (*it).frameLo + (*it).frames > s) {
          sNew = s - (*it).frameLo;
          if (sNew < 0) {
            sNew = 0;
          }

          nNew = n;
          if (sNew + nNew > (*it).frames) {
            nNew =  (*it).frames - sNew;
          }

          vNew = v + (*it).frameLo + sNew;

          if (nNew > 0) {
            iReadSub = readFileFrames((*it).file, fld, vNew, sNew, nNew);
            if (iReadSub > 0) {
              if (iRead < 0) {
                iRead = iReadSub;
              } else {
                iRead += iReadSub;
              }
            }
          }
        }
      }
    }
  }
  return iRead;
}


bool PlanckIDEFSource::isValidField(const QString& field) const {
  bool bRetVal = false;

  if (field == "INDEX") {
    bRetVal = true;
  } else {
    if (*_fields.find(field) != 0L) {
      bRetVal = true;
    }
  }
  return bRetVal;
}


int PlanckIDEFSource::samplesPerFrame(const QString &field) {
  Q_UNUSED(field)
  return 1;
}


int PlanckIDEFSource::frameCount(const QString& fieldName) const {
  int rc = 0;

  if (_isSingleFile) {
    rc = _numFrames;
  } else {
    field* fld;

    if (!fieldName.isEmpty()) {
      fld = *_fields.find(fieldName);
      if (fld != 0L) {
        fileList* folderFields;

        folderFields = *_basefiles.find(fld->basefile);
        if (folderFields != 0L) {
          for (fileList::ConstIterator it = folderFields->begin(); it != folderFields->end(); ++it) {
            rc += (*it).frames;
          }
        }
      }
    }
  }
  return rc;
}


bool PlanckIDEFSource::isEmpty() const {
  return _fields.isEmpty();
}


QString PlanckIDEFSource::fileType() const {
  return PlanckIDEFTypeString;
}


void PlanckIDEFSource::save(QXmlStreamWriter &streamWriter) {
  Kst::DataSource::save(streamWriter);
}


void PlanckIDEFSource::parseProperties(QXmlStreamAttributes &properties) {
  _config->parseProperties(properties);
}


int PlanckIDEFSource::readScalar(double &S, const QString& scalar) {
  if (scalar == "FRAMES") {
    S = 1;
    return 1;
  }
  return 0;
}


int PlanckIDEFSource::readString(QString &S, const QString& string) {
  if (string == "FILE") {
    S = _filename;
    return 1;
  } else if (_metaData.contains(string)) {
    S = _metaData[string];
    return 1;
  }
  return 0;
}



bool PlanckIDEFSource::checkValidPlanckIDEFFolder(const QString& filename) {
  QDir folder(filename, "*.fits *.fits.gz", QDir::Name | QDir::IgnoreCase, QDir::Files | QDir::Readable);
  QStringList files;
  QString pathname;
  bool ok = false;

  files = folder.entryList();
  if (files.size() > 0) {
    for (QStringList::ConstIterator it = files.begin(); it != files.end(); ++it) {
      pathname = folder.path() + QDir::separator() + *it;

      if (checkValidPlanckIDEFFile(pathname, 0L)) {
        ok = true;
        break;
      }
    }
  }

  return ok;
}


bool PlanckIDEFSource::checkValidPlanckIDEFFile(const QString& filename, Config *cfg) {
  bool ok = false;
  fitsfile* ffits;
  int iStatus = 0;

  // determine if it is a Planck IDIS DMC Exchange Format file...
  if (isValidFilename(filename, cfg)) {
    if (fits_open_file(&ffits, filename.ascii(), READONLY, &iStatus) == 0) {
      int iNumHeaderDataUnits;

      if (fits_get_num_hdus(ffits, &iNumHeaderDataUnits, &iStatus) == 0) {
        char  value[FLEN_VALUE];
        char  comment[FLEN_COMMENT];
        int   iHDUType;
        int   iValue;
        int   i;

        char charSimple[] = "SIMPLE";
        char charExtend[] = "EXTEND";
        char charNAxis[] = "NAXIS";

        // the primary header should never have any data...
        if (fits_get_hdu_type(ffits, &iHDUType, &iStatus) == 0) {
          if (iHDUType == IMAGE_HDU) {
            if (fits_read_key(ffits, TLOGICAL, charSimple, &iValue, comment, &iStatus) == 0) {
              if (iValue != 0) {
                if (fits_read_key(ffits, TLOGICAL, charExtend, &iValue, comment, &iStatus) == 0) {
                  if (iValue != 0) {
                    if (fits_read_key(ffits, TINT, charNAxis, &iValue, comment, &iStatus) == 0) {
                      if (iValue == 0) {
                        ok = true;
                      }
                    }
                  }
                }
              }
            }
          }
        }

        // the name of each binary table must conform to aaa-bbbb[-ccc]
        //  where bbbb is a member of {OBTT, TOD., OBTH, HKP.}...
        if (ok && iStatus == 0) {
          if (iNumHeaderDataUnits > 1) {
            long rowsCompare = 0;
            long rows;
            int cols;

            char charExtName[] = "EXTNAME";
            for (i=0; i<iNumHeaderDataUnits-1 && ok; i++) {
              if (fits_movrel_hdu(ffits, 1, &iHDUType, &iStatus) == 0) {
                bool bOBTHeader = false;
                ok = false;

                if (iStatus == 0 && iHDUType == BINARY_TBL) {
                  if (fits_read_keyword(ffits, charExtName, value, comment, &iStatus) == 0) {
                    QString section = value;

                    ok = true;

                    section.stripWhiteSpace();
                    if (section.compare("OBT") == 0) {
                      bOBTHeader = true;
                    }
                  }

                  //
                  // all tables should have the same number of rows...
                  //
                  if (ok) {
                    bool okCols = false;

                    if (fits_get_num_cols(ffits, &cols, &iStatus) == 0) {
                      if (cols > 0) {
                        okCols = true;
                      }
                    }

                    if (okCols) {
                      if (fits_get_num_rows(ffits, &rows, &iStatus) == 0) {
                        if (i == 0) {
                          rowsCompare = rows;
                        } else if (rowsCompare == rows) {
                          ok = true;
                        } else {
                          ok = false;
                        }
                      } else {
                        ok = false;
                      }
                    }
                  }
                }
              } else {
                ok = false;
              }
            }
          } else {
            ok = false;
          }
        }
      }

      if (iStatus != 0)  {
        ok = false;
      }

      iStatus = 0;

      fits_close_file(ffits, &iStatus);
    }
  }

  return ok;
}




class ConfigWidgetPlanckIDEFInternal : public QWidget, public Ui_PlanckIDEFConfig {
  public:
    ConfigWidgetPlanckIDEFInternal(QWidget *parent) : QWidget(parent), Ui_PlanckIDEFConfig() { setupUi(this); }
};


class ConfigWidgetPlanckIDEF : public Kst::DataSourceConfigWidget {
  public:
    ConfigWidgetPlanckIDEF() : Kst::DataSourceConfigWidget() {
      QGridLayout *layout = new QGridLayout(this);
      _hc = new ConfigWidgetPlanckIDEFInternal(this);
      layout->addWidget(_hc, 0, 0);
      layout->activate();
    }

    ~ConfigWidgetPlanckIDEF() {}

    void setConfig(QSettings *cfg) {
      Kst::DataSourceConfigWidget::setConfig(cfg);
    }

    void load() {
      _cfg->beginGroup("PlanckIDEF General");
      _hc->_checkFilename->setChecked(_cfg->readBoolEntry("Check Filename", true));
    }

    void save() {
      _cfg->beginGroup("PlanckIDEF General");
      _cfg->writeEntry("Check Filename", (int)_hc->_checkFilename->isChecked());
    }

    ConfigWidgetPlanckIDEFInternal *_hc;
};


QString PlanckIDEFPlugin::pluginName() const { return "PlanckIDEF Source Reader"; }
QString PlanckIDEFPlugin::pluginDescription() const { return "PlanckIDEF Source Reader"; }


Kst::DataSource *PlanckIDEFPlugin::create(Kst::ObjectStore *store,
                                            QSettings *cfg,
                                            const QString &filename,
                                            const QString &type,
                                            const QDomElement &element) const {

  return new PlanckIDEFSource(store, cfg, filename, type, element);
}


QStringList PlanckIDEFPlugin::matrixList(QSettings *cfg,
                                             const QString& filename,
                                             const QString& type,
                                             QString *typeSuggestion,
                                             bool *complete) const {


  if (typeSuggestion) {
    *typeSuggestion = PlanckIDEFTypeString;
  }
  if ((!type.isEmpty() && !provides().contains(type)) ||
      0 == understands(cfg, filename)) {
    if (complete) {
      *complete = false;
    }
    return QStringList();
  }
  return QStringList();
}


QStringList PlanckIDEFPlugin::scalarList(QSettings *cfg,
                                            const QString& filename,
                                            const QString& type,
                                            QString *typeSuggestion,
                                            bool *complete) const {

  QStringList scalarList;

  if ((!type.isEmpty() && !provides().contains(type)) || 0 == understands(cfg, filename)) {
    if (complete) {
      *complete = false;
    }
    return QStringList();
  }

  if (typeSuggestion) {
    *typeSuggestion = PlanckIDEFTypeString;
  }

  scalarList.append("FRAMES");
  return scalarList;

}


QStringList PlanckIDEFPlugin::stringList(QSettings *cfg,
                                      const QString& filename,
                                      const QString& type,
                                      QString *typeSuggestion,
                                      bool *complete) const {

  QStringList stringList;

  if ((!type.isEmpty() && !provides().contains(type)) || 0 == understands(cfg, filename)) {
    if (complete) {
      *complete = false;
    }
    return QStringList();
  }

  if (typeSuggestion) {
    *typeSuggestion = PlanckIDEFTypeString;
  }

  stringList = PlanckIDEFSource::stringList(filename);

  return stringList;
}

QStringList PlanckIDEFPlugin::fieldList(QSettings *cfg,
                                            const QString& filename,
                                            const QString& type,
                                            QString *typeSuggestion,
                                            bool *complete) const {
  Q_UNUSED(type)
  QStringList fieldList;

  if (complete) {
    *complete = true;
  }

  if (typeSuggestion) {
    *typeSuggestion = PlanckIDEFTypeString;
  }
  if (understands(cfg, filename)) {
    fieldList = PlanckIDEFSource::fieldList(filename);
  }
  return fieldList;
}


int PlanckIDEFPlugin::understands(QSettings *cfg, const QString& filename) const {
  PlanckIDEFSource::Config config;
  QFileInfo fileinfo(filename);
  int       iRetVal = 0;

  config.read(cfg, filename);

  if (fileinfo.isFile()) {
    if (PlanckIDEFSource::checkValidPlanckIDEFFile(filename, &config)) {
      iRetVal = 99;
    }
  } else if (fileinfo.isDir()) {
    if (PlanckIDEFSource::checkValidPlanckIDEFFolder(filename)) {
      iRetVal = 99;
    }
  }

  return iRetVal;
}


bool PlanckIDEFPlugin::supportsTime(QSettings *cfg, const QString& filename) const {
  //FIXME
  Q_UNUSED(cfg)
  Q_UNUSED(filename)
  return true;
}


QStringList PlanckIDEFPlugin::provides() const {
  QStringList rc;
  rc += "PlanckIDEF Source";
  return rc;
}


Kst::DataSourceConfigWidget *PlanckIDEFPlugin::configWidget(QSettings *cfg, const QString& filename) const {
  Q_UNUSED(filename)
  ConfigWidgetPlanckIDEF *config = new ConfigWidgetPlanckIDEF;
  config->setConfig(cfg);
  return config;
}

Q_EXPORT_PLUGIN2(kstdata_qimagesource, PlanckIDEFPlugin)


// vim: ts=2 sw=2 et
