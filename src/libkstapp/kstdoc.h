/***************************************************************************
                          kstdoc.h  -  description
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

#ifndef KSTDOC_H
#define KSTDOC_H

#include <config.h>

// include files for QT

#include <kapplication.h>

#include "kstdebug.h"
#include "kstlistenums.h"
//Added by qt3to4:
#include <QEvent>

class KstDoc : public QObject {
  Q_OBJECT
public:
  KstDoc(QWidget *parent, const char *name=0);
  virtual ~KstDoc();

  /** sets the modified flag for the document after a modifying action
      on the view connected to the document.*/
  void setModified(bool m = true) { _modified = m; }
  /** returns if the document is modified or not. Use this to determine
      if your document needs saving by the user on closing.*/
  bool isModified() const { return _modified; }
  /** asks the user for saving if the document is modified */
  bool saveModified(bool doDelete = true);
  /** initializes the document generally */
  bool newDocument();
  /** closes the acutal document */
  void closeDocument();
  /** loads the document by filename */
  bool openDocument(const KUrl& url, const QString& o_file="|",
        int o_n = -2, int o_f = -2,
        int o_s = -1, bool o_ave = false);
  /** saves the document to a QTextStream */
  void saveDocument(Q3TextStream& ts, bool saveAbsoluteVectorPositions = false);
  /** saves the document under filename */
  bool saveDocument(const QString& filename, bool saveAbsoluteVectorPositions = false, bool prompt = true);
  /** returns the pathname of the current document file*/
  const QString& absFilePath() const;
  /** returns the title of the document */
  const QString& title() const;
  /** returns the pathname of the last saved/loaded document file */
  const QString& lastFilePath() const;

  /** delete a curve from the curvelist and from the plots */
  RemoveStatus removeDataObject(const QString& tag);

  virtual bool event(QEvent *e);

  bool updating() const { return _updating; }

public slots:
  /** deletes the document's contents */
  void deleteContents();
  void purge();
  void wasModified();
  void forceUpdate();
  /** sets the filename of the document */
  void setTitle(const QString& t);
  /** sets the path to the file connected with the document */
  void setAbsFilePath(const QString& filename);
  /** sets the pathname to the last saved/loaded document file */
  void setLastFilePath(const QString& filename);
  /** Increase the starting frame for all vectors by n_frames of the vector */
  void samplesUp();

  /** Decrease the starting frame for all vectors by n_frames of the vector */
  void samplesDown();

  /** set all vectors to read, counting back from the end of the file */
  void samplesEnd();

  /** cancel update */
  void cancelUpdate() { _stopping = true;}

private slots:
  void enableUpdates();

private:
  void createScalars() const;

  /** the modified flag of the current document */
  bool _modified;
  bool _stopping;
  bool _updating;
  bool _nextEventPaint;

  QString _title;
  QString _absFilePath;
  QString _lastFilePath;
  int _lock;

signals:
  /** if something has changed the vectors */
  void dataChanged();
  void updateDialogs();
  void logAdded(const KstDebug::LogMessage& msg);
  void logCleared();
};

#endif // KSTDOC_H
// vim: ts=2 sw=2 et
