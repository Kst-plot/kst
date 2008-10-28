/***************************************************************************
                qimagesource.h  -  data source plugin for qimagesources
                             -------------------
    begin                : Tue Oct 21 2003
    copyright            : (C) 2003 The University of Toronto
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

#ifndef QIMAGESOURCE_H
#define QIMAGESOURCE_H

#include <qimage.h>
#include <kstdatasource.h>


class QimagesourceSource : public KstDataSource {
  public:
    QimagesourceSource(KConfig *cfg, const QString& filename, const QString& type);

    ~QimagesourceSource();

    bool init();

    KstObject::UpdateType update(int = -1);

    int readField(double *v, const QString &field, int s, int n);

    int readMatrix(KstMatrixData* data, const QString& matrix, int xStart,
                   int yStart, int xNumSteps, int yNumSteps);
/*     int readMatrix(KstMatrixData* data, const QString& matrix, int xStart, */
/*                    int yStart, int xNumSteps, int yNumSteps, int skip); */

    
    bool isValidField(const QString &field) const;

    bool isValidMatrix(const QString& field) const;

    bool matrixDimensions( const QString& matrix, int* xDim, int* yDim);

    int samplesPerFrame(const QString &field);

    int frameCount(const QString& field = QString::null) const;

    QString fileType() const;

    void save(QTextStream &ts, const QString& indent = QString::null);

    bool isEmpty() const;

    bool reset();

  private:
    int _frameCount;
    QImage _image;
};


#endif
// vim: ts=2 sw=2 et
