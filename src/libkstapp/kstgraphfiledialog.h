/***************************************************************************
                       kstgraphdialog.h  -  Part of KST
                             -------------------
    begin                :
    copyright            : (C) 2003 The University of Toronto
                           (C) 2003 C. Barth Netterfield
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
#ifndef KSTGRAPHFILEDIALOGI_H
#define KSTGRAPHFILEDIALOGI_H

#include "graphfiledialog.h"

class KstGraphFileDialogI : public KstGraphFileDialog {
  Q_OBJECT
  public:
    KstGraphFileDialogI(QWidget* parent = 0, const char* name = 0,
        bool modal = false, WFlags fl = 0 );
    virtual ~KstGraphFileDialogI();

    void setAutoSave(bool as) {_autoSave = as;}
    bool autoSaving() const {return _autoSave;}

    void setAutoSaveTimer(int time) {_savePeriod = time;}
    int autoSaveTimer() const {return _savePeriod;}

    KURL url() const {return _url;}
    void setURL(const QString&);

    void setFormat(const QString& format) {_format = format;}
    QString format() const {return _format;}

    void setImageSize(int xsize, int ysize) {_w = xsize; _h = ysize;}
    int imageXSize() const {return _w;}
    int imageYSize() const {return _h;} // not used if isSquare

    void setDisplay(int displayOption) {_displayOption = displayOption;}
    int display() const {return _displayOption;}

  public slots:
    void reqGraphFile();
    void reqEpsGraphFile();
    void saveProperties();
    void loadProperties();
    void updateDialog();
    void enableWidthHeight();
    void enableEPSVector(const QString& format);

    void apply_I();
    void ok_I();
    void show_I();
    void applyAutosave();
  private:
    QTimer *_autoSaveTimer;
    QString _url;
    QString _format;
    unsigned _w;
    unsigned _h;
    bool _allWindows;
    bool _autoSave;
    bool _savePeriod;
    bool _saveEPSAsVector;
    int _displayOption;

  signals:
    void graphFileReq(const QString &filename, const QString &format, int w, int h, bool all, int display);
    void graphFileEpsReq(const QString &filename, int w, int h, bool all, int display);
};

#endif
// vim: ts=2 sw=2 et
