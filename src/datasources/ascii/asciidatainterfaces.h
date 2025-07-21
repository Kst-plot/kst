/***************************************************************************
 *                                                                         *
 *   Copyright : (C) 2003 The University of Toronto                        *
 *   email     : netterfield@astro.utoronto.ca                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ASCIIDATAINTERFACES_H
#define ASCIIDATAINTERFACES_H

#include "datasource.h"
#include "dataplugin.h"
#include "asciisource.h"

// #include "curve.h"
// #include "colorsequence.h"
// #include "objectstore.h"


using namespace Kst;


//
// Vector interface
//


//-------------------------------------------------------------------------------------------
class DataInterfaceAsciiVector : public DataSource::DataInterface<DataVector>
{
public:
  explicit DataInterfaceAsciiVector(AsciiSource& a) : ascii(a) {}

  void prepareRead(int);
  void readingDone();
  // read one element
  int read(const QString&, DataVector::ReadInfo&);

  // named elements
  QStringList list() const { return ascii._fieldList; }
  bool isListComplete() const { return ascii._fieldListComplete; }
  bool isValid(const QString& field) const { return ascii._fieldLookup.contains( field ); }

  // T specific
  const DataVector::DataInfo dataInfo(const QString&, int frame = 0) const;
  void setDataInfo(const QString&, const DataVector::DataInfo&) {}

  // meta data
  QMap<QString, double> metaScalars(const QString&);
  QMap<QString, QString> metaStrings(const QString&);


   AsciiSource& ascii;
};


//-------------------------------------------------------------------------------------------
const DataVector::DataInfo DataInterfaceAsciiVector::dataInfo(const QString &field, int frame) const
{
  Q_UNUSED(frame)
  if (!ascii._fieldLookup.contains(field))
    return DataVector::DataInfo();

  return DataVector::DataInfo(ascii._reader.numberOfFrames(), 1);
}

//-------------------------------------------------------------------------------------------
void DataInterfaceAsciiVector::prepareRead(int number_of_read_calls)
{
    ascii.prepareRead(number_of_read_calls);
}

//-------------------------------------------------------------------------------------------
void DataInterfaceAsciiVector::readingDone()
{
    ascii.readingDone();
}

//-------------------------------------------------------------------------------------------
int DataInterfaceAsciiVector::read(const QString& field, DataVector::ReadInfo& p)
{
  return ascii.readField(p.data, field, p.startingFrame, p.numberOfFrames);
}


//-------------------------------------------------------------------------------------------
// TODO FRAMES only in vector?
QMap<QString, double> DataInterfaceAsciiVector::metaScalars(const QString&)
{
  QMap<QString, double> m;
  m["FRAMES"] = ascii._reader.numberOfFrames();
  return m;
}

QMap<QString, QString> DataInterfaceAsciiVector::metaStrings(const QString& field)
{
  QMap<QString, QString> m;
  if (ascii._fieldUnits.contains(field)) {
    m["units"] = ascii._fieldUnits[field];
  }
  return m;
}



//
// String interface
//

class DataInterfaceAsciiString : public DataSource::DataInterface<DataString>
{
public:
  explicit DataInterfaceAsciiString(AsciiSource& s) : ascii(s) {}

  // read one element
  int read(const QString&, DataString::ReadInfo&);

  // named elements
  QStringList list() const { return ascii._strings.keys(); }
  bool isListComplete() const { return true; }
  bool isValid(const QString&) const;

  // T specific
  const DataString::DataInfo dataInfo(const QString&, int frame=0) const { Q_UNUSED(frame) return DataString::DataInfo(); }
  void setDataInfo(const QString&, const DataString::DataInfo&) {}

  // meta data
  QMap<QString, double> metaScalars(const QString&) { return QMap<QString, double>(); }
  QMap<QString, QString> metaStrings(const QString&) { return QMap<QString, QString>(); }


private:
  AsciiSource& ascii;
};


//-------------------------------------------------------------------------------------------
int DataInterfaceAsciiString::read(const QString& string, DataString::ReadInfo& p)
{
  if (isValid(string) && p.value) {
    *p.value = ascii._strings[string];
    return 1;
  }
  return 0;
}


//-------------------------------------------------------------------------------------------
bool DataInterfaceAsciiString::isValid(const QString& string) const
{
  return  ascii._strings.contains( string );
}

#endif
