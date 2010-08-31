/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *   copyright : (C) 2005 by University of British Columbia
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "dialoglauncher.h"
#include "datacollection.h"
#include "debug.h"
#include "image.h"
#include "math_kst.h"
#include "objectstore.h"

#include "kst_i18n.h"

#include <QImage>
#include <QPainter>
#include <QXmlStreamWriter>

#include <math.h>

//#define BENCHMARK

namespace Kst {

const QString Image::staticTypeString = I18N_NOOP("Image");
const QString Image::staticTypeTag = I18N_NOOP("image");

static const QLatin1String& THEMATRIX = QLatin1String("THEMATRIX");

Image::Image(ObjectStore *store) : Relation(store) {
  _typeString = staticTypeString;
  _type = "Image";
  _initializeShortName();

  _hasContourMap = false;
  _hasColorMap = false;
  setColorDefaults();
  setContourDefaults();

}


Image::~Image() {
}

void Image::_initializeShortName() {
  _shortName = "I"+QString::number(_inum);
  if (_inum>max_inum)
    max_inum = _inum;
  _inum++;
}

void Image::save(QXmlStreamWriter &s) {
  s.writeStartElement(staticTypeTag);
  if (_inputMatrices.contains(THEMATRIX)) {
    s.writeAttribute("matrix", _inputMatrices[THEMATRIX]->Name());
  }

  if (_pal.colorCount()>0) {
    s.writeAttribute("palettename", _pal.paletteName());
  }

  s.writeAttribute("hascolormap", QVariant(_hasColorMap).toString());
  s.writeAttribute("lowerthreshold", QString::number(_zLower));
  s.writeAttribute("upperthreshold", QString::number(_zUpper));

  s.writeAttribute("hascontourmap", QVariant(_hasContourMap).toString());
  s.writeAttribute("numcontourlines", QString::number(_numContourLines));
  s.writeAttribute("contourweight", QString::number(_contourWeight));
  s.writeAttribute("contourcolor", _contourColor.name());

  s.writeAttribute("autothreshold", QVariant(_autoThreshold).toString());
  saveNameInfo(s, INUM);
  s.writeEndElement();
}


void Image::internalUpdate() {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  writeLockInputsAndOutputs();

  if (_inputMatrices.contains(THEMATRIX)) {

    MatrixPtr mp = _inputMatrices[THEMATRIX];

    // stats
    NS = mp->sampleCount();
    MinX = mp->minX();
    int xNumSteps = mp->xNumSteps();
    double xStepSize = mp->xStepSize();
    MaxX = xNumSteps*xStepSize + MinX;
    MinY = mp->minY();
    int yNumSteps = mp->yNumSteps();
    double yStepSize = mp->yStepSize();
    MaxY = yNumSteps*yStepSize + MinY;
    _ns_maxx = MaxX;
    _ns_minx = MinX;
    _ns_maxy = MaxY;
    _ns_miny = MinY;
    MinPosY = MinY > 0 ? MinY : 0;
    MinPosX = MinX > 0 ? MinX : 0;


    //recalculate the thresholds if necessary
    if (_autoThreshold) {
      _zLower = mp->minValue();
      _zUpper = mp->maxValue();
    }

    //update the contour lines
    if (hasContourMap()) {
      double min = mp->minValue(), max = mp->maxValue();
      double contourStep  = (max - min) / (double)(_numContourLines + 1);
      if (contourStep > 0) {
        _contourLines.clear();
        for (int i = 0; i < _numContourLines; i++) {
          _contourLines.append(min + (i+1) * contourStep);
        }
      }
    }

    _redrawRequired = true;
  }

  unlockInputsAndOutputs();
  return;
}


QString Image::propertyString() const {
  if (_inputMatrices.contains(THEMATRIX)) {
    return i18n("Image of %1" ).arg(_inputMatrices[THEMATRIX]->Name());
  } else {
    return QString();
  }
}

//FIXME: Inline and optimize!
QColor Image::getMappedColor(double z) {
  int index;
  if (_zUpper - _zLower != 0) {
    index = (int)(((z - _zLower) * (_pal.colorCount() - 1)) / (_zUpper - _zLower));
  } else {
    index = 0;
  }
  return _pal.color(index);
}


void Image::setPalette(const Palette &pal) {
  _pal.changePaletteName(pal.paletteName());
}


void Image::showNewDialog() {
  DialogLauncher::self()->showImageDialog();
}


void Image::showEditDialog() {
  DialogLauncher::self()->showImageDialog(this);
}


void Image::setUpperThreshold(double z) {
  _zUpper = z;
}


void Image::setLowerThreshold(double z) {
  _zLower = z;
}

void Image::setThresholdToSpikeInsensitive(double per) {
  if (per==0) {
    setAutoThreshold(true);
  } else {
    matrix()->writeLock();
    matrix()->calcNoSpikeRange(per);
    matrix()->unlock();
    setLowerThreshold(matrix()->minValueNoSpike());
    setUpperThreshold(matrix()->maxValueNoSpike());
    setAutoThreshold(false);
  }
}


void Image::changeToColorOnly(MatrixPtr in_matrix, double lowerZ,
    double upperZ, bool autoThreshold, const QString &paletteName) {

  _inputMatrices[THEMATRIX] = in_matrix;

  _zLower = lowerZ;
  _zUpper = upperZ;
  _autoThreshold = autoThreshold;
  if (_pal.paletteName() != paletteName) {
    _pal.changePaletteName(paletteName);
  }
  _hasColorMap = true;
  _hasContourMap = false;
}


void Image::changeToContourOnly(MatrixPtr in_matrix, int numContours,
    const QColor& contourColor, int contourWeight) {

  _inputMatrices[THEMATRIX] = in_matrix;
  _numContourLines = numContours;
  _contourWeight = contourWeight;
  _contourColor = contourColor;
  _hasColorMap = false;
  _hasContourMap = true;

}


void Image::changeToColorAndContour(MatrixPtr in_matrix,
    double lowerZ, double upperZ, bool autoThreshold, const QString &paletteName,
    int numContours, const QColor& contourColor, int contourWeight) {

  _inputMatrices[THEMATRIX] = in_matrix;

  _zLower = lowerZ;
  _zUpper = upperZ;
  _autoThreshold = autoThreshold;
  if (_pal.paletteName() != paletteName) {
    _pal.changePaletteName(paletteName);
  }
  _numContourLines = numContours;
  _contourWeight = contourWeight;
  _contourColor = contourColor;
  _hasColorMap = true;
  _hasContourMap = true;

}


void Image::matrixDimensions(double &x, double &y, double &width, double &height) {
  if (_inputMatrices.contains(THEMATRIX)) {
    MatrixPtr mp = _inputMatrices[THEMATRIX];
    if (_inputMatrices.contains(THEMATRIX)) {
      x = mp->minX();
      y = mp->minY();
      width = mp->xNumSteps() * mp->xStepSize();
      height = mp->yNumSteps() * mp->yStepSize();
    } else {
      x = y = width = height = 0;
    }
  } else {
    x = y = width = height = 0;
  }
}


//this should check for duplicates
bool Image::addContourLine(double line) {
  _contourLines.append(line);
  return true;
}


bool Image::removeContourLine(double line) {
  return _contourLines.removeAll(line);
}


void Image::clearContourLines() {
  _contourLines.clear();
}


bool Image::getNearestZ(double x, double y, double& z) {
  bool ok;
  z = _inputMatrices[THEMATRIX]->value(x,y,&ok);
  return ok;
}


QString Image::paletteName() const {
  return _pal.paletteName();
}


void Image::setColorDefaults() {
  _zLower = 0;
  _zUpper = 100;
}


void Image::setContourDefaults() {
  _contourColor = QColor("red");
  _numContourLines = 1;
  _contourWeight = 0;
}


void Image::setAutoThreshold(bool yes) {
  _autoThreshold = yes;
}


RelationPtr Image::makeDuplicate(QMap<RelationPtr, RelationPtr> &duplicatedRelations) {
  ImagePtr image = store()->createObject<Image>();

  if (!_hasContourMap) {
    image->changeToColorOnly(_inputMatrices[THEMATRIX],
        _zLower,
        _zUpper,
        _autoThreshold,
        _pal.paletteName());
  } else if (!_hasColorMap) {
    image->changeToContourOnly(_inputMatrices[THEMATRIX],
        _numContourLines,
        _contourColor,
        _contourWeight);
  } else {
    image->changeToColorAndContour(_inputMatrices[THEMATRIX],
        _zLower,
        _zUpper,
        _autoThreshold,
        _pal.paletteName(),
        _numContourLines,
        _contourColor,
        _contourWeight);
  }

  if (descriptiveNameIsManual()) {
    image->setDescriptiveName(descriptiveName());
  }
  image->writeLock();
  image->registerChange();
  image->unlock();

  duplicatedRelations.insert(this, RelationPtr(image));
  return RelationPtr(image);

}

MatrixPtr Image::matrix() const {
  if (_inputMatrices.contains(THEMATRIX)) {
    return _inputMatrices[THEMATRIX];
  } else {
    return NULL;
  }
}

bool Image::invertXHint() const {
  if (_inputMatrices.contains(THEMATRIX)) {
    return _inputMatrices[THEMATRIX]->invertXHint();
  } else {
    return false;
  }
}

bool Image::invertYHint() const {
  if (_inputMatrices.contains(THEMATRIX)) {
    return _inputMatrices[THEMATRIX]->invertYHint();
  } else {
    return false;
  }
}

QString Image::xLabel() const {
  if (_inputMatrices.contains(THEMATRIX)) {
    return _inputMatrices[THEMATRIX]->xLabel();
  } else {
    return QString();
  }
}


QString Image::yLabel() const {
  if (_inputMatrices.contains(THEMATRIX)) {
    return _inputMatrices[THEMATRIX]->yLabel();
  } else {
    return QString();
  }
}


QString Image::topLabel() const {
  if (_inputMatrices.contains(THEMATRIX)) {
    return _inputMatrices[THEMATRIX]->label();
  } else {
    return QString();
  }
}


DataObjectPtr Image::providerDataObject() const {
  DataObjectPtr provider = 0L;
  // FIXME: fix this.. I don't know what's going on here
#if 0
  matrixList.lock().readLock();
  MatrixPtr mp = *matrixList.findTag(matrixTag());
  matrixList.lock().unlock();
  DataObjectPtr provider = 0L;
  if (mp) {
    mp->readLock();
    provider = kst_cast<DataObject>(mp->provider());
    mp->unlock();
  }
#endif
  return provider;
}


double Image::distanceToPoint(double xpos, double dx, double ypos) const {
  Q_UNUSED(dx)
  // dx is not relevant for double clicks on images - clicks must be inside the image
  if (xpos <= MaxX && xpos >= MinX && ypos <= MaxY && ypos >= MinY) {
    return 0;
  }
  return 1.0E300;
}


void Image::paintObjects(const CurveRenderContext& context) {
  QPainter* p = context.painter;

  if (hasColorMap()) {
    p->drawImage(_imageLocation, _image);
  }

  if (hasContourMap()) {
    QColor lineColor = contourColor();

    foreach(const CoutourLineDetails& lineDetails, _lines) {
      p->setPen(QPen(lineColor, lineDetails._lineWidth, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));
      p->drawLine(lineDetails._line);
    }
  }
}

void Image::updatePaintObjects(const CurveRenderContext& context) {
  double Lx = context.Lx, Hx = context.Hx, Ly = context.Ly, Hy = context.Hy;
  double m_X = context.m_X, m_Y = context.m_Y, b_X = context.b_X, b_Y = context.b_Y;
  double x_max = context.x_max, y_max = context.y_max, x_min = context.x_min, y_min = context.y_min;
  bool xLog = context.xLog, yLog = context.yLog;
  double xLogBase = context.xLogBase;
  double yLogBase = context.yLogBase;

  double x, y, width, height;
  double img_Lx_pix = 0, img_Ly_pix = 0, img_Hx_pix = 0, img_Hy_pix = 0;

#ifdef BENCHMARK
  QTime bench_time, benchtmp;
  int b_1 = 0, b_2 = 0;
  bench_time.start();
  benchtmp.start();
  int numberOfLinesDrawn = 0;
#endif

  ImagePtr image = this;

  _image = QImage();
  _lines.clear();

  if (_inputMatrices.contains(THEMATRIX)) { // don't paint if we have no matrix
    image->matrixDimensions(x, y, width, height);

    // figure out where the image will be on the plot
    if (xLog) {
      x_min = pow(xLogBase, x_min);
      x_max = pow(xLogBase, x_max);
    }
    if (yLog) {
      y_min = pow(yLogBase, y_min);
      y_max = pow(yLogBase, y_max);
    }

    // only draw if img is visible
    if (!(x > x_max || y > y_max || x + width < x_min || y + height < y_min)) {
      if (x < x_min) {
        img_Lx_pix = Lx;
      } else {
        if (xLog) {
          img_Lx_pix = logXLo(x, xLogBase) * m_X + b_X;
        } else {
          img_Lx_pix = x * m_X + b_X;
        }
      }
      if (y < y_min) {
        img_Hy_pix = Hy;
      } else {
        if (yLog) {
          img_Hy_pix = logYLo(y, yLogBase) * m_Y + b_Y;
        } else {
          img_Hy_pix = y * m_Y + b_Y;
        }
      }
      if (x + width > x_max) {
        img_Hx_pix = Hx;
      } else {
        if (xLog) {
          img_Hx_pix = logXLo(x + width, xLogBase) * m_X + b_X;
        } else {
          img_Hx_pix = (x + width) * m_X + b_X;
        }
      }
      if (y + height > y_max) {
        img_Ly_pix = Ly;
      } else {
        if (yLog) {
          img_Ly_pix = logYLo(y + height, yLogBase) * m_Y + b_Y;
        } else {
          img_Ly_pix = (y + height) * m_Y + b_Y;
        }
      }

      // color map
      QColor thisPixel;
      MatrixPtr m = _inputMatrices.value(THEMATRIX);

      if (image->hasColorMap()) {
        int hXlXDiff = d2i(img_Hx_pix - img_Lx_pix);
        int hYlYDiff = d2i(img_Hy_pix - img_Ly_pix - 1);
        _image = QImage(hXlXDiff, hYlYDiff, QImage::Format_RGB32);
        //_image.fill(0);
        int ih = _image.height();
        int iw = _image.width();
        double m_minX = m->minX();
        double m_minY = m->minY();
        double m_numY = m->yNumSteps();
        double m_stepYr = 1.0/m->yStepSize();
        double m_stepXr = 1.0/m->xStepSize();
        int x_index;
        int y_index;
        int palCountMinus1 = _pal.colorCount() - 1;
        double palCountMin1_OverDZ = double(palCountMinus1) / (_zUpper - _zLower);

        for (int y = 0; y < ih; ++y) {
          bool okY = true;

          QRgb *scanLine = (QRgb *)_image.scanLine(y);
          double new_y;
          if (yLog) {
            new_y = pow(yLogBase, (y + 1 + img_Ly_pix - b_Y) / m_Y);
          } else {
            new_y = (y + 1 + img_Ly_pix - b_Y) / m_Y;
          }
          y_index = (int)((new_y - m_minY)*m_stepYr);
          if (y_index<0 || y_index>=m_numY) {
            okY = false;
          }
          double A = img_Lx_pix - b_X;
          double B = 1.0/m_X;
          for (int x = 0; x < iw; ++x) {
            bool okX = true;
            double new_x;
            if (xLog) {
              new_x = pow(xLogBase, (x + img_Lx_pix - b_X) / m_X);
            } else {
              new_x = (x + A)*B;
            }
            x_index = (int)((new_x - m_minX)*m_stepXr);
            double z = m->Z(x_index * m_numY + y_index);

            okX = finite(z);

            if (okX && okY) {
              scanLine[x] = _pal.rgb((int)(((z - _zLower) * palCountMin1_OverDZ)));
            } else {
              scanLine[x] = 0;
            }
          }
        }
        _imageLocation = QPoint(d2i(img_Lx_pix), d2i(img_Ly_pix + 1));
      }
#ifdef BENCHMARK
    b_1 = benchtmp.elapsed();
#endif
      //*******************************************************************
      // CONTOURS
      //*******************************************************************
#ifndef CONTOUR_STEP
#define CONTOUR_STEP 5
#endif
      //draw the contourmap
      if (image->hasContourMap()) {
        QColor tempColor = image->contourColor();
        bool variableWeight = image->contourWeight() < 0;
        int lineWeight=1;
        if (!variableWeight) {
          // + 1 because 0 and 1 are the same width
          lineWeight = image->contourWeight() + 1;
        }
        // do the drawing for each contour line
        QList<double> lines = image->contourLines();
        QPoint lastPoint; // used to remember the previous point
        bool hasPrevBottom = false;
        MatrixPtr mp = _inputMatrices[THEMATRIX];
        for (int k = 0; k < lines.count(); ++k) {
          double lineK = lines[k];
          if (variableWeight) {
            // + 1 because 0 and 1 are the same width
            lineWeight = k + 1;
          }
          int flImgHx = d2i(floor(img_Hx_pix));
          int flImgHy = d2i(floor(img_Hy_pix));
          int ceImgLy = d2i(ceil(img_Ly_pix));
          for (int i = d2i(ceil(img_Lx_pix)); i + CONTOUR_STEP < flImgHx; i += CONTOUR_STEP) {
            for (int j = d2i(ceil(img_Ly_pix)); j + CONTOUR_STEP < flImgHy; j += CONTOUR_STEP) {
              // look at this group of 4 pixels and get the z values
              double zTL, zTR, zBL, zBR;
              double new_x_small = (i - b_X) / m_X, new_y_small = (j + 1 - b_Y) / m_Y;
              double new_x_large = (i + CONTOUR_STEP - b_X) / m_X, new_y_large = (j+1+CONTOUR_STEP - b_Y) / m_Y;

              if (xLog) {
                new_x_small = pow(xLogBase, new_x_small);
                new_x_large = pow(xLogBase, new_x_large);
              }
              if (yLog) {
                new_y_small = pow(yLogBase, new_y_small);
                new_y_large = pow(yLogBase, new_y_large);
              }

              zTL = mp->value(new_x_small, new_y_small, 0L);
              zTR = mp->value(new_x_large, new_y_small, 0L);
              zBL = mp->value(new_x_small, new_y_large, 0L);
              zBR = mp->value(new_x_large, new_y_large, 0L);

              // determine the lines to draw
              int numPoints = 0;
              bool passTop = false, passBottom = false, passLeft = false, passRight = false;
              QPoint topPoint, bottomPoint, leftPoint, rightPoint;

              // passes through the top
              if (hasPrevBottom) {
                topPoint = lastPoint;
                ++numPoints;
                passTop = true;
              } else if (j == ceImgLy && ((lineK < zTR && lineK > zTL) || (lineK < zTL && lineK > zTR))) {
                ++numPoints;
                passTop = true;
                topPoint.setX(int(((lineK - zTL)*CONTOUR_STEP + (zTR - zTL)*i) / (zTR - zTL)));
                topPoint.setY(j);
              }
              hasPrevBottom = false;

              // passes through the bottom
              if ((lineK < zBR && lineK > zBL) || (lineK < zBL && lineK > zBR)) {
                ++numPoints;
                passBottom = true;
                bottomPoint.setX(int(((lineK - zBL)*CONTOUR_STEP + (zBR - zBL)*i) / (zBR - zBL)));
                bottomPoint.setY(j + CONTOUR_STEP);
                if (j + 2*CONTOUR_STEP < flImgHy) {
                  lastPoint = bottomPoint;
                  hasPrevBottom = true;
                }
              }

              // passes through the left
              if ((lineK < zBL && lineK > zTL) || (lineK < zTL && lineK > zBL)) {
                ++numPoints;
                passLeft = true;
                leftPoint.setY(int(((lineK - zTL)*CONTOUR_STEP + (zBL - zTL)*j) / (zBL - zTL)));
                leftPoint.setX(i);
              }

              // passes through the right
              if ((lineK < zBR && lineK > zTR) || (lineK < zTR && lineK > zBR)) {
                ++numPoints;
                passRight = true;
                rightPoint.setY(int(((lineK - zTR)*CONTOUR_STEP + (zBR - zTR)*j) / (zBR - zTR)));
                rightPoint.setX(i + CONTOUR_STEP);
              }

              if (numPoints == 4) {
                // draw a cross
                _lines.append(CoutourLineDetails(QLine(topPoint, bottomPoint), lineWeight));
                _lines.append(CoutourLineDetails(QLine(rightPoint, leftPoint), lineWeight));
#ifdef BENCHMARK
  numberOfLinesDrawn += 2;
#endif
              } else if (numPoints == 3) {
                // draw a V opening to non-intersecting side
                if (!passTop) {
                  _lines.append(CoutourLineDetails(QLine(leftPoint, bottomPoint), lineWeight));
                  _lines.append(CoutourLineDetails(QLine(bottomPoint, rightPoint), lineWeight));
                } else if (!passLeft) {
                  _lines.append(CoutourLineDetails(QLine(topPoint, rightPoint), lineWeight));
                  _lines.append(CoutourLineDetails(QLine(rightPoint, bottomPoint), lineWeight));
                } else if (!passBottom) {
                  _lines.append(CoutourLineDetails(QLine(leftPoint, topPoint), lineWeight));
                  _lines.append(CoutourLineDetails(QLine(topPoint, rightPoint), lineWeight));
                } else {
                  _lines.append(CoutourLineDetails(QLine(topPoint, leftPoint), lineWeight));
                  _lines.append(CoutourLineDetails(QLine(leftPoint, bottomPoint), lineWeight));
                }
#ifdef BENCHMARK
  numberOfLinesDrawn += 2;
#endif
              } else if (numPoints == 2) {
                // two points - connect them
                QPoint point1, point2;
                bool true1 = false;

                if (passTop) {
                  point1 = topPoint;
                  true1 = true;
                }
                if (passBottom) {
                  if (true1) {
                    point2 = bottomPoint;
                  } else {
                    point1 = bottomPoint;
                    true1 = true;
                  }
                }
                if (passLeft) {
                  if (true1) {
                    point2 = leftPoint;
                  } else {
                    point1 = leftPoint;
                  }
                }
                if (passRight) {
                  point2 = rightPoint;
                }
                _lines.append(CoutourLineDetails(QLine(point1, point2), lineWeight));
#ifdef BENCHMARK
  ++numberOfLinesDrawn;
#endif
              }
            }
          }
        }
      }
    }
  }
#ifdef BENCHMARK
  b_2 = benchtmp.elapsed();
  int i = bench_time.elapsed();
  qDebug() << endl << "Plotting image " << (void *)this << ": " << i << "ms";
  qDebug() << "         Without locks: " << b_2 << "ms";
  qDebug() << " Number of lines drawn: " << numberOfLinesDrawn;
  if (b_1 > 0)       qDebug() << "             Color Map: " << b_1 << "ms";
  if (b_2 - b_1 > 0) qDebug() << "         Coutour Lines: " << (b_2 - b_1) << "ms" << endl;
#endif
}


void Image::yRange(double xFrom, double xTo, double* yMin, double* yMax) {
  if (!yMin || !yMax) {
    return;
  }
  // if x range overlaps with image x range, just return image y range
  if ((xFrom <= MinX && xTo >= MinX) ||
      (xTo >= MaxX && xFrom <= MaxX) ||
      (xFrom > MinX && xFrom < MaxX) ||
      (xTo > MinX && xTo < MaxX)) {
    *yMin = MinY;
    *yMax = MaxY;
    return;
  }
  *yMin = 0;
  *yMax = 0;
  return;
}


void Image::paintLegendSymbol(QPainter *p, const QRectF& bound) {
  if (hasColorMap() && (_pal.colorCount()>0)) {
    int l = bound.left(), r = bound.right(), t = bound.top(), b = bound.bottom();
    // draw the color palette
    for (int i = l; i <= r; i++) {
      int index = (int)floor(static_cast<double>(((i - l) * (_pal.colorCount() - 1))) / (r - l));
      QColor sliceColor = _pal.color(index).rgb();
      p->setPen(QPen(sliceColor, 1));
      p->drawLine(i, t, i, b);
    }
  }
  if (hasContourMap()) {
    // draw a box with contour color
    p->setPen(QPen(_contourColor, 1));
    p->drawRect(bound.left(), bound.top(), bound.width(), bound.height());
  }
}


QString Image::_automaticDescriptiveName() const {
  return matrix()->descriptiveName();
}


QString Image::descriptionTip() const {
  QString tip;
  tip = i18n("Image: %1\n" ).arg(Name());
  if (_hasContourMap) {
    tip += "  Contour Map";
  }
  if (_hasColorMap) {
    tip += "  Color Map";
  }
  tip += matrix()->descriptionTip();
  return tip;
}

}
// vim: ts=2 sw=2 et
