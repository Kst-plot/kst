/***************************************************************************
                         kstiface.h  -  Part of KST
                             -------------------
    begin                : Thu Jun 19 2003
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
#ifndef KSTIFACE_H
#define KSTIFACE_H

#include <QtDBus/QtDBus>

#include <qcolor.h>
#include <qstringlist.h>  //Qt 3.1.2 KDE 3.1.4


// Warning: If you change something here, you could break existing scripts.

class KstIface : virtual public QObject {
  Q_CLASSINFO("D-Bus Interface", "org.kde.KstIface")
public Q_SLOTS:
  Q_SCRIPTABLE virtual void showDataManager() = 0;

  Q_SCRIPTABLE virtual QStringList stringList() = 0;
  Q_SCRIPTABLE virtual QStringList scalarList() = 0;
  Q_SCRIPTABLE virtual QStringList vectorList() = 0;
  Q_SCRIPTABLE virtual QStringList objectList() = 0;
  Q_SCRIPTABLE virtual QStringList curveList() = 0;
  Q_SCRIPTABLE virtual QString activeWindow() = 0;
  Q_SCRIPTABLE virtual QStringList windowList() = 0;
  Q_SCRIPTABLE virtual QStringList plotList() = 0;
  Q_SCRIPTABLE virtual QStringList plotList(const QString& window) = 0;
  Q_SCRIPTABLE virtual QStringList pluginList() = 0;
  Q_SCRIPTABLE virtual QStringList filterList() = 0;


  Q_SCRIPTABLE virtual QStringList inputVectors(const QString& objectName) = 0;
  Q_SCRIPTABLE virtual QStringList inputScalars(const QString& objectName) = 0;
  Q_SCRIPTABLE virtual QStringList outputVectors(const QString& objectName) = 0;
  Q_SCRIPTABLE virtual QStringList outputScalars(const QString& objectName) = 0;

  Q_SCRIPTABLE virtual double scalar(const QString& name) = 0;
  Q_SCRIPTABLE virtual bool setScalar(const QString& name, double value) = 0;

  Q_SCRIPTABLE virtual const QString& string(const QString& name) = 0;
  Q_SCRIPTABLE virtual bool setString(const QString& name, const QString& value) = 0;

  Q_SCRIPTABLE virtual double vector(const QString& name, int index) = 0;
  Q_SCRIPTABLE virtual bool setVector(const QString& name, int index, double value) = 0;
  Q_SCRIPTABLE virtual bool resizeVector(const QString& name, int newSize) = 0;
  Q_SCRIPTABLE virtual bool clearVector(const QString& name) = 0;
  Q_SCRIPTABLE virtual int vectorSize(const QString& name) = 0;

  Q_SCRIPTABLE virtual QString generateVector(const QString& name, double from, double to, int points) = 0;
  Q_SCRIPTABLE virtual QString generateScalar(const QString& name, double value) = 0;

  Q_SCRIPTABLE virtual bool plotEquation(const QString& xvector, const QString& equation, const QString& plotName, const QColor& color) = 0;
  Q_SCRIPTABLE virtual bool plotEquation(const QString& xvector, const QString& equation, const QString& plotName) = 0;
  Q_SCRIPTABLE virtual bool plotEquation(double start, double end, int numSamples, const QString& equation, const QString& plotName, const QColor& color) = 0;
  Q_SCRIPTABLE virtual bool plotEquation(double start, double end, int numSamples, const QString& equation, const QString& plotName) = 0;

  Q_SCRIPTABLE virtual bool saveVector(const QString& vector, const QString& filename) = 0;

  Q_SCRIPTABLE virtual bool printImage(const QString& windowname, const QString& url) = 0;
  Q_SCRIPTABLE virtual bool printImage(const QString& filename) = 0;
  Q_SCRIPTABLE virtual bool printPostScript(const QString& windowname, const QString& filename) = 0;
  Q_SCRIPTABLE virtual bool printPostScript(const QString& filename) = 0;

  Q_SCRIPTABLE virtual QString createWindow(const QString& name) = 0;
  Q_SCRIPTABLE virtual QString createPlot(const QString& window, const QString& name) = 0;
  Q_SCRIPTABLE virtual QString createPlot(const QString& name) = 0;

  Q_SCRIPTABLE virtual bool deletePlot(const QString& window, const QString& name) = 0;
  Q_SCRIPTABLE virtual bool deletePlot(const QString& name) = 0;

  Q_SCRIPTABLE virtual QStringList plotContents(const QString& name) = 0;
  Q_SCRIPTABLE virtual bool addCurveToPlot(const QString& window, const QString& plot, const QString& curve) = 0;
  Q_SCRIPTABLE virtual bool addCurveToPlot(const QString& plot, const QString& curve) = 0;
  Q_SCRIPTABLE virtual bool removeCurveFromPlot(const QString& window, const QString& plot, const QString& curve) = 0;
  Q_SCRIPTABLE virtual bool removeCurveFromPlot(const QString& plot, const QString& curve) = 0;

  Q_SCRIPTABLE virtual QString createCurve(const QString& name, const QString& xVector, const QString& yVector, const QString& xErrorVector, const QString& yErrorVector, const QColor& color) = 0; 
  Q_SCRIPTABLE virtual QString createCurve(const QString& name, const QString& xVector, const QString& yVector, const QString& xErrorVector, const QString& yErrorVector) = 0; 

  Q_SCRIPTABLE virtual void reloadVectors() = 0;
  Q_SCRIPTABLE virtual void reloadVector(const QString& vector) = 0;

  Q_SCRIPTABLE virtual QString loadVector(const QString& file, const QString& field) = 0;

  Q_SCRIPTABLE virtual const QString& fileName() = 0;
  Q_SCRIPTABLE virtual bool save() = 0;
  Q_SCRIPTABLE virtual bool saveAs(const QString& fileName) = 0;
  Q_SCRIPTABLE virtual void newFile() = 0;
  Q_SCRIPTABLE virtual bool open(const QString& fileName) = 0;

  //Sets the window MDI mode
  //mode 1=Toplevel, 2=Childframe, 3=Tab Page, 4=IDEAl Mode
  Q_SCRIPTABLE virtual bool setMDIMode(int mode) = 0;

  //set tile mode in childframe mode
  //mode 1=cascade
  //     2=cascade maximized
  //     3=expand vertically
  //     4=expand horizontally
  //     5=tile non-overlapped
  //     6=tile overlapped
  //     7=tile vertically
  Q_SCRIPTABLE virtual bool setTileMode(int mode) = 0;

  //close window
  Q_SCRIPTABLE virtual bool closeWindow(const QString& windowName) = 0;

  //make window active
  Q_SCRIPTABLE virtual bool activateWindow(const QString& windowName) = 0;

  //position/resize window (relative to kst window)
  Q_SCRIPTABLE virtual bool positionWindow(int topLeftX,
      int topLeftY,
      int width,
      int height) = 0;

  //zoom (maximize) plot
  Q_SCRIPTABLE virtual bool maximizePlot(const QString& plotName) = 0;
  Q_SCRIPTABLE virtual bool unmaximizePlot(const QString& plotName) = 0;
  Q_SCRIPTABLE virtual bool toggleMaximizePlot(const QString& plotName) = 0;

  //change plot axes
  Q_SCRIPTABLE virtual bool setPlotAxes(const QString& plotName,
      int xLower,
      int xUpper,
      int yLower,
      int yUpper) = 0;

  //create new histogram object and curve from histogram.  Will not be plotted.
  //normalizationType 1 = number
  //                  2 = percent
  //                  3 = fraction
  //                  4 = maximum 1.0
  //      anything else = number
  //returns the name of the histogram, or empty QString if not successful
  Q_SCRIPTABLE virtual QString createHistogram(const QString& name,
      const QString& vector,
      double min,
      double max,
      int numBins,
      int normalizationType,
      const QColor& color) = 0;
  
  //create new histogram object.  No curve is created.
  //normalizationType 1 = number
  //                  2 = percent
  //                  3 = fraction
  //                  4 = maximum 1.0
  //      anything else = number
  //On success, returns in order: name of the histogram, X slave vector, and Y slave vector
  //On failure, returns empty list
  Q_SCRIPTABLE virtual QStringList createHistogram(const QString& name,
                                  const QString& vector,
                                  double min,
                                  double max,
                                  int numBins,
                                  int normalizationType) = 0;

  //create new power spectrum and curve from power spectrum.  Will not be plotted.
  //returns the name of the ps, or empty QString if not successful
  Q_SCRIPTABLE virtual QString createPowerSpectrum(const QString& name,
      const QString& vector,
      bool appodize,
      bool removeMean,
      int fftLength,
      const QString& rateUnits,
      double sampleRate,
      const QString& vectorUnits,
      const QColor& color) = 0;
  
  //create new power specturm.  No curve is created.  
  //On success, returns in order: name of power spectrum, X slave vector, and Y slave vector
  //On failure, returns empty list
  Q_SCRIPTABLE virtual QStringList createPowerSpectrum(const QString& name,
                                          const QString& vector,
                                          bool appodize,
                                          bool removeMean,
                                          int fftLength,
                                          const QString& rateUnits,
                                          double sampleRate,
                                          const QString& vectorUnits) = 0;

  //create new event.
  //returns the name of the event, or empty QString if not successful.
  //debugLogType 1 = Notice
  //             2 = Warning
  //             3 = Error
  // anything else = no debug logging
  //empty email = no email notification
  Q_SCRIPTABLE virtual QString createEvent(const QString& name,
      const QString& expression,
      const QString& description,
      int debugLogType,
      const QString& email) = 0;

  //create new KST plugin object.
  //This function does not work with dcop command-line tool due to use of
  //QStringList parameters
  //returns output vector names followed by output scalar names
  //returns empty QString if not successful
  Q_SCRIPTABLE virtual QStringList createPlugin(const QString& pluginName,
                            const QStringList& scalarInputs,
                            const QStringList& vectorInputs) = 0;

  //load new KST Matrix from file
  //returns the name of the matrix, or empty QString if not successful
  //xStart - starting x frame to read, -1 to count from end
  //yStart - starting y frame to read, -1 to count from end
  //xNumSteps - number of x frames to read, -1 to read to end
  //yNumSteps - number of y frames to read, -1 to read to end
  //skipFrames - read 1 sample per skipFrames, -1 to read all samples in range
  //boxcarFilter - perform boxcar filtering when skipping frames
  Q_SCRIPTABLE virtual QString loadMatrix(const QString& name, const QString& file, const QString& field,
                             int xStart, int yStart, int xNumSteps, int yNumSteps, 
                             int skipFrames, bool boxcarFilter) = 0;
  
  //create a new gradient matrix
  //xDirection - true to have gradient along x axis, false to have gradient along y axis
  //zAtMin, zAtMax - gradient values at minimum and maximum 
  //xNumSteps, yNumSteps - length and width of the matrix 
  //xMin, yMin - the minimum coordinates of the matrix
  //xStepSize, yStepSize - the dimensions of each block of the matrix
  Q_SCRIPTABLE virtual QString createGradient(const QString& name, bool xDirection, double zAtMin, double zAtMax, 
                                 int xNumSteps, int yNumSteps, double xMin, double yMin, 
                                 double xStepSize, double yStepSize) = 0;

  //create a new KST image
  //returns the name of the image, or empty QString if not successful
  //imageType 0 = colormap only
  //          1 = contour map only
  //          2 = both colormap and contour map
  //if imageType == 0 or 1, unnneeded parameters are ignored
  Q_SCRIPTABLE virtual QString createImage(const QString &name,
                              const QString &matrix,
                              double lowerZ,
                              double upperZ,
                              const QString &paletteName,
                              int numContours,
                              const QColor& contourColor,
                              uint imageType) = 0;

  //create a plot marker
  //returns false if plotName is not valid or markerValue already exists, else returns true
  Q_SCRIPTABLE virtual bool addPlotMarker(const QString &plotName, double markerValue) = 0;

  Q_SCRIPTABLE virtual bool changeDataFile(const QString& fileName) = 0;

  Q_SCRIPTABLE virtual bool changeDataFile(const QString& vector, const QString& fileName) = 0;

  Q_SCRIPTABLE virtual bool changeDataFiles(const QStringList& vectors, const QString& fileName) = 0;

};

#endif
// vim: ts=2 sw=2 et
