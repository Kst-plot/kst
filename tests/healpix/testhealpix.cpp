/*
 *  Copyright 2005, Theodore Kisner
 *  Licensed under GPL.
 */

#include <qmap.h>
#include <kconfig.h>
#include <ksttestcase.h>
#include <healpix_source.h>
#include <ksdebug.h>

#define XDIM 1200
#define YDIM 900
#define DEGRADE 1

QString myName;

typedef QMap<QString,QString> StringMap;

static void exitHelper() {
  KST::vectorList.clear();
  KST::scalarList.clear();
  KST::dataObjectList.clear();
}

int rc = KstTestSuccess;

#define doTest(x) testAssert(x, QString("Line %1").arg(__LINE__))
#define doTestD(x, y) testAssert(x, QString("%1: %2").arg(__LINE__).arg(y))

void testAssert(bool result, const QString& text = "Unknown") {
  if (!result) {
    KstTestFailed();
    printf("Test [%s] failed.\n", text.latin1());
  }
}

extern "C" {
KstDataSource *create_healpix(KConfig *cfg, const QString& filename, const QString& type);

KstDataSource *load_healpix(KConfig *cfg, const QString& filename, const QString& type, const QDomElement& e);

int understands_healpix(KConfig *cfg, const QString& filename);

QStringList matrixList_healpix(KConfig *cfg, const QString& filename, const QString& type, QString *typeSuggestion, bool *complete);

QStringList fieldList_healpix(KConfig *cfg, const QString& filename, const QString& type, QString *typeSuggestion, bool *complete);
}

void doTests() {
  // the data file to use.
  // first determine the path used to call this program,
  // and then prepend that to the data file name.
  QString path = myName.section('/', 0, -3);
  QString datafile = path;
  datafile.append("/healpix_example_sm.fits");
  
  // create a temporary config file to use
  QString cfgfile = "testhealpix.temp";
  KConfig *cfg = new KConfig(cfgfile, false, false);
  cfg->setGroup("Healpix General");
  cfg->setGroup(datafile);
  cfg->writeEntry("Matrix X Dimension", XDIM);
  cfg->writeEntry("Matrix Y Dimension", YDIM);
  cfg->writeEntry("Theta Autoscale", true);
  cfg->writeEntry("Theta Units", 0);
  cfg->writeEntry("Theta Min", 0.0);
  cfg->writeEntry("Theta Max", 1.0);
  cfg->writeEntry("Phi Autoscale", true);
  cfg->writeEntry("Phi Units", 0);
  cfg->writeEntry("Phi Min", 0.0);
  cfg->writeEntry("Phi Max", 1.0);
  cfg->writeEntry("Vector Theta", 1);
  cfg->writeEntry("Vector Phi", 2);
  cfg->writeEntry("Vector Degrade Factor", DEGRADE);
  cfg->writeEntry("Vector Magnitude Autoscale", true);
  cfg->writeEntry("Vector Max Magnitude", 1.0);
  cfg->writeEntry("Vector is QU", true);
  
  // use the C functions to test for healpix support
  int verstehen = understands_healpix(cfg, datafile);
  if (verstehen) {
    kstdDebug() << "Data file " << datafile << " is supported" << endl;
    QString suggestion;
    bool complete;
    
    QStringList matrices = matrixList_healpix(cfg, datafile, "HEALPIX", &suggestion, &complete);
    kstdDebug() << "Available matrices are:" << endl;
    for ( QStringList::Iterator it = matrices.begin(); it != matrices.end(); ++it ) {
      kstdDebug() << "  " << *it << endl;
    }
    kstdDebug() << "  suggestion = " << suggestion << endl;
    kstdDebug() << "  complete = " << complete << endl;

    QStringList fields = fieldList_healpix(cfg, datafile, "HEALPIX", &suggestion, &complete);
    kstdDebug() << "Available fields are:" << endl;
    for ( QStringList::Iterator it = fields.begin(); it != fields.end(); ++it ) {
      kstdDebug() << "  " << *it << endl;
    }
    kstdDebug() << "  suggestion = " << suggestion << endl;
    kstdDebug() << "  complete = " << complete << endl;
    
    // actually create HealpixSource
    HealpixSource *hpx = new HealpixSource(cfg, datafile, "HEALPIX");
    
    // test that saveConfig produces the same as the 
    // original input configuration
    QString cfgfile2 = "testhealpix.temp2";
    KConfig *chk = new KConfig(cfgfile2, false, false);
    hpx->saveConfig(chk);
    StringMap cfgmap = cfg->entryMap(datafile);
    StringMap chkmap = chk->entryMap(datafile);
    
    /*
    kstdDebug() << cfgmap["Matrix X Dimension"] << " " << chkmap["Matrix X Dimension"] << endl;
    kstdDebug() << cfgmap["Matrix Y Dimension"] << " " << chkmap["Matrix Y Dimension"] << endl;
    kstdDebug() << cfgmap["Theta Autoscale"] << " " << chkmap["Theta Autoscale"] << endl;
    kstdDebug() << cfgmap["Theta Units"] << " " << chkmap["Theta Units"] << endl;
    kstdDebug() << cfgmap["Theta Min"] << " " << chkmap["Theta Min"] << endl;
    kstdDebug() << cfgmap["Theta Max"] << " " << chkmap["Theta Max"] << endl;
    kstdDebug() << cfgmap["Phi Autoscale"] << " " << chkmap["Phi Autoscale"] << endl;
    kstdDebug() << cfgmap["Phi Units"] << " " << chkmap["Phi Units"] << endl;
    kstdDebug() << cfgmap["Phi Min"] << " " << chkmap["Phi Min"] << endl;
    kstdDebug() << cfgmap["Phi Max"] << " " << chkmap["Phi Max"] << endl;
    kstdDebug() << cfgmap["Vector Theta"] << " " << chkmap["Vector Theta"] << endl;
    kstdDebug() << cfgmap["Vector Phi"] << " " << chkmap["Vector Phi"] << endl;
    kstdDebug() << cfgmap["Vector Degrade Factor"] << " " << chkmap["Vector Degrade Factor"] << endl;
    kstdDebug() << cfgmap["Vector Magnitude Autoscale"] << " " << chkmap["Vector Magnitude Autoscale"] << endl;
    kstdDebug() << cfgmap["Vector Max Magnitude"] << " " << chkmap["Vector Max Magnitude"] << endl;
    kstdDebug() << cfgmap["Vector is QU"] << " " << chkmap["Vector is QU"] << endl;
    */
    
    if (cfgmap["Matrix X Dimension"] != chkmap["Matrix X Dimension"]) {
      QString msg = "Matrix X Dimension integrity";
      testAssert(false, msg);
    }
    if (cfgmap["Matrix Y Dimension"] != chkmap["Matrix Y Dimension"]) {
      QString msg = "Matrix Y Dimension integrity";
      testAssert(false, msg);
    }
    if (cfgmap["Theta Autoscale"] != chkmap["Theta Autoscale"]) {
      QString msg = "Theta Autoscale integrity";
      testAssert(false, msg);
    }
    if (cfgmap["Theta Units"] != chkmap["Theta Units"]) {
      QString msg = "Theta Units integrity";
      testAssert(false, msg);
    }
    if (cfgmap["Theta Min"] != chkmap["Theta Min"]) {
      QString msg = "Theta Min integrity";
      testAssert(false, msg);
    }
    if (cfgmap["Theta Max"] != chkmap["Theta Max"]) {
      QString msg = "Theta Max integrity";
      testAssert(false, msg);
    }
    if (cfgmap["Phi Autoscale"] != chkmap["Phi Autoscale"]) {
      QString msg = "Phi Autoscale integrity";
      testAssert(false, msg);
    }
    if (cfgmap["Phi Units"] != chkmap["Phi Units"]) {
      QString msg = "Phi Units integrity";
      testAssert(false, msg);
    }
    if (cfgmap["Phi Min"] != chkmap["Phi Min"]) {
      QString msg = "Phi Min integrity";
      testAssert(false, msg);
    }
    if (cfgmap["Phi Max"] != chkmap["Phi Max"]) {
      QString msg = "Phi Max integrity";
      testAssert(false, msg);
    }
    if (cfgmap["Vector Theta"] != chkmap["Vector Theta"]) {
      QString msg = "Vector Theta integrity";
      testAssert(false, msg);
    }
    if (cfgmap["Vector Phi"] != chkmap["Vector Phi"]) {
      QString msg = "Vector Phi integrity";
      testAssert(false, msg);
    }
    if (cfgmap["Vector Degrade Factor"] != chkmap["Vector Degrade Factor"]) {
      QString msg = "Vector Degrade Factor integrity";
      testAssert(false, msg);
    }
    if (cfgmap["Vector Magnitude Autoscale"] != chkmap["Vector Magnitude Autoscale"]) {
      QString msg = "Vector Magnitude Autoscale integrity";
      testAssert(false, msg);
    }
    if (cfgmap["Vector Max Magnitude"] != chkmap["Vector Max Magnitude"]) {
      QString msg = "Vector Max Magnitude integrity";
      testAssert(false, msg);
    }
    if (cfgmap["Vector is QU"] != chkmap["Vector is QU"]) {
      QString msg = "Vector is QU integrity";
      testAssert(false, msg);
    }
    kstdDebug() << "Save/Load config is consistent." << endl;
    
    // print _metaData and compute NSIDE and number
    // of samples in the vectors
    int nside = 0;
    int nvec;
    QString key;
    QString data;
    kstdDebug() << "Checking metaData:" << endl;
    StringMap metamap = hpx->metaData();
    for ( StringMap::Iterator it = metamap.begin(); it != metamap.end(); ++it ) {
      key = it.key();
      data = it.data();
      kstdDebug() << "  " << key.latin1() << " = " << data.latin1() << endl;
      if (key == "NSIDE") {
        nside = data.toInt();
      }
    }
    kstdDebug() << "Data file has nside = " << nside << endl;
    testAssert(nside != 0, "data file NSIDE");
    for (int i = 0; i < DEGRADE; i++) {
      nside = (int)(nside/2);
    }
    nvec = 12*nside*nside;
    kstdDebug() << "Degraded vectorfield has nside = " << nside << " and " << nvec << " full-sphere pixels" << endl;
    
    // check that all returned fields are valid, and that
    // optionally field number names are valid.
    kstdDebug() << "Checking matrix validity:" << endl;
    int num = 1;
    int xdim, ydim, nframe, sampframe;
    for ( QStringList::Iterator it = matrices.begin(); it != matrices.end(); ++it ) {
      if (hpx->isValidMatrix(*it)) {
        kstdDebug() << "  \"" << *it << "\" is VALID" << endl;
        hpx->matrixDimensions(*it, &xdim, &ydim);
        kstdDebug() << "    and has dimensions " << xdim << "x" << ydim << endl;
        testAssert((xdim == XDIM)&&(ydim == YDIM), "dimension integrity");
      } else {
        QString msg = (*it);
        msg.append(" validity");
        testAssert(false, msg);
      }
      QString numfield = QString("%1").arg(num);
      if (hpx->isValidMatrix(numfield)) {
        kstdDebug() << "  \"" << numfield << "\" is VALID" << endl;
        hpx->matrixDimensions(numfield, &xdim, &ydim);
        kstdDebug() << "    and has dimensions " << xdim << "x" << ydim << endl;
        testAssert((xdim == XDIM)&&(ydim == YDIM), "dimension integrity");
      } else {
        QString msg = numfield;
        msg.append(" validity");
        testAssert(false, msg);
      }
      num++;
    }
    kstdDebug() << "Checking field validity:" << endl;
    num = 1;
    for ( QStringList::Iterator it = fields.begin(); it != fields.end(); ++it ) {
      if (hpx->isValidField(*it)) {
        kstdDebug() << "  \"" << *it << "\" is VALID" << endl;
        nframe = hpx->frameCount(*it);
        sampframe = hpx->samplesPerFrame(*it);
        kstdDebug() << "    and has " << nframe << " frames of " << sampframe << " sample(s) each" << endl;
        testAssert(sampframe == 1, "samples per frame");
        testAssert(nframe == nvec, "number of frames");
      } else {
        QString msg = (*it);
        msg.append(" validity");
        testAssert(false, msg);
      }
      QString numfield = QString("%1").arg(num);
      if (hpx->isValidField(numfield)) {
        kstdDebug() << "  \"" << numfield << "\" is VALID" << endl;
        nframe = hpx->frameCount(numfield);
        sampframe = hpx->samplesPerFrame(numfield);
        kstdDebug() << "    and has " << nframe << " frames of " << sampframe << " sample(s) each" << endl;
        testAssert(sampframe == 1, "samples per frame");
        testAssert(nframe == nvec, "number of frames");
      } else {
        QString msg = numfield;
        msg.append(" validity");
        testAssert(false, msg);
      }
      num++;
    }
    
    // check reset function
    if (hpx->reset()) {
      kstdDebug() << "Reset function is implemented." << endl;
    } else {
      testAssert(false, "reset");
    }
        
    
    
    
  } else {
    testAssert(false, "understanding");
  }
  
}


int main(int argc, char **argv) {
  atexit(exitHelper);

  myName = argv[0];
  KApplication app(argc, argv, "testhealpix", false, false);

  doTests();

  exitHelper(); // before app dies
  if (rc == KstTestSuccess) {
    printf("All tests passed!\n");
  }
  return -rc;
}

// vim: ts=2 sw=2 et
