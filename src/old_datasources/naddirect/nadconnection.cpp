/***************************************************************************
                  nadconnection.cpp  -  NAD connection
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

#include <qbuffer.h>
#include <qdatastream.h>
#include <qstring.h>
#include <qthread.h>
#include <qxml.h>

#include <ksdebug.h>
#include <kurl.h>
#include <kio/job.h>
#include <kio/netaccess.h>

#include "nadconnection.h"

#include <curl/curl.h>

using namespace NAD;

class FieldsResponseHandler : public QXmlDefaultHandler {
  public:
    FieldsResponseHandler() : QXmlDefaultHandler() {
      response = 0L;
    }


    ~FieldsResponseHandler() {
      delete response;
      response = 0L;
    }


    bool startDocument() {
      delete response;
      response = new NADFieldsResponse;
      return true;
    }


    bool startElement(const QString & namespaceURI, const QString & localName, const QString & qName, const QXmlAttributes & atts) {
      Q_UNUSED(namespaceURI)
      Q_UNUSED(localName)
//      kstdDebug() << "startElement: namespaceURL=" << namespaceURI << ",localName=" << localName << ",qName=" << qName << endl;
      if (qName == "GetFieldsResponse") {
      } else if (qName == "DataSet") {
        int index;
        if ((index = atts.index("name")) != -1) {
          response->datasetName = atts.value(index);
        }
      } else if (qName == "Fields") {
      } else if (qName == "Field") {
        NADFieldsResponseField *rf = new NADFieldsResponseField;
        int index;
        if ((index = atts.index("name")) != -1) {
          rf->name = atts.value(index);
        } else {
          delete rf;
          return true;
        }
        if ((index = atts.index("spf")) != -1) {
          rf->samplesPerFrame = atts.value(index).toInt();
        } else {
          rf->samplesPerFrame = 1;
        }
        if ((index = atts.index("start")) != -1) {
          rf->startFrame = atts.value(index).toInt();
//          kstdDebug() << "setting start for " << rf->name << " to " << rf->startFrame << endl;
        } else {
          rf->startFrame = 0;
        }
        if ((index = atts.index("stop")) != -1) {
          rf->stopFrame = atts.value(index).toInt();
//          kstdDebug() << "setting stop for " << rf->name << " to " << rf->stopFrame << endl;
        } else {
          rf->stopFrame = 0;
        }
        if ((index = atts.index("type")) != -1) {
          QString s = atts.value(index);
          rf->type = s.ascii()[0];
        } else {
          rf->type = 'd';
        }

        response->fields.insert(rf->name, rf);
      }
      return true;
    }

    bool endElement(const QString & namespaceURI, const QString & localName, const QString & qName) {
      Q_UNUSED(namespaceURI)
      Q_UNUSED(localName)
//      kstdDebug() << "endElement: namespaceURL=" << namespaceURI << ",localName=" << localName << ",qName=" << qName << endl;
      if (qName == "GetFieldsResponse") {
      } else if (qName == "DataSet") {
      } else if (qName == "Fields") {
      } else if (qName == "Field") {
      }
      return true;
    }

    NADFieldsResponse *getResponse() {
      return response;
    }

  private:
    NADFieldsResponse *response;
};


class DataResponseHandler : public QXmlDefaultHandler {
	public:
    DataResponseHandler() : QXmlDefaultHandler() {
      response = 0L;
    }

    ~DataResponseHandler() {
      delete response;
      response = 0L;
    }
    
    bool startDocument() {
      delete response;
      response = new NADDataResponse;
      return true;
    }

    bool startElement(const QString & namespaceURI, const QString & localName, const QString & qName, const QXmlAttributes & atts) {
      Q_UNUSED(namespaceURI)
      Q_UNUSED(localName)
//      kstdDebug() << "startElement: namespaceURL=" << namespaceURI << ",localName=" << localName << ",qName=" << qName << endl;
      buf = "";
      if (qName == "GetDataResponse") {
      } else if (qName == "DataSet") {
        int index;
        if ((index = atts.index("name")) != -1) {
          response->datasetName = atts.value(index);
        }
      } else if (qName == "ChunkSize") {
      } else if (qName == "Endian") {
      } else if (qName == "ResponseFrame") {
      } else if (qName == "Field") {
        NADDataResponseField *rf = new NADDataResponseField;
        int index;
        if ((index = atts.index("name")) != -1) {
          rf->name = atts.value(index);
        } else {
          delete rf;
          return true;
        }
        if ((index = atts.index("spf")) != -1) {
          rf->samplesPerFrame = atts.value(index).toInt();
        } else {
          rf->samplesPerFrame = 1;
        }
        if ((index = atts.index("start")) != -1) {
          rf->startFrame = atts.value(index).toInt();
        } else {
          rf->startFrame = 0;
        }
        if ((index = atts.index("stop")) != -1) {
          rf->stopFrame = atts.value(index).toInt();
        } else {
          rf->stopFrame = UINT_MAX;
        }
        if ((index = atts.index("skip")) != -1) {
          rf->skip = atts.value(index).toInt();
        } else {
          rf->skip = 0;
        }
        if ((index = atts.index("type")) != -1) {
          QString s = atts.value(index);
          rf->type = s.ascii()[0];
        } else {
          rf->type = 'd';
        }
        switch(rf->type) {
          case 'c':
            rf->size = 1;
            break;
          case 's': case 'u':
            rf->size = 2;
            break;
          case 'S': case 'U': case 'f': case 'i':
            rf->size = 4;
            break;
          case 'd':
            rf->size = 8;
            break;
        }

        response->fields.insert(rf->name, rf);
      }
      return true;
    }

    bool endElement(const QString & namespaceURI, const QString & localName, const QString & qName) {
      Q_UNUSED(namespaceURI)
      Q_UNUSED(localName)
//      kstdDebug() << "endElement: namespaceURL=" << namespaceURI << ",localName=" << localName << ",qName=" << qName << endl;
      if (qName == "GetDataResponse") {
      } else if (qName == "DataSet") {
      } else if (qName == "ChunkSize") {
        response->chunkSize = buf.toInt();
      } else if (qName == "Endian") {
        if (buf.lower() == "bigendian") {
          response->endian = QDataStream::BigEndian;
        } else {
          response->endian = QDataStream::LittleEndian;
        }
      } else if (qName == "ResponseFrame") {
      } else if (qName == "Field") {
      }
      return true;
    }

    bool characters(const QString & ch) {
//      kstdDebug() << "characters:" << ch << endl;
      buf += ch;
      return true;
    }

    NADDataResponse *getResponse() {
      return response;
    }

  private:
    NADDataResponse *response;
    QString buf;
};


static int curlWriter(char *data, size_t size, size_t nmemb, QDataStream *ds) {
  int result = 0;

  if (ds) {
    // Append the data to the buffer
    ds->writeRawBytes(data, size*nmemb);

    // How much did we write?
    result = size * nmemb;
  }

  return result;
}


NADConnection::NADConnection(QObject *parent) : QObject(parent) {
  _lastFieldsResponse = 0L;
  _firstTime = true;
}


NADConnection::~NADConnection() {
  delete _lastFieldsResponse;
}


// NADUrlString is currently nad://user:pw@host:port/dsName
void NADConnection::setUrl(const QString &NADUrlString) {
  KURL NADUrl = KURL::fromPathOrURL(NADUrlString);

  _datasetName = NADUrl.path();

  KURL url;
  url.setUser(NADUrl.user());
  url.setPass(NADUrl.pass());
  url.setHost(NADUrl.host());
  url.setPort(NADUrl.port());
  url.setProtocol("http");
  url.setPath("/nad");

  _url = url;

  _firstTime = true;
  updateLastFieldsResponse();
}


// return true if updated, false if not
bool NADConnection::updateLastFieldsResponse() {
  QString postData = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n<GetFields><DataSet><DataSetName>" + _datasetName + "</DataSetName></DataSet></GetFields>\n";
  QByteArray data;
  data.duplicate(postData.local8Bit(), postData.length());

//  kstdDebug() << "xml Request:" << postData << endl;

//  kstdDebug() << "updateLastFieldsResponse(): called from thread=" << (int)QThread::currentThread() << endl;
  CURL *curlHandle = curl_easy_init();
  QString urlQString = _url.url();
  const char *urlStr = urlQString.latin1();
  curl_easy_setopt(curlHandle, CURLOPT_URL, urlStr);

  struct curl_slist *headers=NULL;
  headers = curl_slist_append(headers, "Content-Type: text/xml");

  QByteArray response;
  QBuffer b(response);
  b.open(IO_WriteOnly);
  QDataStream respTempStream(&b);

  curl_easy_setopt(curlHandle, CURLOPT_POSTFIELDS, data.data());

  /* set the size of the postfields data */
  curl_easy_setopt(curlHandle, CURLOPT_POSTFIELDSIZE, postData.length());

  /* pass our list of custom made headers */
  curl_easy_setopt(curlHandle, CURLOPT_HTTPHEADER, headers);

  curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, curlWriter);
  curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, &respTempStream);

  curl_easy_perform(curlHandle); /* post away! */

  curl_slist_free_all(headers); /* free the header list */

//  kstdDebug() << "done HTTP POST for getFields" << endl;

  QXmlInputSource source;
  QXmlSimpleReader xmlReader;
  FieldsResponseHandler *handler = new FieldsResponseHandler();

  source.setData(response);
  xmlReader.setContentHandler(handler);
  bool parseok = xmlReader.parse(source);

  if (!parseok) {
    return false;
  }

  NADFieldsResponse *resp = handler->getResponse();

  bool isNew = true, done = false;
  if (_lastFieldsResponse) {
    QMap<QString, NADFieldsResponseField *> oldFields = _lastFieldsResponse->fields;
    QMap<QString, NADFieldsResponseField *> newFields = resp->fields;

    QValueList<QString> newFieldNames = newFields.keys();
    QValueList<QString> oldFieldNames = oldFields.keys();
    if (newFieldNames != oldFieldNames) { // check for new/deleted fields
//      kstdDebug() << "NAD:updateLastFieldsResponse() new/deleted field found" << endl;
      done = true;
    }

    if (!done) {
      for (QValueList<QString>::ConstIterator i = newFieldNames.begin(); !done && i != newFieldNames.end(); ++i) {
        NADFieldsResponseField *oldF = oldFields[*i];
        NADFieldsResponseField *newF = newFields[*i];
        if (*newF != *oldF) { // check for changed fields
//          kstdDebug() << "NAD:updateLastFieldsResponse() changed field found:" << oldFields[*i]->name << endl;
          done = true;
        }
      }
    }

    if (!done) {
//      kstdDebug() << "NAD:updateLastFieldsResponse() no update found" << endl;
      isNew = false;
    }

    delete _lastFieldsResponse;
  }
  _lastFieldsResponse = resp;

  return isNew;
}


QStringList NADConnection::getFields() const {
  if (!isValid()) {
    return QStringList();
  }

  if (_lastFieldsResponse) {
    QStringList rc;

    QValueList<QString> fieldNames = _lastFieldsResponse->fields.keys();
    for (QValueList<QString>::const_iterator i = fieldNames.begin(); i != fieldNames.end(); ++i) {
      rc += (*i);
    }
    return rc;
  } else {
    return QStringList();
  }
}


bool NADConnection::isValid() const {
  return !_url.host().isEmpty() && !_datasetName.isEmpty() && _lastFieldsResponse;
}


bool NADConnection::updated() {
  bool ret = updateLastFieldsResponse();
  if (_firstTime) {
    _firstTime = false;
    return true;
  } else {
    return ret;
  }
}


QSize NADConnection::range(const QString &field) const {
  if (isValid() && _lastFieldsResponse->fields.contains(field)) {
    NADFieldsResponseField *f = _lastFieldsResponse->fields[field];
    if (f) {
//      kstdDebug() << "NADConnection::range(" << field << ") = (" << f->startFrame << "," << f->stopFrame << ")" << endl;
      return QSize(f->startFrame, f->stopFrame);
    }
  }

  return QSize(0,0);
}


unsigned int NADConnection::samplesPerFrame(const QString &field) const {
  if (isValid() && _lastFieldsResponse->fields.contains(field)) {
    NADFieldsResponseField *f = _lastFieldsResponse->fields[field];
    if (f) {
      return f->samplesPerFrame;
    }
  }

  return 0;
}


int NADConnection::getData(const QString &field, double *buf, long startFrame, long stopFrame, long skip) {
  QString postData = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n<GetData><DataSet><DataSetName>" + _datasetName + "</DataSetName></DataSet><Endian>LittleEndian</Endian><Fields><Field><FieldName>" + field + "</FieldName><Start type=\"frame\">" + QString::number(startFrame) + "</Start><Stop type=\"frame\">" + QString::number(stopFrame) + "</Stop><Skip type=\"frame\">" + QString::number(skip) + "</Skip></Field></Fields></GetData>";

  // remove trailing null
  QByteArray data;
  data.duplicate(postData.local8Bit(), postData.length());

//  kstdDebug() << "xml Request:" << postData << endl;

  CURL *curlHandle = curl_easy_init();
  QString urlQString = _url.url();
  const char *urlStr = urlQString.latin1();
  curl_easy_setopt(curlHandle, CURLOPT_URL, urlStr);

  struct curl_slist *headers=NULL;
  headers = curl_slist_append(headers, "Content-Type: text/xml");

  QByteArray response;
  QBuffer b(response);
  b.open(IO_WriteOnly);
  QDataStream respTempStream(&b);

  QTime timer;
  timer.start();

  curl_easy_setopt(curlHandle, CURLOPT_POSTFIELDS, data.data());

  /* set the size of the postfields data */
  curl_easy_setopt(curlHandle, CURLOPT_POSTFIELDSIZE, postData.length());

  /* pass our list of custom made headers */
  curl_easy_setopt(curlHandle, CURLOPT_HTTPHEADER, headers);

  curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, curlWriter);
  curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, &respTempStream);

  curl_easy_perform(curlHandle); /* post away! */

  curl_slist_free_all(headers); /* free the header list */

  kstdDebug() << "NAD: done HTTP POST for getData: " << QString::number(timer.elapsed()) << " ms" << endl;

  QString bufStr;
  int chunkLength;
  QBuffer respBuf(response);
  respBuf.open(IO_ReadOnly);
  QTextStream respTStream(&respBuf);
  QDataStream respDStream(&respBuf);

  respTStream.skipWhiteSpace();

  // chunk separator
  bufStr = respTStream.readLine();
  if (bufStr.isEmpty() || bufStr.left(7) != "--chunk") {
    return -1;
  }

  // headers
  bufStr = respTStream.readLine();
  while (!bufStr.isEmpty() && bufStr.at(1) != '\n') {
    if (bufStr.left(16) == "Content-Length: ") {
      chunkLength = bufStr.mid(16).toInt();
    }
    bufStr = respTStream.readLine();
  }

  char *xmlbuf = new char[chunkLength+1];
  respTStream.readRawBytes(xmlbuf, chunkLength);
  xmlbuf[chunkLength] = '\0';

  timer.restart();

  QXmlInputSource source;
  QXmlSimpleReader xmlReader;
  DataResponseHandler *handler = new DataResponseHandler();

  source.setData(xmlbuf);
  xmlReader.setContentHandler(handler);
  bool parseok = xmlReader.parse(source);

  kstdDebug() << "NAD: done XML parsing for getData: " << QString::number(timer.elapsed()) << " ms" << endl;

  delete[] xmlbuf;

  if (!parseok) {
    kstdDebug() << "NAD: XML parsing error" << endl;
    return -1;
  }

  NADDataResponse *resp = handler->getResponse();
  respDStream.setByteOrder(resp->endian);

  respTStream.skipWhiteSpace();

  long curOutPos = 0;
  while (!respBuf.atEnd()) {

//    timer.restart();

    // read data chunk
    bufStr = respTStream.readLine();
    if (bufStr.isEmpty() || bufStr.left(7) != "--chunk") {
      delete resp;
      delete handler;
      return -1;
    } else if (bufStr.left(9) == "--chunk--") {
      continue;
    }

//    kstdDebug() << "NAD: Got Data Chunk" << endl;

    // at beginning of chunk
    bufStr = respTStream.readLine();
    while (!bufStr.isEmpty() && bufStr.at(1) != '\n') {
      if (bufStr.left(16) == "Content-Length: ") {
        chunkLength = bufStr.mid(16).toInt();
      } else if (bufStr.left(14) == "Content-type: ") {
        if (bufStr.mid(14).left(24) != "application/octet-stream") {
          kstdDebug() << "NAD: error in data stream" << endl;
          return -1;
        }
      }
      bufStr = respTStream.readLine();
    }

    int32_t startFrame, stopFrame;
    respDStream >> startFrame >> stopFrame;

    QMap<QString, NADDataResponseField *>::iterator i;
    for (i = resp->fields.begin(); i != resp->fields.end(); ++i) {
      unsigned int fieldStartFrame, fieldStopFrame;
      respDStream >> fieldStartFrame >> fieldStopFrame;
        if ((fieldStartFrame == UINT_MAX) && (fieldStopFrame == UINT_MAX)) {
          // no data
        } else {
          long fieldChunkBytes;
          if (i.data()->skip) {
            fieldChunkBytes = ((fieldStopFrame - fieldStartFrame) / (i.data()->skip) + 1) * i.data()->size;
          } else {
            fieldChunkBytes = (fieldStopFrame - fieldStartFrame + 1) * i.data()->samplesPerFrame * i.data()->size;
          }
//          kstdDebug() << "frame:" << fieldStartFrame << "-" << fieldStopFrame << ": fieldChunkBytes=" << fieldChunkBytes << ", curOutPos=" << curOutPos << endl;
          for (long j=0; j < fieldChunkBytes; j += i.data()->size) {
//            assert(curOutPos < n);
            switch(i.data()->type) {
              case 'c':
                {
                  Q_UINT8 temp;
                  respDStream >> temp;
                  buf[curOutPos++] = (double)temp;
                  break;
                }
              case 's':
                {
                  Q_INT16 temp;
                  respDStream >> temp;
                  buf[curOutPos++] = (double)temp;
                  break;
                }
              case 'u':
                {
                  Q_UINT16 temp;
                  respDStream >> temp;
                  buf[curOutPos++] = (double)temp;
                  break;
                }
              case 'S': case 'i':
                {
                  Q_INT32 temp;
                  respDStream >> temp;
                  buf[curOutPos++] = (double)temp;
                  break;
                }
              case 'U':
                {
                  Q_UINT32 temp;
                  respDStream >> temp;
                  buf[curOutPos++] = (double)temp;
                  break;
                }
              case 'f':
                {
                  float temp;
                  respDStream >> temp;
                  buf[curOutPos++] = (double)temp;
                  break;
                }
              case 'd':
                respDStream >> buf[curOutPos++];
                break;
            }
          }
//          kstdDebug() << "frame:" << fieldStartFrame << "-" << fieldStopFrame << ": fieldChunkBytes=" << fieldChunkBytes << ", curOutPos=" << curOutPos << endl;
        }
    }

    respTStream.skipWhiteSpace();

//    kstdDebug() << "NAD: done reading data chunk for getData: " << QString::number(timer.elapsed()) << " ms" << endl;
  }

  return curOutPos;
}

#include "nadconnection.moc"

// vim: ts=2 sw=2 et
