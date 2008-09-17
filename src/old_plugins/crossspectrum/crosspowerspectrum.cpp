/***************************************************************************
                   crosspowerspectrum.cpp
                             -------------------
    begin                : 09/08/06
    copyright            : (C) 2006 The University of Toronto
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
#include "crosspowerspectrum.h"

#include <qstylesheet.h>

#include <kdebug.h>
#include <kmessagebox.h>
#include <kgenericfactory.h>

#include <kstdatacollection.h>

#include "crossspectrumdialog_i.h"

#define KSTPSDMAXLEN 27

extern "C" void rdft(int n, int isgn, double *a); //fast fourier transform...

static const QString& VECTOR_ONE = KGlobal::staticQString("Vector One");
static const QString& VECTOR_TWO = KGlobal::staticQString("Vector Two");

//in scalars
static const QString& FFT_LENGTH = KGlobal::staticQString("FFT Length = 2^");
static const QString& SAMPLE_RATE = KGlobal::staticQString("Sample Rate");

static const QString& REAL = KGlobal::staticQString("Cross Spectrum: Real");
static const QString& IMAGINARY = KGlobal::staticQString("Cross Spectrum: Imaginary");
static const QString& FREQUENCY = KGlobal::staticQString("Frequency");

KST_KEY_DATAOBJECT_PLUGIN( crossspectrum )

K_EXPORT_COMPONENT_FACTORY( kstobject_crossspectrum,
    KGenericFactory<CrossPowerSpectrum>( "kstobject_crossspectrum" ) )

CrossPowerSpectrum::CrossPowerSpectrum( QObject */*parent*/, const char */*name*/, const QStringList &/*args*/ )
    : KstDataObject() {
  _typeString = i18n("Plugin");
  _type = "Plugin";
}


CrossPowerSpectrum::~CrossPowerSpectrum() {
}


QString CrossPowerSpectrum::v1Tag() const {
  KstVectorPtr v = v1();
  if (v) {
    return v->tagName();
  }
  return QString::null;
}


QString CrossPowerSpectrum::v2Tag() const {
  KstVectorPtr v = v2();
  if (v) {
    return v->tagName();
  }
  return QString::null;
}


QString CrossPowerSpectrum::fftTag() const {
  KstScalarPtr s = fft();
  if (s) {
    return s->tagName();
  }
  return QString::null;
}


QString CrossPowerSpectrum::sampleTag() const {
  KstScalarPtr s = sample();
  if (s) {
    return s->tagName();
  }
  return QString::null;
}


QString CrossPowerSpectrum::realTag() const {
  KstVectorPtr v = real();
  if (v) {
    return v->tagName();
  }
  return QString::null;
}


QString CrossPowerSpectrum::imaginaryTag() const {
  KstVectorPtr v = imaginary();
  if (v) {
    return v->tagName();
  }
  return QString::null;
}


QString CrossPowerSpectrum::frequencyTag() const {
  KstVectorPtr v = frequency();
  if (v) {
    return v->tagName();
  }
  return QString::null;
}


KstVectorPtr CrossPowerSpectrum::v1() const {
  return *_inputVectors.find(VECTOR_ONE);
}


KstVectorPtr CrossPowerSpectrum::v2() const {
  return *_inputVectors.find(VECTOR_TWO);
}


KstScalarPtr CrossPowerSpectrum::fft() const {
  return *_inputScalars.find(FFT_LENGTH);
}


KstScalarPtr CrossPowerSpectrum::sample() const {
  return *_inputScalars.find(SAMPLE_RATE);
}


KstVectorPtr CrossPowerSpectrum::real() const {
  return *_outputVectors.find(REAL);
}


KstVectorPtr CrossPowerSpectrum::imaginary() const {
  return *_outputVectors.find(IMAGINARY);
}


KstVectorPtr CrossPowerSpectrum::frequency() const {
  return *_outputVectors.find(FREQUENCY);
}


void CrossPowerSpectrum::setV1(KstVectorPtr new_v1) {
  if (new_v1) {
    _inputVectors[VECTOR_ONE] = new_v1;
  } else {
    _inputVectors.remove(VECTOR_ONE);
  }
  setDirty();
}


void CrossPowerSpectrum::setV2(KstVectorPtr new_v2) {
  if (new_v2) {
    _inputVectors[VECTOR_TWO] = new_v2;
  } else {
    _inputVectors.remove(VECTOR_TWO);
  }
  setDirty();
}


void CrossPowerSpectrum::setFFT(KstScalarPtr new_fft) {
  if (new_fft) {
    _inputScalars[FFT_LENGTH] = new_fft;
  } else {
    _inputScalars.remove(FFT_LENGTH);
  }
  setDirty();
}


void CrossPowerSpectrum::setSample(KstScalarPtr new_sample) {
  if (new_sample) {
    _inputScalars[SAMPLE_RATE] = new_sample;
  } else {
    _inputScalars.remove(SAMPLE_RATE);
  }
  setDirty();
}


void CrossPowerSpectrum::setReal(const QString &name) {
  KstWriteLocker blockVectorUpdates(&KST::vectorList.lock());
  KstVectorPtr v = new KstVector(KstObjectTag(name, tag()), 0, this, false);
  _outputVectors.insert(REAL, v);
}


void CrossPowerSpectrum::setImaginary(const QString &name) {
  KstWriteLocker blockVectorUpdates(&KST::vectorList.lock());
  KstVectorPtr v = new KstVector(KstObjectTag(name, tag()), 0, this, false);
  _outputVectors.insert(IMAGINARY, v);
}


void CrossPowerSpectrum::setFrequency(const QString &name) {
  KstWriteLocker blockVectorUpdates(&KST::vectorList.lock());
  KstVectorPtr v = new KstVector(KstObjectTag(name, tag()), 0, this, false);
  _outputVectors.insert(FREQUENCY, v);
}


KstObject::UpdateType CrossPowerSpectrum::update(int updateCounter) {
  bool force = dirty();
  setDirty(false);

  if (KstObject::checkUpdateCounter(updateCounter) && !force) {
    return lastUpdateResult();
  }

  if (!v1() || !v2() || !fft() || !sample()) {
    return setLastUpdateResult(NO_CHANGE);
  }

  bool depUpdated = force;

  depUpdated = UPDATE == v1()->update(updateCounter) || depUpdated;
  depUpdated = UPDATE == v2()->update(updateCounter) || depUpdated;

  depUpdated = UPDATE == fft()->update(updateCounter) || depUpdated;
  depUpdated = UPDATE == sample()->update(updateCounter) || depUpdated;

  crossspectrum();

  vectorRealloced(real(), real()->value(), real()->length());
  real()->setDirty();
  real()->setNewAndShift(real()->length(), real()->numShift());
  real()->update(updateCounter);

  vectorRealloced(imaginary(), imaginary()->value(), imaginary()->length());
  imaginary()->setDirty();
  imaginary()->setNewAndShift(imaginary()->length(), imaginary()->numShift());
  imaginary()->update(updateCounter);

  vectorRealloced(frequency(), frequency()->value(), frequency()->length());
  frequency()->setDirty();
  frequency()->setNewAndShift(frequency()->length(), frequency()->numShift());
  frequency()->update(updateCounter);

  return setLastUpdateResult(depUpdated ? UPDATE : NO_CHANGE);
}


void CrossPowerSpectrum::crossspectrum() {
  KstVectorPtr v1 = *_inputVectors.find(VECTOR_ONE);
  KstVectorPtr v2 = *_inputVectors.find(VECTOR_TWO);

  KstScalarPtr fft = *_inputScalars.find(FFT_LENGTH);
  KstScalarPtr sample = *_inputScalars.find(SAMPLE_RATE);

  KstVectorPtr real = *_outputVectors.find(REAL);
  KstVectorPtr imaginary = *_outputVectors.find(IMAGINARY);
  KstVectorPtr frequency = *_outputVectors.find(FREQUENCY);

  double SR = sample->value(); // sample rate
  double df;
  int i,  xps_len;
  double *a,  *b;
  double mean_a,  mean_b;
  int dv0,  dv1,  v_len;
  int i_subset,  n_subsets;
  int i_samp,  copyLen;
  double norm_factor;

  /* parse fft length */
  xps_len = int( fft->value() - 0.99);
  if ( xps_len > KSTPSDMAXLEN ) xps_len = KSTPSDMAXLEN;
  if ( xps_len<2 ) xps_len = 2;
  xps_len = int ( pow( 2,  xps_len ) );

  /* input vector lengths */
  v_len = ( ( v1->length() < v2->length() ) ?
            v1->length() : v2->length() );
  dv0 = v_len/v1->length();
  dv1 = v_len/v2->length();

  while ( xps_len > v_len ) xps_len/=2;

  // allocate the lengths
  if ( real->length() != xps_len ) {
    real->resize( xps_len, false );
    imaginary->resize( xps_len, false );
    frequency->resize( xps_len, false );
  }

  /* Fill the frequency and zero the xps */
  df = SR/( 2.0*double( xps_len-1 ) );
  for ( i=0; i<xps_len; i++ ) {
    frequency->value()[i] = double( i ) * df;
    real->value()[i] = 0.0;
    imaginary->value()[i] = 0.0;
  }

  /* allocate input arrays */
  int ALen = xps_len * 2;
  a = new double[ALen];
  b = new double[ALen];

  /* do the fft's */
  n_subsets = v_len/xps_len + 1;

  for ( i_subset=0; i_subset<n_subsets; i_subset++ ) {
        /* copy each chunk into a[] and find mean */
    if (i_subset*xps_len + ALen <= v_len) {
      copyLen = ALen;
    } else {
      copyLen = v_len - i_subset*xps_len;
    }
    mean_b = mean_a = 0;
    for (i_samp = 0; i_samp < copyLen; i_samp++) {
      i = ( i_samp + i_subset*xps_len )/dv0;
      mean_a += (
        a[i_samp] = v1->value()[i]
        );
      i = ( i_samp + i_subset*xps_len )/dv1;
      mean_b += (
        b[i_samp] = v2->value()[i]
        );
    }
    if (copyLen>1) {
      mean_a/=(double)copyLen;
      mean_b/=(double)copyLen;
    }

    /* Remove Mean and apodize */
    for (i_samp=0; i_samp<copyLen; i_samp++) {
      a[i_samp] -= mean_a;
      b[i_samp] -= mean_b;
    }

    for (;i_samp < ALen; i_samp++) {
      a[i_samp] = 0.0;
      b[i_samp] = 0.0;
    }

    /* fft */
    rdft(ALen, 1, a);
    rdft(ALen, 1, b);

    /* sum each bin into psd[] */
    real->value()[0] += ( a[0]*b[0] );
    real->value()[xps_len-1] += ( a[1]*b[1] );
    for (i_samp=1; i_samp<xps_len-1; i_samp++) {
      real->value()[i_samp]+= ( a[i_samp*2] * b[i_samp*2] -
                                   a[i_samp*2+1] * b[i_samp*2+1] );
      imaginary->value()[i_samp]+= ( -a[i_samp*2] * b[i_samp*2+1] +
                                   a[i_samp*2+1] * b[i_samp*2] );
    }// (a+ci)(b+di)* = ab+cd +i(-ad + cb)
  }

  /* renormalize */
  norm_factor = 1.0/((double(SR)*double(xps_len))*double(n_subsets));
  for ( i=0; i<xps_len; i++ ) {
    real->value()[i]*=norm_factor;
    imaginary->value()[i]*=norm_factor;
  }

  /* free */
  delete[] b;
  delete[] a;
//   return 0;
}


QString CrossPowerSpectrum::propertyString() const {
  return "crosspowerspectrum";
}


KstDataObjectPtr CrossPowerSpectrum::makeDuplicate(KstDataObjectDataObjectMap&) {
  return 0;
}


void CrossPowerSpectrum::showNewDialog() {
  CrossSpectrumDialogI *dialog = new CrossSpectrumDialogI;
  dialog->show();
}


void CrossPowerSpectrum::showEditDialog() {
  CrossSpectrumDialogI *dialog = new CrossSpectrumDialogI;
  dialog->showEdit(tagName());
}


void CrossPowerSpectrum::load(const QDomElement &e) {
  QDomNode n = e.firstChild();

  while (!n.isNull()) {
    QDomElement e = n.toElement();
    if (!e.isNull()) {
      if (e.tagName() == "tag") {
        setTagName(KstObjectTag::fromString(e.text()));
      } else if (e.tagName() == "ivector") {
        _inputVectorLoadQueue.append(qMakePair(e.attribute("name"), e.text()));
      } else if (e.tagName() == "iscalar") {
        _inputScalarLoadQueue.append(qMakePair(e.attribute("name"), e.text()));
      } else if (e.tagName() == "istring") {
        _inputStringLoadQueue.append(qMakePair(e.attribute("name"), e.text()));
      } else if (e.tagName() == "ovector") {
        KstWriteLocker blockVectorUpdates(&KST::vectorList.lock());
        KstVectorPtr v;
        if (e.attribute("scalarList", "0").toInt()) {
          v = new KstVector(KstObjectTag(e.text(), tag()), 0, this, true);
        } else {
          v = new KstVector(KstObjectTag(e.text(), tag()), 0, this, false);
        }
        _outputVectors.insert(e.attribute("name"), v);
      } else if (e.tagName() == "oscalar") {
        KstWriteLocker blockScalarUpdates(&KST::scalarList.lock());
        KstScalarPtr sp = new KstScalar(KstObjectTag(e.text(), tag()), this);
        _outputScalars.insert(e.attribute("name"), sp);
      } else if (e.tagName() == "ostring") {
        KstWriteLocker blockStringUpdates(&KST::stringList.lock());
        KstStringPtr sp = new KstString(KstObjectTag(e.text(), tag()), this);
        _outputStrings.insert(e.attribute("name"), sp);
      }
    }
    n = n.nextSibling();
  }
}


void CrossPowerSpectrum::save(QTextStream& ts, const QString& indent) {
  QString l2 = indent + "  ";
  ts << indent << "<plugin name=\"Cross Power Spectrum\">" << endl;
  ts << l2 << "<tag>" << QStyleSheet::escape(tagName()) << "</tag>" << endl;
  for (KstVectorMap::Iterator i = _inputVectors.begin(); i != _inputVectors.end(); ++i) {
    ts << l2 << "<ivector name=\"" << QStyleSheet::escape(i.key()) << "\">"
        << QStyleSheet::escape(i.data()->tagName())
        << "</ivector>" << endl;
  }
  for (KstScalarMap::Iterator i = _inputScalars.begin(); i != _inputScalars.end(); ++i) {
    ts << l2 << "<iscalar name=\"" << QStyleSheet::escape(i.key()) << "\">"
        << QStyleSheet::escape(i.data()->tagName())
        << "</iscalar>" << endl;
  }
  for (KstStringMap::Iterator i = _inputStrings.begin(); i != _inputStrings.end(); ++i) {
    ts << l2 << "<istring name=\"" << QStyleSheet::escape(i.key()) << "\">"
        << QStyleSheet::escape(i.data()->tagName())
        << "</istring>" << endl;
  }
  for (KstVectorMap::Iterator i = _outputVectors.begin(); i != _outputVectors.end(); ++i) {
    ts << l2 << "<ovector name=\"" << QStyleSheet::escape(i.key());
    if (i.data()->isScalarList()) {
      ts << "\" scalarList=\"1";
    }
    ts << "\">" << QStyleSheet::escape(i.data()->tagName())
        << "</ovector>" << endl;
  }
  for (KstScalarMap::Iterator i = _outputScalars.begin(); i != _outputScalars.end(); ++i) {
    ts << l2 << "<oscalar name=\"" << QStyleSheet::escape(i.key()) << "\">"
        << QStyleSheet::escape(i.data()->tagName())
        << "</oscalar>" << endl;
  }
  for (KstStringMap::Iterator i = _outputStrings.begin(); i != _outputStrings.end(); ++i) {
    ts << l2 << "<ostring name=\"" << QStyleSheet::escape(i.key()) << "\">"
        << QStyleSheet::escape(i.data()->tagName())
        << "</ostring>" << endl;
  }
  ts << indent << "</plugin>" << endl;
}

#include "crosspowerspectrum.moc"
// vim: ts=2 sw=2 et
