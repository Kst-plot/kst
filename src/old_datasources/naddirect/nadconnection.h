/***************************************************************************
                  nadconnection.h  -  NAD connection
                             -------------------
    begin                : Wed Mar 22 2006
    copyright            : (C) 2006 Staikos Computing Services Inc.
    email                : info@staikos.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef NAD_H
#define NAD_H

#include <qobject.h>
#include <qptrlist.h>
#include <qsize.h>
#include <qstring.h>
#include <qstringlist.h>
#include <kurl.h>

namespace NAD {
  
struct NADFieldsResponseField {
  QString name;
  unsigned int samplesPerFrame;
  unsigned int startFrame, stopFrame;
  char type;

  int operator==(const NADFieldsResponseField &x) {
    return (x.name == name && x.samplesPerFrame == samplesPerFrame && x.startFrame == startFrame && x.stopFrame == stopFrame);
  }

  int operator!=(const NADFieldsResponseField &x) {
    return !operator==(x);
  }
};

struct NADFieldsResponse {
  QString datasetName;
  QMap<QString, NADFieldsResponseField *> fields;
};

struct NADDataResponseField {
  QString name;
  unsigned int samplesPerFrame;
  unsigned int startFrame, stopFrame, skip;
  char type;
  unsigned char size;
};

struct NADDataResponse {
  QString datasetName;
  unsigned int chunkSize;
  QDataStream::ByteOrder endian;
  QMap<QString, NADDataResponseField *> fields;
};


class NADConnection : public QObject {
  Q_OBJECT

  public:
    NADConnection(QObject *parent = NULL);
    virtual ~NADConnection();

    void setUrl(const QString &NADUrlString);

    QStringList getFields() const;

    bool updated();

    bool isValid() const;

    QSize range(const QString &field) const;
    unsigned int samplesPerFrame(const QString &field) const;
    int getData(const QString &field, double *buf, long startFrame, long stopFrame, long skip=0);

  private:
    bool NADConnection::updateLastFieldsResponse();

    KURL _url;
    QString _datasetName;
    struct NADFieldsResponse * _lastFieldsResponse;
    bool _firstTime;
};

}

#endif
// vim: ts=2 sw=2 et
