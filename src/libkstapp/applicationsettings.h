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

    bool useOpenGL() const;
    void setUseOpenGL(bool useOpenGL);

    int referenceViewWidth() const;
    int referenceViewHeight() const;

    double referenceViewWidthCM() const;
    void setReferenceViewWidthCM(const double width);

    double referenceViewHeightCM() const;
    void setReferenceViewHeightCM(const double height);

    double minimumFontSize() const;
    void setMinimumFontSize(const double points);

    QFont defaultFont() const;
    void setDefaultFont(const QFont &font);

    qreal defaultFontScale() const;
    void setDefaultFontScale(const qreal scale);

    QColor defaultFontColor() const;
    void setDefaultFontColor(const QColor &color);

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

    QBrush backgroundBrush() const;
    void setBackgroundBrush(const QBrush brush);

    QGradientStops gradientStops() const;

    QSizeF layoutMargins() const;
    void setLayoutMargins(QSizeF margins);

    QSizeF layoutSpacing() const;
    void setLayoutSpacing(QSizeF spacing);

  Q_SIGNALS:
    void modified();

  private:
    ApplicationSettings();
    ~ApplicationSettings();
    static void cleanup();

  private:
    QSettings *_settings;
    bool _useOpenGL;
    double _refViewWidth;
    double _refViewHeight;
    double _minFontSize;
    QString _defaultFontFamily;
    QFont _defaultFont;
    qreal _defaultFontScale;
    QColor _defaultFontColor;
    int _maxUpdate;
    bool _showGrid;
    bool _snapToGrid;
    qreal _gridHorSpacing;
    qreal _gridVerSpacing;
    QBrush _backgroundBrush;
    QGradientStops _gradientStops;
    bool _shareAxis;
    QSizeF _layoutMargins;
    QSizeF _layoutSpacing;

    friend class ApplicationSettingsDialog;
};

}

#endif

// vim: ts=2 sw=2 et
