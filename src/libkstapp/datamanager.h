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

#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QDialog>
#include "ui_datamanager.h"

namespace Kst {
class Document;

class DataManager : public QDialog
{
  Q_OBJECT
  public:
    DataManager(QWidget *parent, Document *doc);
    ~DataManager();

  private:
    Ui::DataManager ui;
    Document *_doc;
};

}

#endif

// vim: ts=2 sw=2 et
