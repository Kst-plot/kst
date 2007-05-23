/***************************************************************************
                                  main.cpp
                             -------------------
    begin                : Tue Aug 22 13:46:13 CST 2000
    copyright            : (C) 2000 by Barth Netterfield
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

#include <stdlib.h>
#include <stdio.h>

#include <qstringlist.h>
#include <qfileinfo.h>

#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <kimageio.h>

#include "dialoglauncher-gui.h"
#include "kst.h"
#include "kstcolorsequence.h"
#include "kstdatacollection-gui.h"
#include "kstdataobjectcollection.h"
#include "kstdefaultnames.h"
#include "kstdoc.h"
#include "kstequation.h"
#include "ksthistogram.h"
#include "kst2dplot.h"
#include "kstviewlegend.h"
#include "kstpsd.h"
#include "kstrvector.h"
#include "kstsvector.h"
#include "kstavector.h"
#include "kstvcurve.h"
#include "kstrmatrix.h"
#include "kstimage.h"
#include <defaultprimitivenames.h>


static const char description[] = I18N_NOOP("Kst: a data viewing program.");
static QStringList startupErrors;

static KCmdLineOptions options[] = {
  { "y <Y>",  I18N_NOOP("Field for Y axis (multiple allowed)"), 0 },
  { "z <Z>",  I18N_NOOP("Field for a Z image (multiple allowed)"), 0 },
  { "ye <equation>",  I18N_NOOP("Equation for Y axis (multiple allowed)"), 0 },
  { "E <text>",  I18N_NOOP("Pass argument to extension.  text is of format extensionname:argumentlist"), 0 },
  { "e <E>",  I18N_NOOP("Field for Y errors (multiple allowed)"), 0 },
  { "x <X>",  I18N_NOOP("Field or range for X axis"),        "INDEX"},
  { "xe <X>", I18N_NOOP("X vector for equations x0:x1:n"),   "INDEX"},
  { "p <Y>",  I18N_NOOP("Field for power spectrum (multiple allowed)"), 0},
  { "h <Y>",  I18N_NOOP("Field for histogram (multiple allowed)"), 0},
  { "r <f>",  I18N_NOOP("Sample rate for power spectrum"),      "60.0"},
  { "ru <U>", I18N_NOOP("Units for psd sample rate"),           "Hz"},
  { "yu <U>", I18N_NOOP("Units for y vectors"),                 "V"},
  { "l <P>",  I18N_NOOP("Length of FFTs is 2^P"),               "10"},
  { "f <F0>", I18N_NOOP("First frame to read"),                 "-2"},
  { "n <NS>", I18N_NOOP("Number of frames to read"),            "-2"},
  { "s <NS>", I18N_NOOP("Number of frames to skip each read"),  "-1"},
  { "a",      I18N_NOOP("Apply boxcar filter before skipping frames"),0},
  { "m <NC>", I18N_NOOP("Separate plots arranged in <NC> columns"),0},
  { "d",      I18N_NOOP("Display as points rather than curves"),0},
  { "g",      I18N_NOOP("Provide a legend box"),0},
  { "w <file>",      I18N_NOOP("Display the data wizard"),"<none>"},
  { "print <file>",  I18N_NOOP("Print to file and exit"),"<none>"},
  { "png <file>",  I18N_NOOP("Save as a png file and exit"),"<none>"},
  { "nq",     I18N_NOOP("Bypass the quickstart dialog"), 0},
  {"F <dataFile>", I18N_NOOP("Override a *.kst file's data file with <datafile>"), "|"},
  { "+[Files]", I18N_NOOP("Data files (if -y given) or *.kst file"), 0},
  KCmdLineLastOption
};

struct InType {
  int skip;
  bool doskip;
  bool doave;
  bool dolegend;
  int n;
  int f;
  double rate;
  int len;
  bool has_points;
  bool sep_plots;
  int n_plots;
  int n_cols;
  int n_rows;
  QString VUnits;
  QString RUnits;
};

/****************************************************************/
/*                                                              */
/*        check command line options for simple usage errors    */
/*                                                              */
/****************************************************************/
static void CheckForCMDErrors(KCmdLineArgs *args) {
  bool nOK;

  if (args->getOption("n").toInt(&nOK) == 0) {
    qWarning() << i18n("Exiting because '-n 0' requests vectors with no data in them.") << endl;
    exit(0); // ugly but now safe
  }
  if (args->getOption("n").toInt(&nOK)>0) {
    if (args->getOption("n").toInt(&nOK) < 2*args->getOption("s").toInt(&nOK)) {
      qWarning() << i18n("Requested data skipping would leave vector with fewer than two points.") << endl;
      exit(0); // ugly but now safe
    }
  }
}


/****************************************************************/
/*                                                              */
/*        fill the in struct from the command line args         */
/*                                                              */
/****************************************************************/
static void SetCMDOptions(KCmdLineArgs *args, InType &in, int n_y) {
  bool nOK;

  in.n = args->getOption("n").toInt(&nOK);
  in.f = args->getOption("f").toInt(&nOK);
  in.rate = args->getOption("r").toDouble(&nOK);
  in.len = args->getOption("l").toInt(&nOK);
  if (in.n < -1) {
    in.n = -1;
  }
  if (in.f < -1) {
    in.f = -1;
  }
  if (in.rate <= 0) {
    in.rate = 1.0;
  }
  if (in.len < 3) {
    in.len = 3;
  }
  if (in.len > 23) {
    in.len = 23;
  }

  in.has_points = args->isSet("d");
  in.n_cols = args->getOption("m").toInt(&nOK);
  if (in.n_cols > 0) {
    if (in.n_cols > 10) {
      in.n_cols = 10;
    }
    if (args->count() > 0) {
      in.sep_plots = true;
      in.n_plots = args->count() * (n_y);
    } else if (args->getOptionList("ye").count() > 0) {
      in.sep_plots = true;
      in.n_plots = args->getOptionList("ye").count();
    }
  } else {
    in.n_cols = 1;
    in.n_plots = 1;
    in.sep_plots = false;
  }

  in.n_rows = (in.n_plots-1)/in.n_cols + 1;

  // set skip options
  in.skip = args->getOption("s").toInt(&nOK);
  if (in.skip < 1) {
    in.skip = 0;
    in.doskip = false;
    in.doave = false;
  } else {
    in.doskip = true;
    in.doave = args->isSet("a");
  }

  // set Units for PSDs
  in.VUnits = args->getOption("yu");
  in.RUnits = args->getOption("ru");
  in.dolegend = args->isSet("g");
}

/****************************************************************/
/*                                                              */
/*        create the plots                                      */
/*                                                              */
/****************************************************************/
static void CreatePlots(InType &in, KstTopLevelViewPtr tlv) {
  KstApp *kst = KstApp::inst();
  QString creatingPlots = i18n("Creating plots");

  kst->slotUpdateProgress( 0, 0, creatingPlots );
  int count = in.n_plots;
  int handled = 0;

  for (int i_plot = 0; i_plot < in.n_plots; ++i_plot) {
    Kst2DPlotPtr plot = new Kst2DPlot(KST::suggestPlotName());
    tlv->appendChild(plot.data());

    plot->resizeFromAspect(double(i_plot%in.n_cols)/double(in.n_cols),
                            double(i_plot/in.n_cols)/double(in.n_rows),
                            1.0/double(in.n_cols), 1.0/double(in.n_rows));

    ++handled;
    kst->slotUpdateProgress( count, handled, creatingPlots );
  }

  tlv->setColumns(in.n_cols);
  tlv->setOnGrid(true);

  kst->slotUpdateProgress( 0, 0, QString::null );
}

/******************************************************************/
/*                                                                */
/*        set X range for equations if -xe is specified           */
/*        start:end:Nsamp                                         */
/*                                                                */
/******************************************************************/
static void SetEqXRanges(QString xeqS, double *min, double *max, int *n, bool *ok) {
  *ok = true;
  QStringList fields = xeqS.split( QChar(':') );
  if (fields.count() != 3) {
    *ok = false;
    *n = 2;
    *min = -1;
    *max = 1;
    return;
  }
  QString f;

  f = fields.at(0);
  *min = f.toDouble(ok);
  f = fields.at(1);
  *max = f.toDouble(ok);
  f = fields.at(2);
  *n = f.toInt(ok);

  if (*n < 2) {
    *ok=false;
  }
  if (*min >= *max) {
    *ok = false;
  }
}

/******************************************************************/
/*                                                                */
/*        create vectors, including name and error checking       */
/*        if a vector with the same field and file exists, use it */
/*                                                                */
/******************************************************************/
static KstRVector *GetOrCreateVector(const QString& field, KstDataSourcePtr file, InType &in) {
  int i_v = 0, n_v;
  KstRVector *V;
  KstRVectorList vectorList = kstObjectSubList<KstVector,KstRVector>(KST::vectorList);

  n_v = vectorList.count();

  for (i_v=0; i_v<n_v; i_v++) {
    V = vectorList.at(i_v);
    if (V->field() == field && V->filename() == file->fileName()) {
      return V;
    }
  }

  V = new KstRVector(file, field, KstObjectTag(KST::suggestVectorName(field), file->tag()), in.f, in.n, in.skip, in.doskip, in.doave);
  if (!V->isValid()) {
    if (file->fileType() == "stdin") {
      startupErrors.append(i18n("Failed to create vector '%1' from file '%2'.  Trying again later.").arg(field).arg(file->fileName()));
    } else {
      startupErrors.append(i18n("Failed to create vector '%1' from file '%2'.").arg(field).arg(file->fileName()));
      V = 0L;
    }
  } else {
  }
    

  return V;
}

/****************************************************************/
/*                                                              */
/*        Process Equation: replace fields with vector tags     */
/*        Create vectors that don't exist.                      */
/*                                                              */
/****************************************************************/
static bool NoVectorEq(const QString& eq) {
  return eq.contains('[');
}

static void ProcessEq(QString &eq, KstDataSourcePtr file, InType &in, bool *ok) {
  QString field;
  int i0=0, i1=0;
  KstVector *v;
  *ok = true;

  while ((i0 = eq.indexOf('[', i0))>=0) {
    i1 = eq.indexOf(']', i0);
    if (i1>0) {
      field = eq.mid(i0+1, i1-i0-1);
      v = GetOrCreateVector(field, file, in);
      if (v) {
        eq.replace(i0,i1-i0+1, "["+v->tagName()+"]");
       }
    }
    i0++;
  }
}


static void exitHelper(void) {
  KstApp::doubleCheckCleanup();
}

/****************************************************************/
/*                                                              */
/*        main for kst.  mostly command line handling           */
/*                                                              */
/****************************************************************/
int main(int argc, char *argv[]) {
  int i_file, i_v, i_curve;
  int i_plot;
  QString fullPath;

  KAboutData aboutData("kst", I18N_NOOP("Kst"),
                       KSTVERSION, description, KAboutData::License_GPL,
                       I18N_NOOP("(c) 2000-2006 Barth Netterfield"),
                       0,
                       "http://kst.kde.org/");
  aboutData.addAuthor("Barth Netterfield",
                      I18N_NOOP("Original author and maintainer."),
                      "netterfield@astro.utoronto.ca",
                      "http://omega.astro.utoronto.ca/");
  aboutData.addAuthor("Staikos Computing Services Inc.",
                      I18N_NOOP("Developed for the University of Toronto."),
                      "info@staikos.net",
                      "http://www.staikos.net/");
  aboutData.addAuthor("Sumus Technology Limited",
                      I18N_NOOP("Developed for the University of British Columbia"),
                      "info@sumusltd.com",
                      "http://www.sumusltd.com/");
  aboutData.addAuthor("Rick Chern",
                      I18N_NOOP("University of British Columbia"),
                      "",
                      "");
  aboutData.addAuthor("Nicolas Brisset",
                      "",
                      "",
                      "");
  aboutData.addAuthor("Matthew Truch",
                      "",
                      "http://matt.truch.net/",
                      "matt@truch.net");
  aboutData.addAuthor("Theodore Kisner",
                      "",
                      "tskisner.public@gmail.com",
                      "");

  KCmdLineArgs::init( argc, argv, &aboutData );
  KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.

  KApplication app;

  KstDialogs::replaceSelf(new KstGuiDialogs);
  KstData::replaceSelf(new KstGuiData);
  KstApp::initialize();

  atexit(exitHelper);

  if (qApp->isSessionRestored()) {
    RESTORE(KstApp)
  } else {
    KstApp *kst = new KstApp;
    InType in;
    QColor color;
    KstDataSourcePtr file;
    QByteArrayList ycolList;
    QByteArrayList matrixList;
    QByteArrayList yEqList;
    QByteArrayList psdList;
    QByteArrayList hsList;
    QByteArrayList errorList;
    unsigned int i_ycol;
    QByteArrayList::Iterator hs_string;
    QByteArrayList::Iterator eq_i;
    QByteArrayList::Iterator mat_i;
    bool showQuickStart = false;
    bool showDataWizard = false;
    bool nOK;
    int n_y = 0;
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    CheckForCMDErrors(args);

    QString wizardfile = args->getOption("w");
    QString printfile = args->getOption("print");
    QString pngfile = args->getOption("png");
    bool print_and_exit = false;

    if (printfile != "<none>") {
      print_and_exit = true;
    }
    if (pngfile != "<none>") {
      print_and_exit = true;
    }

    if (!print_and_exit) {
      kst->show();
    }

    // get Y axis columns
    ycolList = args->getOptionList("y");
    matrixList = args->getOptionList("z");
    yEqList = args->getOptionList("ye");
    psdList = args->getOptionList("p");
    hsList = args->getOptionList("h");
    errorList = args->getOptionList("e");

    // y axis or PSD specified, so the files are data files, not kst files.
    n_y = ycolList.count() + psdList.count() + hsList.count() + yEqList.count() + matrixList.count();
    if (n_y > 0) {
      QString creatingEquations = i18n("Creating equations");
      QString creatingCurves = i18n("Creating curves");
      QString creatingPlots = i18n("Creating plots");
      int count;
      int handled;

      kst->slotUpdateProgress( 0, 0, QString::null );

      SetCMDOptions(args, in, n_y);

      KstTopLevelViewPtr tlv = kst->activeView();

      if (!tlv) {
        // if there was no active view then we create one...
        kst->newWindow(false);
        tlv = kst->activeView();
      }

      if (!tlv) {
        qWarning() << i18n("Can't create a view.") << endl;
        return 0;
      }

      CreatePlots(in, tlv);
      Kst2DPlotList plist = kstObjectSubList<KstViewObject, Kst2DPlot>(tlv->children());

      i_plot = 0;
      Kst2DPlotPtr plot = plist.at(i_plot);

      KstVCurveList vcurves = kstObjectSubList<KstBaseCurve,KstVCurve>(plot->Curves);

      // make stand alone equations if there are no files
      if (args->count() < 1) {
        if (!yEqList.isEmpty()) {
          QString eqS;
          double max, min;
          int n;
          bool xeq;

          SetEqXRanges(args->getOption("xe"), &min, &max, &n, &xeq);
          if (xeq) {
            count = yEqList.size();
            handled = 0;
            kst->slotUpdateProgress( count, handled, creatingEquations );

            for (eq_i = yEqList.begin(); eq_i != yEqList.end(); ++eq_i) {
              eqS = *eq_i;
              if (NoVectorEq(eqS)) {
                KstEquationPtr eq = new KstEquation(KST::suggestEQName(eqS), eqS, min, max, n);
                KstVCurvePtr vc = new KstVCurve(KST::suggestCurveName(eq->tag(), true),
                                        eq->vX(), eq->vY(), 0L, 0L, 0L, 0L,
                                        KstColorSequence::next(vcurves,plot->backgroundColor()));
                KST::dataObjectList.lock().writeLock();
                KST::dataObjectList.append(eq.data());
                KST::dataObjectList.append(vc.data());
                KST::dataObjectList.lock().unlock();
                plot->addCurve(vc.data());

                if (in.sep_plots) {
                  i_plot++;
                  if (i_plot < in.n_plots) {
                    plot = plist.at(i_plot);
                  }
                }
              }

              handled++;
              kst->slotUpdateProgress( count, handled, creatingEquations );
            }
          }
        }
      }

      // make the requested curves for each data file
      count = args->count();
      handled = 0;
      kst->slotUpdateProgress( count, handled, creatingCurves );

      for (i_curve = i_v = 0, i_file = 0; i_file < args->count(); i_file++) {
        // make the file
        if (QFile::exists(args->arg(i_file))) {
          fullPath = QFileInfo(args->arg(i_file)).absoluteFilePath();
        } else {
          fullPath = args->arg(i_file);
        }
        file = KstDataSource::loadSource(fullPath);
        if (file) {
          if (!file->isValid() || file->isEmpty()) {
            qWarning() << i18n("No data in file %1.  Trying to continue...").arg(args->arg(i_file)) << endl;
            // The file might get data later!
          }

          KST::dataObjectList.lock().writeLock();
          KST::dataSourceList.append(file);
          KST::dataObjectList.lock().unlock();

          KstRVectorPtr yvector;
          KstRVectorPtr evector;
          KstVCurvePtr curve;
          KstPSDPtr psd;
          KstHistogramPtr hs;
          KstRVectorPtr xvector;

          if (!ycolList.isEmpty()) { // if there are some xy plots
            // make the x axis vector
            xvector = GetOrCreateVector(args->getOption("x"), file, in);
            if (xvector) {
              // make the y axis vectors
              for (i_ycol = 0; i_ycol < ycolList.count(); ++i_ycol ) {
                yvector = GetOrCreateVector((ycolList.at(i_ycol)), file, in);
                if (yvector) {
                  // make the curves
                  color = KstColorSequence::next(vcurves,plot->backgroundColor());
                  curve = new KstVCurve(KST::suggestCurveName(yvector->tag(), false), // FIXME: this was yvector->field(), is this right?
                                      KstVectorPtr(xvector), KstVectorPtr(yvector),
                                      0L, 0L, 0L, 0L, color);
                  if (in.has_points) {
                    curve->setHasPoints(true);
                    curve->setHasLines(false);
                  }

                  if (i_ycol<errorList.count()) {
                    evector = GetOrCreateVector((errorList.at(i_ycol)), file, in);
                    if (evector) {
                      curve->setYError(KstVectorPtr(evector));
                      curve->setYMinusError(KstVectorPtr(evector));
                    }
                  }

                  KST::dataObjectList.lock().writeLock();
                  KST::dataObjectList.append(curve.data());
                  KST::dataObjectList.lock().unlock();
                  plot->addCurve(curve.data());

                  if (in.sep_plots) {
                    plot->setTagName(curve->tag());
                    i_plot++;
                    if (i_plot < in.n_plots) {
                      plot = plist.at(i_plot);
                    }
                  } // end (if they are separate plots)
                }
              } // next y col
            }
          } // end (if there are some xy plots)

          if (!yEqList.isEmpty()) {
            QString eqS;
            double max, min;
            int n;
            bool xeq, eq_ok;

            SetEqXRanges(args->getOption("xe"), &min, &max, &n, &xeq);
            for (eq_i = yEqList.begin(); eq_i != yEqList.end(); ++eq_i) {
              KstEquationPtr eq;

              eqS = *eq_i;
              ProcessEq(eqS, file, in, &eq_ok);

              if (xeq) {
                eq = new KstEquation(KST::suggestEQName(eqS), eqS, min,max,n);
              } else {
                if (!xvector) {
                  xvector = GetOrCreateVector(args->getOption("x"), file, in);
                }
                if (xvector) {
                  eq = new KstEquation(KST::suggestEQName(eqS), eqS, KstVectorPtr(xvector), true);
                }
              }

              if (eq) {
                KstVCurvePtr vc = new KstVCurve(KST::suggestCurveName(eq->tag(), true),
                                      eq->vX(), eq->vY(), 0L, 0L, 0L, 0L,
                                      KstColorSequence::next(vcurves,plot->backgroundColor()));
                KST::dataObjectList.lock().writeLock();
                KST::dataObjectList.append(eq.data());
                KST::dataObjectList.append(vc.data());
                KST::dataObjectList.lock().unlock();
                plot->addCurve(vc.data());

                if (in.sep_plots) {
                  plot->setTagName(eq->tag());
                  i_plot++;
                  if (i_plot <in.n_plots) {
                    plot = plist.at(i_plot);
                  }
                }
              }
            }
          }

          if (psdList.count() > 0) { // if there are some psd plots
            KstRVectorList rvl = kstObjectSubList<KstVector,KstRVector>(KST::vectorList);
            for (QByteArrayList::ConstIterator it = psdList.begin(); it != psdList.end(); ++it) {

              yvector = GetOrCreateVector(*it, file, in);
              if (yvector) {
                color = KstColorSequence::next(vcurves,plot->backgroundColor());

                psd = new KstPSD( KST::suggestPSDName(yvector->tag()), // FIXME: this was yvector->field(), is this right?
                    KstVectorPtr(yvector), in.rate, true, in.len,
                    true, true, in.VUnits, in.RUnits, WindowOriginal);
                KstVCurvePtr vc = new KstVCurve(KST::suggestCurveName(psd->tag(), true),
                    psd->vX(), psd->vY(),
                    0L, 0L, 0L, 0L, color);
                if (in.has_points) {
                  vc->setHasPoints(true);
                  vc->setHasLines(false);
                }
                KST::dataObjectList.lock().writeLock();
                KST::dataObjectList.append(psd.data());
                KST::dataObjectList.append(vc.data());
                KST::dataObjectList.lock().unlock();
                plot->addCurve(vc.data());

                if (in.sep_plots) {
                  plot->setTagName(psd->tag());
                  i_plot++;
                  if (i_plot <in.n_plots) {
                    plot = plist.at(i_plot);
                  }
                }
              }
            } // next psd
          } // end (if there are some psds)

          if (hsList.count() > 0) { // if there are some histograms
            double max, min;
            int N;

            KstRVectorList rvl = kstObjectSubList<KstVector,KstRVector>(KST::vectorList);
            for (hs_string = hsList.begin(); hs_string != hsList.end(); ++hs_string) {
              yvector = GetOrCreateVector(*hs_string, file, in);
              if (yvector) {
                color = KstColorSequence::next(vcurves,plot->backgroundColor());

                KstHistogram::AutoBin(KstVectorPtr(yvector), &N, &max, &min);

                hs = new KstHistogram(KST::suggestHistogramName(yvector->tag()), // FIXME: this was yvector->field(), is this right?
                    KstVectorPtr(yvector), min, max, N, KST_HS_NUMBER);
                KstVCurvePtr vc = new KstVCurve(KST::suggestCurveName(hs->tag(), true),
                    hs->vX(), hs->vY(),
                    0L, 0L, 0L, 0L, color);

                vc->setHasPoints(false);
                vc->setHasLines(false);
                vc->setHasBars(true);
                vc->setBarStyle(1);

                KST::dataObjectList.lock().writeLock();
                KST::dataObjectList.append(hs.data());
                KST::dataObjectList.append(vc.data());
                KST::dataObjectList.lock().unlock();
                plot->addCurve(vc.data());

                if (in.sep_plots) {
                  plot->setTagName(hs->tag());
                  i_plot++;
                  if (i_plot < in.n_plots) {
                    plot = plist.at(i_plot);
                  }
                }
              }
            } // next histogram
          } // end (if there are some histograms)

          if (matrixList.count() > 0) { // if there are some matrixes
            for (mat_i = matrixList.begin(); mat_i != matrixList.end(); ++mat_i) {
              QString tag_name = KST::suggestMatrixName(*mat_i);
              if (!file->isValidMatrix(*mat_i)) {
                startupErrors.append(i18n("Failed to create matrix '%1' from file '%2'.").arg(QString(*mat_i)).arg(file->fileName()));
              }
              KstRMatrixPtr matrix = new KstRMatrix(file, *mat_i,
                  KstObjectTag(tag_name, file->tag()),
                  0,0,-1,-1,false,false,0);
                  // xStart, yStart, xNumSteps, yNumSteps, 
                  //doAve, doSkip, skip);
              
              // Time to create the image from the matrix
              tag_name = KST::suggestImageName(matrix->tag());
              QStringList palList = KPalette::getPaletteList();
              QString pal;
              if (palList.contains("IDL 13 RAINBOW")) {
                pal = QString("IDL 13 RAINBOW");
              } else {
                pal = QString(palList.at(0));
              }
              
              KPalette* newPal = new KPalette(pal);
              KstImagePtr image = new KstImage(tag_name, KstMatrixPtr(matrix), 0.0, 1.0,
                                   true, newPal);
              plot->addCurve(KstBaseCurvePtr(image));
              KST::dataObjectList.lock().writeLock();
              KST::dataObjectList.append(image.data());
              KST::dataObjectList.lock().unlock();
              image = 0L; // drop the reference
              
              if (in.sep_plots) {
                plot->setTagName(matrix->tag());
                i_plot++;
                if (i_plot < in.n_plots) {
                  plot = plist.at(i_plot);
                }
              }
            }
          }
        } else {
          startupErrors.append(i18n("Failed to load file '%1'.").arg(args->arg(i_file)));
        }
        handled++;
        kst->slotUpdateProgress( count, handled, creatingCurves );
      } // next data file

      count = in.n_plots;
      handled = 0;
      kst->slotUpdateProgress( count, handled, creatingPlots );
      for (i_plot = 0; i_plot < in.n_plots; i_plot++) {
        plot = plist.at(i_plot);
        plot->generateDefaultLabels();
      
        // if we have only images in a plot then set the scale mode to AUTO (instead of AUTOBORDER)
        KstImageList images = kstObjectSubList<KstBaseCurve,KstImage>(plot->Curves);
        if (images.count() == plot->Curves.count()) {
          plot->setXScaleMode(AUTO);
          plot->setYScaleMode(AUTO);
        }
    
        if (plot->Curves.count() > 3 || in.dolegend) {
          KstViewLegendPtr vl = plot->getOrCreateLegend();
          vl->resizeFromAspect(0.1, 0.1, 0.2, 0.1);
          vl->setBorderWidth(2);
        }
        handled++;
        kst->slotUpdateProgress( count, handled, creatingPlots );
      }

      kst->slotUpdateProgress( 0, 0, QString::null );
    } else if (args->count() > 0) { // open a kst file
      // some of the options can be overridden
      kst->openDocumentFile(args->arg(0),
          args->getOption("F"),             // override FileName
          args->getOption("n").toInt(&nOK), // override number of frames
          args->getOption("f").toInt(&nOK), // override starting frame
          args->getOption("s").toInt(&nOK), // override skip
          args->isSet("a"),                 // add averaging
          !print_and_exit);                 // delayed
    } else {
      //kst->openDocumentFile();
      showQuickStart = true;
    }

    if (args->isSet("nq")) {
      showQuickStart = false;
    }
    if (args->isSet("w")) {
      showDataWizard = true;
      showQuickStart = false;
    }

    if (printfile != "<none>") {
      kst->forceUpdate();
      kst->immediatePrintToFile(printfile, false);
    }

    if (pngfile != "<none>") {
      kst->forceUpdate();
      kst->immediatePrintToPng(pngfile);
    }

    if (print_and_exit) {
      delete kst;
      return 0;
    } else {
      kst->updateDialogs();

      if (showQuickStart) {
        kst->showQuickStartDialog();
      }
      if (showDataWizard) {
        kst->showDataWizardWithFile(wizardfile);
      }
      for (size_t i = 0; i < startupErrors.size(); ++i) {
        KstDebug::self()->log(startupErrors[i], KstDebug::Error);
      }
      startupErrors.clear();
    }
    int rc = app.exec();
    return rc;
  }

  return app.exec();
}
// vim: ts=2 sw=2 et
