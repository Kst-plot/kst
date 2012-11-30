/***************************************************************************
 *                                                                         *
 *   Copyright : (C) 2012 Peter Kümmel                                     *
 *   email     : syntheticpp@gmx.net                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

//#define KST_SMALL_PRREALLOC

#include "kst_atof.h"
#include "math_kst.h"

#include <QtTest>
#include <QTime>


class kst_atofTest: public QObject
{
    Q_OBJECT
    
public:
    
    kst_atofTest()
    {
    }
   
    void setFormat(const QString& fmt)
    {
      LexicalCast::instance().setUseDotAsDecimalSeparator(true);
      LexicalCast::instance().setTimeFormat(fmt);
    }


    double msecsTo(const QString& time, const QString& fmt)
    {
      const QTime t = QTime::fromString(time, fmt);
      return QTime(0, 0, 0).msecsTo(t) / 1000.0;
    }


    double msecsToDate(const QString& time, const QString& fmt)
    {
      QDateTime t = QDateTime::fromString(time, fmt);
      if (t.isValid()) {
        t.setTimeSpec(Qt::UTC);
  #if QT_VERSION >= 0x040700
        return t.toMSecsSinceEpoch() / 1000.0;
  #else
        return t.toTime_t();
  #endif
      }
      return 0;
    }


private slots:

  void time()
  {
      double ref = msecsTo("12:00:00", "hh:mm:ss");
      
      setFormat("hh:mm:ss");
      QVERIFY(LexicalCast::instance().toDouble("12:00:00") == ref);

      QVERIFY(LexicalCast::instance().toDouble("12:00:00 ") == ref);
      QVERIFY(KST_ISNAN(LexicalCast::instance().toDouble("  12:00:00")));

      QVERIFY(LexicalCast::instance().toDouble("12:00:00xxx") == ref);

      QVERIFY(KST_ISNAN(LexicalCast::instance().toDouble("12:00:0")));
      QVERIFY(KST_ISNAN(LexicalCast::instance().toDouble(" 12:00:00")));
      QVERIFY(KST_ISNAN(LexicalCast::instance().toDouble(" 12:00:00.200")));

      setFormat("hh:mm:ss.zzz");
      QVERIFY(LexicalCast::instance().toDouble("12:00:00.123") == ref + 0.123);
      QVERIFY(LexicalCast::instance().toDouble("12:00:00.123 ") == ref + 0.123);

      setFormat("hh  :  mm  :  ss");
      QVERIFY(LexicalCast::instance().toDouble("12  :  00  :  00") == ref);
      QVERIFY(LexicalCast::instance().toDouble("12  :  00  :  00  ") == ref);
  }


  void dateAndTime()
  {
      double ref = msecsToDate("11.11.2011 12:00:00", "dd.MM.yyyy hh:mm:ss");
      
      setFormat("dd.MM.yyyy hh:mm:ss");
      QVERIFY(LexicalCast::instance().toDouble("11.11.2011 12:00:00") == ref);
      QVERIFY(LexicalCast::instance().toDouble("11.11.2011 12:00:00  ") == ref);
      QVERIFY(KST_ISNAN(LexicalCast::instance().toDouble("11.11.2011 12:00:0")));
      QVERIFY(KST_ISNAN(LexicalCast::instance().toDouble("11.11.2011  12:00:00")));

      setFormat("dd.MM.yyyy  hh:mm:ss");
      QVERIFY(LexicalCast::instance().toDouble("11.11.2011  12:00:00") == ref);
      setFormat("dd.MM.yyyy  XYZ   hh:mm:ss");
      QVERIFY(LexicalCast::instance().toDouble("11.11.2011  XYZ   12:00:00") == ref);
  }


};



QTEST_MAIN(kst_atofTest)



#include "moc_asciiatoftest.cpp"
