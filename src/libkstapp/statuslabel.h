/***************************************************************************
                             statuslabel.h
                             -------------------
    begin                : July 20, 2005
    copyright            : (C) 2005 The University of British Columbia
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

#ifndef STATUSLABEL_H
#define STATUSLABEL_H

#include <qlabel.h>
#include <qtooltip.h>

class StatusLabel : public QLabel {
  Q_OBJECT
  public:
    StatusLabel(const QString &text, QWidget *parent, const char *name = 0, WFlags f = 0);
    virtual ~StatusLabel();

    void setFullText();
    void setTextWidth(const QFontMetrics &metrics, int width);
    void setFullText(const QString &text);
    const QString& fullText() const;

  private:
    QString squeeze(const QString& s, const QFontMetrics& fm, uint width);
  
    QString _fullText;
    int     _width;
};


#endif
// vim: ts=2 sw=2 et
