/***************************************************************************
                  nadslave.cpp  - NAD KIOSlave
                             -------------------
    begin                : Mon Apr 04 2006
    copyright            : (C) 2006 Staikos Computing Services Inc.
    email                : info@staikos.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as               *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include "nadslave.h"

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <qvaluelist.h>
#include <qregexp.h>

#include <qbuffer.h>
#include <qdatastream.h>
#include <qstring.h>
#include <qthread.h>
#include <qxml.h>

#include <kapplication.h>
#include <kdebug.h>
#include <kglobal.h>
#include <kio/ioslave_defaults.h>
#include <kio/job.h>
#include <kio/netaccess.h>
#include <klocale.h>
#include <kmimetype.h>
#include <kurl.h>

extern "C" int KDE_EXPORT kdemain(int argc, char **argv);

int kdemain(int argc, char **argv) {
  if (argc != 4) {
    kdDebug() << "Usage: kio_nad protocol domain-socket1 domain-socket2" << endl;
    return -1;
  }

  KApplication app(argc, argv, false, false);
//  KInstance instance("kio_nad");
  NadProtocol slave(argv[2], argv[3]);
  slave.dispatchLoop();
  return 0;
}


using namespace KIO;


static void createFileEntry(const QString& name, const QString& mimetype, UDSEntry& entry) {
  UDSAtom atom;

  atom.m_uds = UDS_NAME;
  atom.m_str = name;
  entry.append(atom);

  atom.m_uds = UDS_MIME_TYPE;
  atom.m_str = mimetype;
  entry.append(atom);

  atom.m_uds = KIO::UDS_FILE_TYPE;
  atom.m_str = QString::null;
  atom.m_long = S_IFREG;
  entry.append(atom);
}


static void createDirectoryEntry(const QString& name, UDSEntry& entry) {
  UDSAtom atom;

  atom.m_uds = UDS_NAME;
  atom.m_str = name;
  entry.append(atom);

  atom.m_uds = UDS_MIME_TYPE;
  atom.m_str = "inode/directory";
  entry.append(atom);

  atom.m_uds = KIO::UDS_FILE_TYPE;
  atom.m_str = QString::null;
  atom.m_long = S_IFDIR;
  entry.append(atom);
}


struct NADBrowseResponse {
  QString datasetName;
  QString datasetType;
  QValueList<QPair<QString, QString> > children;
};


class BrowseResponseHandler : public QXmlDefaultHandler {
  public:
    BrowseResponseHandler() : QXmlDefaultHandler() {
      response = 0L;
    }

    ~BrowseResponseHandler() {
      delete response;
      response = 0L;
    }

    bool startDocument() {
      delete response;
      response = new NADBrowseResponse;
      return true;
    }

    bool startElement(const QString & namespaceURI, const QString & localName, const QString & qName, const QXmlAttributes & atts) {
      Q_UNUSED(namespaceURI)
      Q_UNUSED(localName)
//      kstdDebug() << "startElement: namespaceURL=" << namespaceURI << ",localName=" << localName << ",qName=" << qName << endl;
      if (qName == "BrowseResponse") {
      } else if (qName == "DataSet") {
        int index;
        if ((index = atts.index("name")) != -1) {
          response->datasetName = atts.value(index);
        }
        if ((index = atts.index("type")) != -1) {
          response->datasetType = atts.value(index);
        }
//        kdDebug() << "DataSet: name=" << response->datasetName << ", type=" << response->datasetType << endl;
      } else if (qName == "ChildDataSet") {
        QPair<QString, QString> childData;
        int index;
        if ((index = atts.index("name")) != -1) {
          childData.first = atts.value(index);
        }
        if ((index = atts.index("type")) != -1) {
          childData.second = atts.value(index);
        }
//        kdDebug() << "ChildDataSet: name=" << childData.first << ", type=" << childData.first << endl;

        if (!childData.first.isEmpty() && !childData.second.isEmpty()) {
          response->children.append(childData);
        }
      }
      return true;
    }

    bool endElement(const QString & namespaceURI, const QString & localName, const QString & qName) {
      Q_UNUSED(namespaceURI)
      Q_UNUSED(localName)
//      kdDebug() << "endElement: namespaceURL=" << namespaceURI << ",localName=" << localName << ",qName=" << qName << endl;
      if (qName == "BrowseResponse") {
      } else if (qName == "DataSet") {
      } else if (qName == "ChildDataSet") {
      }
      return true;
    }

    NADBrowseResponse *getResponse() {
      return response;
    }

  private:
    NADBrowseResponse *response;
};


static KURL nadToHttpUrl(const KURL &url) {
  KURL httpUrl;
  httpUrl.setUser(url.user());
  httpUrl.setPass(url.pass());
  httpUrl.setHost(url.host());
  httpUrl.setPort(url.port());
  httpUrl.setProtocol("http");
  httpUrl.setPath("/nad");

  return httpUrl;
}


NadProtocol::NadProtocol(const QCString& pool, const QCString& app)
: SlaveBase("nad", pool, app) {
}


NadProtocol::~NadProtocol() {
}


void NadProtocol::chmod(const KURL& url, int permissions) {
  error(KIO::ERR_UNSUPPORTED_ACTION, url.path());
  //error( KIO::ERR_ACCESS_DENIED, url.path());
  //error( KIO::ERR_CANNOT_CHMOD, url.path());
  //finished();
}


void NadProtocol::mkdir(const KURL& url, int permissions) {
  error(KIO::ERR_UNSUPPORTED_ACTION, url.path());
  /*
  error( KIO::ERR_ACCESS_DENIED, url.path() );
  error( KIO::ERR_DISK_FULL, url.path() );
  error( KIO::ERR_COULD_NOT_MKDIR, url.path() );
  finished();
  error( KIO::ERR_DIR_ALREADY_EXIST, url.path() );
  error( KIO::ERR_FILE_ALREADY_EXIST, url.path() );
  */
}


void NadProtocol::get(const KURL& url) {
//  kdDebug() << "get on " << url << " path=[" << url.path() << "]" << endl;

  error(KIO::ERR_UNSUPPORTED_ACTION, url.path());
  return;

  /*
  error( KIO::ERR_ACCESS_DENIED, url.path() );
  error( KIO::ERR_DOES_NOT_EXIST, url.path() );
  error( KIO::ERR_IS_DIRECTORY, url.path() );
  error( KIO::ERR_CANNOT_OPEN_FOR_READING, url.path() );
  error( KIO::ERR_COULD_NOT_READ, url.path());
  */
}


void NadProtocol::put(const KURL& url, int mode, bool overwrite, bool resume) {
//  kdDebug() << "put on " << url << " path=[" << url.path() << "]" << endl;
  
  error(KIO::ERR_UNSUPPORTED_ACTION, url.path());
  return;

  /*
  error( KIO::ERR_COULD_NOT_WRITE, url.path());
  error( KIO::ERR_DIR_ALREADY_EXIST, dest_orig );
  error( KIO::ERR_FILE_ALREADY_EXIST, dest_orig );
  error( KIO::ERR_WRITE_ACCESS_DENIED, dest );
  error( KIO::ERR_CANNOT_OPEN_FOR_WRITING, dest );
  error( KIO::ERR_DISK_FULL, dest_orig);
  */
}


void NadProtocol::copy(const KURL& src, const KURL& dest, int mode, bool overwrite) {
  /*
  error( KIO::ERR_ACCESS_DENIED, src.path() );
  error( KIO::ERR_DOES_NOT_EXIST, src.path() );
  error( KIO::ERR_IS_DIRECTORY, src.path() );
  error( KIO::ERR_CANNOT_OPEN_FOR_READING, src.path() );
  error( KIO::ERR_DIR_ALREADY_EXIST, dest.path() );
  error( KIO::ERR_FILE_ALREADY_EXIST, dest.path() );
  error( KIO::ERR_WRITE_ACCESS_DENIED, dest.path() );
  error( KIO::ERR_CANNOT_OPEN_FOR_WRITING, dest.path() );
  error( KIO::ERR_DISK_FULL, dest.path());
  error( KIO::ERR_SLAVE_DEFINED,
      i18n("Cannot copy file from %1 to %2. (Errno: %3)")
      .arg( src.path() ).arg( dest.path() ).arg( errno ) );
  error( KIO::ERR_COULD_NOT_READ, src.path());
  error( KIO::ERR_DISK_FULL, dest.path());
  error( KIO::ERR_COULD_NOT_WRITE, dest.path());
  finished();
  */
  error(KIO::ERR_UNSUPPORTED_ACTION, src.path());
}


void NadProtocol::rename(const KURL& src, const KURL& dest, bool overwrite) {
  /*
  error( KIO::ERR_ACCESS_DENIED, src.path() );
  error( KIO::ERR_DOES_NOT_EXIST, src.path() );
  error( KIO::ERR_DIR_ALREADY_EXIST, dest.path() );
  error( KIO::ERR_FILE_ALREADY_EXIST, dest.path() );
  error( KIO::ERR_ACCESS_DENIED, dest.path() );
  error( KIO::ERR_UNSUPPORTED_ACTION, QString::fromLatin1("rename"));
  error( KIO::ERR_CANNOT_DELETE, src.path() );
  error( KIO::ERR_CANNOT_RENAME, src.path() );
  finished();
  */
  error(KIO::ERR_UNSUPPORTED_ACTION, src.path());
}


void NadProtocol::del(const KURL& url, bool isfile) {
  /*
  error( KIO::ERR_ACCESS_DENIED, url.path());
  error( KIO::ERR_IS_DIRECTORY, url.path());
  error( KIO::ERR_CANNOT_DELETE, url.path() );
  error( KIO::ERR_ACCESS_DENIED, url.path());
  error( KIO::ERR_COULD_NOT_RMDIR, url.path() );
  finished();
  */
  error(KIO::ERR_UNSUPPORTED_ACTION, url.path());
}


void NadProtocol::stat(const KURL& url) {
  kdDebug() << "stat on " << url << " path=[" << url.path() << "]" << endl;

  KURL httpUrl = nadToHttpUrl(url);

  QString postData = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n<Browse><DataSet><DataSetName>" + url.path() + "</DataSetName></DataSet></Browse>\n";
  QByteArray data;
  data.duplicate(postData.local8Bit(), postData.length());

  KIO::TransferJob* job = KIO::http_post(httpUrl, data, false);

  QMap<QString, QString> metaData;
  metaData.insert( "content-type", "Content-Type: text/xml" );
  QByteArray response;
  if (!KIO::NetAccess::synchronousRun(job, 0, &response, &httpUrl, &metaData)) {
    kdDebug() << "NAD: error with HTTP post" << endl;
    error(KIO::ERR_UNSUPPORTED_ACTION, url.path());
    return;
  }

  QXmlInputSource source;
  QXmlSimpleReader xmlReader;
  BrowseResponseHandler *handler = new BrowseResponseHandler();

  source.setData(response);
  xmlReader.setContentHandler(handler);
  bool parseok = xmlReader.parse(source);

  if (!parseok) {
    error(KIO::ERR_UNSUPPORTED_ACTION, url.path());
    return;
  }

  NADBrowseResponse *resp = handler->getResponse();

  if (resp->datasetType.lower() == "invalid") {
    error(KIO::ERR_DOES_NOT_EXIST, url.path());
    return;
  }

  UDSEntry entry;

  if (resp->datasetType.lower() == "virtual") {
    createDirectoryEntry(url.path(), entry);
  } else if (resp->datasetType.lower() == "dirfile") {
    createFileEntry(url.path(), "application/x-dirfile", entry);
  } else if (resp->datasetType.lower() == "planck" || resp->datasetType.lower() == "piolib") {
    createFileEntry(url.path(), "application/x-piolib", entry);
    /*
    UDSAtom atom;
    atom.m_uds = UDS_SIZE;
    atom.m_long = appfs.size(rest);
    entry.append(atom);
    */
  }

  statEntry(entry);
  finished();
  return;
}


void NadProtocol::listDir(const KURL& url) {
  kdDebug() << "listDir on " << url << " path=[" << url.path() << "]" << endl;

  KURL httpUrl = nadToHttpUrl(url);

  QString postData = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n<Browse><DataSet><DataSetName>" + url.path() + "</DataSetName></DataSet></Browse>\n";
  QByteArray data;
  data.duplicate(postData.local8Bit(), postData.length());

//  kdDebug() << "httpUrl:" << httpUrl.prettyURL() << endl;
//  kdDebug() << "postData:" << QString(data) << endl;

  KIO::TransferJob* job = KIO::http_post(httpUrl, data, false);

//  kdDebug() << "made http_post" << endl;

  QMap<QString, QString> metaData;
  metaData.insert( "content-type", "Content-Type: text/xml" );
  QByteArray response;
  if (!KIO::NetAccess::synchronousRun(job, 0, &response, &httpUrl, &metaData)) {
    kdDebug() << "NAD: error with HTTP post" << endl;
    error(KIO::ERR_UNSUPPORTED_ACTION, url.path());
    return;
  }

//  kdDebug() << "done synchronousRun" << endl;

  QXmlInputSource source;
  QXmlSimpleReader xmlReader;
  BrowseResponseHandler *handler = new BrowseResponseHandler();

  source.setData(response);
  xmlReader.setContentHandler(handler);
  bool parseok = xmlReader.parse(source);

  if (!parseok) {
    error(KIO::ERR_UNSUPPORTED_ACTION, url.path());
    return;
  }

//  kdDebug() << "done XML parse" << endl;

  NADBrowseResponse *resp = handler->getResponse();
  
//  kdDebug() << "resp: dsName=" << resp->datasetName << ", dsType=" << resp->datasetType << endl;

  if (resp->datasetType.lower() == "dirfile" || resp->datasetType.lower() == "planck" || resp->datasetType.lower() == "piolib") {
    error(KIO::ERR_IS_FILE, url.path());
    return;
  }
  if (resp->datasetType.lower() == "invalid") {
    error(KIO::ERR_DOES_NOT_EXIST, url.path());
    return;
  }

  int count = 0;
  for (QValueList<QPair<QString, QString> >::const_iterator i = resp->children.begin(); i != resp->children.end(); ++i) {
    UDSEntry entry;
    if ((*i).second.lower() == "virtual") {
//        kdDebug() << "got directory entry: " << (*i).first << endl;
        createDirectoryEntry((*i).first, entry);
    } else if ((*i).second.lower() == "dirfile") {
//        kdDebug() << "got dirfile entry: " << (*i).first << endl;
        createFileEntry((*i).first, "application/x-dirfile", entry);
        /*
        UDSAtom atom;
        atom.m_uds = UDS_SIZE;
        atom.m_long = appfs.size(rest + *i);
        entry.append(atom);
        */
    } else if ((*i).second.lower() == "planck" || (*i).second.lower() == "piolib") {
//        kdDebug() << "got piolib entry: " << (*i).first << endl;
        createFileEntry((*i).first, "application/x-piolib", entry);
    } else {
      continue;
    }

    listEntry(entry, false);
    count++;
  }

  totalSize(count);
  listEntry(UDSEntry(), true);
  finished();
  return;

  /*
  error( KIO::ERR_IS_FILE, url.path() );
  error(ERR_ACCESS_DENIED, _path);
  error(ERR_CANNOT_ENTER_DIRECTORY, _path);
  error(KIO::ERR_UNSUPPORTED_ACTION, url.path());
  */
}

#include "nadslave.moc"

// vim: ts=2 sw=2 et
