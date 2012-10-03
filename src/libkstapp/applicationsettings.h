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

#ifndef APPLICATIONSETTINGS_H
#define APPLICATIONSETTINGS_H

#include <QObject>
#include <QBrush>
#include <QFont>

class QSettings;

namespace Kst {

class ApplicationSettingsDialog;

class ApplicationSettings : public QObject
{
  Q_OBJECT
  public:
    static ApplicationSettings *self();

    bool transparentDrag() const;
    void setTransparentDrag(bool transparent_drag);

    bool useOpenGL() const;
    void setUseOpenGL(bool useOpenGL);

    int referenceViewWidth() const;
    int referenceViewHeight() const;

    qreal referenceViewWidthCM() const;
    void setReferenceViewWidthCM(const qreal width);

    qreal referenceViewHeightCM() const;
    void setReferenceViewHeightCM(const qreal height);

    qreal minimumFontSize() const;
    void setMinimumFontSize(const qreal points);

    int minimumUpdatePeriod() const;
    void setMinimumUpdatePeriod(const int period);

    bool showGrid() const;
    void setShowGrid(bool showGrid);

    bool snapToGrid() const;
    void setSnapToGrid(bool snapToGrid);

    qreal gridHorizontalSpacing() const;
    void setGridHorizontalSpacing(qreal spacing);

    qreal gridVerticalSpacing() const;
    void setGridVerticalSpacing(qreal spacing);

    QGradientStops gradientStops() const;

    QSizeF layoutMargins() const;
    void setLayoutMargins(QSizeF margins);

    QSizeF layoutSpacing() const;
    void setLayoutSpacing(QSizeF spacing);

    bool antialiasPlots() const;
    void setAntialiasPlots(bool antialias);

  Q_SIGNALS:
    void modified();

  private:
    ApplicationSettings();
    ~ApplicationSettings();
    static void cleanup();

  private:
    QSettings *_settings;
    bool _transparentDrag;
    bool _useOpenGL;
    qreal _refViewWidth;
    qreal _refViewHeight;
    qreal _minFontSize;
    int _maxUpdate;
    bool _showGrid;
    bool _snapToGrid;
    qreal _gridHorSpacing;
    qreal _gridVerSpacing;
    QBrush _backgroundBrush;
    QGradientStops _gradientStops;
    QSizeF _layoutMargins;
    QSizeF _layoutSpacing;
    bool _antialiasPlots;

    friend class ApplicationSettingsDialog;
};

}

#endif

// vim: ts=2 sw=2 et
