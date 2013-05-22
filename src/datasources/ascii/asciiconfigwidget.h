/***************************************************************************
 *                                                                         *
 *   Copyright : (C) 2003 The University of Toronto                        *
 *   email     : netterfield@astro.utoronto.ca                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ASCII_CONFIG_WINDGET_H
#define ASCII_CONFIG_WINDGET_H

#include "asciisource.h"
#include "dataplugin.h"

#include "ui_asciiconfig.h"

class QTextStream;

class AsciiConfigWidgetInternal : public QWidget, public Ui_AsciiConfig
{
  Q_OBJECT

  public:
    AsciiConfigWidgetInternal(QWidget *parent);

    AsciiSourceConfig config();
    void setConfig(const AsciiSourceConfig&);
    void setFilename(const QString& filename);

  private Q_SLOTS:
    void showBeginning();
    void showPreviewWindow();

  protected Q_SLOTS:
    void interpretationChanged(bool enabled);
    void testAsciiFormatString(QString format);

  private:
    const int _index_offset;
    QString _filename;
    QPlainTextEdit _previewWidget;
    QString readLine(QTextStream& in, int maxLength);
    void showBeginning(QPlainTextEdit* widget, int numberOfLines);
};


class AsciiConfigWidget : public Kst::DataSourceConfigWidget
{
  Q_OBJECT

  public:
    AsciiConfigWidget(QSettings&);
    ~AsciiConfigWidget();

    void load();
    void save();
    bool isOkAcceptabe() const;
    void setDialogParent(QDialog* parent);

    void setFilename(const QString& filename);

    AsciiConfigWidgetInternal *_ac;
    AsciiSourceConfig _oldConfig;

  public Q_SLOTS:
    void updateIndexVector();
    virtual void cancel();

};



#endif
// vim: ts=2 sw=2 et
