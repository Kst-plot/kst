/***************************************************************************
                   datawizard.h
                             -------------------
    begin                : 02/28/07
    copyright            : (C) 2007 The University of Toronto
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

#ifndef DATAWIZARD_H
#define DATAWIZARD_H

#include <Q3Wizard>
#include "ui_datawizard4.h"

#include "kstdatacollection.h"

class DataWizard : public Q3Wizard, public Ui::DataWizard {
  Q_OBJECT

public:
  DataWizard(QWidget *parent = 0);
  ~DataWizard();

  bool xVectorOk();
  bool yVectorsOk();
  double getFontSize(int count);
  void showPage(QWidget *page);
  void saveSettings();
  void loadSettings();

public slots:
  void setInput(const QString &input);

private slots:
  void init();
  void destroy();
  void plotColsChanged();
  void xChanged();
  void testUrl();
  void sourceChanged(const QString &txt);
  void fieldListChanged();
  void updateWindowBox();
  void updateColumns();
  void updatePlotBox();
  void vectorSubset(const QString &filter);
  void newFilter();
  void finished();
  void applyFiltersChecked(bool on);
  void enableXEntries();
  void disableXEntries();
  void enablePSDEntries();
  void disablePSDEntries();
  void _search();
  void _disableWindowEntries();
  void _enableWindowEntries();
  void markSourceAndSave();
  void configureSource();
  void clear();
  void down();
  void up();
  void updateVectorPageButtons();
  void add();
  void remove();
  void vectorsDroppedBack(QDropEvent *e);

private:
  static const QString &defaultTag;
  QString _file;
  bool _inTest;
  QPointer<QWidget> _configWidget;
  KstDataSourceList _sourceCache;
  QMap<QString, QString> _countMap;
};

#endif
// vim: ts=2 sw=2 et
