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

#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QDialog>
#include <QShortcut>
#include "object.h"

#include "ui_datamanager.h"

#include "kst_export.h"

class QToolBar;

namespace Kst {

class Document;

class DataManager : public QDialog, Ui::DataManager
{
  Q_OBJECT
  public:
    DataManager(QWidget *parent, Document *doc);
    virtual ~DataManager();

  public Q_SLOTS:
    void showContextMenu(const QPoint &);
    void showEditDialog(QModelIndex qml);
    void showEditDialog();
    void deleteObject();

    void showVectorDialog();
    void showMatrixDialog();
    void showScalarDialog();
    void showStringDialog();

    void showCurveDialog();
    void showCSDDialog();
    void showHistogramDialog();
    void showPowerSpectrumDialog();
    void showImageDialog();
    void showEventMonitorDialog();
    void showEquationDialog();
    void showPluginDialog(QString &pluginName);
    void showFitDialog();
    void showFilterDialog();

    void addToPlot(QAction* action);
    void removeFromPlot(QAction* action);

    virtual bool event(QEvent * event);

    void setUsedFlags();
    void purge();

  private:
    Document *_doc;

    QToolBar *_primitives;
    QToolBar *_dataObjects;
    QToolBar *_plugins;
    QToolBar *_fits;
    QToolBar *_filters;

    ObjectPtr _currentObject;
    QShortcut *_deleteShortcut;
    QMenu *_contextMenu;

    void showEvent(QShowEvent* event);

};

}

#endif

// vim: ts=2 sw=2 et
