/***************************************************************************
 *                                                                         *
 *   Copyright : (C) 2003 The University of Toronto                        *
 *   email     : netterfield@astro.utoronto.ca                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ASCII_SOURCE_H
#define ASCII_SOURCE_H

#include "asciidatareader.h"
#include "asciisourceconfig.h"

#include "datasource.h"
#include "dataplugin.h"

#include <QTime>
#include <QElapsedTimer>


class QFile;
class DataInterfaceAsciiString;
class DataInterfaceAsciiVector;
class QProgressBar;
class QLabel;

class AsciiSource : public Kst::DataSource
{
  Q_OBJECT

  public:
    AsciiSource(Kst::ObjectStore *store, QSettings *cfg, const QString& filename, const QString& type, const QDomElement& e = QDomElement());
    ~AsciiSource();

    bool initRowIndex(QFile *file);

    UpdateType internalDataSourceUpdate(bool read_completely);

    virtual UpdateType internalDataSourceUpdate();

    void prepareRead(int count);
    void readingDone();
    int readField(double *v, const QString &field, int s, int n);

    QString fileType() const;
    void setUpdateType(UpdateCheckType);

    void save(QXmlStreamWriter &s);
    void parseProperties(QXmlStreamAttributes &properties);

    bool isEmpty() const;

    bool supportsTimeConversions() const;

    int sampleForTime(double ms, bool *ok);

    int sampleForTime(const QDateTime& time, bool *ok);

    bool isTime(const QString &field) const;

    virtual void reset();
    void updateLists();

    virtual QString typeString() const;

    static const QString asciiTypeKey();

    static QStringList fieldListFor(const QString& filename, AsciiSourceConfig cfg);
    static QStringList unitListFor(const QString& filename, AsciiSourceConfig cfg);
    static QStringList scalarListFor(const QString& filename, AsciiSourceConfig cfg);
    static QStringList stringListFor(const QString& filename, AsciiSourceConfig cfg);

    Kst::ObjectList<Kst::Object> autoCurves(Kst::ObjectStore& objectStore);

    QString timeFormat() const;

    void triggerRowProgress() { emit signalRowProgress(); }

    virtual void enableUpdates() {_updatesDisabled = false;}

signals:
    void signalRowProgress();

private:
    AsciiDataReader _reader;
    AsciiFileBuffer _fileBuffer;
    bool _busy;
    int _read_count_max;
    int _read_count;

    friend class AsciiConfigWidget;
    mutable AsciiSourceConfig _config;

    qint64 _fileSize;
    qint64 _lastFileSize;

    bool _haveHeader;
    bool _fieldListComplete;
    bool _haveWarned;
    double _fileCreationTime_t;
    bool _showFieldProgress;
    double _progress;
    double _progressSteps;
    void updateFieldMessage(const QString&);
    void updateFieldProgress(const QString&);
    void emitProgress(int precent, const QString&);
    QString _actualField;

    QStringList _scalarList;
    QMap<QString, QString> _strings;
    QStringList _fieldList;
    QHash<QString, int> _fieldLookup;
    QMap<QString, QString> _fieldUnits;

    bool useThreads() const;
    bool useSlidingWindow(qint64 bytesToRead)  const;

    int tryReadField(double *v, const QString &field, int s, int n);
    int parseWindowSinglethreaded(QVector<AsciiFileData>& fileData, int col, double* v, int start, const QString& field, int sRead);
    int parseWindowMultithreaded(QVector<AsciiFileData>& fileData, int col, double* v, int start, const QString& field);

    int columnOfField(const QString& field) const;
    static int splitHeaderLine(const QByteArray& line, const AsciiSourceConfig& cfg, QStringList* parts = 0);

    DataInterfaceAsciiString* is;
    DataInterfaceAsciiVector* iv;

    int _progressValue;
    qint64 _progressRows;
    static void rowProgress(QObject*);
    //QTime _progressTimer;
    QElapsedTimer _progressTimer;

    // TODO remove
    friend class DataInterfaceAsciiString;
    friend class DataInterfaceAsciiVector;

    bool _updatesDisabled;
};


#endif
// vim: ts=2 sw=2 et
