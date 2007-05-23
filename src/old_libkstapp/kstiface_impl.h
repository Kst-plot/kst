/***************************************************************************
                      kstiface_impl.h  -  Part of KST
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
#ifndef KSTIFACE_IMPL_H
#define KSTIFACE_IMPL_H

#include "kstiface.h"

class KstDoc;
class KstApp;

// Keep in sync with kstiface.h

class KstIfaceImpl : virtual public KstIface {
  public:
    KstIfaceImpl(KstDoc *doc, KstApp *app);
    virtual ~KstIfaceImpl();

    virtual void showDataManager();

    virtual QStringList stringList();
    virtual QStringList scalarList();
    virtual QStringList vectorList();
    virtual QStringList objectList();
    virtual QStringList curveList();
    virtual QString activeWindow();
    virtual QStringList windowList();
    virtual QStringList plotList();
    virtual QStringList plotList(const QString& window);
    virtual QStringList pluginList();
    virtual QStringList filterList();

    virtual bool plotEquation(const QString& xvector, const QString& equation, const QString& plotName, const QColor& color);
    virtual bool plotEquation(const QString& xvector, const QString& equation, const QString& plotName);
    virtual bool plotEquation(double start, double end, int numSamples, const QString& equation, const QString& plotName, const QColor& color);
    virtual bool plotEquation(double start, double end, int numSamples, const QString& equation, const QString& plotName);

    virtual QString generateVector(const QString& name, double from, double to, int points);
    virtual QString generateScalar(const QString& name, double value);

    virtual bool saveVector(const QString& vector, const QString& filename);
    virtual QStringList inputVectors(const QString& objectName);
    virtual QStringList inputScalars(const QString& objectName);
    virtual QStringList outputVectors(const QString& objectName);
    virtual QStringList outputScalars(const QString& objectName);
    virtual double scalar(const QString& name);
    virtual bool setScalar(const QString& name, double value);
    virtual const QString& string(const QString& name);
    virtual bool setString(const QString& name, const QString& value);
    virtual double vector(const QString& name, int index);
    virtual bool setVector(const QString& name, int index, double value);
    virtual bool resizeVector(const QString& name, int newSize);
    virtual bool clearVector(const QString& name);
    virtual int vectorSize(const QString& name);

    virtual bool printImage(const QString& windowname, const QString& url);
    virtual bool printImage(const QString& filename);
    virtual bool printPostScript(const QString& windowname, const QString& filename);
    virtual bool printPostScript(const QString& filename);

    virtual QString createWindow(const QString& name);
    virtual QString createPlot(const QString& window, const QString& name);
    virtual QString createPlot(const QString& name);

    virtual bool deletePlot(const QString& window, const QString& name);
    virtual bool deletePlot(const QString& name);

    virtual QStringList plotContents(const QString& name);
    bool addCurveToPlot(KMdiChildView *win, const QString& plot, const QString& curve);
    virtual bool addCurveToPlot(const QString& window, const QString& plot, const QString& curve);
    virtual bool addCurveToPlot(const QString& plot, const QString& curve);
    bool removeCurveFromPlot(KMdiChildView *win, const QString& plot, const QString& curve);
    virtual bool removeCurveFromPlot(const QString& window, const QString& plot, const QString& curve);
    virtual bool removeCurveFromPlot(const QString& plot, const QString& curve);

    virtual QString createCurve(const QString& name, const QString& xVector, const QString& yVector, const QString& xErrorVector, const QString& yErrorVector);
    virtual QString createCurve(const QString& name, const QString& xVector, const QString& yVector, const QString& xErrorVector, const QString& yErrorVector, const QColor& color);

    virtual void reloadVectors();
    virtual void reloadVector(const QString& vector);

    virtual QString loadVector(const QString& file, const QString& field);

    virtual const QString& fileName();
    virtual bool save();
    virtual bool saveAs(const QString& fileName);
    virtual void newFile();
    virtual bool open(const QString& fileName);


    //------------------Suggested functions-------------------------

    virtual bool setMDIMode(int mode); //set the MDI mode

    virtual bool setTileMode(int mode);  //set the tile mode in childframe mode

    virtual bool closeWindow(const QString& windowname);  //close window

    virtual bool activateWindow(const QString& windowname);  //make window active

    virtual bool positionWindow(int topleftX,
        int topleftY,
        int width,
        int height); //position/resize window

    virtual bool maximizePlot(const QString& plotName); //zoom (maximize) plot
    virtual bool unmaximizePlot(const QString& plotName);
    virtual bool toggleMaximizePlot(const QString& plotName);

    virtual bool setPlotAxes(const QString& plotName,
        int XLower,
        int XUpper,
        int YLower,
        int YUpper); //change plot axes

    virtual QString createHistogram(const QString& name,
        const QString& vector,
        double min,
        double max,
        int numBins,
        int normalizationType,
        const QColor& colour); //create histogram and curve
    
    virtual QStringList createHistogram(const QString& name,
                                        const QString& vector,
                                        double min,
                                        double max,
                                        int numBins,
                                        int normalizationType); //create histogram

    virtual QString createPowerSpectrum(const QString & name,
        const QString& vector,
        bool appodize,
        bool removeMean,
        int fftLength,
        const QString& rateUnits,
        double sampleRate,
        const QString& vectorUnits,
        const QColor& colour); //create power spectrum and curve
    
    virtual QStringList createPowerSpectrum(const QString& name,
                                            const QString& vector,
                                            bool appodize,
                                            bool removeMean,
                                            int fftLength,
                                            const QString& rateUnits,
                                            double sampleRate,
                                            const QString& vectorUnits); //create power spectrum

    virtual QString createEvent(const QString& name,
        const QString& expression,
        const QString& description,
        int debugLogType,
        const QString& email); //create event

    virtual QStringList createPlugin(const QString& pluginName,
        const QStringList& scalarInputs,
        const QStringList& vectorInputs);  //create plugin

    virtual QString loadMatrix(const QString& name, const QString& file, const QString& field,
                               int xStart, int yStart, int xNumSteps, int yNumSteps, 
                               int skipFrames, bool boxcarFilter);
  
    virtual QString createGradient(const QString& name, bool xDirection, double zAtMin, double zAtMax, 
                                   int xNumSteps, int yNumSteps, double xMin, double yMin, 
                                   double xStepSize, double yStepSize);

    virtual QString createImage(const QString &name,
        const QString &matrix,
        double lowerZ,
        double upperZ,
        const QString &paletteName,
        int numContours,
        const QColor& contourColor,
        uint imageType); //create image

    virtual bool addPlotMarker(const QString &plotName, double markerValue); //create plotmarker

    virtual bool changeDataFile(const QString& fileName);

    virtual bool changeDataFile(const QString& vector, const QString& fileName);

    virtual bool changeDataFiles(const QStringList& vectors, const QString& fileName);

  private:
    bool changeDataFile(const QString& vector, const QString& fileName, bool update);
    KstDoc *_doc;
    KstApp *_app;
};

#endif
// vim: ts=2 sw=2 et
