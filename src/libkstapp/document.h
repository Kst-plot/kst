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

#include <QPointer>
#include <QString>

namespace Kst {

class MainWindow;
class SessionModel;

class Document {
  public:
    Document(MainWindow *win);
    ~Document();

    SessionModel* session() const;

    QString fileName() const;

    bool open(const QString& file);
    bool save(const QString& to = QString::null);

    bool isChanged() const;
    void setChanged(bool changed);
    bool isOpen() const;

    QString lastError() const;

  private:
    QPointer<MainWindow> _win;
    SessionModel *_session;
    bool _dirty;
    bool _isOpen;
    QString _fileName;
};

}

#endif

// vim: ts=2 sw=2 et
