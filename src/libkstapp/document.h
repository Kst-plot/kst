/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QString>

namespace Kst {

class SessionModel;

class Document {
  public:
    Document();
    ~Document();

    SessionModel* session() const;

    bool open(const QString& file);
    bool save(const QString& to = QString::null);

    bool isChanged() const;
    void setChanged(bool changed);
    bool isOpen() const;

    QString lastError() const;

  private:
    SessionModel *_session;
    bool _dirty;
    bool _isOpen;
};

}

#endif

// vim: ts=2 sw=2 et
