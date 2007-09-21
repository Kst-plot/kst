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

#ifndef APPLICATIONSETTINGS_H
#define APPLICATIONSETTINGS_H

#include <QObject>

namespace Kst {

class ApplicationSettingsDialog;

class ApplicationSettings : public QObject
{
  Q_OBJECT
  public:
    static ApplicationSettings *self();

    bool useOpenGL() const;
    void setUseOpenGL(bool useOpenGL);

    qreal referenceViewWidth() const;
    void setReferenceViewWidth(qreal width);

    qreal referenceViewHeight() const;
    void setReferenceViewHeight(qreal height);

    int referenceFontSize() const;
    void setReferenceFontSize(int points);

    int minimumFontSize() const;
    void setMinimumFontSize(int points);

    bool showGrid() const;
    void setShowGrid(bool showGrid);

    bool snapToGrid() const;
    void setSnapToGrid(bool snapToGrid);

    qreal gridHorizontalSpacing() const;
    void setGridHorizontalSpacing(qreal spacing);

    qreal gridVerticalSpacing() const;
    void setGridVerticalSpacing(qreal spacing);

  Q_SIGNALS:
    void modified();

  private:
    ApplicationSettings();
    ~ApplicationSettings();
    static void cleanup();
    bool _useOpenGL;
    qreal _refViewWidth;
    qreal _refViewHeight;
    int _refFontSize;
    int _minFontSize;
    bool _showGrid;
    bool _snapToGrid;
    qreal _gridHorSpacing;
    qreal _gridVerSpacing;

    friend class ApplicationSettingsDialog;
};

}

#endif

// vim: ts=2 sw=2 et
