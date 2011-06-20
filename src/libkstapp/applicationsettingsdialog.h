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

#ifndef APPLICATIONSETTINGSDIALOG_H
#define APPLICATIONSETTINGSDIALOG_H

#include "dialog.h"

#include <QPointer>

#include "kst_export.h"

namespace Kst {

class GeneralTab;
class GridTab;
class DefaultLabelPropertiesTab;
class LayoutTab;

class ApplicationSettingsDialog : public Dialog
{
  Q_OBJECT
  public:
    ApplicationSettingsDialog(QWidget *parent = 0);
    virtual ~ApplicationSettingsDialog();

  private Q_SLOTS:
    void generalChanged();
    void gridChanged();
    void defaultLabelPropertiesChanged();
    void layoutChanged();

  private:
    void setupGeneral();
    void setupGrid();
    void setupDefaultLabelProperties();
    void setupLayout();

  private:
    GeneralTab *_generalTab;
    GridTab *_gridTab;
    DefaultLabelPropertiesTab *_defaultLabelPropertiesTab;
    LayoutTab *_layoutTab;
};

}

#endif

// vim: ts=2 sw=2 et
