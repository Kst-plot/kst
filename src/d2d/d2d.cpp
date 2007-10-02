/***************************************************************************
                                d2d.cpp
                             -------------------
    begin                : Tue Jan 16 2007
    copyright            : (C) 2007 by The University of Toronto
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

#include <stdlib.h> // atoi
#include <qsettings.h>

// hack to make main() a friend of kstdatasource
#define protected public
#include "datavector.h"
#include "kstdatacollection.h"
#include "kstdataobjectcollection.h"
#undef protected

#include "kstdataplugin.h"

void Usage() {
  fprintf(stderr, "usage: d2d in_filename out_filename [-t <out_type>]\n");
  fprintf(stderr, "           [-f <first frame>] [-n <numframes>] [-s skip [-a]] \n");
  fprintf(stderr, "           [-x] col1 [[-x] col2 ... [-x] coln]\n");
}


static void exitHelper() {
  KST::vectorList.clear();
  KST::scalarList.clear();
  KST::dataObjectList.clear();
}

int main(int argc, char *argv[]) {
  atexit(exitHelper);

  QSettings *settingsObject = new QSettings("kstdatarc", QSettings::IniFormat);
  KstDataSource::setupOnStartup(settingsObject);

  char field_list[40][120], in_filename[180], out_filename[180], out_type[40];
  int n_field=0;
  int start_frame=0, n_frames=-1;
  bool do_ave = false, do_skip = false;
  int n_skip = 0;

  if (argc < 4 || argv[1][0] == '-' || argv[2][0] == '-') {
    Usage();
    return -1;
  }

  strncpy(in_filename, argv[1], 180);
  strncpy(out_filename, argv[2], 180);
  for (int i = 3; i < argc; i++) {
    if (argv[i][0] == '-') {
      if (argv[i][1] == 'f') {
        i++;
        start_frame = atoi(argv[i]);
      } else if (argv[i][1] == 'n') {
        i++;
        n_frames = atoi(argv[i]);
      } else if (argv[i][1] == 's') {
        i++;
        n_skip = atoi(argv[i]);
        if (n_skip>0) do_skip = true;
      } else if (argv[i][1] == 'a') {
        do_ave = true;
      } else if (argv[i][1] == 't') {
        i++;
        strncpy(out_type, argv[i], 40);
      } else {
        Usage();
      }
    } else {
      strncpy(field_list[n_field], argv[i], 120);
      n_field++;
    }
  }

  if (!do_skip) do_ave = false;

  KstDataSourcePtr file = KstDataSource::loadSource(in_filename);
  if (!file || !file->isValid() || file->isEmpty()) {
    fprintf(stderr, "d2asc error: file %s has no data\n", in_filename);
    return -2;
  }

  if (n_frames < 0) {
    n_frames = file->frameCount();
  }

  /** make vectors and fill the list **/
  QList<Kst::DataVector*> vlist;

  for (int i = 0; i < n_field; i++) {
    if (!file->isValidField(field_list[i])) {
      fprintf(stderr, "d2asc error: field %s in file %s is not valid\n",
              field_list[i], in_filename);
      return -3;
    }
    Kst::DataVectorPtr v = new Kst::DataVector(file, field_list[i], KstObjectTag(field_list[i], file->tag()), start_frame, n_frames, n_skip, n_skip>0, do_ave);
    vlist.append(v);
  }

  /* find NS */
  int NS=0;
  for (int i = 0; i < n_field; i++) {
    while (vlist.at(i)->update(-1) != KstObject::NO_CHANGE)
      ; // read vector

    if (vlist.at(i)->length() > NS)
      NS = vlist.at(i)->length();
  }

  KstDataSourcePtr out_file;
//FIXME
#if 0
  KService::List sl = KServiceTypeTrader::self()->query("Kst Data Source");
  for (KService::List::ConstIterator it = sl.begin(); it != sl.end(); ++it) {
    if ((*it)->library() == out_type) {
      KstSharedPtr<KST::DataSourcePlugin> p = new KST::DataSourcePlugin(*it);
      out_file = p->create(kConfigObject, out_filename, QString::null);
      break;
    }
  }
#endif

  if (out_file) {
    for (int i = 0; i < n_field; i++) {
      double *data = vlist.at(i)->value();
      int n = out_file->writeField(data, field_list[i], start_frame, n_frames);
      qDebug() << "wrote" << n << "samples for field" << field_list[i];
    }
  }
}

/* vim: sw=2 ts=2 et
 */
