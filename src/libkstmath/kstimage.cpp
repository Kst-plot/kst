/***************************************************************************
                   KstImage.cpp: Image class for Kst
                             -------------------
    begin                : July 2004
    copyright            : (C) 2004 University of British Columbia
    email                :
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *   Permission is granted to link with any opensource library             *
 *                                                                         *
 ***************************************************************************/

#include "dialoglauncher.h"
#include "kstdatacollection.h"
#include "kstdebug.h"
#include "kstimage.h"
#include "kstmath.h"

#include "kst_i18n.h"

#include <qimage.h>
#include <qpainter.h>
#include <QTextDocument>

#include <math.h>

static const QLatin1String& THEMATRIX = QLatin1String("THEMATRIX");

KstImage::KstImage(const QDomElement& e) : KstRelation(e) {
  QString in_matrixName, in_paletteName;
  bool in_hasColorMap = false, in_hasContourMap = false;
  double in_zLower = 0, in_zUpper = 0;
  _autoThreshold = false;

  /* parse the DOM tree */
  QDomNode n = e.firstChild();
  while( !n.isNull() ) {
    QDomElement e = n.toElement(); // try to convert the node to an element.
    if( !e.isNull() ) { // the node was really an element.
      if (e.tagName() == "tag") {
        setTagName(KstObjectTag::fromString(e.text()));
      } else if (e.tagName() == "matrixtag") {
        in_matrixName = e.text();
      } else if (e.tagName() == "legend") {
        setLegendText(e.text());
      } else if (e.tagName() == "palettename") {
        in_paletteName = e.text();
      } else if (e.tagName() == "lowerthreshold") {
        in_zLower = e.text().toDouble();
      } else if (e.tagName() == "upperthreshold") {
        in_zUpper = e.text().toDouble();
      } else if (e.tagName() == "hascolormap") {
        in_hasColorMap = (e.text() != "0");
      } else if (e.tagName() == "hascontourmap") {
        in_hasContourMap = (e.text() != "0");
      } else if (e.tagName() == "numcontourlines") {
        _numContourLines = e.text().toInt();
      } else if (e.tagName() == "contourcolor") {
        _contourColor.setNamedColor(e.text());
      } else if (e.tagName() == "contourweight") {
        _contourWeight = e.text().toInt();
      } else if (e.tagName() == "autothreshold") {
        _autoThreshold = (e.text() != "0");
      }

    }
    n = n.nextSibling();
  }

  _inputMatrixLoadQueue.append(qMakePair(QString(THEMATRIX), in_matrixName));

  _typeString = i18n("Image");
  _type = "Image";
  _hasColorMap = in_hasColorMap;
  _hasContourMap = in_hasContourMap;
  _zLower = in_zLower;
  _zUpper = in_zUpper;

  if (_hasColorMap) {
    KstPalette in_pal;
    //maybe the palette doesn't exist anymore.  Generate a grayscale palette then.
    for (int i = 0; i < 256; i++) {
      in_pal.insert(i, QColor(i,i,i));
    }
    KstDebug::self()->log(i18n("Unable to find palette %1.  Using a 256 color grayscale palette instead.").arg(in_paletteName), KstDebug::Warning);
    _pal = in_pal;
  }

  if (!_hasColorMap) {
    setColorDefaults();
  }
  if (!_hasContourMap) {
    setContourDefaults();
  }
}


//constructor for colormap only
KstImage::KstImage(const QString &in_tag, KstMatrixPtr in_matrix, double lowerZ, double upperZ, bool autoThreshold, const KstPalette &pal) : KstRelation(){

  _inputMatrices[THEMATRIX] = in_matrix;
  setTagName(KstObjectTag(in_tag, KstObjectTag::globalTagContext));  // FIXME: always top-level?
  _typeString = i18n("Image");
  _type = "Image";
  _zLower = lowerZ;
  _zUpper = upperZ;
  _autoThreshold = autoThreshold;
  _pal = pal;
  _hasContourMap = false;
  _hasColorMap = true;

  setContourDefaults();
  setDirty();
}


//constructor for contour map only
KstImage::KstImage(const QString &in_tag, KstMatrixPtr in_matrix, int numContours, const QColor& contourColor, int contourWeight) : KstRelation(){
  _inputMatrices[THEMATRIX] = in_matrix;
  setTagName(KstObjectTag(in_tag, KstObjectTag::globalTagContext));  // FIXME: always top-level?
  _typeString = i18n("Image");
  _type = "Image";
  _contourColor = contourColor;
  _numContourLines = numContours;
  _contourWeight = contourWeight;
  _hasContourMap = true;
  _hasColorMap = false;

  setColorDefaults();
  setDirty();

}


//constructor for both colormap and contour map
KstImage::KstImage(const QString &in_tag,
                   KstMatrixPtr in_matrix,
                   double lowerZ,
                   double upperZ,
                   bool autoThreshold,
                   const KstPalette &pal,
                   int numContours,
                   const QColor& contourColor,
                   int contourWeight) {
  _inputMatrices[THEMATRIX] = in_matrix;
  setTagName(KstObjectTag(in_tag, KstObjectTag::globalTagContext));  // FIXME: always top-level?
  _typeString = i18n("Image");
  _type = "Image";
  _contourColor = contourColor;
  _numContourLines = numContours;
  _contourWeight = contourWeight;
  _hasContourMap = true;
  _hasColorMap = true;
  _zLower = lowerZ;
  _zUpper = upperZ;
  _autoThreshold = autoThreshold;
  _pal = pal;
  setDirty();
}


KstImage::~KstImage() {
}


void KstImage::save(QTextStream &ts, const QString& indent) {
  QString l2 = indent + "  ";
  ts << indent << "<image>" << endl;
  ts << l2 << "<tag>" << Qt::escape(tagName()) << "</tag>" << endl;
  if (_inputMatrices.contains(THEMATRIX)) {
    ts << l2 << "<matrixtag>" << Qt::escape(_inputMatrices[THEMATRIX]->tag().tagString()) << "</matrixtag>" << endl;
  }
  ts << l2 << "<legend>" << Qt::escape(legendText()) << "</legend>" << endl;
  ts << l2 << "<hascolormap>" << _hasColorMap << "</hascolormap>" <<endl;

//FIXME!!
#if 0
  if (!_pal.isEmpty()) {
    ts << l2 << "<palettename>" << Qt::escape(_pal->name()) << "</palettename>" << endl;
  }
#endif

  ts << l2 << "<lowerthreshold>" << _zLower << "</lowerthreshold>" << endl;
  ts << l2 << "<upperthreshold>" << _zUpper << "</upperthreshold>" << endl;
  ts << l2 << "<hascontourmap>" << _hasContourMap << "</hascontourmap>" << endl;
  ts << l2 << "<numcontourlines>" << _numContourLines << "</numcontourlines>" << endl;
  ts << l2 << "<contourweight>" << _contourWeight << "</contourweight>" << endl;
  ts << l2 << "<contourcolor>" << Qt::escape(_contourColor.name()) << "</contourcolor>" << endl;
  ts << l2 << "<autothreshold>" << _autoThreshold << "</autothreshold>" << endl;
  ts << indent << "</image>" << endl;
}


KstObject::UpdateType KstImage::update(int update_counter) {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  bool force = dirty();
  setDirty(false);

  if (KstObject::checkUpdateCounter(update_counter) && !force) {
    return lastUpdateResult();
  }

  writeLockInputsAndOutputs();

  if (_inputMatrices.contains(THEMATRIX)) {
    KstMatrixPtr mp = _inputMatrices[THEMATRIX];
    bool updated = UPDATE == mp->update(update_counter);
  
    if (updated || force) {
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

      unlockInputsAndOutputs();
      return setLastUpdateResult(UPDATE);
    }
  }

  unlockInputsAndOutputs();
  return setLastUpdateResult(NO_CHANGE);
}


QString KstImage::propertyString() const {
  if (_inputMatrices.contains(THEMATRIX)) {
    return i18n("Using matrix %1" ).arg(_inputMatrices[THEMATRIX]->tag().displayString());
  } else {
    return QString();
  }
}


QColor KstImage::getMappedColor(double x, double y) {
  bool ok;
  
  double z = _inputMatrices[THEMATRIX]->value(x, y, &ok);
  if (ok) {
    int index;
    if (_zUpper - _zLower != 0) {
      if (z > _zUpper) {
        index = _pal.count() - 1;
      } else if (z < _zLower) {
        index = 0;
      } else {
          index = (int)floor(((z - _zLower) * (_pal.count() - 1)) / (_zUpper - _zLower));
      }
    } else {
      index = 0;
    }
    return _pal.value(index);
  }
  return QColor();
}


void KstImage::setPalette(const KstPalette &pal) {
  _pal = pal;
}


void KstImage::showNewDialog() {
  Kst::DialogLauncher::self()->showImageDialog();
}


void KstImage::showEditDialog() {
  Kst::DialogLauncher::self()->showImageDialog(this);
}


void KstImage::setUpperThreshold(double z) {
  setDirty();
  _zUpper = z;
}


void KstImage::setLowerThreshold(double z) {
  setDirty();
  _zLower = z;
}

void KstImage::setThresholdToSpikeInsensitive(double per) {
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


void KstImage::changeToColorOnly(const QString &in_tag, KstMatrixPtr in_matrix,
                                     double lowerZ, double upperZ, bool autoThreshold, const KstPalette &pal) {
  setTagName(KstObjectTag(in_tag, KstObjectTag::globalTagContext));  // FIXME: always top-level?
  if (_inputMatrices.contains(THEMATRIX)) {
    _inputMatrices[THEMATRIX] = in_matrix;
  }
  _zLower = lowerZ;
  _zUpper = upperZ;
  _autoThreshold = autoThreshold;
  if (_pal != pal) {
    _pal = pal;
  }
  _hasColorMap = true;
  _hasContourMap = false;
  setDirty();
}


void KstImage::changeToContourOnly(const QString &in_tag, KstMatrixPtr in_matrix,
                                       int numContours, const QColor& contourColor, int contourWeight) {
  setTagName(KstObjectTag(in_tag, KstObjectTag::globalTagContext));  // FIXME: always top-level?
  if (_inputMatrices.contains(THEMATRIX)) {
    _inputMatrices[THEMATRIX] = in_matrix;
  }
  _numContourLines = numContours;
  _contourWeight = contourWeight;
  _contourColor = contourColor;
  _hasColorMap = false;
  _hasContourMap = true;

//FIXME
#if 0
  if (_pal) {
    _lastPaletteName = _pal->name();
  }
#endif

  setDirty();
}


void KstImage::changeToColorAndContour(const QString &in_tag, KstMatrixPtr in_matrix,
                                               double lowerZ, double upperZ, bool autoThreshold, const KstPalette &pal,
                                               int numContours, const QColor& contourColor, int contourWeight) {
  setTagName(KstObjectTag(in_tag, KstObjectTag::globalTagContext));  // FIXME: always top-level?
  if (_inputMatrices.contains(THEMATRIX)) {
    _inputMatrices[THEMATRIX] = in_matrix;
  }
  _zLower = lowerZ;
  _zUpper = upperZ;
  _autoThreshold = autoThreshold;
  if (_pal != pal) {
    _pal = pal;
  }
  _numContourLines = numContours;
  _contourWeight = contourWeight;
  _contourColor = contourColor;
  _hasColorMap = true;
  _hasContourMap = true;
  setDirty();
}


void KstImage::matrixDimensions(double &x, double &y, double &width, double &height) {
  if (_inputMatrices.contains(THEMATRIX)) {
    KstMatrixPtr mp = _inputMatrices[THEMATRIX];
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
bool KstImage::addContourLine(double line) {
  _contourLines.append(line);
  setDirty();
  return true;
}


bool KstImage::removeContourLine(double line) {
  setDirty();
  return _contourLines.removeAll(line);
}


void KstImage::clearContourLines() {
  setDirty();
  _contourLines.clear();
}


bool KstImage::getNearestZ(double x, double y, double& z) {
  bool ok;
  z = _inputMatrices[THEMATRIX]->value(x,y,&ok);
  return ok;
}


QString KstImage::paletteName() const {
  return _lastPaletteName;
}


void KstImage::setColorDefaults() {
  _zLower = 0;
  _zUpper = 100;
  setDirty();
}


void KstImage::setContourDefaults() {
  _contourColor = QColor("red");
  _numContourLines = 1;
  _contourWeight = 0;
  setDirty();
}


void KstImage::setAutoThreshold(bool yes) {
  _autoThreshold = yes;
  setDirty();
}


#if 0
KstDataObjectPtr KstImage::makeDuplicate(KstDataObjectDataObjectMap& duplicatedMap) {
  QString name(tagName() + '\'');
  while (KstData::self()->dataTagNameNotUnique(name, false)) {
    name += '\'';
  }
  KstImagePtr image = new KstImage(name, _inputMatrices[THEMATRIX], _zLower, _zUpper, _autoThreshold, _pal);
  duplicatedMap.insert(this, KstDataObjectPtr(image));
  return KstDataObjectPtr(image);
}
#endif


QString KstImage::matrixTag() const {
  if (_inputMatrices.contains(THEMATRIX)) { 
    return _inputMatrices[THEMATRIX]->tag().displayString(); 
  } else {
    return QString();
  }
}


KstMatrixPtr KstImage::matrix() const {
  if (_inputMatrices.contains(THEMATRIX)) {
    return _inputMatrices[THEMATRIX]; 
  } else {
    return NULL;
  }
}


QString KstImage::xLabel() const {
  if (_inputMatrices.contains(THEMATRIX)) {
    return _inputMatrices[THEMATRIX]->xLabel();  
  } else {
    return QString();
  }
}


QString KstImage::yLabel() const {
  if (_inputMatrices.contains(THEMATRIX)) {
    return _inputMatrices[THEMATRIX]->yLabel();  
  } else {
    return QString();
  }
}


QString KstImage::topLabel() const {
  if (_inputMatrices.contains(THEMATRIX)) {
    return _inputMatrices[THEMATRIX]->label();  
  } else {
    return QString();
  }
}


KstCurveType KstImage::curveType() const {
  return KST_IMAGE;  
}


Kst::DataObjectPtr KstImage::providerDataObject() const {
  KST::matrixList.lock().readLock();
  KstMatrixPtr mp = *KST::matrixList.findTag(matrixTag());
  KST::matrixList.lock().unlock();
  Kst::DataObjectPtr provider = 0L;
  if (mp) {
    mp->readLock();
    provider = kst_cast<Kst::DataObject>(mp->provider());
    mp->unlock();  
  }
  return provider;
}


double KstImage::distanceToPoint(double xpos, double dx, double ypos) const {
  Q_UNUSED(dx)
  // dx is not relevant for double clicks on images - clicks must be inside the image
  if (xpos <= MaxX && xpos >= MinX && ypos <= MaxY && ypos >= MinY) {
    return 0;
  }
  return 1.0E300;
}


void KstImage::paint(const KstCurveRenderContext& context) {
  double Lx = context.Lx, Hx = context.Hx, Ly = context.Ly, Hy = context.Hy;
  double m_X = context.m_X, m_Y = context.m_Y, b_X = context.b_X, b_Y = context.b_Y;
  double x_max = context.x_max, y_max = context.y_max, x_min = context.x_min, y_min = context.y_min;
  bool xLog = context.xLog, yLog = context.yLog;
  double xLogBase = context.xLogBase;
  double yLogBase = context.yLogBase;
  /*Kst*/QPainter* p = context.painter;
  QColor invalid = context.backgroundColor;
  
  double x, y, width, height;
  double img_Lx_pix = 0, img_Ly_pix = 0, img_Hx_pix = 0, img_Hy_pix = 0;

  KstImagePtr image = this;
  
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
      if (image->hasColorMap()) {
        int hXlXDiff = d2i(img_Hx_pix - img_Lx_pix);
        int hYlYDiff = d2i(img_Hy_pix - img_Ly_pix - 1);
        QImage tempImage(hXlXDiff, hYlYDiff, QImage::Format_RGB32);
        for (int i = 0; i < hXlXDiff; ++i) {
          for (int j = 0; j < hYlYDiff; ++j) {
            double new_x, new_y;
            if (xLog) {
              new_x = pow(xLogBase, (i + img_Lx_pix - b_X) / m_X);
            } else {
              new_x = (i + img_Lx_pix - b_X) / m_X;
            }
            if (yLog) {
              new_y = pow(yLogBase, (j + 1 + img_Ly_pix - b_Y) / m_Y);
            } else {
              new_y = (j + 1 + img_Ly_pix - b_Y) / m_Y;
            }
            thisPixel = image->getMappedColor(new_x, new_y);
            if (thisPixel.isValid()) {
              tempImage.setPixel(i, j, thisPixel.rgb());
            } else {
              tempImage.setPixel(i, j, invalid.rgb());
            }
          }
        }
        p->drawImage(d2i(img_Lx_pix), d2i(img_Ly_pix + 1), tempImage);
      }
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
        if (!variableWeight) {
          // + 1 because 0 and 1 are the same width
          p->setPen(QPen(tempColor, image->contourWeight() + 1, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));
        }
        // do the drawing for each contour line
        QList<double> lines = image->contourLines();
        QPoint lastPoint; // used to remember the previous point
        bool hasPrevBottom = false;
        KstMatrixPtr mp = _inputMatrices[THEMATRIX];
        for (int k = 0; k < lines.count(); ++k) {
          double lineK = lines[k];
          if (variableWeight) {
            // + 1 because 0 and 1 are the same width
            p->setPen(QPen(tempColor, k + 1, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));
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
                p->drawLine(topPoint, bottomPoint);
                p->drawLine(rightPoint, leftPoint);
              } else if (numPoints == 3) {
                // draw a V opening to non-intersecting side
                if (!passTop) {
                  p->drawLine(leftPoint, bottomPoint);
                  p->drawLine(bottomPoint, rightPoint);
                } else if (!passLeft) {
                  p->drawLine(topPoint, rightPoint);
                  p->drawLine(rightPoint, bottomPoint);
                } else if (!passBottom) {
                  p->drawLine(leftPoint, topPoint);
                  p->drawLine(topPoint, rightPoint);
                } else {
                  p->drawLine(topPoint, leftPoint);
                  p->drawLine(leftPoint, bottomPoint);
                }
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
                p->drawLine(point1, point2);
              }
            }
          }
        }
      }
    }
  }
}


void KstImage::yRange(double xFrom, double xTo, double* yMin, double* yMax) {
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


void KstImage::paintLegendSymbol(KstPainter *p, const QRect& bound) {
  if (hasColorMap() && !_pal.isEmpty()) {
    int l = bound.left(), r = bound.right(), t = bound.top(), b = bound.bottom();
    // draw the color palette
    for (int i = l; i <= r; i++) {
      int index = (int)floor(static_cast<double>(((i - l) * (_pal.count() - 1))) / (r - l));
      QColor sliceColor = _pal.value(index).rgb();
      p->setPen(QPen(sliceColor, 0));
      p->drawLine(i, t, i, b);
    }
  }
  if (hasContourMap()) {
    // draw a box with contour color
    p->setPen(QPen(_contourColor, 0));
    p->drawRect(bound.left(), bound.top(), bound.width(), bound.height());
  }
}


// vim: ts=2 sw=2 et
